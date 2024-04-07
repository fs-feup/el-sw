// ReSharper disable CppMemberFunctionMayBeConst
#pragma once

#include <Bounce2.h>
#include <model/digitalData.hpp>
#include <model/structure.hpp>

#include "digitalSettings.hpp"

#define DEBOUNCE_INTERVAL 10
#define PRESSED_STATE LOW

class DigitalReceiver {
public:
    static double _current_left_wheel_rpm; // class var to keep digital data non-static
    static unsigned long last_wheel_pulse_ts;

    void digitalReads();
    static void updateLeftWheelRpm(); 

    DigitalReceiver(DigitalData *digitalData, Mission *mission)
        : digitalData(digitalData), mission(mission) {
        pinMode(SDC_STATE_PIN, INPUT);
        pinMode(SDC_LOGIC_WATCHDOG_IN_PIN, INPUT);
        pinMode(SDC_LOGIC_WATCHDOG_OUT_PIN, OUTPUT);

        asms_switch = newButton(ASMS_IN_PIN);

        attachInterrupt(digitalPinToInterrupt(LWSS_PIN), DigitalReceiver::updateLeftWheelRpm, RISING);
    }

private:
    DigitalData *digitalData;
    Mission *mission;

    Button asms_switch;

    Button newButton(uint8_t pin);

    void readLwss();
    void readPneumaticLine();
    void readMission();
    void readAsmsSwitch();
    void readAatsState();
    void readWatchdog();

};

double DigitalReceiver::_current_left_wheel_rpm = 0;
unsigned long DigitalReceiver::last_wheel_pulse_ts = millis();

inline Button DigitalReceiver::newButton(uint8_t pin) {
    Button button;
    button.attach(pin, INPUT_PULLUP);
    button.interval(DEBOUNCE_INTERVAL);
    button.setPressedState(PRESSED_STATE);

    return button;
}

inline void DigitalReceiver::updateLeftWheelRpm() {
    // rpm = 1 / ([dT seconds] * No. Pulses in Rotation) * [60 seconds]
    unsigned long time_interval_s = (millis() - last_wheel_pulse_ts) * 1e-3;
    _current_left_wheel_rpm = 1 / (time_interval_s * PULSES_PER_ROTATION) * 60;  
    last_wheel_pulse_ts = millis(); // refresh timestamp
}

inline void DigitalReceiver::digitalReads() {
    readPneumaticLine();
    readMission();
    readAsmsSwitch();
    readAatsState();
    readWatchdog();
    digitalData->_left_wheel_rpm = _current_left_wheel_rpm;
}

inline void DigitalReceiver::readPneumaticLine() {
    bool pneumatic1 = digitalRead(SENSOR_PRESSURE_1_PIN);
    bool pneumatic2 = digitalRead(SENSOR_PRESSURE_2_PIN);

    digitalData->pneumatic_line_pressure = pneumatic1 && pneumatic2; // both need to be True
}

inline void DigitalReceiver::readMission() {
    // Enum value attributed considering the True Boolean Value
    *mission = static_cast<Mission>(
        digitalRead(MISSION_MANUAL_PIN) * MANUAL |
        digitalRead(MISSION_ACCELERATION_PIN) * ACCELERATION |
        digitalRead(MISSION_SKIDPAD_PIN) * SKIDPAD |
        digitalRead(MISSION_AUTOCROSS_PIN) * AUTOCROSS |
        digitalRead(MISSION_TRACKDRIVE_PIN) * TRACKDRIVE |
        digitalRead(MISSION_EBSTEST_PIN) * EBS_TEST |
        digitalRead(MISSION_INSPECTION_PIN) * INSPECTION);
}

inline void DigitalReceiver::readAsmsSwitch() {
    asms_switch.update();
    if (asms_switch.pressed())
        digitalData->asms_on = true;
    else
        digitalData->asms_on = false;
}

inline void DigitalReceiver::readAatsState() {
    // TS is on if SDC is closed (SDC STATE PIN AS HIGH)
    digitalData->sdcState_OPEN = !digitalRead(SDC_STATE_PIN);
}

inline void DigitalReceiver::readWatchdog() {
    digitalData->watchdog_state = digitalRead(SDC_LOGIC_WATCHDOG_IN_PIN);
    if (digitalData->watchdog_state) {
        digitalData->watchdogTimestamp.reset();
    }
}
