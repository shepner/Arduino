/*
 * fetch the current time from a NTP server
 * https://circuits4you.com/2018/01/02/esp8266-nodemcu-ntp-time-clock/
 * 
 */

/*****************************************************************/

#ifndef CLOCK_H
  #include "CLOCK.h"
#endif  // CLOCK_H

/*****************************************************************/

void CLOCK::printDateTime(DateTime dt) {
   Serial.print(dt.year(), DEC);
   Serial.print('/');
   Serial.print(dt.month(), DEC);
   Serial.print('/');
   Serial.print(dt.day(), DEC);
   Serial.print(" ");
   Serial.print(dt.hour(), DEC);
   Serial.print(':');
   Serial.print(dt.minute(), DEC);
   Serial.print(':');
   Serial.print(dt.second(), DEC);
   Serial.println();
}


DateTime CLOCK::now (void) {

  RTC_PCF8523 rtc; // we are using the PCF8523 RTC

  if (! rtc.begin ()) {
    //Serial.print ("Couldn't find RTC\n");
    return(-1);  // hardware failure?
  } else if (!rtc.initialized()) {
    return (-2);  // hardware found but not initialized?
  }
    
  //Serial.print ("RTC is running!\n");
  DateTime dt = rtc.now ();  // grab the system time  
  return (dt);
}

/*****************************************************************/
