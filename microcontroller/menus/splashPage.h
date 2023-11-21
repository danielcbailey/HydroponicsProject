#pragma once

#include "DisplayElement.h"

class SplashPage : public DisplayElement
{
private:
	void handleButton();
	
	void renderDisplay();
	
public:
	inline ~SplashPage() {};
	
	void onConstruction();
	
	void onDestruction();
	
	void onUpdate(float dt);
};

extern SplashPage* _splashPage;