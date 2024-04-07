#pragma once

#include <Metro.h>
#include "embedded/digitalSettings.hpp"


struct DigitalData {
    // Rear Left Wheel Speed Encoder
    double _left_wheel_rpm = 0;

    // Watchdog
    Metro watchdogTimestamp{WATCHDOG_TIMEOUT};
    bool watchdog_state = true; // starts true until false

    // Other reads
    bool pneumatic_line_pressure = true;
    bool asms_on = false;
    bool sdcState_OPEN{true}; /*< Detects AATS >*/ 
};

