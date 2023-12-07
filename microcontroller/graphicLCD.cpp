#include "hardwareSetup.h"
#include <pico.h>
#include <hardware/gpio.h>
#include <pico/time.h>
#include "graphicLCD.h"
#include "font.h"
#include <cstdio>
#include <stdarg.h>

bool GraphicLCD::posIsLeft = true;
char GraphicLCD::textX = 0;
char GraphicLCD::textY = 0;
bool GraphicLCD::invertBitmaps = false;
alarm_pool_t* GraphicLCD::pool = nullptr;
alarm_id_t GraphicLCD::alarmId = -1;
bool GraphicLCD::isDim = false;
bool GraphicLCD::isOn = true;
critical_section_t GraphicLCD::critical;

void GraphicLCD::init()
{
	//Initializing GPIO
	gpio_init(LCD_BACKLIGHT);
	gpio_init(LCD_CS1A);
	gpio_init(LCD_CS1B);
	gpio_init(LCD_E);
	gpio_init(LCD_RESET);
	gpio_init(LCD_RS);
	gpio_init(LCD_SERIAL_CLOCK);
	gpio_init(LCD_SERIAL_DATA);
	gpio_init(LCD_SERIAL_READY);
	
	gpio_set_dir(LCD_BACKLIGHT, true); //true is out
	gpio_set_dir(LCD_CS1A, true);
	gpio_set_dir(LCD_CS1B, true);
	gpio_set_dir(LCD_E, true);
	gpio_set_dir(LCD_RESET, true);
	gpio_set_dir(LCD_RS, true);
	gpio_set_dir(LCD_SERIAL_CLOCK, true);
	gpio_set_dir(LCD_SERIAL_DATA, true);
	gpio_set_dir(LCD_SERIAL_READY, true);
	
	gpio_put(LCD_BACKLIGHT, false);
	gpio_put(LCD_CS1A, false);
	gpio_put(LCD_CS1B, false);
	gpio_put(LCD_E, true);
	gpio_put(LCD_RESET, false);
	gpio_put(LCD_RS, false);
	gpio_put(LCD_SERIAL_CLOCK, false);
	gpio_put(LCD_SERIAL_DATA, false);
	gpio_put(LCD_SERIAL_READY, false);
	
	gpio_put(LCD_RESET, true); //Reset is active low
	
	turnOn();
	clear();
	setPos(0, 0);
	setBacklight(true);
	
	critical_section_init(&critical);
}

void GraphicLCD::sendCommand(bool isLeft, bool rs, char data)
{
	if (!isLeft)
	{
		gpio_put(LCD_CS1A, true);
		gpio_put(LCD_CS1B, false);
		busy_wait_us(1);
	}
	else
	{
		gpio_put(LCD_CS1A, false);
		gpio_put(LCD_CS1B, true);
		busy_wait_us(1);
	}
	
	gpio_put(LCD_RS, rs);
	
	//sending data as serial
	for (int i = 0 ; 8 > i ; i++)
	{
		char bit = data & 0x1;
		gpio_put(LCD_SERIAL_DATA, bit == 0x1);
		busy_wait_us(1);
		gpio_put(LCD_SERIAL_CLOCK, true);
		busy_wait_us(1);
		gpio_put(LCD_SERIAL_CLOCK, false);
		busy_wait_us(1);
		data >>= 1;
	}
	gpio_put(LCD_SERIAL_READY, true); //Positive-edge clock signal
	busy_wait_us(1);
	gpio_put(LCD_SERIAL_READY, false);
	
	busy_wait_us(1);
	
	//issuing command to display
	gpio_put(LCD_E, false);
	busy_wait_us(2);
	gpio_put(LCD_E, true);
	
	busy_wait_us(1);
	
	//cleaning up
	//gpio_put(LCD_CS1A, false);
	//gpio_put(LCD_CS1B, false);
}

void GraphicLCD::setPos(char x, char y)
{
	//y is the page/line, x is the x
	//if y >= 8, then it is for the right side of the screen
	
	sendCommand(x < 64, false, 0x40 | (x % 64));
	sendCommand(x < 64, false, 0xB8 | (y % 8));
	
	posIsLeft = x < 64;
	textX = x;
	textY = y * 8;
}

void GraphicLCD::writeRaw(char data)
{
	sendCommand(posIsLeft, true, data);
}

void GraphicLCD::turnOn()
{
	sendCommand(false , false, 0x3F);
	sendCommand(true, false, 0x3F);
	isOn = true;
}

void GraphicLCD::turnOff()
{
	sendCommand(false, false, 0x3E);
	sendCommand(true, false, 0x3E);
	isOn = false;
}

void GraphicLCD::setBacklight(bool on)
{
	gpio_put(LCD_BACKLIGHT, on);
	isDim = !on;
}

void GraphicLCD::clear()
{
	for (int page = 0; 8 > page; page++)
	{
		setPos(0, page);
		for (int i = 0; 64 > i; i++)
		{
			writeRaw(0);
		}
	}
	for (int page = 0; 8 > page; page++)
	{
		setPos(64, page);
		for (int i = 0; 64 > i; i++)
		{
			writeRaw(0);
		}
	}
}

