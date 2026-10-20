# Automated Plant Watering Robot

An Arduino-based plant watering system: it reads soil moisture and air
temperature, shows both on an LCD with a smooth pixel-level progress
bar, and switches a relay-driven water pump on automatically when the
soil gets too dry.

Originally built in 2022; firmware rebuilt in 2026.

## What it does

- Reads a soil moisture sensor and converts the raw reading to a
  percentage.
- Shows moisture (as text + a smooth bar graph, drawn with custom LCD
  characters instead of plain "#"/"-" text) and air temperature on a
  16x2 LCD.
- When moisture drops below a threshold, switches on a relay-driven
  pump for a few seconds, then waits a cooldown period before checking
  again — so it doesn't pump continuously while the soil is still dry.

## Hardware

- Arduino Uno
- LCD Keypad Shield (16x2 LCD + buttons)
- Capacitive/resistive soil moisture sensor
- DHT11 temperature/humidity sensor
- Relay module driving a small water pump
- 4x AA battery pack for power

## Setup

1. Open `PlantWateringRobot.ino` in the Arduino IDE.
2. Install these libraries through the Library Manager if they aren't
   already installed:
   - `dht` (by Rob Tillaart)
   - `LcdBarGraph` (by Balázs Kelemen — draws the smooth progress bar;
     if it isn't in the Library Manager, grab it from
     [github.com/prampec/LcdBarGraph](https://github.com/prampec/LcdBarGraph)
     and install as a .zip library)
3. Check the pin definitions near the top of the file (`SOIL_PIN`,
   `DHT_PIN`, `RELAY_PIN`) against your actual wiring.
4. Calibrate the soil sensor: upload the sketch, open the Serial
   Monitor, note the raw reading with the sensor in dry air and in a
   cup of water, and set `SOIL_DRY_RAW` / `SOIL_WET_RAW` accordingly.
5. If the pump turns on backwards (on when it should be off), flip
   `RELAY_ACTIVE_LOW`.

## Known limitations

- Soil moisture calibration is sensor- and soil-specific; the default
  values are a starting point, not a universal setting.
- No WiFi/remote monitoring — this version is fully standalone,
  display-and-pump only.
