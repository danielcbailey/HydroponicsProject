#include "Menu.h"
#include "DisplayManager.h"
#include "splashPage.h"

Menu* _mainMenu;

void onFirstSelected ()
{
	
}

void onBackSelected()
{
	_displayManager->setElement(_splashPage);
}

MenuItem mainMenuItems[] = { 
	MenuItem("Hello Lara", onFirstSelected),
	MenuItem("This is a test", onFirstSelected),
	MenuItem("This is a test 2", onFirstSelected),
	MenuItem("This is a test 3", onFirstSelected),
	MenuItem("This is a test 4", onFirstSelected),
	MenuItem("This is a test 5", onFirstSelected),
	MenuItem("This is a test 6", onFirstSelected),
	MenuItem("This is a test 7", onFirstSelected),
	MenuItem("This is a test 8", onFirstSelected),
	MenuItem("Go Back", onBackSelected)
};

void initializeMainMenu()
{	
	_mainMenu = new Menu("Hydroponics Menu", mainMenuItems, 10);
}

void switchToMainMenu()
{
	_displayManager->setElement(_mainMenu);
}