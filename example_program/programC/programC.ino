/*
   example code
*/

/*****************************************************************/

//  added automatically during compilation: https://github.com/arduino/Arduino/wiki/Build-Process
//#include <Arduino.h>

#include "function.h"

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

  MyExternVariable = 100;
  int w = extern_func(var1, var2);
  Serial.println(w);

  delay(10000);
}

/*****************************************************************/

