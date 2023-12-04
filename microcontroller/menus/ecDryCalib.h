#pragma once

#include "DisplayElement.h"

class ecDryCalibMenu : public DisplayElement
{
private:
	void handleButton();
	
	void renderDisplay();
	
public:
	inline ~ecDryCalibMenu() {}
	;
	
	void onConstruction();
	
	void onDestruction();
	
	void onUpdate(float dt);
};

extern ecDryCalibMenu* _ecDryCalibMenu;
