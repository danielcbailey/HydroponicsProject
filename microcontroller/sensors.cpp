#include "sensors.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardwareSetup.h"
#include <stdint.h>
#include "communication.h"
#include "hardware/uart.h"
#include "hardware/adc.h"
#include <stdlib.h> /*atof */
#include <cmath> /*pow */
#include <iostream>
#include <sstream>

float lastPH = 0.0f;

float lastEC = 0.0f;

float pumpRate = 0.0f;

float lastCO2PPM = 0.0f;

float lastTemp = 0.0f;

float lastHumidity = 0.0f;

void airSensorCommand(uint16_t cmd, uint16_t data);
uint16_t airSensorRead(uint16_t address);
void airSensorReadMultiple(uint16_t address, uint8_t* buf, int length);

float getSingleUART(bool pH);

void wakeUpUartSensor(uart_inst_t* uart);

uint8_t gencrc(uint8_t *data, size_t len)
{
	uint8_t crc = 0xff;
	size_t i, j;
	for (i = 0; i < len; i++) {
		crc ^= data[i];
		for (j = 0; j < 8; j++) {
			if ((crc & 0x80) != 0)
				crc = (uint8_t)((crc << 1) ^ 0x31);
			else
				crc <<= 1;
		}
	}
	return crc;
}

float getpH()
{
	return getSingleUART(1);
}

float getEC() 
{
	float ret = 0;
	for (int i = 0; 10 > i && ret == 0; i++)
	{
		if (i > 0)
		{
			busy_wait_ms(1000);
		}
		ret = getSingleUART(0);
	}
	return ret;
}

bool calibrationRunning = false;
float circularBuffer[16];
int measurementCount; // for use with indexing the circular buffer, hint: % may be useful here
float targetStDev = 0.01;
float lastMean = 0, slopeAcid = 0, slopeBase = 0, zeroOffset = 0;
void startCalibration()
{
	lastMean = 0;
	// do stuff to initialize calibration to prepare for loop body
	for (int i = 0; i < 16; i++)
	{
		circularBuffer[i] = i; // just an initial value that will cause it to have high
							   // stddev so that it doesn't terminate the calibration early
	}
	measurementCount = 0;
	calibrationRunning = true;
}

bool checkStabilization()
{
	//sum the array
	float sum = 0;
	for (int i = 0; i < 16; i++) {
		sum += circularBuffer[i];
	}
	// calculating mean
	float mean = sum / 16;
	
	float values = 0;
	for (int i = 0; i < 16; i++) {
		values += pow(circularBuffer[i] - mean, 2);
	}
	// variance is the square of standard deviation
	float variance = values / 16;
	float standardDeviation = sqrt(variance);
	
	if (standardDeviation > targetStDev)
	{
		return false;
	}
	lastMean = mean;
	return true;
	//if both the mean of the array is in between 6.5 and 7.5 & the st dev is below 0.5
}

void performCalibration(char p)
{
	char c = 0;
	if (p == 'm')
	{
		//mid point calibration
		uart_puts(uart0, "Cal,mid,7.00\r");
	}
	else if (p == 'l')
	{
		//low point calibration
		uart_puts(uart0, "Cal,low,4.00\r");
	}
	else if (p == 'h')
	{
		//high point calibration
		uart_puts(uart0, "Cal,high,10.00\r");
	}
	//loop for dumping OK  
	while (c != '\r')
	{
		c = uart_getc(uart0);
	}
}

void performECCalibration(char p)
{
	char c = 0;
	if (p == 'l')
	{
		//mid point calibration
		uart_puts(uart1, "Cal,low,12880\r");
	}
	else if (p == 'h')
	{
		//low point calibration
		uart_puts(uart1, "Cal,high,80000\r");
	} 
	else if (p == 'd') 
	{
		//dry point calibration
		uart_puts(uart1, "Cal,dry\r");
	}
	//loop for dumping OK  
	while (c != '\r')
	{
		c = uart_getc(uart1);
	}
}

