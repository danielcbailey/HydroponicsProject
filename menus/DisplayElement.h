#pragma once

class DisplayElement
{
public:
	//Pure virtual member functions to make this into an interface
	virtual void onConstruction() = 0;
	
	virtual void onDestruction() = 0;
	
	virtual void onUpdate(float dt) = 0;
};