#include "Menu.h"
#include "Menus.h"
#include "DisplayManager.h"
#include "sensors.h"
#include "ecDryCalib.h"
#include "inputs.h"
#include "graphicLCD.h"

void ecDryCalibMenu::onConstruction()
{
	GraphicLCD::clear();
	setButtonCallback(std::bind(&ecDryCalibMenu::handleButton, this));
	startECCalibration(30);
}

void ecDryCalibMenu::onDestruction()
{
	setButtonCallback(std::bind(&defaultButtonCallback));
}

void ecDryCalibMenu::onUpdate(float dt)
{
	this->renderDisplay();
}

void ecDryCalibMenu::handleButton()
{
	// transition to the navigation menu
	switchToNavigationMenu();
}

void ecDryCalibMenu::renderDisplay()
{
	char calibPoint = 'd';
	
	GraphicLCD::locate(0, 0);
	GraphicLCD::printFullLine((char*)"NO SOLUTION!");
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

void switchToECDryCalibration()
{
	_displayManager->setElement(_ecDryCalibMenu);
}
