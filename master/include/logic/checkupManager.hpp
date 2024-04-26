#pragma once

#include <cstdlib>

#include "model/systemData.hpp"
#include "comm/communicatorSettings.hpp"
#include "embedded/digitalSender.hpp"
#include "embedded/digitalSettings.hpp"

// Also known as Orchestrator
/**
 * @brief The CheckupManager class handles various checkup operations.
 */
class CheckupManager {
private:
    SystemData *_systemData;
    Metro _ebsSoundTimestamp{EBS_BUZZER_TIMEOUT};
    Metro initialCheckupTimestamp{INITIAL_CHECKUP_STEP_TIMEOUT};

public:
    [[nodiscard]] Metro &getInitialCheckupTimestamp() {
        return initialCheckupTimestamp;
    }

    /**
     * @brief The CheckupState enum represents the different states of 
     * the initial checkup process.
     * The checkup process is a sequence of checks that the vehicle must pass
     * before it can transition to ready state.
    */
    enum class CheckupState {
        WAIT_FOR_ASMS,
        START_TOGGLING_WATCHDOG,
        WAIT_FOR_WATCHDOG,
        STOP_TOGGLING_WATCHDOG,
        CHECK_WATCHDOG,
        CLOSE_SDC,
        WAIT_FOR_AATS,
        WAIT_FOR_TS,
        TOGGLE_VALVE,
        CHECK_PRESSURE,
        CHECK_TIMESTAMPS
    };

    /**
     * This is for easier debugging in case initial checkup fails
    */
    enum class CheckupError {
        WAITING_FOR_RESPONSE,
        ERROR,
        SUCCESS
    };

    CheckupState checkupState{CheckupState::WAIT_FOR_ASMS};

    explicit CheckupManager(SystemData *systemData) : _systemData(systemData) {
    };

    void resetCheckupState();

    /**
     * @brief Performs a manual driving checkup.
     */
    [[nodiscard]] bool shouldStayManualDriving() const;

    /**
     * @brief Performs an off checkup.
     */
    bool shouldStayOff(DigitalSender *digitalSender);

    /**
     * @brief Performs an initial checkup.
     */
    CheckupError initialCheckupSequence(DigitalSender *digitalSender);

    [[nodiscard]] bool shouldRevertToOffFromReady() const;

    /**
     * @brief Performs a ready to drive checkup.
     */
    [[nodiscard]] bool shouldStayReady() const;

    /**
     * @brief Performs an emergency checkup.
     */
    [[nodiscard]] bool shouldEnterEmergency(State current_state) const;

    [[nodiscard]] bool shouldStayDriving() const;

    /**
     * @brief Performs a mission finished checkup.
     */
    [[nodiscard]] bool shouldStayMissionFinished() const;

    /**
     * @brief Checks if the emergency sequence is complete and the vehicle can
     * transition to AS_OFF.
     */
    [[nodiscard]] bool emergencySequenceComplete();

    /**
     * @brief Checks if the RES has been triggered.
     *
     * This function checks whether the RES has been triggered or not.
     *
     */
    [[nodiscard]] bool resTriggered() const;
};

inline void CheckupManager::resetCheckupState() {
    checkupState = CheckupState::WAIT_FOR_ASMS;
}

inline bool CheckupManager::shouldStayManualDriving() const {

    if (_systemData->mission != MANUAL || _systemData->digitalData.pneumatic_line_pressure != 0
        || _systemData->digitalData.asms_on) {
        return false;
    }
    return true;
}

inline bool CheckupManager::shouldStayOff(DigitalSender *digitalSender) {
    CheckupError initSequenceState = initialCheckupSequence(digitalSender);

    if (initSequenceState != CheckupError::SUCCESS) {
        return true;
    }
    _systemData->r2dLogics.enterReadyState();
    return false;
}

