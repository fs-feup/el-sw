#pragma once

#include <Arduino.h>

class DigitalSender {
private:
    Metro _blinkTimer{LED_BLINK_INTERVAL};
    Metro _watchdogTimer{WD_PULSE_INTERVAL_MS};

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

public:
    DigitalSender() {
        for (const auto pin: validOutputPins) {
            pinMode(pin, OUTPUT);
        }
    }

    static void openSDC();

    static void closeSDC();

    static void activateEBS();

    static void deactivateEBS();

    static void sendDigitalSignal(int pin, int signal);

    void enterEmergencyState();

    static void enterManualState();

    static void enterOffState();

    static void enterReadyState();

    void enterDrivingState();

    static void enterFinishState();

    void blinkLED(int pin);

    void toggleWatchdog();
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
    _blinkTimer.reset();
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
    activateEBS(); ///  these 2 should be redundant since we do it during initial checkup
    closeSDC(); ///
}

inline void DigitalSender::enterDrivingState() {
    turnOffASSI();
    _blinkTimer.reset();
    deactivateEBS();
    closeSDC();
}

inline void DigitalSender::enterFinishState() {
    turnOffASSI();
    digitalWrite(ASSI_FINISH_PIN, HIGH);
    activateEBS();
    openSDC();
}

inline void DigitalSender::blinkLED(const int pin) {
    static bool blinkState = false;
    if (_blinkTimer.check()) {
        blinkState = !blinkState;
        digitalWrite(pin, blinkState);
    }
}

inline void DigitalSender::toggleWatchdog() {
    static bool watchdogState = false;
    if (_watchdogTimer.check()) {
        watchdogState = !watchdogState;
        digitalWrite(SDC_LOGIC_WATCHDOG_OUT_PIN, watchdogState);
    }
}
