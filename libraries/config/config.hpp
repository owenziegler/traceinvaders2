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

namespace Config {
    namespace Ir {
        namespace Pins {
            inline constexpr uint8_t Pin0 = 35;
            inline constexpr uint8_t Pin1 = 34;
            inline constexpr uint8_t Pin2 = 32;
            inline constexpr uint8_t Pin3 = 36;
            inline constexpr uint8_t Pin4 = 39;
        }
        inline std::unordered_map<uint8_t, int8_t> States {
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
        namespace SpecialStates {
            inline constexpr uint8_t NoLine     = 0b11111;
            inline constexpr uint8_t Cross      = 0b00000;
            inline constexpr uint8_t LeftTick1  = 0b00011;
            inline constexpr uint8_t LeftTick2  = 0b00001;
            inline constexpr uint8_t RightTick1 = 0b11000;
            inline constexpr uint8_t RightTick2 = 0b10000;
        }
    }

    namespace Ui {
        namespace Pins {
           inline constexpr uint8_t PinButton = 4; 
           inline constexpr uint8_t PinSda = 21;
           inline constexpr uint8_t PinScl = 22;
        }        
        namespace Lcd {
            inline constexpr uint8_t Address = 0x27;
            inline constexpr uint8_t Cols = 16;
            inline constexpr uint8_t Rows = 2;
        }
    }

    namespace Driver {
        namespace Pins {
            inline constexpr uint8_t LIN1 = 12;
            inline constexpr uint8_t LIN2 = 13;
            inline constexpr uint8_t PWML = 25;
            inline constexpr uint8_t RIN1 = 26;
            inline constexpr uint8_t RIN2 = 27;
            inline constexpr uint8_t PWMR = 33;
            inline constexpr uint8_t STBY = 14;
        }
    }

    namespace Encoder {
        namespace Pins {
            inline constexpr uint8_t LeftA = 5;
            inline constexpr uint8_t LeftB = 19;
            inline constexpr uint8_t RightA = 18;
            inline constexpr uint8_t RightB = 3;
        }
        inline constexpr float GearRatio = 280.0f / 60000.0f;
    }

    namespace Timer {
        inline constexpr uint32_t Frequency = 1000000;
        inline constexpr bool AutoReload = true;
        inline constexpr uint32_t ReloadCount = 0;
        inline constexpr uint32_t Period = 5000;
    }

    namespace Pid {
        inline constexpr float Kp = 50;
        inline constexpr float Ki = 0;
        inline constexpr float Kd = 0;
    }

    namespace Navigation {
        inline constexpr uint32_t BaseRpm = 450;
        inline constexpr float BasePulseCount = (BaseRpm * (Config::Timer::Period / 1000.0f)) * Config::Encoder::GearRatio; 
        enum class States {
            Reset,
            Countdown,
            Lap,
            Finish,
            Lost
        };
    }
}

#endif