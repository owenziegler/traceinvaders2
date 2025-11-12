#include <Arduino.h>
#include <cstdint>
#include <config.hpp>
//#include <module_navigation.hpp>
#include "module_navigation_test.hpp"
#include <module_lcd.hpp>

Navigation::Navigation(volatile uint8_t* irState, float* targetPulseCountLeft, float* targetPulseCountRight, volatile float* baseSpeed) {
    stop = false;
    timesum = 0;
    count = 0;
    _irState = irState;
    _targetPulseCountLeft = targetPulseCountLeft;
    _targetPulseCountRight = targetPulseCountRight;
    _baseSpeed = baseSpeed;
    _finishCounter = 0;
    _lostCounter = 0;
    _countdown = 0;
    _timer = 0;
    _lcd = new Lcd(Config::Ui::Lcd::Address, "Trace Invader", "Get Ready!");

    state = Config::Navigation::States::Reset;
    nextState = Config::Navigation::States::Reset;
}

void Navigation::navigate() {
    //state machine
    switch(state) {
        case Config::Navigation::States::Reset:
            _handleReset();
            break;
        case Config::Navigation::States::Countdown:
            _handleCountdown();
            break;
        case Config::Navigation::States::Lap:
            _handleLap();
            break;
        case Config::Navigation::States::Finish:
            _handleFinish();
            break;
        case Config::Navigation::States::Lost:
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
    if(_countdown >= 5000000 / Config::Timer::Period) {
        nextState = Config::Navigation::States::Lap;
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
    if(_countdown <= 5000000 / Config::Timer::Period) {
        _countdown++;
    }
    //display to LCD
    else {
        if(_checkTick()) {
            nextState = Config::Navigation::States::Finish;
            stop = true;
            _timer = millis() - _timer;
            //_lcd->display("Time:", String(((float)_timer) / 1000.0));
            _lcd->display(String((float)timesum/(float)count), "");
        }
        else if(_checkLost()) {
            nextState = Config::Navigation::States::Lost;
            _lcd->display("LOST", "");
        }
    }
}

void Navigation::_handleFinish() {
    *_targetPulseCountRight = 0;
    *_targetPulseCountLeft = 0;
}

void Navigation::_handleLost() {
    auto i = Config::Ir::States.find((uint8_t)*_irState);

    if (i != Config::Ir::States.end()) {
        _lostCounter++;
    } else {
        _lostCounter = 0;
    }

    *_targetPulseCountRight = -0.25 * *_baseSpeed;
    *_targetPulseCountLeft  = -0.25 * *_baseSpeed;

    if(_lostCounter == 20) {
        _lcd->display("Running", "");
        nextState = Config::Navigation::States::Lap
        _lostCounter = 0;
    }
}

void Navigation::_followLine() {
    auto i = Config::Ir::States.find((uint8_t)*_irState);
    float outputL = 0;
    float outputR = 0;

    if (i != Config::Ir::States.end()) {
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
    if(*_irState == Config::Ir::SpecialStates::RightTick1 || *_irState == Config::Ir::SpecialStates::RightTick2) {
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
    if(*_irState == Config::Ir::SpecialStates::NoLine) {
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

