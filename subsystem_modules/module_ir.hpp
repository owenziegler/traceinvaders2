/*
module_ir.h

Owen Ziegler
3/19/2025

Description:
IR Sensor Header File. Contains declarations of all IR sensor functions
*/
#ifndef MODULE_IR_HPP
#define MODULE_IR_HPP

#include <stdint.h>
/*
IR Sensor Setup function
Sets IR pins to INPUT mode
Inputs: None
Outputs: None
*/
void irSetup();

/*
Line State function
Returns the state of the line sensor array, packed into the five least significant bits of a byte
Inputs: None
Outputs: uint8_t lineState
*/
uint8_t getLineState();

#endif
