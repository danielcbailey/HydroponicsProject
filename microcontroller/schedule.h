#pragma once
#include "hardware/rtc.h"
#include <list>

//external API of schedule for other parts

enum eventTypes
{
	NO_EVENT, PUMP_ON, PUMP_OFF, LIGHT_ON, LIGHT_OFF
};
typedef struct {
	int eventTime;
	eventTypes eventType; 
	
} gardenEvent;

typedef struct {
	bool light;
	bool pump;
} currState;

gardenEvent getScheduleEvent(datetime_t currTime); //get the event to perform now 

gardenEvent getNextEvent(datetime_t currTime); //for displaying the next event and remaining time on the screen


bool updateSchedule(std::list <gardenEvent>newEvents); //updates the schedule based on the new schedule sent by the server 
//returns a bool if updated successfully 