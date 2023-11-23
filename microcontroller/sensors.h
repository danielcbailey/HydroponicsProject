#pragma once

float getpH();

float getEC();
void calibratePH(); //Calibration procedure 
void calibratePoint(char point, float *lastMean, float *slopeAcid, float *slopeBase, float *zeroOffset); //Calibrates to mid low or high point for pH sensor 
float getContUART(char cont, bool pH); //Get the continuous values from either pH or EC sensor
float getSingleUART(bool pH); //Gets a single value from pH or EC
float getWaterLevel(); //In inches

float getPumpRate(); //In Gallons per minute

float getLightLevel(); //In lumens

float getAirTemp(); //In degrees fahrenheit

float getAirHumidity(); //In RH

float getAirCO2(); //In PPM

void initSensors();