#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "max6675.h"

// === MAX6675 Thermocouple Channel Pins ===
// Channel 1 (hardware SPI)
#define SCK1 13
#define CS1 10
#define SO1 12
// Channel 2 (software SPI)
#define SCK2 11
#define CS2 9
#define SO2 7

MAX6675 thermocouple1(SCK1, CS1, SO1);
MAX6675 thermocouple2(SCK2, CS2, SO2);

// === Control Buttons (active-low) ===
#define BTN1 4   // Increase TS1
#define BTN2 5   // Decrease TS1
#define BTN3 2   // Increase TS2
#define BTN4 3   // Decrease TS2

// === Outputs ===
#define LED_LOW_1 A1   // Channel 1: below target
#define LED_HIGH_1 A2  // Channel 1: at/above target
#define LED_LOW_2 8    // Channel 2: below target
#define LED_HIGH_2 A0  // Channel 2: at/above target
#define BUZZER 6       // Shared audible alert

// === LCD (I2C) ===
LiquidCrystal_I2C lcd(0x27, 16, 2);

// === Target Temperatures ===
float TS1 = 100.0;
float TS2 = 100.0;

// === Button-hold acceleration counters ===
int holdCount1 = 0;
int holdCount2 = 0;
int holdCount3 = 0;
int holdCount4 = 0;

// === Custom LCD characters (up / down arrows) ===
byte upArrow[8] = {
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00000
};

byte downArrow[8] = {
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b10101,
  0b01110,
  0b00100,
  0b00000
};

void setup() {
  // --- Configure inputs and outputs ---
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);
  pinMode(BTN4, INPUT_PULLUP);

  pinMode(LED_LOW_1, OUTPUT);
  pinMode(LED_HIGH_1, OUTPUT);
  pinMode(LED_LOW_2, OUTPUT);
  pinMode(LED_HIGH_2, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(LED_LOW_1, LOW);
  digitalWrite(LED_HIGH_1, LOW);
  digitalWrite(LED_LOW_2, LOW);
  digitalWrite(LED_HIGH_2, LOW);
  digitalWrite(BUZZER, LOW);

  // --- Initialize LCD ---
  lcd.init();
  lcd.backlight();
  delay(300);
  lcd.createChar(0, upArrow);
  lcd.createChar(1, downArrow);

  // --- Display static labels ---
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T1:   | TS1:    ");
  lcd.setCursor(0, 1);
  lcd.print("T2:   | TS2:    ");
}

void loop() {
  // --- Read both thermocouples ---
  double T1 = thermocouple1.readCelsius();
  delay(5); // avoid SPI overlap
  double T2 = thermocouple2.readCelsius();

  // --- Update actual temperature readings ---
  lcd.setCursor(3, 0);
  if (isnan(T1)) lcd.print("Err  ");
  else { lcd.print((int)T1); lcd.print(" "); }

  lcd.setCursor(3, 1);
  if (isnan(T2)) lcd.print("Err  ");
  else { lcd.print((int)T2); lcd.print(" "); }

  // --- Update target setpoints ---
  lcd.setCursor(12, 0);
  lcd.print((int)TS1); lcd.print(" ");
  lcd.setCursor(12, 1);
  lcd.print((int)TS2); lcd.print(" ");

  // === Channel 1 indicator logic ===
  if (T1 < TS1) {
    digitalWrite(LED_LOW_1, HIGH);
    digitalWrite(LED_HIGH_1, LOW);
  } else {
    digitalWrite(LED_LOW_1, LOW);
    digitalWrite(LED_HIGH_1, HIGH);
    tone(BUZZER, 2000, 500); // alert when threshold reached
  }

  // === Channel 2 indicator logic ===
  if (T2 < TS2) {
    digitalWrite(LED_LOW_2, HIGH);
    digitalWrite(LED_HIGH_2, LOW);
  } else {
    digitalWrite(LED_LOW_2, LOW);
    digitalWrite(LED_HIGH_2, HIGH);
    tone(BUZZER, 2000, 500);
  }

  // === Button handling with acceleration ===
  if (digitalRead(BTN1) == LOW) {
    holdCount1++;
    TS1 += 5;
    lcd.setCursor(12, 0); lcd.print((int)TS1); lcd.print(" ");
    lcd.setCursor(15, 0); lcd.write(byte(0));   // up arrow
    delay(holdCount1 > 3 ? 50 : 250);
  } else { holdCount1 = 0; lcd.setCursor(15, 0); lcd.print(" "); }

  if (digitalRead(BTN2) == LOW) {
    holdCount2++;
    TS1 -= 5;
    lcd.setCursor(12, 0); lcd.print((int)TS1); lcd.print(" ");
    lcd.setCursor(15, 0); lcd.write(byte(1));   // down arrow
    delay(holdCount2 > 3 ? 50 : 250);
  } else { holdCount2 = 0; lcd.setCursor(15, 0); lcd.print(" "); }

  if (digitalRead(BTN3) == LOW) {
    holdCount3++;
    TS2 += 5;
    lcd.setCursor(12, 1); lcd.print((int)TS2); lcd.print(" ");
    lcd.setCursor(15, 1); lcd.write(byte(0));   // up arrow
    delay(holdCount3 > 3 ? 50 : 250);
  } else { holdCount3 = 0; lcd.setCursor(15, 1); lcd.print(" "); }

  if (digitalRead(BTN4) == LOW) {
    holdCount4++;
    TS2 -= 5;
    lcd.setCursor(12, 1); lcd.print((int)TS2); lcd.print(" ");
    lcd.setCursor(15, 1); lcd.write(byte(1));   // down arrow
    delay(holdCount4 > 3 ? 50 : 250);
  } else { holdCount4 = 0; lcd.setCursor(15, 1); lcd.print(" "); }

  delay(250); // refresh interval
}
