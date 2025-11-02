#include <Arduino.h>
#include <cstdint>
#include <config.hpp>
#include <module_navigation.hpp>
#include <module_lcd.hpp>

Navigation::Navigation(volatile uint8_t* irState, float* targetPulseCountLeft, float* targetPulseCountRight, volatile float* baseSpeed) {
    _irState = irState;
    _targetPulseCountLeft = targetPulseCountLeft;
    _targetPulseCountRight = targetPulseCountRight;
    _baseSpeed = baseSpeed;
    _finishCounter = 0;
    _countdown = 0;
    _timer = 0;
    _lcd = new Lcd(__I2C_ADDR_LCD, "Trace Invader", "Get Ready!");

    state = __NAV_STATE::RESET;
    nextState = __NAV_STATE::RESET;
}

void Navigation::navigate() {
    //state machine
    switch(state) {
        case __NAV_STATE::RESET:
            _handleReset();
            break;
        case __NAV_STATE::RUNUP:
            _handleRunup();
            break;
        case __NAV_STATE::COUNTDOWN:
            _handleCountdown();
            break;
        case __NAV_STATE::LAP:
            _handleLap();
            break;
        case __NAV_STATE::FINISH:
            _handleFinish();
            break;
        case __NAV_STATE::LOST:
            _handleLost();
            break;
    }
    if(nextState != state) {
        state = nextState;
    }
    Serial.print(" ");
    Serial.print(*_targetPulseCountLeft);
    Serial.print(" ");
    Serial.println(*_targetPulseCountRight);    
}

void Navigation::_handleReset() {
    Serial.print("RESET");
    *_targetPulseCountRight = 0;
    *_targetPulseCountLeft = 0;
}

void Navigation::_handleRunup() {
    Serial.print("RUNUP");
    _followLine();
    if(_checkTick()) {
        nextState = __NAV_STATE::COUNTDOWN;
    }
}

void Navigation::_handleCountdown() {
    Serial.print("COUNTDOWN");
    *_targetPulseCountRight = 0;
    *_targetPulseCountLeft = 0;
    if(_countdown >= 5000000 / __TIMER_PERIOD) {
        nextState = __NAV_STATE::LAP;
        _lcd->display("Running", "");
        _timer = millis();
        _countdown = 0;
    }
    else {
        switch(_countdown) {
            case 0:
                _lcd->display("5", ""); 
                break;
            case 200:
                _lcd->display("4", ""); 
                break;
            case 400:
                _lcd->display("3", ""); 
                break;
            case 600:
                _lcd->display("2", ""); 
                break;
            case 800:
                _lcd->display("1", ""); 
                break;
        }
        _countdown++;
    }
}

void Navigation::_handleLap() {
    Serial.print("LAP");
    _followLine();
    _countdown++;
    if(_countdown <= 5000000 / __TIMER_PERIOD) {
        _countdown++;
    }
    //display to LCD
    else {
        if(_checkTick()) {
            nextState = __NAV_STATE::FINISH;
            _timer = millis() - _timer;
            _lcd->display("Time:", String(((float)_timer) / 1000.0));
        }
    }
}

void Navigation::_handleFinish() {
    Serial.print("FINISH");
    *_targetPulseCountRight = 0;
    *_targetPulseCountLeft = 0;
}

void Navigation::_handleLost() {
    *_targetPulseCountRight = -20;
    *_targetPulseCountLeft = -20;
}

void Navigation::_followLine() {
    int8_t irState;
    try {
        irState = __IR_STATES[(uint8_t)*_irState];
    }
    catch (int ex) {
        *_targetPulseCountLeft = 0.5 * (*_baseSpeed);
        *_targetPulseCountRight = 0.5 * (*_baseSpeed);
    }

    //first degree polynomial
    //float outputL = (float)state * 0.125 + 0.5;
    //float outputR = (float)state * -0.125 + 0.5;

    //second degree polynomial
    //float outputL = (-1.0 / 64.0) * (float)irState * (float)irState + (1.0 / 8.0) * (float)irState + 0.75;
    //float outputR = (-1.0 / 64.0) * (float)irState * (float)irState - (1.0 / 8.0) * (float)irState + 0.75;

    //improved second degree polynomial
    float outputL = (-1.0 / 24.0) * pow((float)irState - 1, 2) + 1;
    float outputR = (-1.0 / 24.0) * pow((float)irState + 1, 2) + 1;

    *_targetPulseCountLeft = outputL * (*_baseSpeed);
    *_targetPulseCountRight = outputR * (*_baseSpeed);
}

bool Navigation::_checkTick() {
    if(*_irState == 0b00011000 || *_irState == 0b00010000) {
        _finishCounter++;
        if(_finishCounter == 8) {
            return true;
            _finishCounter == 0;
        }
        else {
            return false;
        }
    }
    else if (_finishCounter != 0) {
        _finishCounter = 0;
        return false;
    }
}
