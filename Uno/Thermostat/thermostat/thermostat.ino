/*

*/

// keep these as float values
#define tmpMin 140.0  // min safe temp for storing hot water
#define tmpMax 175.0  // boiler outputs at 180F.  Keep this slightly lower

// using variables because why not
const int relay1 = 8;  //the lefthand relay is attached to pin 8
const int relay2 = 9;  //the righthand relay is attached to pin 9

#include <Adafruit_MAX31865.h>  // lib for the temp probe
#define RREF 430.0  // The value of the Rref resistor. Use 430.0!
Adafruit_MAX31865 max = Adafruit_MAX31865(10, 11, 12, 13);  // Use software SPI: CS, DI, DO, CLK
//Adafruit_MAX31865 max = Adafruit_MAX31865(10);  // use hardware SPI, just pass in the CS pin

#include <Adafruit_LEDBackpack.h>  // Adafruit_LED_Backpack
#define DISPLAY_ADDRESS 0x70  // I2C address of the display
Adafruit_7segment Display = Adafruit_7segment();  // Create display and DS1307 objects


void setup() {  // this is run once at startup
  RelayControl(relay1,0);  // turn the relay off by default
  Serial.begin(9600);  // use the USB interface as the serial console
  max.begin(MAX31865_3WIRE);  // the PT100 has 3 wires
  
  Display.begin(DISPLAY_ADDRESS);  // Setup the display.
  Display.setBrightness(0); // (0-15) Dim
}


void loop() {  // this is run continuously after setup() is complete
  float tempF = ReadTempF();
    
  if (tempF < tmpMin) {  // too cold!
    RelayControl(relay1,1);  // turn the relay on
    Display.setBrightness(15);  // (0-15) bright
  } else if (tempF >tmpMax) {  // too hot!
    RelayControl(relay1,0);  // turn the relay off
    Display.setBrightness(0);  // (0-15) dim
  }

  //int tempFfrac = (tempF-int(tempF)) * 10;
  //Display.printFloat(int(tempF), tempFfrac);  // this call doesnt work for some reason
  Display.print (int(tempF), DEC);  // print the temp
  Display.writeDisplay();  // Now push out to the display the new values that were set above.

  delay(1000);  // in msec
}

float ReadTempF () {
  // this will read the temp and return the value in F
  uint16_t rtd = max.readRTD();

  Serial.print("RTD value: "); Serial.println(rtd);
  float ratio = rtd;
  ratio /= 32768;  // what???
  Serial.print("Ratio = "); Serial.println(ratio,8);  // print this to 8 decmal places
  Serial.print("Resistance = "); Serial.println(RREF*ratio,8);

  float tempC = max.temperature(100, RREF); // 100 ohms = 0C
  float tempF = (tempC * 1.80) + 32.00;  //convert Celsius to Fahrenheit; avoiding this: (float(9)/float(5))
  Serial.print("Temperature = "); Serial.print(tempC); Serial.print("C, "); Serial.print(tempF,1); Serial.println("F");

  // Check and print any faults
  uint8_t fault = max.readFault();
  if (fault) {
    Serial.print("Fault 0x"); Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH) {
      Serial.println("RTD High Threshold"); 
    }
    if (fault & MAX31865_FAULT_LOWTHRESH) {
      Serial.println("RTD Low Threshold"); 
    }
    if (fault & MAX31865_FAULT_REFINLOW) {
      Serial.println("REFIN- > 0.85 x Bias"); 
    }
    if (fault & MAX31865_FAULT_REFINHIGH) {
      Serial.println("REFIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.println("Under/Over voltage"); 
    }
    max.clearFault();
  }
  Serial.println();
  
  return (tempF);
}


void RelayControl (int PinNo, int State) {
  // this is to control the Sainsmart 5v relay module.  The module is connected to power and the
  // control pin is connected to ground to engage the relay.  A led will light up as well
  // https://www.sainsmart.com/products/2-channel-5v-relay-module
  //
  // PinNo = the pin number to use
  // State = [0|1]; 0 = off; 1 = on
  switch (State) {
    case 0:
      // disconnect the pin
      pinMode(PinNo, INPUT);
      break;
    case 1:
      // connect the pin to ground
      pinMode(PinNo, OUTPUT);
      digitalWrite(PinNo, LOW);
      break;
    default:
      //do nothing
      break;
  }
}
