#include "splashPage.h"
#include "inputs.h"
#include <functional>
#include "DisplayManager.h"
#include "graphicLCD.h"
#include "sensors.h"

void SplashPage::onConstruction()
{
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
	
}

void SplashPage::renderDisplay()
{
	GraphicLCD::locate(0, 0);
	GraphicLCD::printFullLine((char*)"  DISCONNECTED");
	GraphicLCD::locate(0, 2);
	GraphicLCD::printf("%.1fF %.1f%% RH\n", getAirTemp(), getAirHumidity());
	GraphicLCD::printf("%d PPM CO2\n", (int)getAirCO2());
	GraphicLCD::printf("%.2f PH %.2 EC\n", getPH(), getEC());
	GraphicLCD::printf("%.1fin %.1f GPM\n", getWaterLevel(), getPumpRate());
	GraphicLCD::printf("%.1f lumen", getLightLevel());
}