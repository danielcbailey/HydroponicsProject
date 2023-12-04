#include "Menu.h"
#include "DisplayManager.h"
#include "splashPage.h"
#include "Menus.h"
#include "outputs.h"
Menu* _manualMenu;

void goBackSelected()
{
	switchToNavigationMenu();
}

void lightOnSelected() 
{
	setLight(1);
}

void lightOffSelected() 
{
	setLight(0);
}

void pumpOnSelected() 
{
	setPump(1);
}

void pumpOffSelected() 
{
	setPump(0);
}

MenuItem manualItems[] = { 
	MenuItem("Light ON", lightOnSelected),
	MenuItem("Light OFF", lightOffSelected),
	MenuItem("Pump ON", pumpOnSelected),
	MenuItem("Pump OFF", pumpOffSelected),
	MenuItem("GO BACK TO NAV", goBackSelected),
};

void initializeManualMenu()
{	
	_manualMenu = new Menu("MANUAL CONTROL", manualItems, 5);
}

void switchToManualMenu() 
{
	_displayManager->setElement(_manualMenu);
}