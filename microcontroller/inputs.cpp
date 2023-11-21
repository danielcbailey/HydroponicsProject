#include "inputs.h"
#include <pico.h>
#include <hardware/gpio.h>
#include <pico/stdlib.h>
#include "graphicLCD.h"

RotaryEncoder* _rotaryEncoder;

uint32_t lastButtonClick = 0;
ButtonCallback buttonCallback;

void buttonClick(uint gpio, uint32_t events)
{
	uint32_t currentTime = to_ms_since_boot(get_absolute_time());
	
	if (currentTime - lastButtonClick < 50)
	{
		return; //Debouncing
	}
	
	bool temp = GraphicLCD::reportUserActivity();
	lastButtonClick = currentTime;
	
	buttonCallback();
}

void setButtonCallback(ButtonCallback cb)
{
	buttonCallback = cb;
}

void init_inputs()
{
	_rotaryEncoder = new RotaryEncoder(16, 17);
	
	gpio_init(22);
	gpio_set_dir(22, false);
	gpio_set_irq_enabled_with_callback(22, 0x8, true, buttonClick);
}