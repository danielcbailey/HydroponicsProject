#include "Menu.h"
#include "graphicLCD.h"
#include "inputs.h"
#include <functional>

Menu::Menu(std::string title, MenuItem* items, int numItems)
{
	this->title = title;
	this->items = items;
	this->numItems = numItems;
	this->extraText = "";
	this->selection = 0;
	this->renderedSelection = 0;
	this->renderedRow = 0;
}
	
Menu::Menu(std::string title, MenuItem items[], int numItems, std::string extraText)
{
	this->title = title;
	this->items = items;
	this->numItems = numItems;
	this->extraText = extraText;
	this->selection = 0;
	this->renderedSelection = 0;
	this->renderedRow = 0;
}

void Menu::onConstruction()
{
	//Setting up on the screen
	GraphicLCD::clear();
	this->renderDisplay();
	setButtonCallback(std::bind(&Menu::handleButton, this));
}

void Menu::handleButton()
{
	this->items[this->selection].callback();
}

void Menu::onDestruction()
{
	setButtonCallback(std::bind(&defaultButtonCallback));
}

void Menu::onUpdate(float dt)
{
	//Read value of rotary encoder
	int temp = _rotaryEncoder->getRotations() / 4;
	//if (temp == this->selection)
	//{
	//	return;
	//}
	
	this->selection = temp;
	
	if (this->selection < 0)
	{
		_rotaryEncoder->setRotations(0);
		this->selection = 0;
	}
	else if (this->selection >= this->numItems)
	{
		_rotaryEncoder->setRotations(this->numItems * 4 - 4);
		this->selection = this->numItems - 1;
	}
	
	this->renderDisplay();
}

void Menu::renderDisplay()
{	
	/*
	 * A menu looks like the following:
	 * 
	 *      A title
	 * This is some extra
	 * text to describe
	 * something.
	 * > Back
	 * > Calibrate PH
	 * > Calibrate EC
	 **/
	
	//Rendering title
	GraphicLCD::locate(0, 0);
	int startX = (18 - this->title.length()) / 2;
	GraphicLCD::print((char *) std::string(startX, ' ').c_str());
	GraphicLCD::locate(startX, 0);
	GraphicLCD::printFullLine((char*) this->title.c_str());
	GraphicLCD::locate(0, 1);
	
	//If there is extraText, then render it immediately below the title
	if(this->extraText != "")
	{
		GraphicLCD::printFullLine((char*) this->extraText.c_str());
	}
	
	//Calculating which items get rendered by specifying the number to render and the first one to render.
	int numRows = 8 - GraphicLCD::getTextY();
	int numToRender = this->numItems > numRows ? numRows : this->numItems;
	int firstToRender;
	
	if (selection == 0 || this->numItems <= numRows)
	{
		//First to render must be the 0th item
		firstToRender = 0;
		this->renderedRow = 0;
	}
	else if (selection == this->numItems - 1)
	{
		//First to render must be such that the last item
		firstToRender = this->numItems - numToRender;
		this->renderedRow = numRows - 1;
	}
	else
	{
		//Just going to keep in in the center unless it hits an endstop
		int target = this->selection - numRows / 2;
		
		if (target < 0)
		{
			//Hit the upper endstop
			firstToRender = 0;
		}
		else if (target > this->numItems - numRows)
		{
			//Hit the lower endstop
			firstToRender = this->numItems - numRows;
		}
		else
		{
			//No endstop, nice and easy..
			firstToRender = target;
		}
	}
	
	//Rendering
	for(int i = firstToRender ; numToRender + firstToRender > i ; i++)
	{
		if (i == this->selection)
		{
			GraphicLCD::setInvert(true);
			GraphicLCD::printFullLine((char*) this->items[i].text.c_str());
			GraphicLCD::setInvert(false);
		}
		else
		{
			GraphicLCD::printFullLine((char*) this->items[i].text.c_str());
		}
	}
	
	this->renderedSelection = this->selection;
}