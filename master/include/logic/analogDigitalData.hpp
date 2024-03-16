struct AnalogDigitalData {
    double _rl_wheel_rpm;
    double pneumatic_line_pressure;
    bool asms_on;
    bool aats_on;
    bool watchdog_state;

    AnalogDigitalData();
};

AnalogDigitalData::AnalogDigitalData() : _rl_wheel_rpm(0), pneumatic_line_pressure(0),
        aats_on(false), asms_on(false), watchdog_state(0) {}