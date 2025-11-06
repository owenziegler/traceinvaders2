#include <Arduino.h>
#include <cstdint>
#include <config.hpp>
#include <module_navigation.hpp>
#include <module_lcd.hpp>

Navigation::Navigation(volatile uint8_t* irState, float* targetPulseCountLeft, float* targetPulseCountRight, volatile float* baseSpeed) {
    stop = false;
    _irState = irState;
    _targetPulseCountLeft = targetPulseCountLeft;
    _targetPulseCountRight = targetPulseCountRight;
    _baseSpeed = baseSpeed;
    _finishCounter = 0;
    _lostCounter = 0;
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
}

void Navigation::_handleReset() {
    *_targetPulseCountRight = 0;
    *_targetPulseCountLeft = 0;
}

void Navigation::_handleCountdown() {
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
    _followLine();
    _countdown++;
    if(_countdown <= 5000000 / __TIMER_PERIOD) {
        _countdown++;
    }
    //display to LCD
    else {
        if(_checkTick()) {
            nextState = __NAV_STATE::FINISH;
            stop = true;
            _timer = millis() - _timer;
            _lcd->display("Time:", String(((float)_timer) / 1000.0));
        }
        else if(_checkLost()) {
            nextState = __NAV_STATE::LOST;
            _lcd->display("LOST", "");
        }
    }
}

void Navigation::_handleFinish() {
    *_targetPulseCountRight = 0;
    *_targetPulseCountLeft = 0;
}

void Navigation::_handleLost() {
    auto i = __IR_STATES.find((uint8_t)*_irState);

    if (i != __IR_STATES.end()) {
        _lostCounter++;
    } else {
        _lostCounter = 0;
    }

    *_targetPulseCountRight = -0.25 * *_baseSpeed;
    *_targetPulseCountLeft  = -0.25 * *_baseSpeed;

    if(_lostCounter == 20) {
        _lcd->display("Running", "");
        nextState = __NAV_STATE::LAP;
        _lostCounter = 0;
    }
}

void Navigation::_followLine() {
    auto i = __IR_STATES.find((uint8_t)*_irState);
    float outputL = 0;
    float outputR = 0;

    if (i != __IR_STATES.end()) {
        _lostCounter++;
        outputL = (-1.0 / 24.0) * pow((float)i->second - 2, 2) + 1;
        outputR = (-1.0 / 24.0) * pow((float)i->second + 2, 2) + 1;
    } else {
        return;
    }

    //first degree polynomial
    //float outputL = (float)state * 0.125 + 0.5;
    //float outputR = (float)state * -0.125 + 0.5;

    //second degree polynomial
    //float outputL = (-1.0 / 64.0) * (float)irState * (float)irState + (1.0 / 8.0) * (float)irState + 0.75;
    //float outputR = (-1.0 / 64.0) * (float)irState * (float)irState - (1.0 / 8.0) * (float)irState + 0.75;

    //improved second degree polynomial
    //float outputL = (-1.0 / 24.0) * pow((float)irState - 2, 2) + 1;
    //float outputR = (-1.0 / 24.0) * pow((float)irState + 2, 2) + 1;

    //third degree polynomial
    //float outputL = ( 27.0 / 2000.0) * pow((float)irState, 3) - (272.0 / 5000.0) * pow((float)irState, 2) + 1;
    //float outputR = (-27.0 / 2000.0) * pow((float)irState, 3) - (353.0 / 5000.0) * pow((float)irState, 2) + 1;

    *_targetPulseCountLeft = outputL * (*_baseSpeed);
    *_targetPulseCountRight = outputR * (*_baseSpeed);
}

bool Navigation::_checkTick() {
    if(*_irState == 0b00011000 || *_irState == 0b00010000) {
        _finishCounter++;
        if(_finishCounter == 5) {
            _finishCounter = 0;
            return true;
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

bool Navigation::_checkLost() {
    if(*_irState == 0b00011111) {
        _lostCounter++;
        if(_lostCounter == 50) {
            _lostCounter = 0;
            return true;
        }
        else {
            return false;
        }
    }
    else if (_lostCounter != 0) {
        _lostCounter = 0;
        return false;
    }
}

