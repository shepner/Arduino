/*
 * WiFi related functons
 */

// dependencies specific to these objects
#ifndef WiFi_h
  #include <ESP8266WiFi.h>  // https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi/src
#endif  // WiFi_h

#ifndef WIRELESS_H
  #include "wireless.h"
#endif  // WIRELESS_H

/*****************************************************************/

int Wireless::WiFi_Client_Connected (void) {
    //Serial.print ("[Wireless::WiFi_Client_Connected] Start\n");

  if (WiFi.status () == WL_CONNECTED) {  // https://www.arduino.cc/en/Reference/WiFiStatus
    //Serial.print ("[Wireless::WiFi_Client_Connected] connected\n");
    return (1);  // Connected to WiFi
  }
  
  //Serial.print ("[Wireless::WiFi_Client_Connected] not connected\n");
  return (0);  // Not connected to WiFi
}

/*****************************************************************/

void Wireless::WiFi_Client_Start (void) {
  //Serial.print ("[Wireless::WiFi_Client_Start] Start\n");

  // copy the SSID into a local var of appropriate type
  char _ssid[WiFi_ssid.length ()];
  WiFi_ssid.toCharArray (_ssid, WiFi_ssid.length ()+1);

  // copy the password into a local var of appropriate type
  char _pass[WiFi_pass.length ()];
  WiFi_pass.toCharArray (_pass, WiFi_pass.length()+1);

  // WiFi.begin () just needs to be kicked off.  After that the ESP8266 will manage the connection on its own
  WiFi.begin (_ssid, _pass);  // WiFi network
  //Serial.print ("[Wireless::WiFi_Client_Start] Connecting to SSID %s\n", _ssid);

  // while this automatically (re)establishes connection to WiFi, we are going to wait (forever) until we actually get a connection
  Wireless _W;
  while (!_W.WiFi_Client_Connected ()) {
    delay (1000);  // this needs to go away
  }
  //Serial.print ("[Wireless::WiFi_Client_Start] Connected to SSID %s\n", WiFi_ssid);
}
