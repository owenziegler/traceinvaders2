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
constexpr std::uint8_t __IR_PIN_0 = 35;
constexpr std::uint8_t __IR_PIN_1 = 34;
constexpr std::uint8_t __IR_PIN_2 = 32;
constexpr std::uint8_t __IR_PIN_3 = 36;
constexpr std::uint8_t __IR_PIN_4 = 39;

// UI
constexpr std::uint8_t __UI_PIN_BUTTON = 4;
constexpr std::uint8_t __UI_LCD_COLS = 16;
constexpr std::uint8_t __UI_LCD_ROWS = 2;

// I2C
constexpr std::uint8_t __I2C_PIN_SDA = 21;
constexpr std::uint8_t __I2C_PIN_SCL = 22;
constexpr std::uint8_t __I2C_ADDR_LCD = 0x27;
constexpr std::uint8_t __I2C_ADDR_GYRO = 0;

// Motor Driver
constexpr std::uint8_t __DRIVER_BASE_SPEED = 100;
constexpr std::uint8_t __DRIVER_PIN_LIN1 = 12;
constexpr std::uint8_t __DRIVER_PIN_LIN2 = 13;
constexpr std::uint8_t __DRIVER_PIN_PWML = 25;
constexpr std::uint8_t __DRIVER_PIN_RIN1 = 27;
constexpr std::uint8_t __DRIVER_PIN_RIN2 = 26;
constexpr std::uint8_t __DRIVER_PIN_PWMR = 33;
constexpr std::uint8_t __DRIVER_PIN_STBY = 14;

// Encoders
constexpr std::uint8_t __ENCODER_PIN_L_A = 5;
constexpr std::uint8_t __ENCODER_PIN_L_B = 19;
constexpr std::uint8_t __ENCODER_PIN_R_A = 18;
constexpr std::uint8_t __ENCODER_PIN_R_B = 3;
/*
// Timers & Interrupts
constexpr std::uint32_t __TIMER_HZ = 1000000; // 1 MHz clock
constexpr bool __TIMER_AUTORELOAD = true;
constexpr std::uint32_t __TIMER_MASTER_PERIOD = 100000; // 100 ms (in us)

constexpr std::uint32_t __TIMER_PID_PERIOD = 10000; // 10 ms
constexpr std::uint32_t __TIMER_
*/
#endif