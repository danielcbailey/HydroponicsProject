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
#include "outputs.h"
#include <ctime>

void timeHandler(cJSON* params, int reqID)
{
	datetime_t picoTime;
	rtc_get_datetime(&picoTime);
	
	std::time_t newTime = cJSON_GetObjectItem(params, "unixtime")->valueint;
	
	// Convert Unix time to a tm structure
	std::tm* timeinfo = std::localtime(&newTime);
	int16_t year = timeinfo->tm_year + 1900; // tm_year is years since 1900
	int8_t month = timeinfo->tm_mon + 1; // tm_mon is 0-based
	int8_t day = timeinfo->tm_mday;
	int8_t hour = timeinfo->tm_hour;
	int8_t minute = timeinfo->tm_min;
	int8_t second = timeinfo->tm_sec;
	int8_t dayOfWeek = timeinfo->tm_wday; 
	
	datetime_t timeToSet = { 
		year,
		month,
		day,
		dayOfWeek,
		hour,
		minute, 
		second 
	};
	rtc_set_datetime(&timeToSet);
	
	sendAck(reqID);
	
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
	
	//after the update check if there are any events the controller needs to catch up 
	datetime_t currTime;
	rtc_get_datetime(&currTime);
	currState stateNow = getStateAtTime(currTime);
	setLight(stateNow.light);
	setPump(stateNow.pump);

	if (success)
	{
		sendAck(reqID);
	}
	else
	{
		sendError(reqID, ERROR_UNKNOWN_ERROR, "Update Schedule was not successful!");
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