void GraphicLCD::writeBitmap(bool data[], char width, char height, char x, char y)
{
	//y must be the pixel y, not the page y.
	
	//must convert the bitmap encoding of
	//0, 1, 2, 3
	//4, 5, 6, 7
	//...
	//to the LCD format of 8 vertical pixels at once
	
	//As such, it will only write the pixels that fully fit into a byte for the screen.
	//So, a bit map must have a height as an integral multiple of 8, and any arbitrary width
	char numberOfPages = height >> 3; //height / 8
	char midXMax = x + width > 64 ? 64 : x + width;
	
	if (x < 64)
	{
		for (char p = y >> 3; numberOfPages + (y >> 3) > p; p++)
		{
			setPos(x, p);
			for (char xs = x; midXMax > xs; xs++)
			{
				char temp = 0;
				
				for (char ys = p * 8; p * 8 + 8 > ys; ys++)
				{
					temp >>= 1;
					if (data[(ys - y) * width + (xs - x)] ^ invertBitmaps)
					{
						temp |= 0x80;
					}
				}
				
				writeRaw(temp);
			}
		}
	}
	
	if (x >= 64 || x + width > 64)
	{
		//will do later after initial testing of the above.
		char midXStart = x < 64 ? 64 : x;
		for(char p = y >> 3 ; numberOfPages + (y >> 3) > p ; p++)
		{
			setPos(midXStart, p);
			for (char xs = midXStart; x + width > xs; xs++)
			{
				char temp = 0;
				
				for (char ys = p * 8; p * 8 + 8 > ys; ys++)
				{
					temp >>= 1;
					if (data[(ys - y) * width + (xs - x)] ^ invertBitmaps)
					{
						temp |= 0x80;
					}
				}
				
				writeRaw(temp);
			}
		}
	}
	
}

void GraphicLCD::locate(char x, char y)
{
	setPos(x * 7, y);
}
	
void GraphicLCD::print(char* str)
{
	while (*str != 0)
	{
		char value = *str++;
		if (value == '\n' || textX > 121)
		{
			textY += 8;
			textX = 0;
			continue;
		}
		if (value < ' ' || value > '~')
		{
			continue;
		}
		
		writeBitmap((bool*) font_bitmaps[value - ' '], 7, 8, textX, textY);
		textX += 7;
	}
}

void GraphicLCD::printFullLine(char* str)
{
	while (*str != 0)
	{
		char value = *str++;
		if (value == '\n')
		{
			//Erasing remaining pixels on the line
			for(int i = textX ; 128 > i ; i++)
			{
				if (i == 64)
				{
					setPos(64, textY / 8);
				}
				
				writeRaw(0);
			}
			
			textY += 8;
			textX = 0;
			continue;
		}
		if (value < ' ' || value > '~')
		{
			continue;
		}
		
		writeBitmap((bool*) font_bitmaps[value - ' '], 7, 8, textX, textY);
		textX += 7;
		
		if (textX > 121 || *str == 0)
		{
			//Erasing remaining pixels on the line
			for(int i = textX ; 128 > i ; i++)
			{
				if (i == 64)
				{
					setPos(64, textY / 8);
				}
				
				writeRaw(0);
			}
			
			textY += 8;
			textX = 0;
		}
	}
}

void GraphicLCD::printf(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	char buf[256];

	//Generating final string
	int length = std::vsnprintf(buf, 256, fmt, args);
	
	print(buf);
	
	va_end(args);
}

void GraphicLCD::setAlarmPool(alarm_pool_t* aPool)
{
	//Will assume the alarm pool is initialized.
	pool = aPool;
	
	//starting timeout process, assuming that this function call is equivilent to reporting user activity.
	alarmId = alarm_pool_add_alarm_in_ms(pool, displayIdleDim * 1000, alarmCallback, nullptr, true);
}

int64_t GraphicLCD::alarmCallback(alarm_id_t id, void* data)
{
	if (isDim)
	{
		//turnOff();
		alarmId = -1;
		return 0;
	}
	else
	{
		setBacklight(false);
		return displayIdleOff * 1000000;
	}
}

bool GraphicLCD::reportUserActivity()
{
	if (pool == nullptr)
	{
		return false;
	}
	
	critical_section_enter_blocking(&critical);
	if (alarmId != -1)
	{
		alarm_pool_cancel_alarm(pool, alarmId);
	}
	alarmId = alarm_pool_add_alarm_in_ms(pool, displayIdleDim * 1000, alarmCallback, nullptr, true);
	critical_section_exit(&critical);
	
	bool ret = false;
	if (!isOn)
	{
		//turnOn();
		ret = true;
	}
	setBacklight(true);
	return ret;
}

void GraphicLCD::test()
{
	//setInvert(true);
	//print((char*)"_hello world!_\nuierhgiubgd\nUFTFIUYGUOTN\n637835435\n*&%%^$($^\n[]\\;',./{}|:\"<>?");
	printf("test: %0.2f", 3.14);
}