#pragma once

#include "metro.h"
#include "embedded/digitalSettings.hpp"

struct DigitalData
{
    // Rear Left Wheel Speed Encoder
    // double _left_wheel_rpm = 0;

    // Other reads
    bool pneumatic_line_pressure_ = true;
    bool pneumatic_line_pressure_1_ = false;
    bool pneumatic_line_pressure_2_ = false;
    bool asms_on_ = false;
    bool sdc_open_{true}; /*< Detects AATS >*/
};
