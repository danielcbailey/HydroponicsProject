
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

SplashPage* _splashPage;

void blankFunc()
{
	
}


void coreOneReads()
{
	while (true)
	{
		int i = 0;
		char jsonChar[256];
	
		do	
		{
			//std::getline(std::cin, jsonStr);
			jsonChar[i] = getchar();
		} while (jsonChar[i++] != '\n');
	
		readMessage(jsonChar);
	}
	//printf("{\"jsonrpc\": \"2.0\",\"method\": \"event/log\",\"params\": {\"severity\" : \"debug\",\"message\" : \"This is a test!\"}}\n");

	

}

int main() {
	stdio_init_all();
	
	test();
	
	//testing display stuff
	
	/*MenuItem items[] = {
		MenuItem("Test 1", std::bind(&blankFunc)),
		MenuItem("Test 2", std::bind(&blankFunc)),
		MenuItem("Test 3", std::bind(&blankFunc)),
		//MenuItem("Test 4", std::bind(&blankFunc)),
		//MenuItem("Test 5", std::bind(&blankFunc)),
		//MenuItem("Test 6", std::bind(&blankFunc)),
		//MenuItem("Test 7", std::bind(&blankFunc)),
		//MenuItem("Test 8", std::bind(&blankFunc)),
		MenuItem("Test 9", std::bind(&blankFunc)) 
	};
	
	Menu testMenu("This is a test 2", items, 4);*/
	
	
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
	
	GraphicLCD::locate(0, 0);
	GraphicLCD::print((char*)"ERROR INTIALIZING");
	
	_splashPage = new SplashPage();
	
	init_inputs();
	initializeMainMenu();
	_displayManager = new DisplayManager(_splashPage);
	
	uint64_t t = 0;
	while (true)
	{
		sleep_ms(100);
		uint64_t temp = to_us_since_boot(get_absolute_time());
		float dt = (float)(temp - t) / 1000000;
		_displayManager->dispatchTick(dt);
	}
}
