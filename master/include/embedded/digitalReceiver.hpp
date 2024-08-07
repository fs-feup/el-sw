// ReSharper disable CppMemberFunctionMayBeConst
#pragma once

#include <Bounce2.h>
#include <model/digitalData.hpp>
#include <model/structure.hpp>

#include "digitalSettings.hpp"
#include "debugUtils.hpp"

/**
 * @brief Class responsible for the reading of the digital
 * inputs into the Master teensy 
*/
class DigitalReceiver {
public:
    static double _current_left_wheel_rpm; // class var to keep digital data non-static
    static unsigned long last_wheel_pulse_ts; // micros since last pulse

    /**
     * @brief read all digital inputs
    */
    void digitalReads();

    /**
     * @brief callback to update rl wheel rpm
    */
    static void updateLeftWheelRpm(); 

    /**
     * @brief Constructor for the class, sets pintmodes and buttons
    */
    DigitalReceiver(DigitalData *digitalData, Mission *mission)
        : digitalData(digitalData), mission(mission) {
        pinMode(SDC_STATE_PIN, INPUT);
        pinMode(SDC_LOGIC_WATCHDOG_IN_PIN, INPUT);
        pinMode(SDC_LOGIC_WATCHDOG_OUT_PIN, OUTPUT);
        pinMode(MISSION_ACCELERATION_PIN, INPUT);
        pinMode(MISSION_AUTOCROSS_PIN, INPUT);
        pinMode(MISSION_EBSTEST_PIN, INPUT);
        pinMode(MISSION_INSPECTION_PIN, INPUT);
        pinMode(MISSION_MANUAL_PIN, INPUT);
        pinMode(MISSION_SKIDPAD_PIN, INPUT);
        pinMode(MISSION_TRACKDRIVE_PIN, INPUT);
        pinMode(ASMS_IN_PIN, INPUT);
        pinMode(SENSOR_PRESSURE_1_PIN, INPUT);
        pinMode(SENSOR_PRESSURE_2_PIN, INPUT);

        // attachInterrupt(digitalPinToInterrupt(LWSS_PIN), DigitalReceiver::updateLeftWheelRpm, RISING);
    }

private:
    DigitalData *digitalData;
    Mission *mission;

    unsigned int asms_change_counter = 0; // counter to avoid noise
    unsigned int aats_change_counter = 0; // counter to avoid noise
    unsigned int pneumatic_change_counter = 0; // counter to avoid noise
    unsigned int mission_change_counter = 0; // counter to avoid noise
    Mission last_tried_mission_ = MANUAL;

    void readPneumaticLine();
    void readMission();
    void readAsmsSwitch();
    void readAatsState();
    void readWatchdog();

};

// double DigitalReceiver::_current_left_wheel_rpm = 0.0;
// unsigned long DigitalReceiver::last_wheel_pulse_ts = millis();

// inline void DigitalReceiver::updateLeftWheelRpm() {
//     // rpm = 1 / ([dT seconds] * No. Pulses in Rotation) * [60 seconds]
//     unsigned long now = micros();
//     unsigned long time_interval_s = (now - last_wheel_pulse_ts);
//     _current_left_wheel_rpm = 1 / (time_interval_s * 1e-6 * PULSES_PER_ROTATION) * 60;  
//     last_wheel_pulse_ts = now; // refresh timestamp
// //     DEBUG_PRINT_VAR(_current_left_wheel_rpm);
// }

inline void DigitalReceiver::digitalReads() {
    readPneumaticLine();
    readMission();
    readAsmsSwitch();
    readAatsState();
    readWatchdog();
    // digitalData->_left_wheel_rpm = _current_left_wheel_rpm;
}

inline void DigitalReceiver::readPneumaticLine() {
    bool pneumatic1 = digitalRead(SENSOR_PRESSURE_1_PIN);
    bool pneumatic2 = digitalRead(SENSOR_PRESSURE_2_PIN);
    // if (pneumatic1 == 0) {
    //     DEBUG_PRINT_VAR(digitalRead(SENSOR_PRESSURE_1_PIN));
    // }
    // if (pneumatic2 == 0) {
    //     DEBUG_PRINT_VAR(digitalRead(SENSOR_PRESSURE_2_PIN));
    // }
    bool temp_res = pneumatic1 && pneumatic2;

    // Only change the value if it has been different 5 times in a row
    pneumatic_change_counter = temp_res == digitalData->pneumatic_line_pressure ? 0 : pneumatic_change_counter + 1;
    if (pneumatic_change_counter >= DIGITAL_INPUT_COUNTER_LIMIT) {
        digitalData->pneumatic_line_pressure = temp_res; // both need to be True
        pneumatic_change_counter = 0;
    }
    // DEBUG_PRINT_VAR(digitalData->pneumatic_line_pressure);

}

inline void DigitalReceiver::readMission() {
    // Enum value attributed considering the True Boolean Value

    Mission temp_res = static_cast<Mission>(
        digitalRead(MISSION_MANUAL_PIN) * MANUAL |
        digitalRead(MISSION_ACCELERATION_PIN) * ACCELERATION |
        digitalRead(MISSION_SKIDPAD_PIN) * SKIDPAD |
        digitalRead(MISSION_AUTOCROSS_PIN) * AUTOCROSS |
        digitalRead(MISSION_TRACKDRIVE_PIN) * TRACKDRIVE |
        digitalRead(MISSION_EBSTEST_PIN) * EBS_TEST |
        digitalRead(MISSION_INSPECTION_PIN) * INSPECTION);

    mission_change_counter = (temp_res == *mission) && 
                                (temp_res == last_tried_mission_) ? 
                                0 : mission_change_counter + 1;
    this->last_tried_mission_ = temp_res;
    if (mission_change_counter >= DIGITAL_INPUT_COUNTER_LIMIT) {
        *mission = temp_res;
        mission_change_counter = 0;
    }
    // DEBUG_PRINT_VAR(*mission);
}

inline void DigitalReceiver::readAsmsSwitch() {
    bool temp_res = digitalRead(ASMS_IN_PIN);

    // Only change the value if it has been different 5 times in a row
    asms_change_counter = temp_res == digitalData->asms_on ? 0 : asms_change_counter + 1;
    if (asms_change_counter >= DIGITAL_INPUT_COUNTER_LIMIT) {
        digitalData->asms_on = temp_res;
        asms_change_counter = 0;
    }
}

inline void DigitalReceiver::readAatsState() {
    // AATS is on if SDC is closed (SDC STATE PIN AS HIGH)
    bool temp_res = !digitalRead(SDC_STATE_PIN);
    // Only change the value if it has been different 5 times in a row
    aats_change_counter = temp_res == digitalData->sdcState_OPEN ? 0 : aats_change_counter + 1;
    if (aats_change_counter >= DIGITAL_INPUT_COUNTER_LIMIT) {
        digitalData->sdcState_OPEN = temp_res; // both need to be True
        aats_change_counter = 0;
    }
    // DEBUG_PRINT_VAR(digitalData->sdcState_OPEN);
}

inline void DigitalReceiver::readWatchdog() {
    digitalData->watchdog_state = digitalRead(SDC_LOGIC_WATCHDOG_IN_PIN);
    if (digitalData->watchdog_state) {
        digitalData->watchdogTimestamp.reset();
    }
    // DEBUG_PRINT_VAR(digitalData->watchdog_state);
}
