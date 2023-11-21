#pragma once

#include <pico.h>
#include <pico/stdlib.h>
#include "pico/critical_section.h"

class GraphicLCD
{
private:
	static bool posIsLeft;
	static char textX;
	static char textY;
	static bool invertBitmaps;
	static alarm_pool_t* pool;
	static alarm_id_t alarmId;
	static bool isDim;
	static bool isOn;
	static critical_section_t critical;
	
	//Constants
	static const int32_t displayIdleDim = 30; //In seconds
	static const int32_t displayIdleOff = 90; //In seconds after IdleDim
		
	//Private functions
	
	static void sendCommand(bool isLeft, bool rs, char data);
	
	static void setPos(char x, char y);
	
	static void writeRaw(char data);
	
	static int64_t alarmCallback(alarm_id_t id, void* data);
	
public:
	static void init();
	
	static void turnOn();
	
	static void turnOff();
	
	static void setBacklight(bool on);
	
	static void clear();
	
	static void test();
	
	//text coordinates
	static void locate(char x, char y);
	
	inline static int getTextY()
	{
		return textY / 8;
	}
	
	inline static int getTextX()
	{
		return textX;
	}
	
	static void print(char* str);
	
	static void printFullLine(char* str);
	
	static void printf(const char* fmt, ...);
	
	static void writeBitmap(bool data[], char width, char height, char x, char y);
	
	static inline void setInvert(bool invert)
	{
		invertBitmaps = invert;
	}
	
	//Will automatically start the timeout process
	static void setAlarmPool(alarm_pool_t* aPool);
	
	static bool reportUserActivity();
};

