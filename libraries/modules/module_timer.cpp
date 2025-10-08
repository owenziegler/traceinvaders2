/*
module_timer.cpp

Owen Ziegler
2025 04 09

Description:
Implementation file for timer module functions
*/

#include "module_timer.hpp"
#include <Arduino.h>
#include <stdint.h>

//Constructor, sets starting time (milliseconds since power-up)
Timer::Timer() {
  Serial.print("Initializing timer... ");
  start = 0;
  Serial.println("Initialized successfully");
}

void Timer::timerStart() {
  start = millis();
}

float Timer::timerLap() {
  unsigned long end = millis() - start;
  return static_cast<float>(end / 10) / 100;
}
