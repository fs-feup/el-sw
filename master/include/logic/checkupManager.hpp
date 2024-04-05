#pragma once

#include <model/systemData.hpp>
#include <cstdlib>

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
    // Metro _initialCheckupTimestamp;

    enum class CheckupState {
        WAIT_FOR_ASMS,
        START_TOGGLING_WATCHDOG,
        WAIT_FOR_WATCHDOG,
        STOP_TOGGLING_WATCHDOG,
        CHECK_WATCHDOG,
        CLOSE_SDC,
        WAIT_FOR_TS,
        TOGGLE_VALVE,
        CHECK_PRESSURE,
        CHECK_TIMESTAMPS
    };

    //This is for easier debugging in case initial checkup fails
    enum class CheckupError {
        WAITING_FOR_RESPONSE,
        ERROR,
        SUCCESS
    };

public:
    explicit CheckupManager(SystemData *systemData) : _systemData(systemData) {
    };

    /**
     * @brief Performs a manual driving checkup.
     * @return 0 if success, else 1.
     */
    [[nodiscard]] bool shouldStayManualDriving() const;

    /**
     * @brief Performs an off checkup.
     * @return 0 if success, else 1.
     */
    bool shouldStayOff(DigitalSender &digitalSender);

    /**
     * @brief Performs an initial checkup.
     * @return 0 if success, else 1.
     */
    CheckupError initialCheckupSequence(DigitalSender &digitalSender);

    [[nodiscard]] bool shouldRevertToOffFromReady() const;

    /**
     * @brief Performs a ready to drive checkup.
     * @return 0 if success, else 1.
     */
    [[nodiscard]] bool shouldStayR2D() const;

    /**
     * @brief Performs an emergency checkup.
     * @return 0 if the car passes emergency checks, else 1.
     */
    [[nodiscard]] bool shouldEnterEmergency() const;

    [[nodiscard]] bool shouldStayDriving() const;

    /**
     * @brief Performs a mission finished checkup.
     * @return 0 if success, else 1.
     */
    [[nodiscard]] bool shouldStayMissionFinished() const;

    /**
     * @brief Checks if the emergency sequence is complete and the vehicle can
     * transition to AS_OFF.
     * @return 0 if success, else 1.
     */
    [[nodiscard]] bool emergencySequenceComplete();

    /**
     * @brief Checks if the RES has been triggered.
     *
     * This function checks whether the RES has been triggered or not.
     *
     * @return 1 if the RES has been triggered, 0 otherwise.
     */
    [[nodiscard]] bool resTriggered() const;


    CheckupManager();
};

inline bool CheckupManager::shouldStayManualDriving() const {
    /* AATS OFF | MISSION NOT MANUAL | EBS IS DISABLED --> Transition to AS_OFF
     * AATS ON  & MISSION MANUAL     & EBS INACTIVE    --> Transition to AS_READY
     *
     * In EXIT_SUCCESS, the vehicle can transition to AS_OFF.
     * IN true, the vehicle can transition to AS_MANUAL.
     */

    if (_systemData->mission != MANUAL || _systemData->digitalData.pneumatic_line_pressure != 0
        || !_systemData->failureDetection.ts_on || _systemData->digitalData.sdcState_OPEN) {
        return false;
    }
    return true;
}

inline bool CheckupManager::shouldStayOff(DigitalSender &digitalSender) {
    // THIS CHECKUP SEQUENCE IS NOT LONGER NEEDED AS IF ONE OF THOSE GET TRIGGERED DURING THE INITIAL SEQUENCE,
    // THE CAR WOULD REVERT STATE TO OFF OR EMERGENCY ACCORDINGLY.
    // if (!(_systemData->digitalData.asms_on && _systemData->digitalData.aats_on && !_systemData->sdcState_OPEN)) {
    //     return true;
    // }
    CheckupError initSequenceState = initialCheckupSequence(digitalSender);

    if (initSequenceState != CheckupError::SUCCESS) {
        return true;
    }
    _systemData->internalLogics.enterReadyState();
    _systemData->digitalData.watchdogTimestamp.reset();
    return false;
}

