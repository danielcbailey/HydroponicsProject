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
#include "sensors.h"

std::time_t msgTime;

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

void sensorReader(cJSON* params, int reqID) 
{
	cJSON* resp = cJSON_CreateObject();
	cJSON_AddStringToObject(resp, "jsonrpc", "2.0");
	cJSON_AddNumberToObject(resp, "id", reqID);
	cJSON_AddObjectToObject(resp, "result");
	cJSON* resultObj = cJSON_GetObjectItem(resp, "result");

	//add the sensor readings to the result object 
	cJSON_AddNumberToObject(resultObj, "water_level", getWaterLevel());
	cJSON_AddNumberToObject(resultObj, "cumulative_liquid_flow", 0);
	cJSON_AddNumberToObject(resultObj, "current_liquid_flow", 0);
	cJSON_AddNumberToObject(resultObj, "light_intensity", getLightLevel());
	cJSON_AddNumberToObject(resultObj, "pH", getpH());
	cJSON_AddNumberToObject(resultObj, "electrical_conductivity", getEC());
	cJSON_AddNumberToObject(resultObj, "air_temperature", getAirTemp());
	cJSON_AddNumberToObject(resultObj, "air_relative_humidity", getAirHumidity());
	cJSON_AddNumberToObject(resultObj, "air_co2_ppm", (int)getAirCO2());
	
	
	char* returnStr = cJSON_PrintUnformatted(resp);
	printf("%s\n", returnStr);
	free(returnStr); 
	cJSON_Delete(resp); //deallocate 
}

void manualSetPump(cJSON* params, int reqID) 
{	
	bool state = cJSON_GetObjectItem(params, "state");
	bool success;
	if (state)
	{		
		success = setPump(1);
	}
	else
	{
		success = setPump(0);
	}
	
	if (success)
	{
		sendAck(reqID);
	}
	else
	{
		sendError(reqID, ERROR_PUMP, "Water tank is lower than the minimum water level!");
	}
}

void manualSetLight(cJSON* params, int reqID) 
{	
	bool state = cJSON_GetObjectItem(params, "state");
	bool success;
	if (state)
	{
		success = setLight(1);
	}
	else
	{
		success = setLight(0);
	}
	
	if (success)
	{
		sendAck(reqID);
	}
	else
	{
		// to do return error
	}
}

typedef void(*commandHandler)(cJSON*, int);
std::unordered_map<std::string, commandHandler> commandHandlers = { 
	{"config/settime", timeHandler},
	{"config/setschedule", scheduleHandler},
	{"sensors/read", sensorReader},
	{"controls/setpump", manualSetPump},
	{"sensors/setlight", manualSetLight},	
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

// Function to convert datetime_t to Unix timestamp
std::time_t datetimeToUnixTimestamp(const datetime_t& dt) {
	struct std::tm timeinfo = { };

	timeinfo.tm_year = dt.year - 1900; // Years since 1900
	timeinfo.tm_mon = dt.month - 1; // Month (0-based)
	timeinfo.tm_mday = dt.day; // Day of the month
	timeinfo.tm_hour = dt.hour; // Hours
	timeinfo.tm_min = dt.min; // Minutes
	timeinfo.tm_sec = dt.sec; // Seconds

	// Convert to Unix timestamp using mktime
	return std::mktime(&timeinfo);
}

datetime_t getCurrentTime()
{
	datetime_t currentTime;
	currentTime.month = 12;
	rtc_get_datetime(&currentTime);
	return currentTime;
}
bool checkConnectionTimeout()
{
	datetime_t currentTime = getCurrentTime();
	std::time_t currTime = datetimeToUnixTimestamp(currentTime);
	// Check if the latest server message was received in the last minute 
	return (currTime - msgTime) <= 60;
}
void readMessage(std::string testJSON)
{
	//start the timer to calculate the last time
	datetime_t messageTime;
	rtc_get_datetime(&messageTime);
	msgTime = datetimeToUnixTimestamp(messageTime);
	
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
