/*
module_lcd.h

Owen Ziegler
20250421

Description:
Header file for LCD module
*/

#ifndef MODULE_LCD_HPP
#define MODULE_LCD_HPP

#include <cstdint>

#include <LiquidCrystal_I2C.h>

class Lcd  {
    public:
        Lcd(uint8_t addr, String top, String bottom);
        void display(String top, String bottom);
        void clear();
    private:
        LiquidCrystal_I2C _lcd;
        void _error();
};

#endif
