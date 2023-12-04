#include "outputs.h"
#include "hardwareSetup.h"
#include "hardware/gpio.h"
#include "sensors.h"


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

bool setLight(bool on)
{
	if ((on && getAirTemp() < MAX_TEMPERATURE) || !on)
	{
		//temperature should be lower than the max temperature set for the light to turn on 
		// however it could be turned off at any time 
		gpio_put(LIGHT_CONTROL, !on);
		return true;
	}
	else
	{
		return false;
	}
}

bool setPump(bool on)
{
	if ((on && getWaterLevel() >= MIN_WATER_LEVEL) || !on)
	{
		//water level should be higher than the min level if the user wants to turn it on
		// however the pump can be turned off any time  
		gpio_put(PUMP_CONTROL, !on);
		return true;
	}
	else
	{
		return false;
	}
	
}