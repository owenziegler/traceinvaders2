#ifndef MODULE_ENCODER_HPP
#define MODULE_ENCODER_HPP

#include <Arduino.h>
#include <stdint.h>

class Encoder {
  public:
    Encoder(portMUX_TYPE &mux, uint8_t encoderPin);
    void ARDUINO_ISR_ATTR onPulse();
    int getPulseCount();
    uint8_t encoderPin;
  private:
    volatile uint32_t _pulseCount;
    portMUX_TYPE mux;
};

#endif