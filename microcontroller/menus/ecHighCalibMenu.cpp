#include "Menu.h"
#include "Menus.h"
#include "DisplayManager.h"
#include "sensors.h"
#include "ecHighCalib.h"
#include "inputs.h"
#include "graphicLCD.h"

void ecHighCalibMenu::onConstruction()
{
	GraphicLCD::clear();
	setButtonCallback(std::bind(&ecHighCalibMenu::handleButton, this));
	startECCalibration(100); //setup the circular buffer 
}

void ecHighCalibMenu::onDestruction()
{
	setButtonCallback(std::bind(&defaultButtonCallback));
}

void ecHighCalibMenu::onUpdate(float dt)
{
	this->renderDisplay();
}

void ecHighCalibMenu::handleButton()
{
	// transition to the navigation menu
	switchToNavigationMenu();
}

void ecHighCalibMenu::renderDisplay()
{
	char calibPoint = 'h';
	
	GraphicLCD::locate(0, 0);
	GraphicLCD::printFullLine((char*)"SENSOR IN 80k");
	GraphicLCD::locate(0, 1);
	GraphicLCD::printFullLine((char*)"PRESS TO CANCEL");
	bool calibCont = calibrationECTick(calibPoint);
	
	if (calibCont)
	{
		GraphicLCD::locate(0, 2);
		GraphicLCD::printFullLine((char*)"IN PROGRESS...");
		// calibration is still running so display should show as such
	}
	else
	{
		GraphicLCD::locate(0, 2);
		GraphicLCD::printFullLine((char*)"SUCCESS!");
		// the calibration is not running, so the display can show done	
	}
	GraphicLCD::locate(0, 3);
	GraphicLCD::printf("%.1f uS\n", getEC());

	float lastMean = getEClastMean();
	if (lastMean)
	{
		GraphicLCD::locate(0, 4);
		GraphicLCD::printf("MEAN:%.2f\n", lastMean);
	}
}

void switchToECHighCalibration()
{
	_displayManager->setElement(_ecHighCalibMenu);
}
