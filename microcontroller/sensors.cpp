#include "sensors.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardwareSetup.h"
#include <stdint.h>
#include "communication.h"
#include "hardware/uart.h"
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
	return getSingleUART(0); 
}
void calibratePoint(char point, float *lastMean, float *slopeAcid, float *slopeBase, float *zeroOffset)
{
	float stDev, targetStDev = 0.01;
	float mean, sum;
	float levels[10];
	bool stabilized = false;
	
	while (!stabilized)
	{
		mean = 0;
		stDev = 0;
		for (int i = 0; i < size_t(levels); i++)
		{
			levels[i] = getSingleUART(1);
			sum += levels[i];
		}
		//calculate stdev
		mean = sum / size_t(levels);
		for (int i = 0; i < size_t(levels); ++i) {
			stDev += pow(levels[i] - mean, 2);
		}
		stDev = sqrt(stDev / size_t(levels));
		
		if (stDev < targetStDev)
		{
			char c;
			if (point == 'm' && mean > 6 && mean < 8)
			{
				//mid point calibration
				uart_puts(uart0, "Cal,mid,7.00\r");
			}
			else if (point == 'l' && mean < 5)
			{
				//low point calibration
				uart_puts(uart0, "Cal,low,4.00\r");
			}
			else if (point == 'h' && mean > 9)
			{
				//high point calibration
				uart_puts(uart0, "Cal,high,10.00\r");
			}
			//loop for dumping OK  
			while (c != '\r')
			{
				c = uart_getc(uart0);
			}
			stabilized = true;	
		}
	}
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
	for (int i = 0; slopes[i] != '\r'; i++)
	{
		//slope values into the char array 
		slopes[i] = uart_getc(uart0);
	}
	std::istringstream sss(slopes);
	char comma;
	float slopeA, slopeB, zeroOff;
	sss >> slopeA >> comma >> slopeB >> comma >> zeroOff;
	
	*slopeAcid = slopeA;
	*slopeBase = slopeB;
	*zeroOffset = zeroOff;
	return;
}
void calibratePH() 
{
	
	
	//three point calibration
	
	//step 1 dry calibration: "cal,dry"
	//step 2 "cal,mid,n"
	//step 3 "cal,low,n"
	//step 4 "cal,high,n"
	
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
	return 0.0f;
}

float getPumpRate()
{
	return pumpRate;
}

float getLightLevel()
{
	return 0.0f;
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

	//air sensor readings
	i2c_init(i2c0, 50000);
	gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
	gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);

	//light sensor readings
	//i2c_init(isc1, 50000); //TO DO check baudrate i2c1
	//gpio_set_function(TODO , GPIO_FUNC_I2C);
	//gpio_set_function(TODO, GPIO_FUNC_I2C);


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