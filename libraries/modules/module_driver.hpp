/*
module_driver.hpp

Joel Osho & Owen Ziegler
3/19/2025

Description:
Header file for motor driver code, contains declaration of all motor driver functions
*/
#ifndef MODULE_DRIVER_HPP
#define MODULE_DRIVER_HPP

/*

*/
void driverSetup();
void setSpeed(float left, float right);
void setMotorL(int speed);
void setMotorR(int speed);

void drive(int left, int right);

void left();

void right();

void reverse();

void nudgeLeft();

void nudgeRight();

#endif
