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
	cJSON *scheduleArr = cJSON_GetObjectItem(params, "schedule");
	
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
		sendAck(reqID);
	}
}
typedef void(*commandHandler)(cJSON*, int);
std::unordered_map<std::string, commandHandler> commandHandlers = { 
	{"config/settime", timeHandler},
	{"config/setschedule", scheduleHandler}
};

void sendAck(int reqID) 
{
	cJSON* ack = cJSON_CreateObject();
	cJSON_AddStringToObject(ack, "jsonrpc", "2.0");
	cJSON_AddNumberToObject(ack, "id", reqID);
	cJSON_AddObjectToObject(ack, "result");
	char* returnStr = cJSON_PrintUnformatted(ack);
	printf("%s\n", returnStr);

	free(returnStr); 
	cJSON_Delete(ack); //deallocate 

}

void sendError(int reqID, int errorCode, const char* errorMessage)
{
	cJSON* nAck = cJSON_CreateObject();
	cJSON_AddStringToObject(nAck, "jsonrpc", "2.0");
	cJSON_AddNumberToObject(nAck, "id", reqID);
	cJSON_AddObjectToObject(nAck, "error");
	cJSON* errorObj = cJSON_GetObjectItem(nAck, "error");
	cJSON_AddNumberToObject(errorObj, "code", errorCode);
	cJSON_AddStringToObject(errorObj, "message", errorMessage);

	char* returnStr = cJSON_PrintUnformatted(nAck);
	printf("%s\n", returnStr);
	cJSON_Delete(nAck);
}

void readMessage(std::string testJSON)
{
	cJSON *jsonRoot = cJSON_Parse(testJSON.c_str());
	cJSON *jsonRPC = cJSON_GetObjectItem(jsonRoot, "jsonrpc"); //"2.0"
	cJSON *method = cJSON_GetObjectItem(jsonRoot, "method"); // "some/remote/procedure"
	cJSON* id = cJSON_GetObjectItem(jsonRoot, "id");

	if (commandHandlers.count(method->valuestring) == 0)
	{
		sendError(id->valueint, ERROR_INVALID_METHOD, "Method is not recognized!");
		cJSON_Delete(jsonRoot);
		return;
	}

	commandHandler handler = commandHandlers.at(method->valuestring);

	cJSON *params = cJSON_GetObjectItem(jsonRoot, "params");
	handler(params, id->valueint);

	cJSON_Delete(jsonRoot);
}


void makeEventObj(std::string eventType, int eventTime)
{
	
}
