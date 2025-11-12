#ifndef MODULE_NAVIGATION_TEST_HPP
#define MODULE_NAVIGATION_TEST_HPP

#include <cstdint>
#include <config.hpp>
#include <module_lcd.hpp>

class Navigation {
    public:
        Navigation(volatile uint8_t* irState, float* targetPulseCountLeft, float* targetPulseCountRight, volatile float* baseSpeed);
        void navigate(); //entry point for all navigation, state machine switcher
        Config::Navigation::States state;
        Config::Navigation::States nextState;
        bool stop;
        unsigned long timesum;
        unsigned long count;
    private:
        volatile uint8_t* _irState;
        float* _targetPulseCountLeft;
        float* _targetPulseCountRight;
        volatile float* _baseSpeed;
        int _finishCounter;
        int _lostCounter;
        int _countdown;
        int _timer;
        Lcd *_lcd;
        void _handleReset();
        void _handleCountdown();
        void _handleLap();
        void _handleLost();
        void _handleFinish();
        void _followLine(); //ir state to motor control
        bool _checkTick();
        bool _checkLost();
};

#endif