#include "splashPage.h"
#include "inputs.h"
#include <functional>
#include "DisplayManager.h"
#include "graphicLCD.h"
#include "sensors.h"
#include "Menus.h"
#include "serverComm.h"
#include "schedule.h"
#include <map>

void SplashPage::onConstruction()
{
	GraphicLCD::clear();
	setButtonCallback(std::bind(&SplashPage::handleButton, this));
	this->renderDisplay();
}

void SplashPage::onDestruction()
{
	setButtonCallback(std::bind(&defaultButtonCallback));
}

void SplashPage::onUpdate(float dt)
{
	this->renderDisplay();
}

void SplashPage::handleButton()
{
	// transition to the navigation menu
	switchToNavigationMenu();
}

std::string getMonthName(int month) {
	const std::map<int, std::string> monthMap = {
		{ 0, "Jan0" },
		{ 1, "Jan" },
		{ 2, "Feb" },
		{ 3, "Mar" },
		{ 4, "Apr" },
		{ 5, "May" },
		{ 6, "Jun" },
		{ 7, "Jul" },
		{ 8, "Aug" },
		{ 9, "Sep" },
		{ 10, "Oct" },
		{ 11, "Nov" },
		{ 12, "Dec" }
	};
	auto it = monthMap.find(month);
	if (it != monthMap.end()) {
		return it->second;
	}
	else {
		return "Invalid Month";
	}
}

void SplashPage::renderDisplay()
{
	/*
	DISCONNECTED
	10:00 Nov 8 2024
	Next: pump ON 10:30

	Temperature xx.x NORMAL
	Water level x.x NORMAL

	PRESS THE KNOB FOR MENU
	*/
	
	
	bool connectionCheck = checkConnectionTimeout();
	
	GraphicLCD::locate(0, 0);
	if (connectionCheck) {
		GraphicLCD::printFullLine((char*)"  CONNECTED");
	}
	else {
		GraphicLCD::printFullLine((char*)"  DISCONNECTED");
	}
	//get time now 
	datetime_t displayTime = getCurrentTime(); 
	
	int8_t month, day, hour, min;
	int16_t year;
	
	month = displayTime.month;
	day = displayTime.day;
	hour = displayTime.hour;
	min = displayTime.min;
	year = displayTime.year;
	
	//convert month to string
	std::string monthStr = getMonthName(static_cast<int>(month));
	
	GraphicLCD::printf("%02d:%02d %s %d %d\n", hour, min, monthStr.c_str(), day, year); // 10:00 Nov 8 2024
	
	gardenEvent nextEvent = getNextEvent(displayTime); 
	
	std::string eventStr;
	switch (nextEvent.eventType) {
	case NO_EVENT:   
		eventStr = "NO_EVENT";
		break;
	case PUMP_ON:   
		eventStr = "PUMP_ON";
		break;
	case PUMP_OFF:
		eventStr = "PUMP_OFF";
		break;
	case LIGHT_ON:   
		eventStr = "LIGHT_ON";
		break;
	case LIGHT_OFF:  
		eventStr = "LIGHT_OFF";
		break;
	default:    
		break;
	}
	GraphicLCD::printf("NEXT:%s %d\n", eventStr.c_str(), nextEvent.eventTime);

	GraphicLCD::printf("Temp: %.1fF\n", getAirTemp());
	GraphicLCD::printf("Water: %.1fin\n", getWaterLevel());

	GraphicLCD::printFullLine((char*)"PRESS THE KNOB FOR MENU");
	
	

	//GraphicLCD::printf("%.1fF %.1f%% RH\n", getAirTemp(), getAirHumidity());
	//GraphicLCD::printf("%d PPM CO2\n", (int)getAirCO2());
	//GraphicLCD::printf("%.2f PH %.2f EC\n", getpH(), getEC());
	//GraphicLCD::printf("%.1f in %.1f GPM\n", getWaterLevel(), getPumpRate());
	//GraphicLCD::printf("%.1f lumen", getLightLevel());
}