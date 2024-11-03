#pragma once

/**
 * @brief Sensors from CAN
 */
struct Sensors
{
    double _right_wheel_rpm = 0;
    double _left_wheel_rpm = 0;
    int _hydraulic_line_pressure = 0;
};
