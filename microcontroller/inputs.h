#pragma once

#include "RotaryEncoder.h"
#include <functional>

typedef std::function<void()> ButtonCallback;

extern RotaryEncoder* _rotaryEncoder;

void init_inputs();

void setButtonCallback(ButtonCallback cb);

inline void defaultButtonCallback()
{
	//Does nothing.
}