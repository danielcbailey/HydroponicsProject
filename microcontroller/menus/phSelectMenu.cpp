#include "Menu.h"
#include "DisplayManager.h"
#include "splashPage.h"
#include "Menus.h"

Menu* _phSelect;

void onBacktoNavSelected()
{
	switchToNavigationMenu();
}

void noFunc()
{
	
}

void midpointCalib()
{
	switchToMidCalibration();
}

void lowpointCalib()
{
	switchToLowCalibration();
}

void highpointCalib()
{
	switchToHighCalibration();
}


/*PH CALIBRATION
Place probe in the
Solution and click 
an option below
1 – mid point (7.0)
2 – low point (4.0) 
3 – high point (10.0)
GO BACK TO NAV
*/
MenuItem phSelectItems[] = { 
	MenuItem("Place probe in the", noFunc),
	MenuItem("solution and click", noFunc),
	MenuItem("the option below", noFunc),
	MenuItem("1 - MID (7.0)", midpointCalib),
	MenuItem("2 - LOW (4.0)", lowpointCalib),
	MenuItem("3 - HIGH (10.0)", highpointCalib),
	MenuItem("GO BACK TO NAV", onBacktoNavSelected),
};

void initializephSelectMenu()
{	
	_phSelect = new Menu("PH CALIBRATION", phSelectItems, 7);
}

void switchToPhSelectMenu() 
{
	_displayManager->setElement(_phSelect);
}