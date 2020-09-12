/*
	thermostat_PID

	This replaces the built-in thermostat for an indirect hot water heater
	It continually monitors the tempeature and updates the PID on a
	set time interval.  That, in turn, determines if the relay should be
	turned on or off.  It also updates a LED display with the current temp
	and adjusts the brightness according to the relay setting

	The circuit:
	* list the components attached to each input
	* list the components attached to each output

	v1.0 11/2017 Stephen Hepner:  Initial version
	
*/

// keep these set as float values

// min safe temp for storing hot water is 140F, 122F seems to be the abs min
#define tmpMin 125.0

// Based upon measurements, the tmpMax should set to 30F below the input temp from the boiler.
// It is possible to use a smaller differential but this is the point of diminishing returns.
// So if the boiler is providing 180F water, tmpMax should be 150F
// [data](https://docs.google.com/spreadsheets/d/1dkOv5oS63TSUh5Ghl3vkvUJrviqBKV0kFPnoFjA-MRQ/edit#gid=192911922)
#define tmpMax 140.0  // boiler outputs at 170F

// Max rate of change in temp over the span of time.  A faster drop would require the heat to be turned on.
// Note that a minor change in temp indicates a huge amount of water has moved through the tank so need to react quickly.
// Also note that the boiler takes a while to respond so the PID might not notice what is going on soon enough so cant sample too quickly either
#define deltaT -.25 // deg F
unsigned int intervalDeltaT = 60000;  // delay in milliseconds between temp samples

// Identify the relays
#define relay1 8  //the lefthand relay is attached to pin 8
#define relay2 9  //the righthand relay is attached to pin 9

////////////////////////////////////////////////////////////////////////////////////////////////

// timing system
#include <elapsedMillis.h>  //lib to deal with millis() issues

// Adafruit MAX31865 temp probe amp
#include <Adafruit_MAX31865.h>  // lib for the temp probe
#define RREF 430.0  // The value of the Rref resistor. Use 430.0!
Adafruit_MAX31865 max = Adafruit_MAX31865(10, 11, 12, 13);  // Use software SPI: CS, DI, DO, CLK
//Adafruit_MAX31865 max = Adafruit_MAX31865(10);  // use hardware SPI, just pass in the CS pin

// display brightness
#define displayDim 0  //dim setting (0=min)
#define displayBright 15  //bright setting (15=max)

// LED display
#include <Adafruit_LEDBackpack.h>  // Adafruit_LED_Backpack
#define DISPLAY_ADDRESS 0x70  // I2C address of the display
Adafruit_7segment Display = Adafruit_7segment();  // Create display and DS1307 objects
unsigned int intervalDisplay = 5000;  // delay in milliseconds for updating the display
elapsedMillis timeElapsedDisplay;  // declare the display timer
int displaySetting = displayDim;  // declare the display brightness setting var

// PID controller
#include <PID_v1.h> // https://github.com/br3ttb/Arduino-PID-Library
double Setpoint, Input, Output;   //Define Variables we'll be connecting to; Output = 0-255
double Kp=2, Ki=5, Kd=1; //initial tuning parameters
//P_ON_M specifies that Proportional on Measurement be used
//P_ON_E (Proportional on Error) is the default behavior
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, P_ON_M, DIRECT);
unsigned int intervalPID = 10000;  // delay in milliseconds for updating the PID
elapsedMillis timeElapsedPID;  // declare the PID refresh timer

// logging to serial console
const boolean logger = true;  //do we want to write to the console?
unsigned int intervalLog = 10000;  // delay in milliseconds for updating the console
elapsedMillis timeElapsedLog;  // declare the logging timer

// tracking of boiler use
elapsedMillis timeElapsedBoiler;  //declare the timer
boolean timerBoiler = false;  //leave the timer off for now
unsigned long startTimeBoiler = 0;  //(ms) the beginning time for turning the heat on
unsigned long currentTimeBoiler = 0;  //(ms) the amount of time it is currently calling for heat
int qtyTimeBoiler = 0;  //the number of times it has called for heat

// deltaT: watching the rate at which the temp is dropping
elapsedMillis timeElapedDeltaT;  // declare the PID refresh timer
float deltaTStartTemp;  // the saved temp
float deltaTLatest = 0; // the temp differential
boolean deltaTHeat = false;  // dont call for heat

////////////////////////////////////////////////////////////////////////////////////////////////

