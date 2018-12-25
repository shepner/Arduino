/*
FreematicsVDL.h - Library for logging OBD2 data using the
Freematics Vehicle Data Logger
http://blog.freematics.com/2014/freematics-obd-ii-adapter-programming-guide/
*/

#ifndef tdi_h
	#define tdi_h

//*************************************************************************

	#include <Arduino.h>

//*************************************************************************

	class FreematicsVDL {
		public:
			void getOBD();
			void getGPS();
			void getMotion();
			void logDataSD();
		private:
	}

//*************************************************************************
#endif
