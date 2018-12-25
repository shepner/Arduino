/*
 * fetch the current time from a NTP server
 */

/*****************************************************************/

#ifndef NTP_H
  #define NTP_H

  #ifndef WiFi_h
    #include <ESP8266WiFi.h>  // https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi/src
  #endif  // WiFi_h

  #ifndef WIFIUDP_H
    #include <WiFiUdp.h>  // https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi/src
  #endif  // WIFIUDP_H

  #ifndef CLOCK_H
    #include "CLOCK.h"
  #endif  // CLOCK_H

  /*****************************************************************/

  class NTP {
    private:
      // accessible only from within other members of the same class (or from their "friends")
      
      WiFiUDP udp;  // A UDP instance to let us send and receive packets over UDP
      
      CLOCK system_clock;

      int CommunicateUDP (IPAddress& address);

    protected:
      // accessible from other members of the same class (or from their "friends"), 
      // but also from members of their derived classes

    public:
      // accessible from anywhere where the object is visible
      
      static const int packetSize = 48;  // NTP time stamp is in the first 48 bytes of the message
      byte packetBuffer[packetSize];  // buffer to hold incoming and outgoing packets

      //Your UTC Time Zone Differance
      int HH = 0;
      int MM = 0;
      
      unsigned long getNTPTime (IPAddress& address);
      unsigned long NTPtoUNIXtime (unsigned long NTPtime);
      int ntpdate (IPAddress& address);
  };

  /*****************************************************************/

#endif  // NTP_H
