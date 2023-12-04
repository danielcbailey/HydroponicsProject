#include "Menu.h"
#include "DisplayManager.h"
#include "splashPage.h"
#include "Menus.h"
#include "schedule.h"

Menu* _scheduleMenu;

void goBackNavSelected()
{
	switchToNavigationMenu();
}

MenuItem test[20];

void initializeScheduleMenu()
{	
	std::list <gardenEvent>scheduleList = getAllSchedule(); 
	int size = scheduleList.size();
	int index = 0;
	for (const auto& event : scheduleList) {
		std::string eventStr;
		switch (event.eventType) {
		case NO_EVENT:   
			eventStr = "NO_EVENT";
			break;
		case PUMP_ON:   
			eventStr = "PUMP_ON";
			break;
		case PUMP_OFF:
			eventStr = "PUMP_OFF";
			break;
		case LIGHT_ON:   
			eventStr = "LIGHT_ON";
			break;
		case LIGHT_OFF:  
			eventStr = "LIGHT_OFF";
			break;
		default:    
			break;
		}
		std::string label = std::to_string(event.eventTime / 100) + ":" 
			+ (event.eventTime % 100 == 0 ? "00" : std::to_string(event.eventTime % 100))
			+ " " + eventStr;
		test[index] = MenuItem(label, goBackNavSelected);
		++index;
	}
	
	_scheduleMenu = new Menu("SCHEDULE", test, size);
}

void switchToScheduleMenu() 
{
	_displayManager->setElement(_scheduleMenu);
}