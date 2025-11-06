/*
module_driver.cpp

Joel Osho & Owen Ziegler
3/19/2025

Description:
Motor driver functions for Line Follower robot. Contains definitions of all motor driver-related functions
*/

#include <Arduino.h>
#include <cstdint>

#include <module_driver.hpp>
#include <config.hpp>

Motor::Motor(uint8_t in1, uint8_t in2, uint8_t pwm)
  : _in1(in1), _in2(in2), _pwm(pwm) {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(pwm, OUTPUT);
  //set forward direction
  setForward();
}

void Motor::drive(uint8_t pwm) {
  analogWrite(_pwm, constrain(pwm, 0, 255));
}

void Motor::setForward() {
  digitalWrite(_in1, HIGH);
  digitalWrite(_in2, LOW);
}

void Motor::setBackward() {
  digitalWrite(_in1, LOW);
  digitalWrite(_in2, HIGH);
}

Driver::Driver() 
  : _left(__DRIVER_PIN_LIN1, __DRIVER_PIN_LIN2, __DRIVER_PIN_PWML),
    _right(__DRIVER_PIN_RIN1, __DRIVER_PIN_RIN2, __DRIVER_PIN_PWMR) {
  Serial.print("Setting standby pin mode... ");
  pinMode(__DRIVER_PIN_STBY, OUTPUT);
  Serial.println("Set successfully");
  Serial.print("Setting standby pin to HIGH... ");
  digitalWrite(__DRIVER_PIN_STBY, HIGH);
  Serial.println("Set successfully");

}

void Driver::drive(uint8_t pwmL, uint8_t pwmR) {
  _left.drive(pwmL);
  _right.drive(pwmR);
}

void Driver::setForward() {
  _left.setForward();
  _right.setForward();
}

void Driver::setBackward() {
  _left.setBackward();
  _right.setBackward();
}