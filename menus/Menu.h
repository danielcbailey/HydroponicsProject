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
	inline ~Menu() {};
	
	Menu(std::string title, MenuItem* items, int numItems);
	
	Menu(std::string title, MenuItem items[], int numItems, std::string extraText);
	
	void onConstruction();
	
	void onDestruction();
	
	void onUpdate(float dt);
};