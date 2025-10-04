/*
module_lcd.h

Owen Ziegler
20250421

Description:
Header file for LCD module
*/

#ifndef MODULE_LCD_HPP
#define MODULE_LCD_HPP

void lcdSetup();

void displayStrings(String line1, String line2);

void clearDisplay();

void displayError();

#endif