void getValues()
{
	// now the readings are stabilized
	uart_puts(uart0, "Slope,?\r");
	//example response 
	//?Slope,99.7,100.3,-0.89<cr>
	//*OK<cr>
	
	char c; //first part
	while (c != ',')
	{
		c = uart_getc(uart0); //Sensor
	}
	char slopes[256];
	for (int i = 0; i < 256; i++)
	{
		//slope values into the char array 
		slopes[i] = uart_getc(uart0);
		if (slopes[i] == '\r')
		{
			slopes[i] = 0;
			break;
		}
	}
	
	//loop for dumping OK
	c = 0;
	do
	{
		c = uart_getc(uart0);
		
	} while (c != '\r');
	
	std::istringstream sss(slopes);
	char comma;
	sss >> slopeAcid >> comma >> slopeBase >> comma >> zeroOffset;	
}

float returnLastMean()
{
	return lastMean;
}

float returnslopeAcid()
{
	return slopeAcid;
}

float returnslopeBase()
{
	return slopeBase;
}

float returnzeroOffset()
{
	return zeroOffset;
}

bool calibrationTick(char point)
{
	if (!calibrationRunning)
	{
		return false; // the calibration is not running, so the display can show done
	}
	
	bool didStabilize = false; // set to true when the readings stabilize on one number
	
	//reading from the sensor and updating the circular buffer
	circularBuffer[measurementCount % 16] = getpH(); //sensor read here
	measurementCount++;
	didStabilize = checkStabilization();
	
	if (didStabilize)
	{
		// finalize the calibration by telling the sensor that what it is reading right
		wakeUpUartSensor(uart0);
		performCalibration(point);
		getValues();
		// now should actually be ph4,7, or 10 by sending the calibration command.
		calibrationRunning = false;
		return false; // calibration just finished so the screen can say its done
	}
	return true; // calibration is still running so display should show as such
}

bool calibrationRunningEC = false;
float circularBufferEC[16];
int measurementCountEC; // for use with indexing the circular buffer, hint: % may be useful here
float targetStDevEC = 30;

float lastMeanEC;

void startECCalibration(float stdDev)
{
	lastMeanEC = 0;
	// do stuff to initialize calibration to prepare for loop body
	for (int i = 0; i < 16; i++)
	{
		circularBufferEC[i] = i * 1000; // just an initial value that will cause it to have high
							   // stddev so that it doesn't terminate the calibration early
	}
	measurementCountEC = 0;
	calibrationRunningEC = true;
	targetStDevEC = stdDev;
}


bool checkECStabilization()
{
	//sum the array
	float sum = 0;
	for (int i = 0; i < 16; i++) {
		sum += circularBufferEC[i];
	}
	// calculating mean
	float mean = sum / 16;
	
	float values = 0;
	for (int i = 0; i < 16; i++) {
		values += pow(circularBufferEC[i] - mean, 2);
	}
	// variance is the square of standard deviation
	float variance = values / 16;
	float standardDeviation = sqrt(variance);
	
	if (standardDeviation > targetStDevEC)
	{
		return false;
	}
	lastMeanEC = mean;
	return true;
}

float getEClastMean()
{
	return lastMeanEC;
}
bool calibrationECTick(char point)
{
	if (!calibrationRunningEC)
	{
		return false; // the calibration is not running, so the display can show done
	}
	
	bool didStabilize = false; // set to true when the readings stabilize on one number
	
	//reading from the sensor and updating the circular buffer
	circularBufferEC[measurementCountEC % 16] = getEC(); //sensor read here
	measurementCountEC++;
	didStabilize = checkECStabilization();
	
	if (didStabilize)
	{
		// finalize the calibration by telling the sensor that what it is reading right
		while (uart_is_readable(uart1))
		{
			uart_getc(uart1);
		}
		performECCalibration(point);
		calibrationRunningEC = false;
		return false; // calibration just finished so the screen can say its done
	}
	return true; // calibration is still running so display should show as such
}