float ReadTempF () {
  // this function will read the temp and return the value in F
  
  int debugLevel = 1;  //0=off, 1=error, 2=verbose
  
  uint16_t rtd = max.readRTD();  //read the value from the probe

  if (debugLevel > 1) { Serial.print("RTD value: "); Serial.println(rtd); }
  float ratio = rtd;
  ratio /= 32768;  // what???
  if (debugLevel > 1) { Serial.print("Ratio = "); Serial.println(ratio,8); }  // print this to 8 decmal places
  if (debugLevel > 1) { Serial.print("Resistance = "); Serial.println(RREF*ratio,8); }

  float tempC = max.temperature(100, RREF); // 100 ohms = 0C
  float tempF = (tempC * 1.80) + 32.00;  //convert Celsius to Fahrenheit; avoiding this: (float(9)/float(5))
  if (debugLevel > 1) { Serial.print("Temperature = "); Serial.print(tempC); Serial.print("C, "); Serial.print(tempF); Serial.println("F"); }

  // Check and print any faults
  uint8_t fault = max.readFault();
  if (fault) {
    if (debugLevel > 0) { Serial.print("Fault 0x"); Serial.println(fault, HEX); }
    if (fault & MAX31865_FAULT_HIGHTHRESH) {
      if (debugLevel > 0) Serial.println("RTD High Threshold"); 
    }
    if (fault & MAX31865_FAULT_LOWTHRESH) {
      if (debugLevel > 0) Serial.println("RTD Low Threshold"); 
    }
    if (fault & MAX31865_FAULT_REFINLOW) {
      if (debugLevel > 0) Serial.println("REFIN- > 0.85 x Bias"); 
    }
    if (fault & MAX31865_FAULT_REFINHIGH) {
      if (debugLevel > 0) Serial.println("REFIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      if (debugLevel > 0) Serial.println("RTDIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_OVUV) {
      if (debugLevel > 0) Serial.println("Under/Over voltage"); 
    }
    max.clearFault();
  }
  if (debugLevel > 1) Serial.println();
  
  return (tempF);
}

////////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////////

float determineSetpoint(float tmpOrig, float tmpCurrent) {
  // make sure we are staying within the boundries and adjust the setpoints if needed
  
  if (tmpCurrent >= tmpMax) {  //too hot! (switch to cooling mode)
    myPID.SetTunings(Kp, Ki, Kd); // set the PID to to the default tuning values
    return(tmpMin);  //set PID target temp to low point
  } else if (tmpCurrent <= tmpMin) {  //too cold! (switch to heating mode)
    return(tmpMax);  //set PID target temp to high point
  } else {
    return(tmpOrig);  //dont change anything.  Neither too hot nor too cold
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {  // this is run once at startup
  RelayControl(relay1,0);  // turn the relay off by default
  
  max.begin(MAX31865_3WIRE);  // the PT100 has 3 wires

  Display.begin(DISPLAY_ADDRESS);  // Setup the display.
  Display.setBrightness(displayDim); // (0-15) Dim display

  Input = ReadTempF();  //grab the current temp to get things started

  //for now make sure we dont call for heat until we know its actually needed
  Setpoint = deltaTStartTemp = tmpMin;
  
  myPID.SetMode(AUTOMATIC);  // define the PID
  
  if (logger) Serial.begin(9600);  // use the USB interface as the serial console
}

////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {  // this is run continuously after setup() is complete
  float tempF = Input = ReadTempF();  //grab the current temp

  if (timeElapsedPID >= intervalPID) {  //update the PID at a slower rate
    Setpoint = determineSetpoint(Setpoint,tempF);  // decide if we need to change the PID setpoint
    myPID.Compute();  // update the PID with the latest info
    timeElapsedPID = 0;  //reset the timer
  }

  // watch for the temp quickly dropping and switch to heating mode if it is.
  if (timeElapedDeltaT >= intervalDeltaT) { // use a slow sample rate when watching for a drop in temp (ie 3min)
    deltaTLatest = tempF - deltaTStartTemp; 
    if (deltaTLatest < deltaT) { // check for rapid temp drop (ie >1.5F within 3min)
      // temp dropping too fast
      Setpoint = tmpMax;  // set the PID to heating mode
      myPID.SetTunings(2, 40, 1); // switch to very aggressive tunning params
      myPID.Compute();  // update the PID with the latest info
      deltaTHeat = true; // force the heat to turn on now!
    } else {
      deltaTHeat = false; // dont need to force the heat to stay on if it doesnt need to
    }
    deltaTStartTemp = tempF;  // save the current temp for the next time
    timeElapedDeltaT = 0;  //reset the timer
  }

  // here is where we actually take action
  if ((Output > 150) || (deltaTHeat)) {  //the PID is calling for heat or deltaT says we need it immediately (because the PID is slow to react)
    displaySetting = displayBright;  //bright display
    if (timerBoiler) {  //boiler is currently on
      currentTimeBoiler = timeElapsedBoiler; //check in on the time
    } else {  //boiler is currently off
      timerBoiler = true;  //start the timer
      startTimeBoiler = timeElapsedBoiler;  //this is when the boiler was turned on
      RelayControl(relay1,1);  //turn the relay on
    }
  } else if (Output < 100) {  //the PID doesnt want heat anymore
    displaySetting = displayDim;  //dim display
    if (timerBoiler) {  //boiler is currently on
      RelayControl(relay1,0);  //turn the relay off
      timerBoiler = false;  //turn off the timer
      currentTimeBoiler = timeElapsedBoiler;  //record the final time
    } else {  //boiler is currently off
      // do we actually need to do anything?
    }
    timeElapsedBoiler = 0;  //reset the timer
  } else {
    //maintain last state
    //the gap between Output settings is to force the PID to be decisive in its actions and to avoid bouncing the boiler on/off
  }

  if (timeElapsedDisplay >= intervalDisplay) {  // update the display
    //int tempFfrac = (tempF-int(tempF)) * 10;
    //Display.printFloat(int(tempF), tempFfrac);  // printFloat seems to be broken
    Display.print (int(tempF), DEC);  // print the temp
    Display.setBrightness(displaySetting);  // change the brightness accordinglly
    Display.writeDisplay();  // Now push out the new values to the display
    timeElapsedDisplay = 0; //reset the timer
  }

  if (timeElapsedLog >= intervalLog) {  // log what is going on to the console
    //print the info
    Serial.print("tmpMin="); Serial.println(tmpMin);
    Serial.print("tmpMax="); Serial.println(tmpMax);
    Serial.print("tempF="); Serial.println(tempF);
    Serial.print("deltaTLatest="); Serial.println(deltaTLatest);
    Serial.print("Setpoint="); Serial.println(Setpoint);
    Serial.print("Output="); Serial.println(Output);
    Serial.print("deltaTHeat="); Serial.println(deltaTHeat);
    Serial.print("displaySetting="); Serial.println(displaySetting);
    Serial.print("currentTimeBoiler="); Serial.println(currentTimeBoiler);
    Serial.println();
    timeElapsedLog = 0;  //reset the timer
  }
}
