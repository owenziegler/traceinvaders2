#ifndef MODULE_NAVIGATION_HPP
#define MODULE_NAVIGATION_HPP

#include <cstdint>
#include <config.hpp>
#include <module_lcd.hpp>

class Navigation {
    public:
        Navigation(volatile uint8_t* irState, float* targetPulseCountLeft, float* targetPulseCountRight, volatile float* baseSpeed);
        void navigate(); //entry point for all navigation, state machine switcher
        __NAV_STATE state;
        __NAV_STATE nextState;
    private:
        volatile uint8_t* _irState;
        float* _targetPulseCountLeft;
        float* _targetPulseCountRight;
        volatile float* _baseSpeed;
        int _finishCounter;
        int _countdown;
        int _timer;
        Lcd *_lcd;
        void _handleReset();
        void _handleRunup();
        void _handleCountdown();
        void _handleLap();
        void _handleLost();
        void _handleFinish();
        void _followLine(); //ir state to motor control
        bool _checkTick();
};

#endif