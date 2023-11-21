#include "sensors.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardwareSetup.h"
#include <stdint.h>
#include "communication.h"

float lastPH = 0.0f;

float lastEC = 0.0f;

float pumpRate = 0.0f;

float lastCO2PPM = 0.0f;

float lastTemp = 0.0f;

float lastHumidity = 0.0f;

void airSensorCommand(uint16_t cmd, uint16_t data);
uint16_t airSensorRead(uint16_t address);
void airSensorReadMultiple(uint16_t address, uint8_t* buf, int length);

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

float getPH()
{
	return lastPH;
}

float getEC()
{
	return lastEC;
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
	i2c_init(i2c0, 50000);
	gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
	gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
	
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