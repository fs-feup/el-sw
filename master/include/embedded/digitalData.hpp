#pragma once

#include "logic/timestamp.hpp"

struct DigitalData {
    // Rear Left Wheel Speed Encoder
    double _left_wheel_rpm = 0;
    bool last_lwss_state = false;
    int pulse_count = 0;
    Timestamp left_wheel_update_ts;

    // Watchdog
    bool watchdog_state = true; // starts true until false
    bool watchdog_comm_state = false;
    Timestamp wd_pulse_ts;

    // Other reads
    bool pneumatic_line_pressure = true;
    bool asms_on = false;
    bool aats_on = false;
};