inline CheckupManager::CheckupError CheckupManager::initialCheckupSequence(DigitalSender &digitalSender) {
    static auto state = CheckupState::WAIT_FOR_ASMS;
    switch (state) {
        case CheckupState::WAIT_FOR_ASMS:
            // ASMS Activated?
            if (_systemData->digitalData.asms_on) {
                state = CheckupState::START_TOGGLING_WATCHDOG;
            }
            break;
        case CheckupState::START_TOGGLING_WATCHDOG:
            // Start toggling watchdog
            digitalWrite(SDC_LOGIC_WATCHDOG_OUT_PIN, HIGH);
            initialCheckupTimestamp.reset();
            state = CheckupState::WAIT_FOR_WATCHDOG;
            break;
        case CheckupState::WAIT_FOR_WATCHDOG:
            // Watchdog_is_ready == 1
            if (initialCheckupTimestamp.check()) {
                return CheckupError::ERROR;
            }
            if (_systemData->digitalData.watchdog_state) {
                state = CheckupState::STOP_TOGGLING_WATCHDOG;
            }
            break;
        case CheckupState::STOP_TOGGLING_WATCHDOG:
            // Stop toggling watchdog
            digitalWrite(SDC_LOGIC_WATCHDOG_OUT_PIN, LOW);
            initialCheckupTimestamp.reset();
            state = CheckupState::CHECK_WATCHDOG;
            break;
        case CheckupState::CHECK_WATCHDOG:
            // Watchdog_is_ready == 0
            if (initialCheckupTimestamp.check() && !_systemData->digitalData.watchdog_state) {
                state = CheckupState::CLOSE_SDC;
                //Start toggling watchdog again
            }
            break;
        case CheckupState::CLOSE_SDC:
            // Close SDC
            DigitalSender::closeSDC();
            state = CheckupState::WAIT_FOR_TS;
            break;
        case CheckupState::WAIT_FOR_TS:
            digitalSender.toggleWatchdog();
        // TS Activated?
            if (_systemData->failureDetection.ts_on) {
                state = CheckupState::TOGGLE_VALVE;
            }
            break;
        case CheckupState::TOGGLE_VALVE:
            digitalSender.toggleWatchdog();
        // Toggle EBS Valves
            DigitalSender::activateEBS();

            initialCheckupTimestamp.reset();
            state = CheckupState::CHECK_PRESSURE;
            break;
        case CheckupState::CHECK_PRESSURE: {
            digitalSender.toggleWatchdog();
            // Check hyraulic line pressure and pneumatic line pressure
            if (initialCheckupTimestamp.check()) {
                return CheckupError::ERROR;
            }
            if (_systemData->sensors._hydraulic_line_pressure > 0 && _systemData->digitalData.
                pneumatic_line_pressure) {
                state = CheckupState::CHECK_TIMESTAMPS;
            }
            break;
        }
        case CheckupState::CHECK_TIMESTAMPS:
            digitalSender.toggleWatchdog();
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

inline bool CheckupManager::shouldRevertToOffFromReady() const {
    if (!_systemData->digitalData.asms_on || !_systemData->failureDetection.ts_on || _systemData->sensors.
        _hydraulic_line_pressure == 0) {
        return true;
    }
    return false;
}

inline bool CheckupManager::shouldStayR2D() const {
    if (!_systemData->internalLogics.goSignal) {
        return true;
    }

    _systemData->digitalData.watchdogTimestamp.reset();
    return false;
}

inline bool CheckupManager::shouldEnterEmergency() const {
    if (_systemData->failureDetection.hasAnyComponentTimedOut() ||
        _systemData->failureDetection.emergencySignal ||
        _systemData->digitalData.sdcState_OPEN ||
        _systemData->digitalData.pneumatic_line_pressure == 0 ||
        _systemData->digitalData.asms_on == 0 ||
        _systemData->digitalData.watchdogTimestamp.check()) {
        return true;
    }

    _systemData->digitalData.watchdogTimestamp.reset();
    return false;
}

inline bool CheckupManager::shouldStayDriving() const {
    if (_systemData->digitalData._left_wheel_rpm == 0 && _systemData->missionFinished) {
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
    if (_ebsSoundTimestamp.check() && !_systemData->digitalData.asms_on) {
        return false;
    }
    return true;
}

inline bool CheckupManager::resTriggered() const {
    if (_systemData->failureDetection.emergencySignal) {
        return false;
    }
    return true;
}