inline CheckupManager::CheckupError CheckupManager::initialCheckupSequence(DigitalSender *digitalSender) {
    switch (checkupState) {
        case CheckupState::WAIT_FOR_ASMS:
            // ASMS Activated?
            if (_systemData->digitalData.asms_on) {
                checkupState = CheckupState::START_TOGGLING_WATCHDOG;
            }
            break;
        case CheckupState::START_TOGGLING_WATCHDOG:
            // Start toggling watchdog
            digitalWrite(SDC_LOGIC_WATCHDOG_OUT_PIN, HIGH);
            initialCheckupTimestamp.reset();
            checkupState = CheckupState::WAIT_FOR_WATCHDOG;
            break;
        case CheckupState::WAIT_FOR_WATCHDOG:
            // Watchdog_is_ready == 1
            if (initialCheckupTimestamp.check()) {
                return CheckupError::ERROR;
            }
            if (_systemData->digitalData.watchdog_state) {
                checkupState = CheckupState::STOP_TOGGLING_WATCHDOG;
            }
            break;
        case CheckupState::STOP_TOGGLING_WATCHDOG:
            // Stop toggling watchdog
            digitalWrite(SDC_LOGIC_WATCHDOG_OUT_PIN, LOW);
            initialCheckupTimestamp.reset();
            checkupState = CheckupState::CHECK_WATCHDOG;
            break;
        case CheckupState::CHECK_WATCHDOG:
            // Watchdog_is_ready == 0
            if (initialCheckupTimestamp.check() && !_systemData->digitalData.watchdog_state) {
                checkupState = CheckupState::CLOSE_SDC;
                //Start toggling watchdog again
            }
            break;
        case CheckupState::CLOSE_SDC:
            // Close SDC
            DigitalSender::closeSDC();
            checkupState = CheckupState::WAIT_FOR_AATS;
            break;
        case CheckupState::WAIT_FOR_AATS:
            digitalSender->toggleWatchdog();
        // AATS Activated?
            if (!_systemData->digitalData.sdcState_OPEN) {
                checkupState = CheckupState::WAIT_FOR_TS;
            }
            break;
        case CheckupState::WAIT_FOR_TS:
            digitalSender->toggleWatchdog();
        // TS Activated?
            if (_systemData->failureDetection.ts_on) {
                checkupState = CheckupState::TOGGLE_VALVE;
            }
            break;
        case CheckupState::TOGGLE_VALVE:
            digitalSender->toggleWatchdog();
        // Toggle EBS Valves
            DigitalSender::activateEBS();

            initialCheckupTimestamp.reset();
            checkupState = CheckupState::CHECK_PRESSURE;
            break;
        case CheckupState::CHECK_PRESSURE:
            digitalSender->toggleWatchdog();
            // Check hydraulic line pressure and pneumatic line pressure
            if (initialCheckupTimestamp.check()) {
                return CheckupError::ERROR;
            }
            if (_systemData->sensors._hydraulic_line_pressure >= HYDRAULIC_BRAKE_THRESHOLD && _systemData->digitalData.
                pneumatic_line_pressure) {
                checkupState = CheckupState::CHECK_TIMESTAMPS;
            }
            break;
        
        case CheckupState::CHECK_TIMESTAMPS:
            digitalSender->toggleWatchdog();
        // Check if all components have responded and no emergency signal has been sent
            if (_systemData->failureDetection.hasAnyComponentTimedOut() || _systemData->failureDetection.
                emergencySignal) {
                return CheckupError::ERROR;
            }
            return CheckupError::SUCCESS;
        default:
            break;
    }
    return CheckupError::WAITING_FOR_RESPONSE;
}

inline bool CheckupManager::shouldStayReady() const {
    if (!_systemData->r2dLogics.r2d) {
        return true;
    }
    return false;
}

inline bool CheckupManager::shouldEnterEmergency(State current_state) const {
    if (current_state == AS_READY && (
        _systemData->failureDetection.emergencySignal ||
        _systemData->digitalData.pneumatic_line_pressure == 0 ||
        _systemData->failureDetection.hasAnyComponentTimedOut() ||
        _systemData->digitalData.watchdogTimestamp.check() ||
        !_systemData->digitalData.asms_on ||
        !_systemData->failureDetection.ts_on ||
        _systemData->sensors._hydraulic_line_pressure < HYDRAULIC_BRAKE_THRESHOLD ||
        _systemData->digitalData.sdcState_OPEN
        )) {
        return true;
    }
    if (current_state == AS_DRIVING && (
        _systemData->failureDetection.hasAnyComponentTimedOut() ||
        _systemData->failureDetection.emergencySignal ||
        _systemData->digitalData.sdcState_OPEN ||
        _systemData->digitalData.pneumatic_line_pressure == 0 ||
        (_systemData->sensors._hydraulic_line_pressure >= HYDRAULIC_BRAKE_THRESHOLD
            && (millis() - _systemData->r2dLogics.releaseEbsTimestamp) > RELEASE_EBS_TIMEOUT_MS) ||
        _systemData->digitalData.asms_on == 0 ||
        _systemData->digitalData.watchdogTimestamp.check())) {
        return true;
    }

    return false;
}

inline bool CheckupManager::shouldStayDriving() const {
    if (_systemData->digitalData._left_wheel_rpm == 0 && _systemData->sensors._right_wheel_rpm == 0 && _systemData->missionFinished) {
        return false;
    }
    return true;
}

inline bool CheckupManager::shouldStayMissionFinished() const {
    if (_systemData->digitalData.asms_on) {
        return true;
    }
    return false;
}

inline bool CheckupManager::emergencySequenceComplete() {
    if (!_systemData->digitalData.asms_on && _ebsSoundTimestamp.check()) {
        return true;
    }
    return false;
}

inline bool CheckupManager::resTriggered() const {
    if (_systemData->failureDetection.emergencySignal) {
        return false;
    }
    return true;
}

