#pragma once
#include <string>
#include <ctime>
#include <hardware/rtc.h>

//error codes and details in JSON RPC format
#define ERROR_INVALID_METHOD 100
#define ERROR_UNKNOWN_ERROR 101
#define ERROR_PUMP 102
#define ERROR_LIGHT 103 



void sendAck(int reqID); //Send an acknowledgement json object back to server
void sendError(int reqID, int errorCode, const char* errorMessage); 
void readMessage(std::string testJSON); //after getting every char from server, passed to here to send the JSON to correct handler

bool checkConnectionTimeout();

datetime_t getCurrentTime(); 