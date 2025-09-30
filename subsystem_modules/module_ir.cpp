/*
module_ir.c

Owen Ziegler
2025 02 26

Description:
IR sensor module for Trace Invaders linebot. Contains functions pertaining to the IR sensor module.
*/

#include "module_ir.hpp"
#include <Arduino.h>
#include <cstdint>
#include "config.hpp"

void irSetup() {
  pinMode(_IR_PIN_0, INPUT);
  pinMode(_IR_PIN_1, INPUT);
  pinMode(_IR_PIN_2, INPUT);
  pinMode(_IR_PIN_3, INPUT);
  pinMode(_IR_PIN_4, INPUT);
}

uint8_t getLineState() {
  uint8_t lineState = 0;
  lineState |= (digitalRead(_IR_PIN_0) << 4);
  lineState |= (digitalRead(_IR_PIN_1) << 3);
  lineState |= (digitalRead(_IR_PIN_2) << 2);
  lineState |= (digitalRead(_IR_PIN_3) << 1);
  lineState |= (digitalRead(_IR_PIN_4) << 0);
  return lineState;
}