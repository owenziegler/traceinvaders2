#ifndef MODULE_NAVIGATION_HPP
#define MODULE_NAVIGATION_HPP

#include <cstdint>
#include <config.hpp>

class Navigation {
    public:
        Navigation(volatile uint8_t* irState, float* targetPulseCountLeft, float* targetPulseCountRight);
        void navigate(); //entry point for all navigation, state machine switcher
        __NAV_STATE state;
        __NAV_STATE nextState;
    private:
        volatile uint8_t* _irState;
        float* _targetPulseCountLeft;
        float* _targetPulseCountRight;
        void _handleRunup();
        void _handleStraight();
        void _handleLost();
        void _handleFinish();
        void _followLine(); //ir state to motor control
};

#endif