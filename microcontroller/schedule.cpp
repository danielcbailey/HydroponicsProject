#include "schedule.h"
#include "stdio.h"
#include <iostream>
#include <list>


//initialize the schedule to a default before server sends another schedule to update 
std::list <gardenEvent>gardenSchedule = {
	{500, PUMP_ON},
	{510, PUMP_OFF},
	{600, LIGHT_ON},
	{1100, PUMP_ON},
	{1110, PUMP_OFF},
	{1700, PUMP_ON},
	{1710, PUMP_OFF},
	{2000, LIGHT_OFF},
	{2300, PUMP_ON},
	{2310, PUMP_OFF},
};

int translateTime(datetime_t timeGiven) { //change datetime_t object into the int HHMM
	return (timeGiven.hour * 100 + timeGiven.min);
}

gardenEvent getScheduleEvent(datetime_t currTime)
{
	int time = translateTime(currTime);

	std::list<gardenEvent>::reverse_iterator revit;
	for (revit = gardenSchedule.rbegin(); revit != gardenSchedule.rend(); revit++) {
		if (revit->eventTime <= time) {
			return *revit;
		}
	}
	//if there is no current event to do return a no_event struct 
	gardenEvent noEvent;
	noEvent.eventType = NO_EVENT;
	noEvent.eventTime = time;
	return noEvent;
}

gardenEvent getNextEvent(datetime_t currTime)
{
	int time = translateTime(currTime);
	for (const auto& e : gardenSchedule) {
		if (e.eventTime > time) {
			return e; //the next event is returned
		}
	}
	//if there is no remaining events then return a no_event
	gardenEvent noEvent;
	noEvent.eventType = NO_EVENT;
	noEvent.eventTime = time;
	return noEvent;
}


currState getStateAtTime(datetime_t currTime) //If after updating the schedule and/or time, if there is any missing event this function will find it
{
	int time = translateTime(currTime);
	currState currentState = { 0, 0 };
	for (const auto& e : gardenSchedule) {
		if (e.eventTime > time) {
			break;
		}
		switch (e.eventType) {
		case PUMP_ON:
			currentState.pump = 1;
			break;
		case PUMP_OFF:
			currentState.pump = 0;
			break;
		case LIGHT_ON:
			currentState.light = 1;
			break;
		case LIGHT_OFF:
			currentState.light = 0;
			break;
		}
	}
	return currentState;
}

bool updateSchedule(std::list <gardenEvent>newEvents)
{
	gardenSchedule = newEvents;
	return true;
}