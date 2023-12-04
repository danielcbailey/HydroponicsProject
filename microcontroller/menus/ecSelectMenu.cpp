#include "Menu.h"
#include "DisplayManager.h"
#include "splashPage.h"
#include "Menus.h"

Menu* _ecSelect;

void onBackNavSelected()
{
	switchToNavigationMenu();
}

void noFunction()
{
	
}

void dryCalib()
{
	switchToECDryCalibration();
}
void lowCalib()
{
	switchToECLowCalibration();
}

void highCalib()
{
	switchToECHighCalibration();

}

MenuItem ecSelectItems[] = { 
	MenuItem("Place probe in the", noFunction),
	MenuItem("solution and click", noFunction),
	MenuItem("the option below", noFunction),
	MenuItem("1 -Dry (NO SOL)", dryCalib),
	MenuItem("2 - 12,880uS", lowCalib),
	MenuItem("3 - 80,000uS", highCalib),
	MenuItem("GO BACK TO NAV", onBackNavSelected),
};

void initializeECSelectMenu()
{	
	_ecSelect = new Menu("EC CALIBRATION", ecSelectItems, 7);
}

void switchToECSelectMenu() 
{
	_displayManager->setElement(_ecSelect);
}