# Arduino — agent guide

All of Stephen's Arduino-family firmware (Arduino IDE / arduino-cli sketches), one folder per platform.
Personal hub project (`~/local/personal/projects/Arduino`); remote `origin` is GitHub `shepner/Arduino`
(`master`). The core rules in `~/local/personal/AGENTS.md` and `core/` apply.

## Layout

| Folder | What | State |
| --- | --- | --- |
| `Adafruit_HUZZAH_ESP8266/Countdown_Timer/` | Garage-light countdown (ESP8266 + relay). Has its own README, host-side timing test (`test/`). | Active; bench-verified 2026-10-03, not yet re-verified in the garage |
| `Adafruit_HUZZAH_ESP8266/Timer/` | Older cron/NTP/WiFi timer. Needs a local `settings/config.h` (WiFi credentials). | Unfinished (its README lists open items); last touched 2020 |
| `Adafruit_HUZZAH_ESP8266/Thermal_Camera/` | AMG88xx thermal-camera sketch (Adafruit example) | Dormant (2018) |
| `Uno/Thermostat/` | Hot-water-heater thermostat, simple and PID versions (see its README) | Dormant (one tweak 2026-02) |
| `Teensy/Eyes/` | Teensy 3.1 animated eyes (from adafruit/Teensy3.1_Eyes; our change: optional eyelid removal) | Dormant (2018) |
| `Freematics/` | Forks of the Freematics / ArduinoOBD vehicle loggers (`00-clone.sh` fetches upstream) | Dormant (2018-2020) |

## Working here

- Build with arduino-cli or the Arduino IDE; the FQBN and core version are in each sketch's README
  (Countdown_Timer: `esp8266:esp8266:huzzah`, ESP8266 core 3.1.2). Read the sketch README first.
- **Credentials never get committed.** `.gitignore` ignores every `**/config.h`; a real WiFi SSID and
  password sit in the ignored `Timer/settings/config.h` and an APN in an ignored Freematics
  `config.h`. Edit `config.h.sample` for new keys; never print or copy a `config.h` value
  (core non-negotiable 6).
- `libraries/` is ignored on purpose (third-party Arduino libraries, restored by the IDE or
  `arduino-cli lib install`). `libraries/Arduino-PID-Library` is a clone of br3ttb's upstream;
  `libraries/Logging/doc` is a nested repo holding one old doxygen-HTML commit of ours. Do not
  try to commit either into this repo.
- Do not run `update_git.sh`: it does `git add .` and an unreviewed 'automatic update' commit,
  which violates commit-by-path (non-negotiable 7). Commit named paths with a real message.
- Pushing to GitHub needs the operator's go-ahead.
