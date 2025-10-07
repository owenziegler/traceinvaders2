/*
module_ir.c

Owen Ziegler
2025 02 26

Description:
IR sensor module for Trace Invaders linebot. Contains functions pertaining to the IR sensor module.
*/

#include <Arduino.h>
#include <cstdint>

#include <module_ir.hpp>
#include <config.hpp>

IrArray::IrArray() {
  Serial.println("Initializing IR Sensor Array...");

  Serial.print("Setting pin modes... ");
  pinMode(__IR_PIN_0, INPUT); Serial.print("pin0 ");
  pinMode(__IR_PIN_1, INPUT); Serial.print("pin1 ");
  pinMode(__IR_PIN_2, INPUT); Serial.print("pin2 ");
  pinMode(__IR_PIN_3, INPUT); Serial.print("pin3 ");
  pinMode(__IR_PIN_4, INPUT); Serial.print("pin4 ");

  Serial.println("All IR sensor pins set successfully.");
}

uint8_t IrArray::getLineState() {
  uint8_t lineState = 0;
  lineState |= (digitalRead(__IR_PIN_0) << 4);
  lineState |= (digitalRead(__IR_PIN_1) << 3);
  lineState |= (digitalRead(__IR_PIN_2) << 2);
  lineState |= (digitalRead(__IR_PIN_3) << 1);
  lineState |= (digitalRead(__IR_PIN_4) << 0);
  return lineState;
}