#pragma once

#include "DisplayElement.h"

class ecHighCalibMenu : public DisplayElement
{
private:
	void handleButton();
	
	void renderDisplay();
	
public:
	inline ~ecHighCalibMenu() {}
	;
	
	void onConstruction();
	
	void onDestruction();
	
	void onUpdate(float dt);
};

extern ecHighCalibMenu* _ecHighCalibMenu;
