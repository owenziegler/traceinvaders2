/*
module_driver.cpp

Joel Osho & Owen Ziegler
3/19/2025

Description:
Motor driver functions for Line Follower robot. Contains definitions of all motor driver-related functions
*/

#include "module_driver.hpp"
#include <Arduino.h>
#include "config.hpp"


void driverSetup() {
  pinMode(_DRIVER_PIN_LIN1, OUTPUT);
  pinMode(_DRIVER_PIN_LIN2, OUTPUT);
  pinMode(_DRIVER_PIN_PWML, OUTPUT);
  pinMode(_DRIVER_PIN_RIN1, OUTPUT);
  pinMode(_DRIVER_PIN_RIN2, OUTPUT);
  pinMode(_DRIVER_PIN_PWMR, OUTPUT);
  pinMode(_DRIVER_PIN_STBY, OUTPUT);
  digitalWrite(_DRIVER_PIN_STBY, HIGH);
}

// Set up motors
void setSpeed(float left, float right) {
  left = constrain(left,-1,1);
  right = constrain(right,-1,1);
  setMotorL((int)(left * _DRIVER_BASE_SPEED));
  setMotorR((int)(right * _DRIVER_BASE_SPEED));
}

void setMotorL(int speed) {
  if (speed >= 0) {
    digitalWrite(_DRIVER_PIN_LIN1, HIGH);
    digitalWrite(_DRIVER_PIN_LIN2, LOW);
  } else {
    digitalWrite(_DRIVER_PIN_LIN1, LOW);
    digitalWrite(_DRIVER_PIN_LIN2, HIGH);
    speed = -speed;
  }
  analogWrite(_DRIVER_PIN_PWML, constrain(speed, 0, 255));
}

void setMotorR(int speed) {
  if (speed >= 0) {
    digitalWrite(_DRIVER_PIN_RIN1, HIGH);
    digitalWrite(_DRIVER_PIN_RIN2, LOW);
  } else {
    digitalWrite(_DRIVER_PIN_RIN1, LOW);
    digitalWrite(_DRIVER_PIN_RIN2, HIGH);
    speed = -speed;
  }
  analogWrite(_DRIVER_PIN_PWMR, constrain(speed, 0, 255));
}

void nudgeLeft(int baseSpeed, int nudgeAmount) {
  setMotorL(baseSpeed - nudgeAmount);  // Left motor slower
  setMotorR(baseSpeed);   // Right motor normal
}

void nudgeRight(int baseSpeed, int nudgeAmount) {
  setMotorL(baseSpeed);                // Left motor normal
  setMotorR(baseSpeed - nudgeAmount);   // Right motor slower
  }
/*
void loop() {
  int left = digitalRead(SENSOR_LEFT);
  int center = digitalRead(SENSOR_CENTER);
  int right = digitalRead(SENSOR_RIGHT);

  int baseSpeed = 150;
  int nudgeAmount = 30;  // Adjustable

// Drive forward
  if (center == HIGH && left == LOW && right == LOW) {
    setMotorA(baseSpeed);
    setMotorB(baseSpeed);
  } 
// Left nudge
  else if (left == HIGH && center == LOW && right == LOW) {
    nudgeLeft(baseSpeed, nudgeAmount);
  } 
// Right nude
  else if (right == HIGH && center == LOW && left == LOW) {
    nudgeRight(baseSpeed, nudgeAmount);
  } 
// More turn left
  else if (left == HIGH && center == HIGH && right == LOW) {
    setMotorA(baseSpeed / 2);
    setMotorB(baseSpeed);
  } 
// More turn right
  else if (right == HIGH && center == HIGH && left == LOW) {
    setMotorA(baseSpeed);
    setMotorB(baseSpeed / 2);
  }
  // Lost line or intersection — stop or search (go in reverse)
  else {
    setMotorA(0);
    setMotorB(0);
    delay(2);
    setMotorA(-55);
    setMotorB(-55);
  }
}*/
