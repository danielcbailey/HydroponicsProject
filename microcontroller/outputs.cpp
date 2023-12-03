#include "outputs.h"
#include "hardwareSetup.h"
#include "hardware/gpio.h"

void outputsInit() {
	gpio_init(PUMP_CONTROL);
	gpio_init(LIGHT_CONTROL);
	//set pins as outputs
	gpio_set_dir(PUMP_CONTROL, true);
	gpio_set_dir(LIGHT_CONTROL, true);
	
	//relays are active low, 0 - on 1 - off 
	gpio_put(PUMP_CONTROL, true);
	gpio_put(LIGHT_CONTROL, true);
}

void setLight(bool on)
{
	gpio_put(LIGHT_CONTROL, !on);
}

void setPump(bool on)
{
	gpio_put(PUMP_CONTROL, !on);
}