#include "embedded/digitalSettings.hpp"
#include "logic/timestamp.hpp"

enum Mission {
    MANUAL,
    ACCELERATION,
    SKIDPAD,
    AUTOCROSS,
    TRACKDRIVE,
    EBS_TEST,
    INSPECTION
};

struct DigitalData {
    // Rear Left Wheel Speed Encoder
    double _left_wheel_rpm;
    bool last_lwss_state = 0;
    int pulse_count = 0;
    Timestamp left_wheel_update_ts;

    double pneumatic_line_pressure;
    bool asms_on;
    bool aats_on;
    bool watchdog_state;
    Mission mission; // TODO(andre) change later to State Class maybe

    // TODO(andre): organize vars order

    DigitalData();
    void digitalReads();

    void readLwss();
    void readPneumaticLine();
    void readMission();
    void readAsmsSwitch();
    void readAatsSwitch();
    void readWatchdog();

    void updateLeftWheelRpm();
};

DigitalData::DigitalData() : _left_wheel_rpm(0), pneumatic_line_pressure(0),
        aats_on(false), asms_on(false), watchdog_state(0) {
            pinMode(LWSS_PIN, INPUT);
        }

void DigitalData::updateLeftWheelRpm() {
    _left_wheel_rpm = pulse_count / (WHEEL_MEASUREMENT_INTERVAL_MIN * PULSES_PER_ROTATION);

    pulse_count = 0;
    left_wheel_update_ts.update();
}

void DigitalData::digitalReads() {
    readLwss();
    readPneumaticLine();
    readMission();
    readAsmsSwitch();
    readAatsSwitch();
    readWatchdog();
}

void DigitalData::readLwss() {
    bool current_lwss_state = digitalRead(LWSS_PIN);
  
    if (current_lwss_state == HIGH && last_lwss_state == LOW)
        pulse_count++;

    last_lwss_state = current_lwss_state;

    if (left_wheel_update_ts.hasTimedOut(WHEEL_MEASUREMENT_INTERVAL_MIN))
        updateLeftWheelRpm();
}

void DigitalData::readPneumaticLine() {
    // TODO(andre): wait for eletro indications
}

void DigitalData::readMission() {
    // Enum value attributed considering the True Boolean Value
    Mission mission = static_cast<Mission>(
        digitalRead(MISSION_MANUAL_PIN) * MANUAL |
        digitalRead(MISSION_ACCELERATION_PIN) * ACCELERATION |
        digitalRead(MISSION_SKIDPAD_PIN) * SKIDPAD |
        digitalRead(MISSION_AUTOCROSS_PIN) * AUTOCROSS |
        digitalRead(MISSION_TRACKDRIVE_PIN) * TRACKDRIVE |
        digitalRead(MISSION_EBSTEST_PIN) * EBS_TEST |
        digitalRead(MISSION_INSPECTION_PIN) * INSPECTION
    );
}

void DigitalData::readAsmsSwitch() {
    asms_on = digitalRead(ASMS_SWITCH_PIN);
}

void DigitalData::readAatsSwitch() {
    aats_on = digitalRead(AATS_SWITCH_PIN);
}

void DigitalData::readWatchdog() {
    // TODO(andre): ask first, then read
}