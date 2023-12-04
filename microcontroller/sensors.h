#pragma once

float getpH();

float returnLastMean();

float returnslopeAcid();
float returnslopeBase();

float returnzeroOffset();

float getEC();

void startCalibration();

void startECCalibration(float stdDev);

void calibratePoint(char point, float *lastMean, float *slopeAcid, float *slopeBase, float *zeroOffset); //Calibrates to mid low or high point for pH sensor 
float getContUART(char cont, bool pH); //Get the continuous values from either pH or EC sensor
float getSingleUART(bool pH); //Gets a single value from pH or EC

bool calibrationTick(char point); //the pH calibration screen will call this
float getWaterLevel(); //In inches

float getPumpRate(); //In Gallons per minute

float getLightLevel(); //In lumens

float getAirTemp(); //In degrees fahrenheit

float getAirHumidity(); //In RH

float getAirCO2(); //In PPM

void initSensors();

float getEClastMean();

bool calibrationECTick(char point);

