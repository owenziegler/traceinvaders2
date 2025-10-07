/*
module_lcd.cpp

Owen Ziegler
20250421

Description:
Implementation for LCD module. Contains definitions of all LCD module functions.
*/

#include <Arduino.h>

#include <LiquidCrystal_I2C.h>

#include <module_lcd.hpp>
#include <config.hpp>


Lcd::Lcd(uint8_t addr, String top, String bottom)
  : _lcd(addr, __UI_LCD_COLS, __UI_LCD_ROWS) {
  Wire.begin(__I2C_PIN_SDA,__I2C_PIN_SCL);
  _lcd.init();
  _lcd.backlight();
}

void Lcd::display(String top, String bottom) {
  //check for valid input
  if(top.length() > 16 || bottom.length() > 16) {
    _error();
  }
  else {
    _lcd.clear();
    _lcd.setCursor(0,0);
    _lcd.print(top);
    _lcd.setCursor(0,1);
    _lcd.print(bottom);
  }
}

void Lcd::clear() {
  _lcd.clear();
}

void Lcd::_error() {
  _lcd.clear();
  _lcd.setCursor(0,0);
  _lcd.print("Error");
  _lcd.setCursor(0,1);
  _lcd.print("Invalid Input");
}
