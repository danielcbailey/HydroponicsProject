#pragma once

#include "DisplayElement.h"

class phHighCalibMenu : public DisplayElement
{
private:
	void handleButton();
	
	void renderDisplay();
	
public:
	inline ~phHighCalibMenu() {}
	;
	
	void onConstruction();
	
	void onDestruction();
	
	void onUpdate(float dt);
};

extern phHighCalibMenu* _phHighCalibMenu;
