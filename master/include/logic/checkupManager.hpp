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
class CheckupManager
{
private:
    SystemData *_systemData; ///< Pointer to the system data object containing system status and sensor information.
    // Metro initialCheckupTimestamp{INITIAL_CHECKUP_STEP_TIMEOUT}; ///< Timer for the initial checkup sequence.

public:
    Metro _ebsSoundTimestamp{EBS_BUZZER_TIMEOUT}; ///< Timer for the EBS buzzer sound check.

    /**
     * @brief The CheckupState enum represents the different states of
     * the initial checkup process.
     * The checkup process is a sequence of checks that the vehicle must pass
     * before it can transition to ready state.
     */
    enum class CheckupState
    {
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
        CHECK_TIMESTAMPS,
        CHECKUP_COMPLETE
    };

    /**
     * This is for easier debugging in case initial checkup fails
     */
    enum class CheckupError
    {
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
     * @brief Resets the checkup state to the initial state
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

inline void CheckupManager::resetCheckupState()
{
    checkupState = CheckupState::WAIT_FOR_ASMS;
    _systemData->missionFinished = false;
}

inline bool CheckupManager::shouldStayManualDriving() const
{
    if (_systemData->mission != MANUAL || _systemData->digitalData.pneumatic_line_pressure != 0 || _systemData->digitalData.asms_on)
    {
        return false;
    }

    return true;
}

inline bool CheckupManager::shouldStayOff(DigitalSender *digitalSender)
{
    CheckupError initSequenceState = initialCheckupSequence(digitalSender);

    if (initSequenceState != CheckupError::SUCCESS)
    {
        return true;
    }
    return false;
}

inline CheckupManager::CheckupError CheckupManager::initialCheckupSequence(DigitalSender *digitalSender)
{
    switch (checkupState)
    {
    case CheckupState::WAIT_FOR_ASMS:
        // ASMS Activated?
        if (_systemData->digitalData.asms_on)
        {
            checkupState = CheckupState::CLOSE_SDC;
        }
        break;
    case CheckupState::CLOSE_SDC:
        // Close SDC
        DigitalSender::closeSDC();
        checkupState = CheckupState::WAIT_FOR_AATS;

        break;
    case CheckupState::WAIT_FOR_AATS:

        // AATS Activated?
        if (!_systemData->digitalData.sdcState_OPEN)
        {
            // At this point, the emergency signal should be set to false, since it is
            // expected that the RES has already sent all initial emergency signals,
            // and if RES unexpectedly sends another emergency signal, it will be
            // set after the AATS button is pressed.
            _systemData->failureDetection.emergencySignal = false;
            checkupState = CheckupState::WAIT_FOR_TS;
        }
        break;
    case CheckupState::WAIT_FOR_TS:
        if (_systemData->failureDetection.ts_on)
        {
            DEBUG_PRINT("TS activated");

            checkupState = CheckupState::TOGGLE_VALVE;
        }
        break;
    case CheckupState::TOGGLE_VALVE:
        // Toggle EBS Valves
        checkupState = CheckupState::CHECK_TIMESTAMPS;
        DEBUG_PRINT("EBS activated");
        DigitalSender::activateEBS();

        break;
    case CheckupState::CHECK_PRESSURE:
        // Check hydraulic line pressure and pneumatic line pressure
        if (_systemData->sensors._hydraulic_line_pressure >= HYDRAULIC_BRAKE_THRESHOLD && _systemData->digitalData.pneumatic_line_pressure)
        {
            checkupState = CheckupState::CHECK_TIMESTAMPS;
        }
        break;

    case CheckupState::CHECK_TIMESTAMPS:
    {

        // Check if all components have responded and no emergency signal has been sent
        checkupState = CheckupState::CHECK_TIMESTAMPS;
        if (_systemData->failureDetection.has_any_component_timed_out() || _systemData->failureDetection.emergencySignal)
        {
            DEBUG_PRINT("Returning ERROR from CHECK_TIMESTAMPS")
            return CheckupError::ERROR;
        }
        checkupState = CheckupState::CHECKUP_COMPLETE;
        DEBUG_PRINT("Checkup complete and returning success");
        return CheckupError::SUCCESS;
    }
    default:
        break;
    }
    return CheckupError::WAITING_FOR_RESPONSE;
}

inline bool CheckupManager::shouldGoReadyFromOff() const
{
    if (!_systemData->digitalData.asms_on || !_systemData->failureDetection.ts_on || _systemData->digitalData.sdcState_OPEN)
    {
        return false;
    }
    _systemData->r2dLogics.enterReadyState();
    return true;
}

inline bool CheckupManager::shouldStayReady() const
{
    if (!_systemData->r2dLogics.r2d)
    {
        return true;
    }
    _systemData->r2dLogics.enterDrivingState();
    return false;
}

inline bool CheckupManager::shouldEnterEmergency(State current_state) const
{
    if (current_state == AS_READY)
    {
        return _systemData->failureDetection.emergencySignal ||
               (_systemData->digitalData.pneumatic_line_pressure == 0 && _systemData->r2dLogics.engageEbsTimestamp.checkWithoutReset()) || // 5 seconds have passed since ready state and line pressure is 0
               _systemData->failureDetection.has_any_component_timed_out() ||
               !_systemData->digitalData.asms_on ||
               !_systemData->failureDetection.ts_on ||
               (_systemData->sensors._hydraulic_line_pressure < HYDRAULIC_BRAKE_THRESHOLD && _systemData->r2dLogics.engageEbsTimestamp.checkWithoutReset()) ||
               _systemData->digitalData.sdcState_OPEN;
    }
    else if (current_state == AS_DRIVING)
    {
        return _systemData->failureDetection.has_any_component_timed_out() ||
               _systemData->failureDetection.emergencySignal ||
               _systemData->digitalData.sdcState_OPEN ||
               (_systemData->digitalData.pneumatic_line_pressure == 0 && _systemData->r2dLogics.releaseEbsTimestamp.checkWithoutReset()) ||                      // car has one second to make pneumatic pressure 1
               (_systemData->sensors._hydraulic_line_pressure >= HYDRAULIC_BRAKE_THRESHOLD && _systemData->r2dLogics.releaseEbsTimestamp.checkWithoutReset()) || // car has 1 second to reduce hydraulic pressure
               !_systemData->digitalData.asms_on ||
               !_systemData->failureDetection.ts_on;
    }

    return false;
}

inline bool CheckupManager::shouldStayDriving() const
{
    if (abs(_systemData->sensors._left_wheel_rpm) < 0.1 && abs(_systemData->sensors._right_wheel_rpm) < 0.1 && _systemData->missionFinished)
    {
        return false;
    }
    return true;
}

inline bool CheckupManager::shouldStayMissionFinished() const
{
    if (_systemData->digitalData.asms_on)
    {
        return true;
    }
    return false;
}

inline bool CheckupManager::emergencySequenceComplete() const
{
    if (!_systemData->digitalData.asms_on && _ebsSoundTimestamp.checkWithoutReset())
    {
        return true;
    }
    return false;
}

inline bool CheckupManager::resTriggered() const
{
    if (_systemData->failureDetection.emergencySignal)
    {
        return true;
    }
    return false;
}
