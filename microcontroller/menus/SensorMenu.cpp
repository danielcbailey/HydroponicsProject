#include "SensorMenu.h"
#include "Menu.h"
#include "Menus.h"
#include "DisplayManager.h"
#include "graphicLCD.h"
#include "sensors.h"
#include <functional>
#include "inputs.h"


void SensorMenu::onConstruction()
{
	GraphicLCD::clear();
	setButtonCallback(std::bind(&SensorMenu::handleButton, this));
	this->renderDisplay();
}

void SensorMenu::onDestruction()
{
	setButtonCallback(std::bind(&defaultButtonCallback));
}

void SensorMenu::onUpdate(float dt)
{
	this->renderDisplay();
}

void SensorMenu::handleButton()
{
	// transition to the navigation menu
	switchToNavigationMenu();
}



void SensorMenu::renderDisplay()
{
	GraphicLCD::locate(0, 0);
	GraphicLCD::printFullLine((char*)"	READINGS");
	GraphicLCD::locate(0, 1);
	GraphicLCD::printf("%.1fF %.1f%% RH\n", getAirTemp(), getAirHumidity());
	GraphicLCD::printf("%d PPM CO2\n", (int)getAirCO2());
	GraphicLCD::printf("%.2f PH %.2f EC\n", getpH(), getEC());
	GraphicLCD::printf("%.1f in %.1f GPM\n", getWaterLevel(), getPumpRate());
	GraphicLCD::printf("%.1f lumen\n", getLightLevel());
	GraphicLCD::printFullLine((char*)"PRESS THE KNOB TO GO BACK");
}

void switchToSensorMenu()
{
	_displayManager->setElement(_SensorMenu);
}