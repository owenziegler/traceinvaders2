/*
pins.h

Owen Ziegler
20250908

Description:
Global constants and setting flags
*/

#ifndef CONFIG_HPP
#define CONFIG_HPP

// Imports
#include <cstdint>
#include <Arduino.h>

//Settings

// IR Sensors
constexpr std::uint8_t _IR_PIN_0 = 36;
constexpr std::uint8_t _IR_PIN_1 = 39;
constexpr std::uint8_t _IR_PIN_2 = 34;
constexpr std::uint8_t _IR_PIN_3 = 35;
constexpr std::uint8_t _IR_PIN_4 = 32;

// UI
constexpr std::uint8_t _UI_PIN_BUTTON = 1;
constexpr std::uint8_t _UI_LCD_COLS = 16;
constexpr std::uint8_t _UI_LCD_ROWS = 2;

// I2C
constexpr std::uint8_t _I2C_PIN_SDA = 21;
constexpr std::uint8_t _I2C_PIN_SCL = 22;
constexpr std::uint8_t _I2C_ADDR_LCD = 0x27;
constexpr std::uint8_t _I2C_ADDR_GYRO = 0;

// Motor Driver
constexpr std::uint8_t _DRIVER_BASE_SPEED = 100;
constexpr std::uint8_t _DRIVER_PIN_AIN1 = 12;
constexpr std::uint8_t _DRIVER_PIN_AIN2 = 13;
constexpr std::uint8_t _DRIVER_PIN_PWMA = 25;
constexpr std::uint8_t _DRIVER_PIN_BIN1 = 27;
constexpr std::uint8_t _DRIVER_PIN_BIN2 = 26;
constexpr std::uint8_t _DRIVER_PIN_PWMB = 33;
constexpr std::uint8_t _DRIVER_PIN_STBY = 14;

#endif