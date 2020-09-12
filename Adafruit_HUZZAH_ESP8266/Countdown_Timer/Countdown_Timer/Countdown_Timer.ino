/*
 * Minimal Arduino library for sunrise and sunset time
 * https://github.com/dmkishi/Dusk2Dawn
 */

/*****************************************************************/

//#include <Arduino.h>  // dont use this

#ifndef CLOCK_H
  #include "CLOCK.h"
#endif  // CLOCK_H

// needed for DateTime
#ifndef _RTCLIB_H_
  #include <RTClib.h> // https://github.com/adafruit/RTClib
#endif  // _RTCLIB_H_

CLOCK system_clock;
DateTime Time_Current;
DateTime Time_TurnLightsOff;
// DateTime Time_Display;

#ifndef CONFIG_H
  #include "settings/config.h"  // this contains all of the global #defines
#endif  // CONFIG_H

/*****************************************************************/

void setup () {
  Serial.begin (BAUDRATE);
  while (!Serial) { ; }  // wait for serial port to connect. Needed for native USB port only
  
  pinMode (Light_GPIO, OUTPUT);  // GPIO 12
  digitalWrite (Light_GPIO, LOW);  // turn off the relay
  Time_TurnLightsOff = system_clock.now ();

  pinMode (Door1_GPIO, INPUT_PULLUP);  // GPIO 13; with internal pullup enabled
  pinMode (Door2_GPIO, INPUT_PULLUP);  // GPIO 14; with internal pullup enabled
}


/*****************************************************************/

void loop () {
  Time_Current = system_clock.now ();

  /*
  if (Time_Current.unixtime () >= Time_Display.unixtime ()) {
    Time_Display = Time_Current + 1;  // only display the time once a sec
    Serial.print ("System time: ");  system_clock.printDateTime (Time_Current);
  }
  */

  if ((digitalRead (Door1_GPIO) == 0) || (digitalRead (Door2_GPIO) == 0)) {  // 0 == open circuit; 1 == shorted to ground  
    digitalWrite (Light_GPIO, HIGH);  // turn on the relay

    Time_TurnLightsOff = Time_Current + Time_Delay;
    Serial.print ("Turn lights off at: ");  system_clock.printDateTime (Time_TurnLightsOff);
       
  } else if (Time_Current.unixtime () >= Time_TurnLightsOff.unixtime ()) {
    digitalWrite (Light_GPIO, LOW);  // turn off the relay 
  }

  // delay (1000);  // remove after testing
}

/*****************************************************************/
