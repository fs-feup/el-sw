#pragma once

#include <cstdlib>

#include "model/systemData.hpp"
#include "comm/communicatorSettings.hpp"
#include "embedded/digitalSender.hpp"
#include "embedded/digitalSettings.hpp"
#include "debugUtils.hpp"

// Also known as Orchestrator
/**
 * @brief The CheckupManager class handles various checkup operations.
 */
class CheckupManager {
private:
    SystemData *_systemData; ///< Pointer to the system data object containing system status and sensor information.
    Metro initialCheckupTimestamp{INITIAL_CHECKUP_STEP_TIMEOUT}; ///< Timer for the initial checkup sequence.

public:
    Metro _ebsSoundTimestamp{EBS_BUZZER_TIMEOUT}; ///< Timer for the EBS buzzer sound check.

    /**
     * @brief Provides access to the initial checkup timestamp timer.
     * @return Reference to the initial checkup timestamp timer.
     */
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

    CheckupState checkupState{CheckupState::WAIT_FOR_ASMS}; ///< Current state of the checkup process.

    /**
     * @brief Constructor for the CheckupManager class.
     * @param systemData Pointer to the system data object.
     */
    explicit CheckupManager(SystemData *systemData) : _systemData(systemData) { 
    };

    /**
     * @brief Resets the checkup state to the initial state.
     */
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

    /**
     * @brief Performs a last re-check for off to ready transition.
    */
    [[nodiscard]] bool shouldGoReadyFromOff() const;

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
    [[nodiscard]] bool emergencySequenceComplete() const;

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
    _systemData->missionFinished = false;
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
    return false;
}

inline CheckupManager::CheckupError CheckupManager::initialCheckupSequence(DigitalSender *digitalSender) {
    // DEBUG_PRINT_VAR(static_cast<int>(checkupState));
    switch (checkupState) {
        case CheckupState::WAIT_FOR_ASMS:
            // ASMS Activated?
            if (_systemData->digitalData.asms_on) {
                // checkupState = CheckupState::START_TOGGLING_WATCHDOG;
                checkupState = CheckupState::CLOSE_SDC;
            }
            break;
        // case CheckupState::START_TOGGLING_WATCHDOG:
        //     // Start toggling watchdog
        //     digitalWrite(SDC_LOGIC_WATCHDOG_OUT_PIN, HIGH);
        //     initialCheckupTimestamp.reset();
        //     checkupState = CheckupState::WAIT_FOR_WATCHDOG;
        //     break;
        // case CheckupState::WAIT_FOR_WATCHDOG:
        //     // Watchdog_is_ready == 1
        //     if (initialCheckupTimestamp.check()) {
        //         return CheckupError::ERROR;
        //     }
        //     if (_systemData->digitalData.watchdog_state) {
        //         checkupState = CheckupState::STOP_TOGGLING_WATCHDOG;
        //     }
        //     break;
        // case CheckupState::STOP_TOGGLING_WATCHDOG:
        //     // Stop toggling watchdog
        //     digitalWrite(SDC_LOGIC_WATCHDOG_OUT_PIN, LOW);
        //     initialCheckupTimestamp.reset();
        //     checkupState = CheckupState::CHECK_WATCHDOG;
        //     break;
        // case CheckupState::CHECK_WATCHDOG:
        //     // Watchdog_is_ready == 0
        //     if (initialCheckupTimestamp.check() && !_systemData->digitalData.watchdog_state) {
        //         checkupState = CheckupState::CLOSE_SDC;
        //         //Start toggling watchdog again
        //     }
        //     break;
        case CheckupState::CLOSE_SDC:
            // Close SDC
            DigitalSender::closeSDC();
            checkupState = CheckupState::WAIT_FOR_AATS;
            break;
        case CheckupState::WAIT_FOR_AATS:
            // digitalSender->toggleWatchdog();

        // AATS Activated?
            if (!_systemData->digitalData.sdcState_OPEN) {
            // At this point, the emergency signal should be set to false, since it is
            // expected that the RES has already sent all initial emergency signals,
            // and if RES unexpectedly sends another emergency signal, it will be
            // set after the AATS button is pressed.
            _systemData->failureDetection.emergencySignal = false;
                checkupState = CheckupState::WAIT_FOR_TS;
            }
            break;
        case CheckupState::WAIT_FOR_TS:
            // digitalSender->toggleWatchdog();
        // TS Activated?
            if (_systemData->failureDetection.ts_on) {
                checkupState = CheckupState::CHECK_TIMESTAMPS;
            }
            break;
        case CheckupState::TOGGLE_VALVE:
            // digitalSender->toggleWatchdog();
            // Toggle EBS Valves
            DigitalSender::activateEBS();

            initialCheckupTimestamp.reset();
            checkupState = CheckupState::CHECK_PRESSURE;
            break;
        case CheckupState::CHECK_PRESSURE:
            // digitalSender->toggleWatchdog();
            // Check hydraulic line pressure and pneumatic line pressure
            // DEBUG_PRINT_VAR(_systemData->sensors._hydraulic_line_pressure);
            // DEBUG_PRINT_VAR(_systemData->digitalData.pneumatic_line_pressure);
            if (_systemData->sensors._hydraulic_line_pressure >= HYDRAULIC_BRAKE_THRESHOLD && _systemData->digitalData.
                pneumatic_line_pressure) {
                checkupState = CheckupState::CHECK_TIMESTAMPS;
            }
            break;
        
        case CheckupState::CHECK_TIMESTAMPS: {

            // digitalSender->toggleWatchdog();
            // Check if all components have responded and no emergency signal has been sent
            if (_systemData->failureDetection.hasAnyComponentTimedOut()) {
                DEBUG_PRINT_VAR(_systemData->failureDetection.hasAnyComponentTimedOut());
            }
            if (_systemData->failureDetection.hasAnyComponentTimedOut() || _systemData->failureDetection.
                emergencySignal) {
                return CheckupError::ERROR;
            }
            return CheckupError::SUCCESS;
        }
        default:
            break;
    }
    return CheckupError::WAITING_FOR_RESPONSE;
}

