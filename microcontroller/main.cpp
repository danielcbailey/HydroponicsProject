
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "graphicLCD.h"
#include "RotaryEncoder.h"
#include "DisplayManager.h"
#include "menus/Menu.h"
#include "menus/splashPage.h"
#include "inputs.h"
#include "sensors.h"
#include "menus/Menus.h"
#include <iostream>
#include "serverComm.h"
#include "schedule.h"
#include "hardware/rtc.h"
#include "outputs.h"
#include "menus/phHighCalibMenu.h"
#include "menus/SensorMenu.h"
#include "menus/phCalibMenu.h"
#include "menus/phLowCalibMenu.h"
#include "menus/ecHighCalib.h"
#include "menus/ecLowCalib.h"
#include "menus/ecDryCalib.h"
SplashPage* _splashPage;

SensorMenu* _SensorMenu;

phCalibMenu* _phCalibMenu;

phLowCalibMenu* _phLowCalibMenu;

phHighCalibMenu* _phHighCalibMenu;

ecLowCalibMenu* _ecLowCalibMenu;

ecHighCalibMenu* _ecHighCalibMenu;

ecDryCalibMenu* _ecDryCalibMenu;

void blankFunc()
{
	
}

char jsonChar[1024];

void coreOneReads()
{
	while (true)
	{
		int i = 0;
	
		do	
		{
			//std::getline(std::cin, jsonStr);
			jsonChar[i] = getchar();
		} while (jsonChar[i++] != '\n');
		
		jsonChar[i-1] = 0;
	
		readMessage(jsonChar);
	}
	//printf("{\"jsonrpc\": \"2.0\",\"method\": \"event/log\",\"params\": {\"severity\" : \"debug\",\"message\" : \"This is a test!\"}}\n");

	

}

void performEvent(eventTypes currEvent) {
	switch (currEvent) {
	case NO_EVENT:
		break;
	case PUMP_ON:
		setPump(1);
		break;
	case PUMP_OFF:
		setPump(0);
		break;
	case LIGHT_ON:
		setLight(1);
		break;
	case LIGHT_OFF:
		setLight(0);
		break;
	}
}

void safetyCheck()
{
	if (getAirTemp() >= MAX_TEMPERATURE)
	{	
		setLight(0);
		//send info to server 
		//log to a warning log 
	}
	if (getWaterLevel() <= MIN_WATER_LEVEL) 
	{
		setPump(1);
		//send info to server 
		//log to a warning log 
	}
	
}

int main() {
	rtc_init();
	stdio_init_all();
	datetime_t dummyTime = { 2000, 1, 1, 1, 1, 1, 1 };
	
	rtc_set_datetime(&dummyTime);
	
	
	//Initialize core 1 to await server messages
	sleep_ms(10);
	multicore_reset_core1();
	sleep_ms(10);
	multicore_launch_core1(coreOneReads);
	
	
	
	GraphicLCD::init();
	alarm_pool_init_default();
	alarm_pool_t* defaultPool = alarm_pool_get_default();
	GraphicLCD::setAlarmPool(defaultPool);
	
	GraphicLCD::print((char*)"Initializing...");
	sleep_ms(3000);
	initSensors();
	outputsInit();
	
	GraphicLCD::locate(0, 0);
	GraphicLCD::print((char*)"ERROR INTIALIZING");
	
	_splashPage = new SplashPage();
	
	//init all menus 
	init_inputs();
	initializeMainMenu();
	_SensorMenu = new SensorMenu();
	_phCalibMenu = new phCalibMenu();
	_phLowCalibMenu = new phLowCalibMenu();
	_phHighCalibMenu = new phHighCalibMenu();
	_ecLowCalibMenu = new ecLowCalibMenu();
	_ecHighCalibMenu = new ecHighCalibMenu();
	_ecDryCalibMenu = new ecDryCalibMenu();

	
	initializeManualMenu();
	initializeScheduleMenu();
	initializephSelectMenu();
	initializeECSelectMenu();
	
	_displayManager = new DisplayManager(_splashPage);
	
	uint64_t t = 0;
	datetime_t currTime;
	gardenEvent prevEvent;
	while (true)
	{
		sleep_ms(100);
		uint64_t temp = to_us_since_boot(get_absolute_time());
		float dt = (float)(temp - t) / 1000000;
		_displayManager->dispatchTick(dt);

		rtc_get_datetime(&currTime);
		gardenEvent currEvent = getScheduleEvent(currTime); //check if there is an event to do 
		if (currEvent.eventType != prevEvent.eventType) {
			//perform the event type
			performEvent(currEvent.eventType);
		}
		prevEvent = currEvent;
		
		//perform safety check as well
		safetyCheck();
	}
}