void wakeUpUartSensor(uart_inst_t* uart)
{
	uart_puts(uart, "Status\r");
	
	char c;
	int pos = 0;
	char expecting[] = "*\r";
	while (pos < 2)
	{ 
		c = uart_getc(uart);
		if (c == expecting[pos])
		{
			pos++;
		}
	}
	
	busy_wait_us(200000);
}

float getContUART(char cont, bool pH = 1)
{
	//cont = 1 reading per once per sec
	//cont = 2 to 99 reading every n sec
	//cont = 0 disable cont readings
	//cont = ? reading mode on/off? ? = 63
}

float getSingleUART(bool pH = 1)
{
	uart_inst_t *uart;
	if (pH)
	{
		uart = uart0; //get pH value
	}
	else
	{
		uart = uart1; //get EC value
	}
	
	wakeUpUartSensor(uart);
	
	uart_puts(uart, "R\r");
	//example response for pH
	// 9.560 <cr>
	// *OK <cr>
	//example response for EC
	// 1413 <cr>
	// *OK <cr>
	float val;
	char fl[32];
	char c;

		
	for (int i = 0; i <32; i++)
	{
		c = uart_getc(uart);
		if (c == '\r')
		{
			fl[i] = 0;
			break;
		}
		//first buffer for the float
		fl[i] = c;
	}
	
	if (fl[0] == '*')
	{
		// an error occurred.
		
		return 0;
	}
	
	//loop for dumping OK  
	do
	{
		c = uart_getc(uart);
		
	} while (c != '\r');
	
	val = atof(fl);
	
	if (pH)
	{
		uart_puts(uart, "Sleep\r");
		char expectingSL[] = "*SL\r";
		int pos = 0;
		while (pos < 4)
		{ 
			c = uart_getc(uart);
			if (c == expectingSL[pos])
			{
				pos++;
			}
		}
	}
	
	
	return val;
}

float getWaterLevel()
{
	float seriesResistor = 1000; //Change if the hardware setup is changes
	float Vcc = 5;
	const float convFactor = 3.3f / (1 << 12); // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
	const float expectedRef = 1500;

	adc_select_input(2); //Select an ADC input. 0...3 are GPIOs 26...29 respectively.
	float sensorRead = 0; 
	for (int i = 0; 100 > i; i++)
	{
		uint16_t resultSense = adc_read();
		sensorRead += resultSense * convFactor;
		busy_wait_us(10);
	}
	sensorRead /= 100;
	
	adc_select_input(1); //Select an ADC input. 0...3 are GPIOs 26...29 respectively.
	float refRead = 0;
	for (int i = 0; 100 > i; i++)
	{
		uint16_t resultRef = adc_read();
		refRead += resultRef * convFactor; 
		;
		busy_wait_us(10);
	}
	refRead /= 100;

	//Vread = Vin * (Rsensor / Rsensor + series)
	// Rsensor = (Rseries * Vread) / (Vread - Vin)
	
	float refResistor = (seriesResistor * refRead) / (Vcc - refRead);
	float sensorResistor = (seriesResistor * sensorRead) / (Vcc - sensorRead);
	
	float offset = refResistor / 1500;
	
	float x1 = 1500 * offset;
	//x1 = x1 * (1 + offsetPerc);
	//x1 = refResistor;
	
	//two point form equation: y - y1 = ((y2 - y1) / (x2- x1)) * (x - x1)
	//  y - y1 = ((y2 - y1) / (x2- x1)) * (x - x1)
	//P1: (1500 - offset, 1)
	//P2: (1100, 8)
	float result = (((8 - 1) / (400 * offset - x1)) * (sensorResistor - x1)) + 1;
	return result;
}

float getPumpRate()
{
	return pumpRate;
}


