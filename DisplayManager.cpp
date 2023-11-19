#include "DisplayManager.h"
#include "menus/splashPage.h"

DisplayManager* _displayManager;

void DisplayManager::dispatchTick(float dt)
{
	if (this->requestedElem != nullptr)
	{
		this->currentElem->onDestruction();
		this->currentElem = this->requestedElem;
		this->currentElem->onConstruction();
		this->requestedElem = nullptr;
	}
	
	this->currentElem->onUpdate(dt);
}
	
void DisplayManager::setElement(DisplayElement* elem)
{
	this->requestedElem = elem;
}