#include "schedule.h"
#include "stdio.h"
#include <iostream>

gardenEvent getScheduleEvent(datetime_t currTime)
{
	
}

gardenEvent getNextEvent(datetime_t currTime)
{
	
}

bool updateSchedule(std::list <gardenEvent>newEvents)
{
	printf("update schedule beginning \n");
	for (const auto& event : newEvents) {
		printf("Event time %d \n", event.eventTime);
		printf("Event type %d \n", event.eventType);
		}
	
	printf("update schedule end \n ");
	return true;
}