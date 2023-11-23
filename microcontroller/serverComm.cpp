#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>
#include <string>
#include "serverComm.h"
#include <stdio.h>
#include <unordered_map>

void timeHandler(cJSON* params, int reqID)
{
	printf("TIME HANDLER WOO HOOO (id %d)\n", reqID);
}

typedef void(*commandHandler)(cJSON*, int);

std::unordered_map<std::string, commandHandler> commandHandlers = { 
	{"config/settime", timeHandler}	
};

void sendAck() 
{
	
}

void readMessage(std::string testJSON)
{
	printf("readMessage func %s\n", testJSON.c_str());
	cJSON *test = cJSON_Parse(testJSON.c_str());
	cJSON *jsonRPC = cJSON_GetObjectItem(test, "jsonrpc"); //"2.0"
	printf(cJSON_Print(jsonRPC));
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
	
	cJSON *schedule = cJSON_GetObjectItem(params, "schedule");
	printf("stupid json schedule \n");
	
	printf(cJSON_Print(schedule));

	cJSON *second = cJSON_GetArrayItem(schedule, 1);
	printf("item \n");
	printf(cJSON_Print(second));
	printf(cJSON_Print(id));
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