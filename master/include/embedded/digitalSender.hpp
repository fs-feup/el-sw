#pragma once

#include <Arduino.h>

class DigitalSender {
private:
    static constexpr std::array<int, 7> validOutputPins = {
        ASSI_DRIVING_PIN,
        ASSI_READY_PIN,
        ASSI_FINISH_PIN,
        ASSI_EMERGENCY_PIN,
        EBS_VALVE_1_PIN,
        EBS_VALVE_2_PIN,
        MASTER_SDC_OUT_PIN
    };
public:
    static void sendDigitalSignal(int pin, int signal) ;

    static void openSDC();

    static void closeSDC();

    static void enterEmergencyState();

};

inline void DigitalSender::sendDigitalSignal(const int pin, const int signal) {
    if (std::find(validOutputPins.begin(), validOutputPins.end(), pin) != validOutputPins.end()) {
        digitalWrite(pin, signal);
    }
}

inline void DigitalSender::openSDC() {
    digitalWrite(SDC_LOGIC_CLOSE_SDC_PIN, HIGH);
    digitalWrite(MASTER_SDC_OUT_PIN, HIGH);
}

inline void DigitalSender::closeSDC() {
    digitalWrite(SDC_LOGIC_CLOSE_SDC_PIN, LOW);
    digitalWrite(MASTER_SDC_OUT_PIN, LOW);
}

inline void DigitalSender::enterEmergencyState() {
    sendDigitalSignal(EBS_VALVE_1_PIN, HIGH);
    sendDigitalSignal(EBS_VALVE_2_PIN, HIGH);
    sendDigitalSignal(ASSI_EMERGENCY_PIN, HIGH);
    openSDC();
}
