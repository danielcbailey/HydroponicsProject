#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>
#include <string>
#include "serverComm.h"
#include <stdio.h>
#include <unordered_map>
#include <list>
#include "schedule.h"
#include "string.h"

void timeHandler(cJSON* params, int reqID)
{
	printf("TIME HANDLER WOO HOOO (id %d)\n", reqID);
}

void scheduleHandler(cJSON* params, int reqID) 
{
	printf("SCHEDULE HANDLER WOO HOOO (id %d)\n", reqID);
	
	cJSON *scheduleArr = cJSON_GetObjectItem(params, "schedule");
	printf("stupid json schedule \n");
	
	//printf(cJSON_Print(schedule));
	std::list <gardenEvent>eventsList;
	cJSON *eventJSON, *eventType, *eventTime;
	gardenEvent newEvent;
	for (int i = 0; i < cJSON_GetArraySize(scheduleArr); i++) 
	{
		eventJSON = cJSON_GetArrayItem(scheduleArr, i);
		eventType = cJSON_GetObjectItem(eventJSON, "event_type");
		eventTime = cJSON_GetObjectItem(eventJSON, "event_time");

		//getting event type 
		eventTypes newEnum;
		if (strcmp(eventType->valuestring, "PUMP_ON") == 0)
		{
			newEnum = PUMP_ON;
		}
		else if (strcmp(eventType->valuestring, "PUMP_OFF") == 0)
		{
			newEnum = PUMP_OFF;
		} 
		else if (strcmp(eventType->valuestring, "LIGHT_ON") == 0)
		{
			newEnum = LIGHT_ON;
		}
		else if (strcmp(eventType->valuestring, "LIGHT_OFF") == 0)
		{
			newEnum = LIGHT_OFF;
		}
		newEvent.eventType = newEnum;
		newEvent.eventTime = eventTime->valueint;
		eventsList.push_back(newEvent);
		
	}
	bool success = updateSchedule(eventsList);
	if (success)
	{
		printf("yay!");
		sendAck(reqID);
	}
}
typedef void(*commandHandler)(cJSON*, int);
std::unordered_map<std::string, commandHandler> commandHandlers = { 
	{"config/settime", timeHandler},
	{"config/setschedule", scheduleHandler}
};

void pumpOn(cJSON* event, int reqID)
{
	
}
void pumpOff(cJSON* event, int reqID)
{
	
}

void lightOn(cJSON* event, int reqID)
{
	
}
void lightOff(cJSON* event, int reqID)
{
	
}



void sendAck(int reqID) 
{
	
}

void readMessage(std::string testJSON)
{
	printf("readMessage func %s\n", testJSON.c_str());
	cJSON *test = cJSON_Parse(testJSON.c_str());
	cJSON *jsonRPC = cJSON_GetObjectItem(test, "jsonrpc"); //"2.0"
	cJSON *method = cJSON_GetObjectItem(test, "method"); // "some/remote/procedure"
	if (commandHandlers.count(method->valuestring) == 0)
	{
		printf("command not found! TODO: make this a proper response error!\n");
		cJSON_Delete(method);
		cJSON_Delete(test);
		cJSON_Delete(jsonRPC);
		return;
	}
	commandHandler handler = commandHandlers.at(method->valuestring);

	cJSON* id = cJSON_GetObjectItem(test, "id");
	cJSON *params = cJSON_GetObjectItem(test, "params");
	handler(params, id->valueint);
	
}


void test()
{

	
	
}
void makeEventObj(std::string eventType, int eventTime)
{
	
}
void setSchedule()
{
	cJSON *schedule = cJSON_CreateObject(); 
	cJSON_AddStringToObject(schedule, "jsonrpc", "2.0"); 
	cJSON_AddStringToObject(schedule, "method", "config/setschedule"); 
	
	
	cJSON *scheduleArr = cJSON_CreateArray();
	
	//cJSON *eventOne = makeEventObj("PUMP_ON")
		
		cJSON_CreateObject();
	//cJSON_AddStringToObject(eventOne, "event_type", "2.0"); 
	//cJSON_AddNumberToObject(eventOne, "event_time", 500); 
	//cJSON *eventTwo = cJSON_CreateObject();

	
	
	cJSON_AddArrayToObject(schedule, "schedule");
	cJSON_AddNumberToObject(schedule, "id", 1); 
	
	
}