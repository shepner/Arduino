# Thermostat

## Description
The whole purpose of this is to replace the mechanical thermostat in an indirect hot water heater.  The built-in unit, at the max setting, turns on at 125F and off at 140F.  The water heater has a mixing valve so scalding is not an issue and the minimum storage tempeature for hot water to prevent bacterial growth is 135F.  The water heater itself can output at up to 180F and the heater at idle will loose .5F/hr.

This was written using the [Arduino Create](https://create.arduino.cc) which is a web based IDE.  The original sketch, [thermostat](https://create.arduino.cc/editor/shepner/bee4baca-0dba-47e2-9222-cc4cdaee5413/preview), is a an analog of a mechanical thermostat.  The newer version, [thermostat_PID](https://create.arduino.cc/editor/shepner/c575e711-e72f-412a-a6ef-6e209f8f05ce/preview), is more sophisticated.  Now that simple thermostat is controlling the setpoint of a [PID controller](https://en.wikipedia.org/wiki/PID_controller).  The PID, in turn, tries to hit the target tempeature without overshooting.  There is some dead-space added into the middle of the range to prevent the possibility of rapid cycling of the boiler.  Only when the PID is firmly calling for heat is when the relay will be engaged.  The LCD display shows the current temp.  Low brightness is when the relay is off while high brightness is when the relay is on.

The goal is to have the widest tempeature swing possible with the minimum set at 135F.  Ideally the maximum tempeature would be the point of deminishing returns as determined by the real-time data.

---

Arduino IDE settings (OSX)
* Board: Arduino/Genuino UNO
* Port: /dev/cu.usbmodem1461(Arduino/Genuino UNO)
* Programmer: ArduinoISP
