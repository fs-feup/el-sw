#pragma once

#include <embedded/digitalData.hpp>
#include <logic/structure.hpp>

class DigitalReceiver {
public:
    void digitalReads();

    DigitalReceiver(DigitalData *digitalData, Mission *mission)
        : digitalData(digitalData), mission(mission) {
        pinMode(LWSS_PIN, INPUT);
        pinMode(SDC_LOGIC_WATCHDOG_IN_PIN, INPUT);
        pinMode(SDC_LOGIC_WATCHDOG_OUT_PIN, OUTPUT);
    }

private:
    DigitalData *digitalData;
    Mission *mission;

    void readLwss() const;

    void readPneumaticLine();

    void readMission() const;

    void readAsmsSwitch() const;

    void readAatsSwitch() const;

    void askReadWatchdog() const;

    void updateLeftWheelRpm() const;
};

inline void DigitalReceiver::updateLeftWheelRpm() const {
    digitalData->_left_wheel_rpm =
            digitalData->pulse_count /
            (WHEEL_MEASUREMENT_INTERVAL_MIN * PULSES_PER_ROTATION);

    digitalData->pulse_count = 0;
    digitalData->left_wheel_update_ts.update();
}

inline void DigitalReceiver::digitalReads() {
    readLwss();
    readPneumaticLine();
    readMission();
    readAsmsSwitch();
    readAatsSwitch();
    askReadWatchdog();
}

inline void DigitalReceiver::readLwss() const {
    bool const current_lwss_state = digitalRead(LWSS_PIN);

    if (current_lwss_state == HIGH && digitalData->last_lwss_state == LOW)
        digitalData->pulse_count++;

    digitalData->last_lwss_state = current_lwss_state;

    if (digitalData->left_wheel_update_ts.hasTimedOut(
        WHEEL_MEASUREMENT_INTERVAL_MS))
        updateLeftWheelRpm();
}

inline void DigitalReceiver::readPneumaticLine() {
    // TODO: wait for eletro indications
}

inline void DigitalReceiver::readMission() const {
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

inline void DigitalReceiver::readAsmsSwitch() const {
    digitalData->asms_on = digitalRead(ASMS_IN_PIN);
}

inline void DigitalReceiver::readAatsSwitch() const {
    //TODO: FIRST DEFINE CORRECT PIN
   // digitalData->aats_on = digitalRead(AATS_SWITCH_PIN);
}

inline void DigitalReceiver::askReadWatchdog() const {
    if (digitalData->wd_pulse_ts.hasTimedOut(
        WD_WAIT_INTERVAL_MS)) {
        // After timeout send pulse
        digitalData->wd_pulse_ts.update();
        digitalWrite(SDC_LOGIC_WATCHDOG_OUT_PIN, HIGH);
        digitalData->watchdog_comm_state = true;
    } else if (digitalData->wd_pulse_ts.hasTimedOut(WD_PULSE_INTERVAL_MS) &&
               digitalData->watchdog_comm_state) {
        // after pulse put pin in low again
        digitalWrite(SDC_LOGIC_WATCHDOG_OUT_PIN, LOW);
        digitalData->watchdog_comm_state = false;
    }

    if (digitalRead(SDC_LOGIC_WATCHDOG_IN_PIN) == LOW) // if low, failure checks will open sdc
        digitalData->watchdog_state = false;
}
