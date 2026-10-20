/*
 * Automated Plant Watering Robot — Arduino Uno + LCD Keypad Shield
 * =================================================================
 * Reads soil moisture and air temperature, shows both on the LCD with
 * a simple progress bar, and switches a relay-driven water pump on
 * when the soil is too dry.
 *
 * Hardware:
 *   - Arduino Uno + LCD Keypad Shield (16x2 LCD + 5 buttons)
 *   - Capacitive/resistive soil moisture sensor -> analog pin
 *   - DHT11 temperature/humidity sensor -> digital pin
 *   - Relay module -> digital pin, driving a small water pump
 *
 * Stefan Catalin Radu (2022) — rebuilt firmware, 2026
 *
 * PIN NOTES — adjust these three if your wiring differs. Everything
 * else (the LCD pins) matches a standard LCD Keypad Shield and should
 * not need to change.
 */

#include <LiquidCrystal.h>
#include <dht.h>

// ─── PINS — CHANGE THESE IF YOUR WIRING IS DIFFERENT ──────────────────
#define SOIL_PIN     A1   // soil moisture sensor, analog input
#define DHT_PIN      2    // DHT11 data pin
#define RELAY_PIN    3    // relay module, controls the pump

// Standard LCD Keypad Shield wiring (usually does not need changing):
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// ─── CALIBRATION ───────────────────────────────────────────────────────
// Raw analogRead() values for your sensor in air (dry) and in water (wet).
// To calibrate: upload this sketch, open Serial Monitor, note the raw
// value with the sensor in dry air and the raw value in a cup of water,
// then put those numbers here.
#define SOIL_DRY_RAW   780   // reading in dry air — adjust after testing
#define SOIL_WET_RAW   330   // reading in water — adjust after testing

#define MOISTURE_LOW_THRESHOLD 30   // % — below this, the pump turns on
#define PUMP_RUN_MS            3000 // how long the pump runs each time it waters
#define PUMP_COOLDOWN_MS       60000 // minimum time between waterings, so it
                                     // doesn't pump continuously if still dry

#define RELAY_ACTIVE_LOW true  // most small relay modules trigger on LOW;
                                // set to false if your pump turns on backwards

dht DHT;

unsigned long lastWateredAt = 0;

void setup() {
  Serial.begin(9600);

  lcd.begin(16, 2);

  pinMode(RELAY_PIN, OUTPUT);
  setPump(false);

  lcd.setCursor(0, 0);
  lcd.print("Plant Watering");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(1500);
  lcd.clear();
}

void loop() {
  int soilRaw = analogRead(SOIL_PIN);
  int moisturePct = rawToPercent(soilRaw);

  int chk = DHT.read11(DHT_PIN);
  float tempC = (chk == DHTLIB_OK) ? DHT.temperature : NAN;

  Serial.print("Soil raw: ");
  Serial.print(soilRaw);
  Serial.print("  Moisture: ");
  Serial.print(moisturePct);
  Serial.print("%  Temp: ");
  Serial.println(tempC);

  showOnLcd(moisturePct, tempC);

  // Water the plant if dry and the cooldown period has passed
  if (moisturePct < MOISTURE_LOW_THRESHOLD &&
      millis() - lastWateredAt > PUMP_COOLDOWN_MS) {
    waterPlant();
  }

  delay(2000);
}

// ─── Soil reading -> percentage ───────────────────────────────────────
int rawToPercent(int raw) {
  int pct = map(raw, SOIL_DRY_RAW, SOIL_WET_RAW, 0, 100);
  return constrain(pct, 0, 100);
}

// ─── LCD display ────────────────────────────────────────────────────
void showOnLcd(int moisturePct, float tempC) {
  lcd.setCursor(0, 0);
  if (moisturePct < MOISTURE_LOW_THRESHOLD) {
    lcd.print("UMIDITATE MICA  ");
  } else {
    lcd.print("UMIDITATE BUNA  ");
  }

  lcd.setCursor(0, 1);
  lcd.print("(");
  printBar(moisturePct);
  lcd.print(")");

  char line2[6];
  snprintf(line2, sizeof(line2), "%3d%%", moisturePct);
  lcd.print(line2);

  lcd.print(" ");
  if (!isnan(tempC)) {
    char tbuf[5];
    snprintf(tbuf, sizeof(tbuf), "%2dC", (int)tempC);
    lcd.print(tbuf);
  } else {
    lcd.print("--C");
  }
}

// Draws a 4-character bar, e.g. "##--" for 50%
void printBar(int pct) {
  const int barWidth = 4;
  int filled = map(pct, 0, 100, 0, barWidth);
  for (int i = 0; i < barWidth; i++) {
    lcd.print(i < filled ? '#' : '-');
  }
}

// ─── Pump control ──────────────────────────────────────────────────────
void setPump(bool on) {
  bool activeLevel = RELAY_ACTIVE_LOW ? LOW : HIGH;
  bool idleLevel   = RELAY_ACTIVE_LOW ? HIGH : LOW;
  digitalWrite(RELAY_PIN, on ? activeLevel : idleLevel);
}

void waterPlant() {
  Serial.println("Watering...");
  lcd.setCursor(0, 0);
  lcd.print("Watering...     ");

  setPump(true);
  delay(PUMP_RUN_MS);
  setPump(false);

  lastWateredAt = millis();
}
