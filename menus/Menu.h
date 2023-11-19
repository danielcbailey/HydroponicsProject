#pragma once

#include "DisplayElement.h"
#include <string>
#include <functional>

typedef std::function<void()> MenuCallback;

class MenuItem
{
public:
	std::string text;
	MenuCallback callback;
	
	inline MenuItem(std::string text, MenuCallback cb)
	{
		this->text = text;
		this->callback = cb;
	}
};

class Menu: public DisplayElement
{
private:
	std::string title;
	MenuItem* items;
	int numItems;
	std::string extraText;
	int selection;
	int renderedSelection;
	int renderedRow;
	
	void renderDisplay();
	
	void handleButton();
	
public:
	
	inline Menu(std::string title, MenuItem items[], int numItems)
	{
		this->title = title;
		this->items = items;
		this->numItems = numItems;
		this->extraText = "";
		this->selection = 0;
		this->renderedSelection = 0;
		this->renderedRow = 0;
	}
	
	inline Menu(std::string title, MenuItem items[], int numItems, std::string extraText)
	{
		this->title = title;
		this->items = items;
		this->numItems = numItems;
		this->extraText = extraText;
		this->selection = 0;
		this->renderedSelection = 0;
		this->renderedRow = 0;
	}
	
	void onConstruction();
	
	void onDestruction();
	
	void onUpdate(float dt);
};