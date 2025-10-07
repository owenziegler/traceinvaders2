/*
module_ir.h

Owen Ziegler
3/19/2025

Description:
IR Sensor Header File. Contains declarations of all IR sensor functions
*/
#ifndef MODULE_IR_HPP
#define MODULE_IR_HPP

#include <cstdint>

class IrArray {
  public:
   IrArray();
   uint8_t getLineState(); 
};

#endif