void initLightSensor()
{
	int8_t setMedGain = 0b00000001;
	uint8_t buf[2];
	buf[0] = 0x14; // register for configuring the gain
	buf[1] = setMedGain;
	
	//config the gain
	//7 sreset - 0 
	//6 reserverd - 0
	//54 again - 00 low gain 01 medium gain 10 high gain 11 max high gain mode
	//3 reserved - 0 
	//2:0 integration time - 000 36k 001 and rest full range

	if (i2c_write_blocking(i2c0, 0x29, buf, 2, false) == PICO_ERROR_GENERIC)
	{
		//an error occured while attempting to communicate with the device.
		printf("could not communicate with the light  sensor.\n");
	}
}
float getLightLevel()
{
	const float ResponsivityPerCount = 264.1;
	
	uint8_t buf[2];
	buf[0] = 0x14;
	if (i2c_write_blocking(i2c0, 0x29, buf, 1, true) != PICO_ERROR_NONE)
	{
		return 0;
	}
	//Channel 0 reading
	if (i2c_read_blocking(i2c0, 0x29, buf, 2, false) != PICO_ERROR_NONE) {
		//an error occured while attempting to communicate with the device.
		//reporterror("could not communicate with the air quality sensor.");
		return 0;
	}
	
	return ((buf[0] << 8) | buf[1]) * ResponsivityPerCount;
	
}

float getAirTemp()
{
	uint16_t ready = airSensorRead(0x0202);
	
	if (ready != 1)
	{
		return lastTemp;
	}
	
	uint8_t buf[18];
	airSensorReadMultiple(0x0300, buf, 18);
	
	uint8_t temp[4];
	temp[0] = buf[4];
	temp[1] = buf[3];
	temp[2] = buf[1];
	temp[3] = buf[0];
	
	lastCO2PPM = *((float*)temp);
	
	temp[0] = buf[10];
	temp[1] = buf[9];
	temp[2] = buf[7];
	temp[3] = buf[6];
	
	lastTemp = *((float*)temp) * 1.8 + 32;
	
	temp[0] = buf[16];
	temp[1] = buf[15];
	temp[2] = buf[13];
	temp[3] = buf[12];
	
	lastHumidity = *((float*)temp);
	
	return lastTemp;
}

float getAirHumidity()
{
	uint16_t ready = airSensorRead(0x0202);
	
	if (ready != 1)
	{
		return lastHumidity;
	}
	
	uint8_t buf[18];
	airSensorReadMultiple(0x0300, buf, 18);
	
	uint8_t temp[4];
	temp[0] = buf[4];
	temp[1] = buf[3];
	temp[2] = buf[1];
	temp[3] = buf[0];
	
	lastCO2PPM = *((float*)temp);
	
	temp[0] = buf[10];
	temp[1] = buf[9];
	temp[2] = buf[7];
	temp[3] = buf[6];
	
	lastTemp = *((float*)temp) * 1.8 + 32;
	
	temp[0] = buf[16];
	temp[1] = buf[15];
	temp[2] = buf[13];
	temp[3] = buf[12];
	
	lastHumidity = *((float*)temp);
	
	return lastHumidity;
}

float getAirCO2()
{
	uint16_t ready = airSensorRead(0x0202);
	
	if (ready != 1)
	{
		return lastCO2PPM;
	}
	
	uint8_t buf[18];
	airSensorReadMultiple(0x0300, buf, 18);
	
	uint8_t temp[4];
	temp[0] = buf[4];
	temp[1] = buf[3];
	temp[2] = buf[1];
	temp[3] = buf[0];
	
	lastCO2PPM = *((float*)temp);
	
	temp[0] = buf[10];
	temp[1] = buf[9];
	temp[2] = buf[7];
	temp[3] = buf[6];
	
	lastTemp = *((float*)temp) * 1.8 + 32;
	
	temp[0] = buf[16];
	temp[1] = buf[15];
	temp[2] = buf[13];
	temp[3] = buf[12];
	
	lastHumidity = *((float*)temp);
	
	return lastCO2PPM;
}

