/*
VW TDI data logger
Collect OBD2, GPS, and accelerometer data
log to file
*/

#include <Arduino.h>
//#include <Wire.h>  //Two Wire Interface (TWI/I2C)
//#include <SPI.h>  //Serial Peripheral Interface (SPI) Bus

#include "config.h"
//#include "FreematicsVDL.h"

//*************************************************************************

//FreematicsVDL logger();

void setup() {
	if (VERBOSE) { Serial.begin(BAUD); }
}

void loop() {
	unsigned long time = millis();  //get time

	if (VERBOSE) {
		Serial.print("this is a test\n");
		Serial.println("");
	}





	//logger.getOBD();
	//logger.getGPS();
	//logger.getMotion();
	//logger.logDataSD();

	time = millis() - time;  //get time elapsed
	if (time < MIN_DELAY) { delay(MIN_DELAY - time); }  //Wait the diff
}



