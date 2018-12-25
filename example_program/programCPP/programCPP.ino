/*
   example code
*/

/*****************************************************************/

//  added automatically during compilation: https://github.com/arduino/Arduino/wiki/Build-Process
//#include <Arduino.h>

#include "myclass.h"

#define BAUDRATE 9600  // for serial console

int var1 = 10;
int var2 = 2;

/*****************************************************************/

void setup() {
  Serial.begin (BAUDRATE);
}

/*****************************************************************/

void loop() {
  Serial.println("starting loop");



  delay(10000);
}

/*****************************************************************/

