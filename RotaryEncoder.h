#pragma once

#include <pico.h>
#include "pico/critical_section.h"

class RotaryEncoder
{
private:
	static int rotations;
	static uint32_t lastPulse; //debouncing
	
	static int roundNearest4(int input);
	
	critical_section_t critical;
	
	static void pioIrqHandler();
	
public:
	RotaryEncoder(uint rotary_encoder_A, uint rotary_encoder_B);
	
	void setRotations(int rotations);
	
	int getRotations();
};