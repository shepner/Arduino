/*
 * Minimal Arduino library for sunrise and sunset time
 * https://github.com/dmkishi/Dusk2Dawn
 */

/*****************************************************************/

//#include <Arduino.h>  // dont use this

#ifndef WiFi_h
  #include <ESP8266WiFi.h>  // https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi/src
#endif  // WiFi_h

/*****************************************************************/

#ifndef WIRELESS_H
  #include "wireless.h"
#endif  // WIRELESS_H

#ifndef NTP_H
  #include "ntp.h"
#endif  // NTP_H

#ifndef CLOCK_H
  #include "CLOCK.h"
#endif  // CLOCK_H

// needed for DateTime
#ifndef _RTCLIB_H_
  #include <RTClib.h> // https://github.com/adafruit/RTClib
#endif  // _RTCLIB_H_

#ifndef CRON_H
  #include "cron.h"
#endif  // CRON_H

/*****************************************************************/

#ifndef CONFIG_H
  #include "settings/config.h"  // this contains all of the global values
#endif  // CONFIG_H

Wireless wireless;  // global setup for wireless communication

// global setup for NTP
NTP ntp;
IPAddress timeServerIP;

CLOCK system_clock;
DateTime Time_Current;


/*****************************************************************/

void setup () {
  Serial.begin (BAUDRATE);
  while (!Serial) { ; }  // wait for serial port to connect. Needed for native USB port only

  //Your UTC Time Zone Differance
  ntp.HH = TZHH;
  ntp.MM = TZMM;

  wireless.WiFi_ssid = SSID;
  wireless.WiFi_pass = SSID_PASSWORD;
  wireless.WiFi_Client_Start ();  // Connect to WiFi

  Time_Current = Time_Previous = system_clock.now ();
}

char * split (char * input_string, char * delimiter) {
    char * output_string;
    input_string.trim();  // needed?
    output_string = input_string.substring(0,input_string.indexOf(delimiter));
    return (output_string);
}

DateTime calculate_next_run (char[30] crontab, DateTime Time_Current) {
  // https://linux.die.net/man/5/crontab
  // minute(0-59) hour(0-23) day of month(1-31) month(1-12) day of week(0-7)
  // * * * * * user command
  // * = all
  // */2 = every 2
  // 1-5 = range 

  return (result);
}

void update_clock (DateTime Time_Current) {
  char[30] crontab = "*/1 * * * *";  // run every minute

  if (Time_Current.unixtime () >= Time_Scheduled_update_clock.unixtime ()) {

    //////////////////////////////////////////////////////////////////////////////////////////
    // scheduled job goes here
    
    if (!WiFi.hostByName (NTPServerName, timeServerIP)) {  // Get the IP address of the NTP server
      Serial.print ("DNS lookup failed. Rebooting\n");
      Serial.flush ();
      ESP.reset ();
    } else {
      Serial.print ("Time server IP:\t");
      Serial.println (timeServerIP);
      ntp.ntpdate(timeServerIP);  // set the time
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////

    Time_Scheduled_update_clock = calculate_next_run (crontab, Time_Current);  // determine the next time to run
  }
}


/*****************************************************************/

void loop () {
  Time_Current = system_clock.now ();
  Serial.print ("System time: ");
  system_clock.printDateTime (Time_Current);

  update_clock(Time_Current);

  Time_Previous = Time_Current;
  delay (10000);
}

/*****************************************************************/
