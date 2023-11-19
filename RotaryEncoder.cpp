#include "RotaryEncoder.h"
#include <hardware/irq.h>
#include <hardware/pio.h>
#include <pico/stdio.h>
#include <pico/stdlib.h>

#include "rotaryEncoder.pio.h"
#include "graphicLCD.h"

int RotaryEncoder::rotations = 0;
uint32_t RotaryEncoder::lastPulse = 0;

RotaryEncoder::RotaryEncoder(uint rotary_encoder_A, uint rotary_encoder_B)
{
	critical_section_init(&this->critical);
	
	// pio 0 is used
    PIO pio = pio0;
	// state machine 0
	uint8_t sm = 0;
	// configure the used pins as input with pull up
	pio_gpio_init(pio, rotary_encoder_A);
	gpio_set_pulls(rotary_encoder_A, true, false);
	pio_gpio_init(pio, rotary_encoder_B);
	gpio_set_pulls(rotary_encoder_B, true, false);
	// load the pio program into the pio memory
	uint offset = pio_add_program(pio, &pio_rotary_encoder_program);
	// make a sm config
	pio_sm_config c = pio_rotary_encoder_program_get_default_config(offset);
	// set the 'in' pins
	sm_config_set_in_pins(&c, rotary_encoder_A);
	// set shift to left: bits shifted by 'in' enter at the least
	// significant bit (LSB), no autopush
	sm_config_set_in_shift(&c, false, false, 0);
	// set the IRQ handler
	irq_set_exclusive_handler(PIO0_IRQ_0, RotaryEncoder::pioIrqHandler);
	// enable the IRQ
	irq_set_enabled(PIO0_IRQ_0, true);
	pio0_hw->inte0 = PIO_IRQ0_INTE_SM0_BITS | PIO_IRQ0_INTE_SM1_BITS;
	// init the sm.
	// Note: the program starts after the jump table -> initial_pc = 16
	pio_sm_init(pio, sm, 16, &c);
	// enable the sm
	pio_sm_set_enabled(pio, sm, true);
}

int RotaryEncoder::roundNearest4(int input)
{
	switch (input % 4)
	{
	case -3:
		return input - 1;
	case -2:
		return input - 2;
	case -1:
		return input + 1;
	case 0:
		return input;
	case 1:
		return input - 1;
	case 2:
		return input + 2;
	case 3:
		return input + 1;
	}
	
	//Not possible, but just for the compiler
	return input;
}

void RotaryEncoder::pioIrqHandler()
{
	uint32_t currentTime = to_ms_since_boot(get_absolute_time());
	if (currentTime - lastPulse < 5)
	{
		pio0_hw->irq = 3;
		return;
	}
	
	// test if irq 0 was raised
        if(pio0_hw->irq & 1)
	{
		rotations = rotations + 1;
	}
	// test if irq 1 was raised
	if(pio0_hw->irq & 2)
	{
		rotations = rotations - 1;
	}
	
	//Evaluating if the encoder is in the detent to round the rotations to a multiple of 4
	if(gpio_get(16) && gpio_get(17))
	{
		rotations = roundNearest4(rotations);
	}
	
	// clear both interrupts
	pio0_hw->irq = 3;
	lastPulse = currentTime;
	GraphicLCD::reportUserActivity();
}

void RotaryEncoder::setRotations(int _rotations)
{
	critical_section_enter_blocking(&this->critical);
	rotations = _rotations;
	critical_section_exit(&this->critical);
}

int RotaryEncoder::getRotations()
{
	return rotations;
}