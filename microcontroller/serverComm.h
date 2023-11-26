#pragma once
#include <string>


//error codes and details in JSON RPC format
#define ERROR_INVALID_METHOD 100


void sendAck(int reqID); //Send an acknowledgement json object back to server

void readMessage(std::string testJSON); //after getting every char from server, passed to here to send the JSON to correct handler
