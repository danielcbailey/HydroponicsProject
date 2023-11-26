#include "Menu.h"
#include "DisplayManager.h"
#include "splashPage.h"
#include "sensors.h"

Menu* _phMenu;

void onFirst2Selected()
{
}

void onMidPointSelected()
{
	float lastMean, slopeAcid, slopeBase, zeroOffset;
	calibratePoint('m', &lastMean, &slopeAcid, &slopeBase, &zeroOffset);
	GraphicLCD::locate(0, 6);
	GraphicLCD::printf("Last mean: %.2fF, Zero Offset: %.2fF\n", lastMean, slopeAcid);
	GraphicLCD::printf("Acid slope: %.2fF, Acid base: %.2fF\n", slopeAcid, slopeBase);
}

void onLowPointSelected()
{
	float lastMean, slopeAcid, slopeBase, zeroOffset;
	calibratePoint('l', &lastMean, &slopeAcid, &slopeBase, &zeroOffset);
	GraphicLCD::locate(0, 6);
	GraphicLCD::printf("Last mean: %.2fF, Zero Offset: %.2fF\n", lastMean, slopeAcid);
	GraphicLCD::printf("Acid slope: %.2fF, Acid base: %.2fF\n", slopeAcid, slopeBase);
}

void onHighPointSelected()
{
	float lastMean, slopeAcid, slopeBase, zeroOffset;
	calibratePoint('h', &lastMean, &slopeAcid, &slopeBase, &zeroOffset);
	GraphicLCD::locate(0, 6);
	GraphicLCD::printf("Last mean: %.2fF, Zero Offset: %.2fF\n", lastMean, slopeAcid);
	GraphicLCD::printf("Acid slope: %.2fF, Acid base: %.2fF\n", slopeAcid, slopeBase);
}

void onBack2Selected()
{
	_displayManager->setElement(_splashPage);
}

MenuItem phCalibrationItems[] = { 
	MenuItem("pH Sensor Calibration", onFirst2Selected),
	MenuItem("Sensor in mid-point", onMidPointSelected),
	MenuItem("Sensor in low-point", onLowPointSelected),
	MenuItem("Sensor in high-point", onHighPointSelected),
	MenuItem("Go Back", onBack2Selected)
};

void initializePhMenu()
{	
	_phMenu = new Menu("pH Calibration Menu", phCalibrationItems, 5);
}

void switchToPhMenu()
{
	_displayManager->setElement(_phMenu);
}
