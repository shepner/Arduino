// Garage light countdown timer for an Adafruit Feather HUZZAH ESP8266 with a
// Relay FeatherWing. While either garage-door "moving" signal is asserted the
// light relay is on; it stays on for kOnTimeMs after the last assertion.
//
// Time comes from millis() only: no RTC, no battery, no WiFi. See README.md.

#include <ESP8266WiFi.h>

#include "CountdownTimer.h"

// Feather HUZZAH ESP8266 pins (3.3 V max): https://learn.adafruit.com/assets/46249
constexpr uint8_t kRelayPin = 12;  // Relay FeatherWing signal
constexpr uint8_t kDoor1Pin = 13;  // door signal, pulled up; LOW = door moving
constexpr uint8_t kDoor2Pin = 14;  // door signal, pulled up; LOW = door moving

constexpr uint32_t kOnTimeMs = 10UL * 60UL * 1000UL;  // 10 minutes
constexpr uint32_t kPollMs = 20;                      // loop period; also lets the CPU idle
constexpr uint32_t kHeartbeatMs = 5000;               // status line period on serial

CountdownTimer light_timer(kOnTimeMs);
bool light_on = false;
uint32_t last_heartbeat_ms = 0;

void setup() {
  Serial.begin(115200);

  // Radio is not used: stop it so it cannot associate, draw power, or crash.
  WiFi.persistent(false);  // do not write WiFi state to flash
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(1);

  digitalWrite(kRelayPin, LOW);  // set level before enabling the output: no glitch at boot
  pinMode(kRelayPin, OUTPUT);
  pinMode(kDoor1Pin, INPUT_PULLUP);
  pinMode(kDoor2Pin, INPUT_PULLUP);

  Serial.println(F("\nGarage light timer: WiFi off, millis() timing"));
  Serial.print(F("reset reason: "));
  Serial.println(ESP.getResetReason());
}

void loop() {
  const uint32_t now = millis();

  const bool door1 = digitalRead(kDoor1Pin) == LOW;
  const bool door2 = digitalRead(kDoor2Pin) == LOW;
  if (door1 || door2) {
    light_timer.trigger(now);  // door moving: (re)start the countdown
  }

  const bool want_on = light_timer.update(now);
  if (want_on != light_on) {
    light_on = want_on;
    digitalWrite(kRelayPin, light_on ? HIGH : LOW);
    Serial.print(now / 1000);
    Serial.println(light_on ? F("s light ON") : F("s light OFF"));
  }

  // Periodic status so a serial monitor can see the state at any moment.
  if (static_cast<uint32_t>(now - last_heartbeat_ms) >= kHeartbeatMs) {
    last_heartbeat_ms = now;
    Serial.printf("up=%lus door1=%s door2=%s relay=%s remaining=%lus\n",
                  static_cast<unsigned long>(now / 1000), door1 ? "MOVING" : "idle",
                  door2 ? "MOVING" : "idle", light_on ? "ON" : "off",
                  static_cast<unsigned long>(light_timer.remaining_ms(now) / 1000));
  }

  delay(kPollMs);  // also feeds the ESP8266 watchdog
}