inline bool CheckupManager::shouldGoReadyFromOff() const {
    if (!_systemData->digitalData.asms_on || !_systemData->failureDetection.ts_on || _systemData->digitalData.sdcState_OPEN) {
        return false;
    }
    _systemData->r2dLogics.enterReadyState();
    return true;
}

inline bool CheckupManager::shouldStayReady() const {
    if (!_systemData->r2dLogics.r2d) {
        return true;
    }
    _systemData->r2dLogics.enterDrivingState();
    return false;
}

inline bool CheckupManager::shouldEnterEmergency(State current_state) const {
    if (current_state == AS_READY) {
        if (_systemData->failureDetection.hasAnyComponentTimedOut()) {
            DEBUG_PRINT_VAR(_systemData->failureDetection.hasAnyComponentTimedOut());
        }
        if (_systemData->failureDetection.emergencySignal) {
            DEBUG_PRINT_VAR(_systemData->failureDetection.emergencySignal);
        }
        if (_systemData->digitalData.sdcState_OPEN) {
            DEBUG_PRINT_VAR(_systemData->digitalData.sdcState_OPEN);
        }
        if (!_systemData->digitalData.asms_on) {
            DEBUG_PRINT_VAR(_systemData->digitalData.asms_on);
        }
        if (!_systemData->failureDetection.ts_on) {
            DEBUG_PRINT_VAR(_systemData->failureDetection.ts_on);
        }
        if (_systemData->digitalData.pneumatic_line_pressure == 0) {
            DEBUG_PRINT_VAR(_systemData->digitalData.pneumatic_line_pressure);
            DEBUG_PRINT_VAR(_systemData->r2dLogics.engageEbsTimestamp.checkWithoutReset());
        }
        if (_systemData->sensors._hydraulic_line_pressure < HYDRAULIC_BRAKE_THRESHOLD) {
            DEBUG_PRINT_VAR(_systemData->sensors._hydraulic_line_pressure);
            DEBUG_PRINT_VAR(_systemData->r2dLogics.engageEbsTimestamp.checkWithoutReset());
        }
        return _systemData->failureDetection.emergencySignal ||
            (_systemData->digitalData.pneumatic_line_pressure == 0 
                && _systemData->r2dLogics.engageEbsTimestamp.checkWithoutReset()) ||
            _systemData->failureDetection.hasAnyComponentTimedOut() ||
            // _systemData->digitalData.watchdogTimestamp.check() ||
            !_systemData->digitalData.asms_on ||
            !_systemData->failureDetection.ts_on ||
            //(_systemData->sensors._hydraulic_line_pressure < HYDRAULIC_BRAKE_THRESHOLD
            //    && _systemData->r2dLogics.engageEbsTimestamp.checkWithoutReset()) ||
            _systemData->digitalData.sdcState_OPEN
            ;
    } else if (current_state == AS_DRIVING) {
        if (_systemData->failureDetection.hasAnyComponentTimedOut()) {
            DEBUG_PRINT_VAR(_systemData->failureDetection.hasAnyComponentTimedOut());
        }
        if (_systemData->failureDetection.emergencySignal) {
            DEBUG_PRINT_VAR(_systemData->failureDetection.emergencySignal);
        }
        if (_systemData->digitalData.sdcState_OPEN) {
            DEBUG_PRINT_VAR(_systemData->digitalData.sdcState_OPEN);
        }
        if (!_systemData->digitalData.asms_on) {
            DEBUG_PRINT_VAR(_systemData->digitalData.asms_on);
        }
        if (!_systemData->failureDetection.ts_on) {
            DEBUG_PRINT_VAR(_systemData->failureDetection.ts_on);
        }
        if (_systemData->digitalData.pneumatic_line_pressure == 0) {
            DEBUG_PRINT_VAR(_systemData->digitalData.pneumatic_line_pressure);
            DEBUG_PRINT_VAR(_systemData->r2dLogics.releaseEbsTimestamp.checkWithoutReset());
        }
        if (_systemData->sensors._hydraulic_line_pressure >= HYDRAULIC_BRAKE_THRESHOLD) {
            DEBUG_PRINT_VAR(_systemData->sensors._hydraulic_line_pressure);
            DEBUG_PRINT_VAR(_systemData->r2dLogics.releaseEbsTimestamp.checkWithoutReset());
        }
        return _systemData->failureDetection.hasAnyComponentTimedOut() ||
            _systemData->failureDetection.emergencySignal ||
            _systemData->digitalData.sdcState_OPEN ||
            //(_systemData->digitalData.pneumatic_line_pressure == 0 
            //    && _systemData->r2dLogics.releaseEbsTimestamp.checkWithoutReset()) ||
            //(_systemData->sensors._hydraulic_line_pressure >= HYDRAULIC_BRAKE_THRESHOLD
            //    && _systemData->r2dLogics.releaseEbsTimestamp.checkWithoutReset()) ||
            !_systemData->digitalData.asms_on ||
            // _systemData->digitalData.watchdogTimestamp.check() ||
            !_systemData->failureDetection.ts_on;
    }

    return false;
}

inline bool CheckupManager::shouldStayDriving() const {
    if (abs(_systemData->sensors._left_wheel_rpm) < 0.1 && abs(_systemData->sensors._right_wheel_rpm) < 0.1 && _systemData->missionFinished) {
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

inline bool CheckupManager::emergencySequenceComplete() const {
    if (!_systemData->digitalData.asms_on && _ebsSoundTimestamp.checkWithoutReset()) {
        return true;
    }
    return false;
}

inline bool CheckupManager::resTriggered() const {
    if (_systemData->failureDetection.emergencySignal) {
        return true;
    }
    return false;
}

