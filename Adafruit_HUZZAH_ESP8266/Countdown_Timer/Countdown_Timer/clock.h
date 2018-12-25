/*
 * interact with the system clock
 */

/*****************************************************************/

#ifndef CLOCK_H
  #define CLOCK_H

  // needed to talk to the RTC via I2C
  #ifndef TwoWire_h
    #include <Wire.h> // https://github.com/esp8266/Arduino/tree/master/libraries/Wire
  #endif  // TwoWire_h

  // needed to interface with the RTC itself
  #ifndef _RTCLIB_H_
    #include <RTClib.h> // https://github.com/adafruit/RTClib
  #endif  // _RTCLIB_H_

  /*****************************************************************/

  class CLOCK {
    private:
      // accessible only from within other members of the same class (or from their "friends")

    protected:
      // accessible from other members of the same class (or from their "friends"), 
      // but also from members of their derived classes

    public:
      // accessible from anywhere where the object is visible

      void printDateTime(DateTime dt);
      DateTime now (void);
  };

  /*****************************************************************/

#endif  // CLOCK_H
