/*
module_lcd.cpp

Owen Ziegler
20250421

Description:
Implementation for LCD module. Contains definitions of all LCD module functions.
*/

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "module_lcd.hpp"
#include "config.hpp"

#define COLS 16
#define ROWS 2

LiquidCrystal_I2C lcd(_I2C_ADDR_LCD, COLS, ROWS);

void lcdSetup() {
  Wire.begin(_I2C_PIN_SDA,_I2C_PIN_SCL);
  lcd.init();
  lcd.backlight();
}

void displayStrings(String line1, String line2) {
  //check for valid input
  if(line1.length() > 16 || line2.length() > 16) {
    displayError();
    //return -1;
  }
  else {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(line1);
    lcd.setCursor(0,1);
    lcd.print(line2);
  }
}

void clearDisplay() {
  lcd.clear();
}

void displayError() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Error");
  lcd.setCursor(0,1);
  lcd.print("Invalid Input");
}
