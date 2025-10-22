#include <Arduino.h>
#include <cstdint>
#include <config.hpp>
#include <module_navigation.hpp>

Navigation::Navigation(volatile uint8_t* irState, float* targetPulseCountLeft, float* targetPulseCountRight) {
    _irState = irState;
    _targetPulseCountLeft = targetPulseCountLeft;
    _targetPulseCountRight = targetPulseCountRight;

    state = __NAV_STATE::RESET;
    nextState = __NAV_STATE::RESET;
}

void Navigation::navigate() {
    _followLine();
}

void Navigation::_handleRunup() {
    //follow line
}

void Navigation::_handleStraight() {
    //follow line
}

void Navigation::_handleLost() {
    
}

void Navigation::_handleFinish() {

}

void Navigation::_followLine() {
    int8_t state;
    try {
        state = __IR_STATES[(uint8_t)*_irState];
    }
    catch (int ex) {
        *_targetPulseCountLeft = 0.5 * __NAV_BASE_SPEED;
        *_targetPulseCountRight = 0.5 * __NAV_BASE_SPEED;
    }
    
    //float outputL = (float)state * 0.125 + 0.5;
    //float outputR = (float)state * -0.125 + 0.5;

    float outputL = (-1.0 / 64.0) * (float)state * (float)state + (1.0 / 8.0) * (float)state + 0.75;
    float outputR = (-1.0 / 64.0) * (float)state * (float)state - (1.0 / 8.0) * (float)state + 0.75;

    *_targetPulseCountLeft = outputL * __NAV_BASE_SPEED;
    *_targetPulseCountRight = outputR * __NAV_BASE_SPEED;
}
