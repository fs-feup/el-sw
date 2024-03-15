struct Sensors {
    double _rl_wheel_rpm;
    double _hydraulic_line_pressure;

    Sensors();
    void updateRL(double value);
    void updateHydraulic(double value);
};

Sensors::Sensors() : _rl_wheel_rpm(0), _hydraulic_line_pressure(0) {}

void Sensors::updateRL(double value){
    // check what it receives and convert to rpm
    _rl_wheel_rpm = value;
}
void Sensors::updateHydraulic(double value){
    // check what it receives and convert to rpm
    _hydraulic_line_pressure = value;
}
