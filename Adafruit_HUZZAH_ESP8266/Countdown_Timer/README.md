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
- Serial (115200) logs only `light ON` / `light OFF` transitions.

## Why millis() and no RTC

The original used the Adalogger's PCF8523 RTC. Its coin cell is the only part
with a finite life, and a dead RTC made `now()` return a frozen bogus time, which
could leave the relay latched. The countdown only needs *elapsed* time, so
`CountdownTimer.h` uses `millis()` with unsigned subtraction. That is correct
across the 32-bit wrap (~49.7 days), which is where `deadline = millis() + n;
millis() >= deadline` goes wrong. The Adalogger can stay installed; it is unused.

## Build

Arduino IDE / arduino-cli, ESP8266 core, board "Adafruit Feather HUZZAH ESP8266".
No third-party libraries. The old `settings/config.h` is gone (it held only constants).

## Test the timing logic on a host

    cd test && g++ -std=c++11 -I../Countdown_Timer countdown_timer_test.cpp -o /tmp/ct && /tmp/ct

Covers idle, expiry, re-trigger, wrap-around, and no spurious restart over a full counter cycle.
