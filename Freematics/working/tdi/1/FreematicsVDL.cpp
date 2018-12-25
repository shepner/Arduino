/*
FreematicsVDL.cpp -  Library for logging OBD2 data using the
Freematics Vehicle Data Logger
http://blog.freematics.com/2014/freematics-obd-ii-adapter-programming-guide/
*/

#include <Arduino.h>

#include "FreematicsVDL.h"

//*************************************************************************

FreematicsVDL::FreematicsVDL() {

}

//*************************************************************************

void FreematicsVDL::getOBD() {

}

//*************************************************************************

void FreematicsVDL::getGPS() {
	//#include <TinyGPS.h>
}

//*************************************************************************

int FreematicsVDL::initMotion() {
	//#include <Wire.h> //Two Wire Interface (TWI/I2C)
	//#include <MPU6050.h>

	#define STATE_ACC_READY 0x10

	int state = 0;
	Wire.begin();
	if (MPU6050_init() == 0) {
		state |= STATE_ACC_READY;  //x|=y; is the same as x=x|y;
	}

	return state;
}

//*************************************************************************

void FreematicsVDL::getMotion() {
	#include <Wire.h> //Two Wire Interface (TWI/I2C)
	#include <MPU6050.h>

	if ((state & STATE_ACC_READY)) {
accel_t_gyro_union accData;
MPU6050_readout(&accData);
dataTime = millis();
#if VERBOSE
SerialInfo.print("ACC:");
SerialInfo.print(accData.reg.x_accel_h);
SerialInfo.print('/');
SerialInfo.print(accData.reg.y_accel_h);
SerialInfo.print('/');
SerialInfo.println(accData.reg.z_accel_h);
#endif
// log x/y/z of accelerometer
logData(PID_ACC, accData.value.x_accel >> 4, accData.value.y_accel >> 4, accData.value.z_accel >> 4);
// log x/y/z of gyro meter
//logData(PID_GYRO, accData.value.x_gyro, accData.value.y_gyro, accData.value.z_gyro);
}

}

//*************************************************************************

void FreematicsVDL::logDataSD() {
	#include <SD.h> //SD card lib:  32GB max

}

//*************************************************************************

