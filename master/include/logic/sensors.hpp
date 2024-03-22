#pragma once

struct Sensors {
    double _rl_wheel_rpm = 0;
    double _hydraulic_line_pressure = 0;

    void updateRL(double value);

    void updateHydraulic(double value);
};

void Sensors::updateRL(double value) {
    // check what it receives and convert to rpm
    _rl_wheel_rpm = value;
}

void Sensors::updateHydraulic(double value) {
    // check what it receives and convert to rpm
    _hydraulic_line_pressure = value;
}
