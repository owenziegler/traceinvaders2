/*
utils.cpp

Owen Ziegler

2025 03 24

Description:
Utility functions for trace-invaders project that don't belong to any specific module or subsystem.
*/
#include "utils.hpp"
#include "config.hpp"
#include <stdint.h>
#include <Arduino.h>

String uint8ToBinary(uint8_t value) {
    String binary = "";
    // Loop through each bit (from most significant to least significant)
    for (int i = 7; i >= 0; --i) {
        // Check if the bit at position i is set (1) or not (0)
        if ((value & (1 << i)) != 0) {
            binary += '1';
        } else {
            binary += '0';
        }
    }
    binary = binary + "\n";
    return binary;
}