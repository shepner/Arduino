/*
 * fetch the current time from a NTP server
 * https://circuits4you.com/2018/01/02/esp8266-nodemcu-ntp-time-clock/
 * 
 */

/*****************************************************************/

#ifndef NTP_H
  #include "ntp.h"
#endif  // NTP_H

/*****************************************************************/

int NTP::CommunicateUDP (IPAddress& address) {
  const unsigned int UDP_Source_Port = random (1025, 65535);  // randomly pick a source port to use
  
  udp.begin (UDP_Source_Port);  // prepare a socket
  //Serial.print ("Source port: ");
  //Serial.println (udp.localPort());
  
  udp.beginPacket (address, 123);  // NTP requests are to port 123
  udp.write (packetBuffer, packetSize);  // send packet
  udp.endPacket ();
  
  delay (1000);  // wait to see if a reply is available
  
  int cb = udp.parsePacket ();  // read response

  if (!cb || (cb < packetSize)) {
    Serial.print("No UDP packet received\n");
    return (0);
    
  } else {
    //Serial.print ("packet received, length=");
    //Serial.println (cb);
    
    udp.read (packetBuffer, packetSize);  // read the packet into the buffer
    return (1);
  }
}


// send an NTP request to the time server at the given address
unsigned long NTP::getNTPTime (IPAddress& address) {
  //Serial.print ("Sending NTP packet to ");
  //Serial.println (address);

  memset (packetBuffer, 0, packetSize);  // set all bytes in the buffer to 0

  // Initialize values needed to form NTP request
  packetBuffer[0] = 0b11100011;  // LI, Version, Mode
  packetBuffer[1] = 0;           // Stratum, or type of clock
  packetBuffer[2] = 6;           // Polling Interval
  packetBuffer[3] = 0xEC;        // Peer Clock Precision
  
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;

  if (!CommunicateUDP (address)) {  // submit the packet and fetch the response
    return (-1);  // return an error if no data is returned

  } else {
    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words
    unsigned long highWord = word (packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word (packetBuffer[42], packetBuffer[43]);
    
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900)
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    //Serial.print ("Seconds since Jan 1 1900 = ");
    //Serial.println (secsSince1900);

    return (secsSince1900);
  }
}


// convert NTP time into UNIX time
unsigned long NTP::NTPtoUNIXtime (unsigned long NTPtime) {
  // NTP epoch is Jan 1 1900
  // UNIX epoch is Jan 1 1970.  In seconds, this is 2208988800
  const unsigned long seventyYears = 2208988800UL;
  
  unsigned long UNIXepoch = NTPtime - seventyYears;  // subtract seventy years
  //Serial.print ("Unix time = ");  
  //Serial.println (UNIXepoch);

  return (UNIXepoch);
}


int NTP::ntpdate (IPAddress& address) {

  RTC_PCF8523 rtc; // we are using the PCF8523 RTC

  if (! rtc.begin ()) {
    //Serial.print ("Couldn't find RTC\n");
    return(-1);  // hardware failure?
  }
  
  if (rtc.initialized()) {  // update the time if the RTC is available
    //Serial.print ("RTC is running!\n");

    DateTime dt = rtc.now ();  // grab the system time
    //Serial.print ("RTC time: ");
    //system_clock.printDateTime (dt);

    unsigned long NTP_Time = getNTPTime (address);  // get the NTP time (seconds)
    //Serial.print ("NTP time: ");
    //Serial.println (NTP_Time);

    // calculate the time diff between UTC and local timezone
    int offset;
    if (HH < 0) {
      offset = (HH*60*60) - (MM*60);
    } else {
      offset = (HH*60*60) + (MM*60);
    }
    //Serial.println (offset);

    NTP_Time = NTP_Time + offset;  // adjust the time to the local timezone
    //Serial.print ("Adjusted NTP time: ");
    //Serial.println (NTP_Time);
    
    DateTime dtNTP = NTPtoUNIXtime (NTP_Time);  // convert NTP time to UNIX time
    //Serial.print ("UNIX NTP time: ");
    //system_clock.printDateTime (dtNTP);

    TimeSpan td;
    td = dt.unixtime () - dtNTP.unixtime ();  // return the time in seconds and find the difference
    int timeDiff = abs (td.totalseconds ());
    //Serial.print ("Time difference = " + String (timeDiff) + " sec\n");

    int maxdrift = 86400;  // 24hrs = 24*60*60 = 86400
    if (timeDiff <= maxdrift) {
      // This line sets the RTC with an explicit date & time
      // rtc.adjust (DateTime (2014, 1, 21, 3, 0, 0));  // a fixed datetime: January 21, 2014 at 3am
      // rtc.adjust (DateTime (F (__DATE__), F (__TIME__)));  // use the sketch compile datetime
      rtc.adjust (dtNTP);  // use datetime from ntpd
      
      return (1);  // update successful
    } else {  // NTP time was too far off from system time
      //Serial.print ("[error] ntpdate: Time diff is " + String(timeDiff) + " sec.  Max drift " + String(maxdrift) + " sec\n");
      return (0);  // failed to update time
    }
  } else {  // the RTC is not available for some reason
    //Serial.print ("RTC is NOT running!\n");
    return (-1);  // hardware failure?
  }

  return (-999); // unknown error occured
}

/*****************************************************************/
