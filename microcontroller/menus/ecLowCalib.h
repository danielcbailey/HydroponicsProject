#pragma once

#include "DisplayElement.h"

class ecLowCalibMenu : public DisplayElement
{
private:
	void handleButton();
	
	void renderDisplay();
	
public:
	inline ~ecLowCalibMenu() {}
	;
	
	void onConstruction();
	
	void onDestruction();
	
	void onUpdate(float dt);
};

extern ecLowCalibMenu* _ecLowCalibMenu;
