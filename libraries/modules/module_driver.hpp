/*
module_driver.hpp

Joel Osho & Owen Ziegler
3/19/2025

Description:
Header file for motor driver code, contains declaration of all motor driver functions
*/
#ifndef MODULE_DRIVER_HPP
#define MODULE_DRIVER_HPP

#include <cstdint>

class Motor {
    public:
        Motor(uint8_t in1, uint8_t in2, uint8_t pwm);
        void drive(int pwm);
    private:
        uint8_t _in1, _in2, _pwm;
};

class Driver {
    public:
        Driver();
        void drive(int pwmL, int pwmR);
    private:
        Motor _left;
        Motor _right;
};

#endif