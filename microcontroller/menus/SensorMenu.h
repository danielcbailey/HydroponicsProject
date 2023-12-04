#pragma once

#include "DisplayElement.h"

class SensorMenu : public DisplayElement
{
private:
	void handleButton();
	
	void renderDisplay();
	
public:
	inline ~SensorMenu() {}
	;
	
	void onConstruction();
	
	void onDestruction();
	
	void onUpdate(float dt);
};

extern SensorMenu* _SensorMenu;