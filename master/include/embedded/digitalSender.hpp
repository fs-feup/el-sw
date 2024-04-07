#pragma once

#include <Metro.h>
#include <Arduino.h>
#include "digitalSettings.hpp"

class DigitalSender {
private:
    Metro _blinkTimer{LED_BLINK_INTERVAL};
    Metro _watchdogTimer{WD_PULSE_INTERVAL_MS};


    static void turnOffASSI();

public:
    static constexpr std::array<int, 9> validOutputPins = {
        ASSI_BLUE_PIN,
        ASSI_YELLOW_PIN,
        EBS_VALVE_1_PIN,
        EBS_VALVE_2_PIN,
        MASTER_SDC_OUT_PIN,
        SDC_LOGIC_CLOSE_SDC_PIN,
        SDC_LOGIC_WATCHDOG_OUT_PIN
    };

    DigitalSender() {
        for (const auto pin: validOutputPins) {
            pinMode(pin, OUTPUT);
        }
    }

    static void openSDC();

    static void closeSDC();

    static void activateEBS();

    static void deactivateEBS();

    void enterEmergencyState();

    static void enterManualState();

    static void enterOffState();

    static void enterReadyState();

    void enterDrivingState();

    static void enterFinishState();

    void blinkLED(int pin);

    void toggleWatchdog();
};

inline void DigitalSender::openSDC() {
    digitalWrite(SDC_LOGIC_CLOSE_SDC_PIN, LOW);
    digitalWrite(MASTER_SDC_OUT_PIN, HIGH);
}

inline void DigitalSender::closeSDC() {
    digitalWrite(SDC_LOGIC_CLOSE_SDC_PIN, HIGH);
    digitalWrite(MASTER_SDC_OUT_PIN, LOW);
}

inline void DigitalSender::activateEBS() {
    digitalWrite(EBS_VALVE_1_PIN, HIGH);
    digitalWrite(EBS_VALVE_2_PIN, HIGH);
}

inline void DigitalSender::deactivateEBS() {
    digitalWrite(EBS_VALVE_1_PIN, LOW);
    digitalWrite(EBS_VALVE_2_PIN, LOW);
}

inline void DigitalSender::turnOffASSI() {
    digitalWrite(ASSI_YELLOW_PIN, LOW);
    digitalWrite(ASSI_BLUE_PIN, LOW);
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
    digitalWrite(ASSI_YELLOW_PIN, HIGH);
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
    digitalWrite(ASSI_BLUE_PIN, HIGH);
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
