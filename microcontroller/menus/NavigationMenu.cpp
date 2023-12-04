#include "Menu.h"
#include "DisplayManager.h"
#include "splashPage.h"
#include "Menus.h"

Menu* _mainMenu;

void sensorsSelected()
{
	switchToSensorMenu();
}

void lightPumpSelected()
{
	switchToManualMenu();
}

void scheduleSelected()
{
	switchToScheduleMenu();
}

void phCalibSelected()
{
	switchToPhSelectMenu();
}

void ecCalibSelected()
{
	switchToECSelectMenu();
}

void errorsSelected()
{
	
}

void onBackSelected()
{
	_displayManager->setElement(_splashPage);
}

MenuItem navigationItems[] = { 
	MenuItem("Sensor Readings", sensorsSelected),
	MenuItem("Light&Pump Control", lightPumpSelected),
	MenuItem("See Schedule", scheduleSelected),
	MenuItem("PH Calibration", phCalibSelected),
	MenuItem("EC Calibration", ecCalibSelected),
	MenuItem("Error Log", errorsSelected),
	MenuItem("GO BACK TO MAIN", onBackSelected),
};

void initializeMainMenu()
{	
	_mainMenu = new Menu("	NAVIGATE TO", navigationItems, 7);
}

void switchToNavigationMenu()
{
	_displayManager->setElement(_mainMenu);
}