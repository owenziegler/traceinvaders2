/*

*/

#include <Arduino.h>
#include <config.hpp>
#include <stdint.h>
#include <module_encoder.hpp>

//Trampoline function: allows us to use a class method as an ISR
void ARDUINO_ISR_ATTR _encoderIsrHandler(void *arg) {
    static_cast<Encoder*>(arg)->onPulse();
}

Encoder::Encoder(portMUX_TYPE &mux, uint8_t encoderPin) : _pulseCount(0), mux(mux), encoderPin(encoderPin) {
    Serial.print("Initializing encoder on pin ");
    Serial.print(encoderPin);
    Serial.println("...");
    
    Serial.print("Setting pin mode... ");
    pinMode(encoderPin, INPUT);
    Serial.println("Pin mode set successfully."); 
    
    Serial.print("Attaching ISR to pin... ");
    attachInterruptArg(
        digitalPinToInterrupt(encoderPin), 
        &_encoderIsrHandler,
        this,
        RISING
    );
    Serial.println("Interrupt attached successfully.");

    Serial.println("Encoder initialized successfully.");
}

void ARDUINO_ISR_ATTR Encoder::onPulse() {
    portENTER_CRITICAL_ISR(&mux);
    _pulseCount++;
    portEXIT_CRITICAL_ISR(&mux);
}

int Encoder::getPulseCount() {
    portENTER_CRITICAL_ISR(&mux);
    int result = _pulseCount;
    portEXIT_CRITICAL_ISR(&mux);
    return result;
}

void Encoder::resetPulseCount() {
    portENTER_CRITICAL_ISR(&mux);
    _pulseCount = 0;
    portEXIT_CRITICAL_ISR(&mux);
}


