#pragma once

#define HYDRAULIC_LINE_ACTIVE_PRESSURE 100

/**
 * @brief Sensors from CAN
*/
struct Sensors {
    double _rl_wheel_rpm = 0;
    double _hydraulic_line_pressure = 0;
};
