#pragma once

#include "metro.h"
#include <Arduino.h>
#include "digitalSettings.hpp"

/**
 * @brief Class responsible for controlling digital outputs in the Master Teensy.
 * 
 * The DigitalSender class handles various operations such as controlling LEDs,
 * EBS valves, SDC state, and watchdog signals. It also manages different operational states
 * such as emergency, manual, ready, driving, and finish.
 */
class DigitalSender {
private:
    Metro _blinkTimer{LED_BLINK_INTERVAL}; ///< Timer for blinking LED
    // Metro _watchdogTimer{WD_PULSE_INTERVAL_MS}; ///< Timer for toggling watchdog signal

    /**
     * @brief Turns off both ASSI LEDs (yellow and blue).
     */
    static void turnOffASSI();

public:
    // Array of valid output pins
    static constexpr std::array<int, 9> validOutputPins = {
        ASSI_BLUE_PIN,
        ASSI_YELLOW_PIN,
        EBS_VALVE_1_PIN,
        EBS_VALVE_2_PIN,
        MASTER_SDC_OUT_PIN,
        SDC_LOGIC_CLOSE_SDC_PIN,
        // SDC_LOGIC_WATCHDOG_OUT_PIN
    };

    /**
     * @brief Constructor for the DigitalSender class.
     * 
     * Initializes the pins for output as defined in validOutputPins.
     */
    DigitalSender() {
        for (const auto pin: validOutputPins) {
            pinMode(pin, OUTPUT);
        }
    }

    /**
     * @brief Opens the SDC in Master and SDC Logic.
     */
    static void openSDC();

    /**
     * @brief Closes the SDC in Master and SDC Logic.
     */
    static void closeSDC();

    /**
     * @brief Activates the solenoid EBS valves.
     */
    static void activateEBS();

    /**
     * @brief Deactivates the solenoid EBS valves.
     */
    static void deactivateEBS();

    /**
     * @brief ASSI LEDs blue flashing, sdc open and buzzer ringing.
    */
    void enterEmergencyState();

    /**
     * @brief Everything off, sdc closed.
    */
    static void enterManualState();

    /**
     * @brief Everything off, sdc open.
    */
    static void enterOffState();

    /**
     * @brief ASSI yellow LED on, ebs valves activated, sdc closed.
    */
    static void enterReadyState();

    /**
     * @brief ASSI LEDs yellow flashing, ebs valves deactivated, sdc closed.
    */
    void enterDrivingState();

    /**
     * @brief ASSI blue LED on, ebs valves activated, sdc open.
    */
    static void enterFinishState();

    /**
     * @brief Blinks the LED at the given pin.
     * @param pin The pin to blink.
     */
    void blinkLED(int pin);

    // /**
    //  * @brief Toggles the watchdog pin, to ensure certify of masters' liveness.
    //  */
    // void toggleWatchdog();
};

inline void DigitalSender::openSDC() {
    digitalWrite(SDC_LOGIC_CLOSE_SDC_PIN, LOW);
    digitalWrite(MASTER_SDC_OUT_PIN, LOW);
}

inline void DigitalSender::closeSDC() {
    digitalWrite(SDC_LOGIC_CLOSE_SDC_PIN, HIGH);
    digitalWrite(MASTER_SDC_OUT_PIN, HIGH);
}

inline void DigitalSender::activateEBS() {
    // pinMode(EBS_VALVE_1_PIN, OUTPUT);
    // pinMode(EBS_VALVE_2_PIN, OUTPUT);
    digitalWrite(EBS_VALVE_1_PIN, HIGH);
    digitalWrite(EBS_VALVE_2_PIN, HIGH);
}

inline void DigitalSender::deactivateEBS() {
    digitalWrite(EBS_VALVE_1_PIN, LOW);
    digitalWrite(EBS_VALVE_2_PIN, LOW);
}

inline void DigitalSender::turnOffASSI() {
    analogWrite(ASSI_YELLOW_PIN, LOW);
    analogWrite(ASSI_BLUE_PIN, LOW);
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
    analogWrite(ASSI_YELLOW_PIN, 1023); // Analog works better
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
    analogWrite(ASSI_BLUE_PIN, 1023); // Analog works better
    activateEBS();
    openSDC();
}

inline void DigitalSender::blinkLED(const int pin) {
    static bool blinkState = false;
    if (_blinkTimer.check()) {
        blinkState = !blinkState;
        analogWrite(pin, blinkState * 1023); // Analog works better
    }
}

// inline void DigitalSender::toggleWatchdog() {
//     static bool watchdogState = false;
//     if (_watchdogTimer.check()) {
//         watchdogState = !watchdogState;
//         digitalWrite(SDC_LOGIC_WATCHDOG_OUT_PIN, watchdogState);
//         _watchdogTimer.reset();
//     }
// }
