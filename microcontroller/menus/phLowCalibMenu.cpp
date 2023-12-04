#include "Menu.h"
#include "Menus.h"
#include "DisplayManager.h"
#include "sensors.h"
#include "phLowCalibMenu.h"
#include "inputs.h"
#include "graphicLCD.h"

void phLowCalibMenu::onConstruction()
{
	GraphicLCD::clear();
	setButtonCallback(std::bind(&phLowCalibMenu::handleButton, this));
	startCalibration(); //setup the circular buffer 
}

void phLowCalibMenu::onDestruction()
{
	setButtonCallback(std::bind(&defaultButtonCallback));
}

void phLowCalibMenu::onUpdate(float dt)
{
	this->renderDisplay();
}

void phLowCalibMenu::handleButton()
{
	// transition to the navigation menu
	switchToNavigationMenu();
}

void phLowCalibMenu::renderDisplay()
{
	/*SENSOR IN MIDPOINT
	Press knob to GO BACK
	Reading: 7.80 Ph
	Sensor calibrating...
	Mean:	Stdev:
	SUCCESSFUL!
	*/
	char calibPoint = 'l';
	
	GraphicLCD::locate(0, 0);
	GraphicLCD::printFullLine((char*)"SENSOR IN 4.0pH");
	GraphicLCD::locate(0, 1);
	GraphicLCD::printFullLine((char*)"PRESS TO CANCEL");
	bool calibCont = calibrationTick(calibPoint);
	
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
	GraphicLCD::printf("%.3f PH \n", getpH());

	float lastMean = returnLastMean();
	float zeroOffset = returnzeroOffset();
	if (lastMean && zeroOffset)
	{
		GraphicLCD::locate(0, 4);
		GraphicLCD::printf("MEAN: %.2f OFFST: %.3f \n", lastMean, zeroOffset);
	}
	float slopeAcid = returnLastMean();
	float slopeBase = returnzeroOffset();
	if (slopeAcid && slopeBase)
	{
		GraphicLCD::locate(0, 5);
		GraphicLCD::printf("ACID: %.3f BASE: %.3f \n", lastMean, zeroOffset);
	}
}

void switchToLowCalibration()
{
	_displayManager->setElement(_phLowCalibMenu);
}
