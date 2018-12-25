# Timer

independent operation

update config via webpage?
*  doesnt seem to work well.  Puts interface to sleep?

fetch schedule from central repo?  Github?



## Complete

*  Conect to local WiFi
*  perform NTP request
*  update RTC time from NTP


## ToDo

*  setup mode
    *  enable AP
    *  present authenticated webpage
    *  collect config data and write to EEPROM
*  scheduler
    *  sunrise/sunset
    *  DST
*  sleep when not in use
*  
*  


### Notes and research items

*  Reference
    *  [ESP8266 Technical Reference](http://espressif.com/sites/default/files/documentation/esp8266-technical_reference_en.pdf)

*  Library
    *  [ESP8266 core for Arduino](https://github.com/esp8266/Arduino)
    *  [ESP8266 Arduino Core](http://esp8266.github.io/Arduino/versions/2.3.0/doc/filesystem.html)

*  EEPROM stuff
    *  [Arduino/libraries/EEPROM/examples/](https://github.com/esp8266/Arduino/tree/master/libraries/EEPROM/examples)
        *  [eeprom_write.ino](https://github.com/esp8266/Arduino/tree/master/libraries/EEPROM/examples/eeprom_write)
        *  [eeprom_read.ino](https://github.com/esp8266/Arduino/tree/master/libraries/EEPROM/examples/eeprom_read)
        *  [eeprom_clear.ino](https://github.com/esp8266/Arduino/tree/master/libraries/EEPROM/examples/eeprom_clear)
    *  [ESP8266 Web Server to storing ap config to EEPROM](https://gist.github.com/dogrocker/f998dde4dbac923c47c1)

*  Power saving
    *  [ESP8266 Deep Sleep with Arduino IDE](https://randomnerdtutorials.com/esp8266-deep-sleep-with-arduino-ide/)
    *  [LIGHT_SLEEP HOW ? #1381](https://github.com/esp8266/Arduino/issues/1381)

*  Timezone
    *  [Arduino Timezone Library v1.0](https://github.com/JChristensen/Timezone)

*  Sunrise/sunset
    *  [AVR-Libc](http://swfltek.com/arduino/sunrise.html)
    *  [ARDUINO ASTRONOMICAL CLOCK FOR AUTOMATIC LIGHT CONTROL](http://paulorenato.com/index.php/125)
    *  [dmkishi/Dusk2Dawn](https://github.com/dmkishi/Dusk2Dawn)

*  Scheduler
    *  [Arduino TaskScheduler library](https://github.com/arkhipenko/TaskScheduler)
    *  https://openhomeautomation.net/esp8266-battery/

---

### Other

*  [Arduino Projects: Digital Voice Recorder Part 2](http://www.apcmag.com/arduino-projects-digital-voice-recorder-part-2.htm/)
*  [Arduino/cores/esp8266/i2s.h](https://github.com/esp8266/Arduino/blob/master/cores/esp8266/i2s.h)


