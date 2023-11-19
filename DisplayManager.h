#pragma once

#include "menus/DisplayElement.h"

class DisplayManager
{
private:
	DisplayElement* currentElem;
	DisplayElement* requestedElem;
	
public:
	inline DisplayManager(DisplayElement* elem) : currentElem(elem), requestedElem(nullptr) 
	{
		this->currentElem->onConstruction();
	}
	
	void dispatchTick(float dt);
	
	void setElement(DisplayElement* elem);
};

extern DisplayManager* _displayManager;