
#include "DisplayElement.h"

class phLowCalibMenu : public DisplayElement
{
private:
	void handleButton();
	
	void renderDisplay();
	
public:
	inline ~phLowCalibMenu() {}
	;
	
	void onConstruction();
	
	void onDestruction();
	
	void onUpdate(float dt);
};

extern phLowCalibMenu* _phLowCalibMenu;
