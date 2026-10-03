# Garage light countdown timer

Adafruit Feather HUZZAH **ESP8266** + Relay FeatherWing. The garage doors have no
lights, but they expose a "door moving" signal. While either signal is asserted
the relay turns the garage lights on; ten minutes after the last assertion it
turns them off.

## Behaviour

- Door signals: GPIO 13 and 14, `INPUT_PULLUP`, **LOW = door moving**.
- Relay: GPIO 12, HIGH = on. Forced off at boot.
- Timer restarts on every assertion, so the 10 minutes run from when the door stops.
- WiFi is switched off at boot and not persisted to flash.
- Serial (115200) logs boot info and `light ON` / `light OFF` transitions. Set
  `kDebugSerial = true` for a 5 s status line (door pins, relay, time remaining).
  **Do not leave a serial monitor open while the unit is meant to run:** it appeared
  to hold the board in reset and stop the relay tripping.

## Why millis() and no RTC

The original used the Adalogger's PCF8523 RTC. Its coin cell is the only part
with a finite life, and a dead RTC made `now()` return a frozen bogus time, which
made the relay drop the moment the door stopped. The countdown only needs *elapsed* time, so
`CountdownTimer.h` uses `millis()` with unsigned subtraction. That is correct
across the 32-bit wrap (~49.7 days), which is where `deadline = millis() + n;
millis() >= deadline` goes wrong. The Adalogger and its battery have been removed from the unit.

## Build

Arduino IDE / arduino-cli, ESP8266 core 3.1.2 (the latest in the official package
index at the time; originally written on 2.4.2), board "Adafruit Feather HUZZAH ESP8266".
FQBN for arduino-cli: `esp8266:esp8266:huzzah`.
No third-party libraries. The old `settings/config.h` is gone (it held only constants).

## Test the timing logic on a host

    cd test && g++ -std=c++11 -I../Countdown_Timer countdown_timer_test.cpp -o /tmp/ct && /tmp/ct

Covers idle, expiry, re-trigger, wrap-around, and no spurious restart over a full counter cycle.

## Status

Verified on the bench (2026-10-03): relay trips on a door signal and turns off
10 minutes after the signal is released. Not yet re-verified in the garage.
