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
#include <unordered_map>
#include <Arduino.h>

//Settings

// IR Sensors
constexpr std::uint8_t __IR_PIN_0 = 35;
constexpr std::uint8_t __IR_PIN_1 = 34;
constexpr std::uint8_t __IR_PIN_2 = 32;
constexpr std::uint8_t __IR_PIN_3 = 36;
constexpr std::uint8_t __IR_PIN_4 = 39;

static std::unordered_map<std::uint8_t, std::int8_t> __IR_STATES {
    {0b01111, -4}, //hard left turn
    {0b00111, -3},
    {0b10111, -2},
    {0b10011, -1},
    {0b11011,  0}, //centered
    {0b11001,  1},
    {0b11101,  2},
    {0b11100,  3},
    {0b11110,  4}, //hard right turn
};

enum class __IR_SPECIAL_STATES {
    NOLINE =        0b11111,
    CROSS =         0b00000,
    LEFT_TICK_1 =   0b00011,
    LEFT_TICK_2 =   0b00001,
    RIGHT_TICK_1 =  0b11000,
    RIGHT_TICK_2 =  0b10000
};

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
constexpr std::uint8_t __DRIVER_PIN_LIN1 = 12;
constexpr std::uint8_t __DRIVER_PIN_LIN2 = 13;
constexpr std::uint8_t __DRIVER_PIN_PWML = 25;
constexpr std::uint8_t __DRIVER_PIN_RIN1 = 26;
constexpr std::uint8_t __DRIVER_PIN_RIN2 = 27;
constexpr std::uint8_t __DRIVER_PIN_PWMR = 33;
constexpr std::uint8_t __DRIVER_PIN_STBY = 14;

// Encoders
constexpr std::uint8_t __ENCODER_PIN_L_A = 5;
constexpr std::uint8_t __ENCODER_PIN_L_B = 19;
constexpr std::uint8_t __ENCODER_PIN_R_A = 18;
constexpr std::uint8_t __ENCODER_PIN_R_B = 3;
constexpr float __ENCODER_RPM_NUMERATOR = 280;
constexpr float __ENCODER_RPM_DENOMINATOR = 60000;

// Timers & Interrupts
constexpr std::uint32_t __TIMER_HZ = 1000000; // 1 MHz clock
constexpr bool __TIMER_AUTORELOAD = true;
constexpr std::uint32_t __TIMER_RELOAD_COUNT = 0; //infinite reloads
constexpr std::uint32_t __TIMER_PERIOD = 5000; // 100 ms (100000 us)

//PID constants
constexpr float __PID_KP = 70;
constexpr float __PID_KI = 0;
constexpr float __PID_KD = 0;

// Navigation and State Machine
constexpr std::uint32_t __NAV_BASE_RPM_LOW = 300; //RPM
constexpr std::uint32_t __NAV_BASE_RPM_HIGH = 480; //RPM
constexpr float __NAV_BASE_PULSECOUNT_LOW = (__NAV_BASE_RPM_LOW * __ENCODER_RPM_NUMERATOR * (__TIMER_PERIOD / 1000)) / __ENCODER_RPM_DENOMINATOR;
constexpr float __NAV_BASE_PULSECOUNT_HIGH = (__NAV_BASE_RPM_HIGH * __ENCODER_RPM_NUMERATOR * (__TIMER_PERIOD / 1000)) / __ENCODER_RPM_DENOMINATOR;
constexpr std::uint8_t __NAV_CURVE = 0;
constexpr std::uint8_t __NAV_SPEEDUP = 0;

enum class __NAV_STATE {
    RESET,
    COUNTDOWN,
    LAP,
    FINISH,
    LOST
};

#endif