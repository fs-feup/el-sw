#pragma once

#include <Arduino.h>

class DigitalSender {
private:


    static constexpr std::array<int, 9> validOutputPins = {
        ASSI_DRIVING_PIN,
        ASSI_READY_PIN,
        ASSI_FINISH_PIN,
        ASSI_EMERGENCY_PIN,
        EBS_VALVE_1_PIN,
        EBS_VALVE_2_PIN,
        MASTER_SDC_OUT_PIN,
        SDC_LOGIC_CLOSE_SDC_PIN,
        SDC_LOGIC_WATCHDOG_OUT_PIN
    };

    static void turnOffASSI();

    static void openSDC();

    static void closeSDC();

    static void activateEBS();

    static void deactivateEBS();

public:
    DigitalSender() {
        for (const auto pin : validOutputPins) {
            pinMode(pin, OUTPUT);
        }
    }

    static void sendDigitalSignal(int pin, int signal);

    static void enterEmergencyState();

    static void enterManualState();

    static void enterOffState();

    static void enterReadyState();

    static void enterDrivingState();

    static void enterFinishState();
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

inline void DigitalSender::activateEBS() {
    digitalWrite(EBS_VALVE_1_PIN, LOW);
    digitalWrite(EBS_VALVE_2_PIN, LOW);
}

inline void DigitalSender::deactivateEBS() {
    digitalWrite(EBS_VALVE_1_PIN, HIGH);
    digitalWrite(EBS_VALVE_2_PIN, HIGH);
}

inline void DigitalSender::turnOffASSI() {
    analogWrite(ASSI_DRIVING_PIN, 0);
    analogWrite(ASSI_READY_PIN, 0);
    analogWrite(ASSI_FINISH_PIN, 0);
    analogWrite(ASSI_EMERGENCY_PIN, 0);
}

inline void DigitalSender::enterEmergencyState() {
    turnOffASSI();
    digitalWrite(ASSI_EMERGENCY_PIN, HIGH);
    activateEBS();
    openSDC();
}

inline void DigitalSender::enterManualState() {
    turnOffASSI();
    deactivateEBS();
    closeSDC();
}

inline void DigitalSender::enterOffState() {
    turnOffASSI();
    deactivateEBS();
    openSDC();
}

inline void DigitalSender::enterReadyState() {
    turnOffASSI();
    digitalWrite(ASSI_READY_PIN, HIGH);
    activateEBS();
    closeSDC();
}

inline void DigitalSender::enterDrivingState() {
    turnOffASSI();
    digitalWrite(ASSI_DRIVING_PIN, HIGH);
}

inline void DigitalSender::enterFinishState() {
    turnOffASSI();
    digitalWrite(ASSI_FINISH_PIN, HIGH);
    activateEBS();
    openSDC();
}
