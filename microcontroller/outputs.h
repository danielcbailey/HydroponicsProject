#pragma once

#define MIN_WATER_LEVEL 2
#define MAX_TEMPERATURE 90

void outputsInit(); //initialize the pins for pump and light

bool setLight(bool on);
//if it is higher than the MAX_TEMPERATURE than the light won't turn on even if the user wants to
bool setPump(bool on);
//if it is less than the MIN_WATER_LEVEL than the pump won't turn on even if the user wants to