#pragma once

#include "metro.h"
#include "embedded/digitalSettings.hpp"


struct DigitalData {
    // Rear Left Wheel Speed Encoder
    // double _left_wheel_rpm = 0;

    // Other reads
    bool pneumatic_line_pressure = true;
    bool pneumatic_line_pressure_1 = false;
    bool pneumatic_line_pressure_2 = false;
    bool asms_on = false;
    bool sdcState_OPEN{true}; /*< Detects AATS >*/ 
};