void initSensors()
{

	//air and light sensor readings
	i2c_init(i2c0, 50000);
	gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
	gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
	initLightSensor();
	
	//init analog readings 
	adc_init(); //init ADC HW
	adc_gpio_init(LEVEL_SENSOR); //pin 28 initialized
	adc_gpio_init(LEVEL_COMPENSATION); //pin 27 initialized
	
	
	//LEVEL_COMPENSATION 27 //ADC1??
	
	//initialize UART for pH readings
    uart_init(uart0, 9600); //default rate for ezo_PH
	//UART - 0 is TX, 1 is RX
	gpio_set_function(PH_TX, GPIO_FUNC_UART);
	gpio_set_function(PH_RX, GPIO_FUNC_UART);
	
	//disable the default continuous reading for pH sensor
	wakeUpUartSensor(uart0);
	uart_puts(uart0, "C,0\r");
	char c;
	int pos = 0;
	char expecting[] = "*OK\r";
	while (pos < 4)
	{ 
		c = uart_getc(uart0);
		if (c == expecting[pos])
		{
			pos++;
		}
	}
	
	uart_puts(uart0, "Sleep\r"); 
	pos = 0;
	char expectingSL[] = "*SL\r";
	while (pos < 4)
	{ 
		c = uart_getc(uart0);
		if (c == expectingSL[pos])
		{
			pos++;
		}
	}
	
	//initialize UART for electrical conductivity readings
	uart_init(uart1, 9600); //default rate for EC
	gpio_set_function(EC_TX, GPIO_FUNC_UART);
	gpio_set_function(EC_RX, GPIO_FUNC_UART);
	
	//disable the default continuous reading for EC sensor
	wakeUpUartSensor(uart1);
	uart_puts(uart1, "C,0\r");
	pos = 0;
	while (pos < 4)
	{ 
		c = uart_getc(uart1);
		if (c == expecting[pos])
		{
			pos++;
		}
	}
	
	/*
	uart_puts(uart1, "Sleep\r"); 
	pos = 0;
	while (pos < 4)
	{ 
		c = uart_getc(uart1);
		if (c == expectingSL[pos])
		{
			pos++;
		}
	}
	*/
	
	airSensorCommand(0x0010, 0);
	sleep_ms(3);
}

void airSensorCommand(uint16_t cmd, uint16_t data)
{
	uint8_t src[5];
	src[0] = cmd >> 8;
	src[1] = cmd;
	src[2] = data >> 8;
	src[3] = data;
	src[4] = gencrc(src + 2, 2);
	
	if (i2c_write_blocking(i2c0, 0x61, src, 5, false) == PICO_ERROR_GENERIC)
	{
		//An error occured while attempting to communicate with the device.
		reportError("Could not communicate with the air quality sensor.");
		return;
	}
}

uint16_t airSensorRead(uint16_t address)
{
	uint8_t buf[3];
	buf[0] = address >> 8;
	buf[1] = address;
	
	if (i2c_write_blocking(i2c0, 0x61, buf, 2, false) == PICO_ERROR_GENERIC)
	{
		//An error occured while attempting to communicate with the device.
		reportError("Could not communicate with the air quality sensor.");
		return 0;
	}
	
	if (i2c_read_blocking(i2c0, 0x61, buf, 3, false) == PICO_ERROR_GENERIC) {
		//An error occured while attempting to communicate with the device.
		reportError("Could not communicate with the air quality sensor.");
		return 0;
	}
	
	return (buf[0] << 8) | buf[1];
}

void airSensorReadMultiple(uint16_t address, uint8_t* buf, int length)
{
	uint8_t bufOut[2];
	bufOut[0] = address >> 8;
	bufOut[1] = address;
	
	if (i2c_write_blocking(i2c0, 0x61, bufOut, 2, false) == PICO_ERROR_GENERIC)
	{
		//An error occured while attempting to communicate with the device.
		reportError("Could not communicate with the air quality sensor.");
		return;
	}
	
	sleep_ms(3);
	
	if (i2c_read_blocking(i2c0, 0x61, buf, length, false) == PICO_ERROR_GENERIC) {
		//An error occured while attempting to communicate with the device.
		reportError("Could not communicate with the air quality sensor.");
		return;
	}
}