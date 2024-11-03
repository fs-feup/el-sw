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
    SystemData *_system_data_; ///< Pointer to the system data object containing system status and sensor information.
    // Metro initialCheckupTimestamp{INITIAL_CHECKUP_STEP_TIMEOUT}; ///< Timer for the initial checkup sequence.

public:
    Metro _ebs_sound_timestamp_{EBS_BUZZER_TIMEOUT}; ///< Timer for the EBS buzzer sound check.

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

    CheckupState checkup_state_{CheckupState::WAIT_FOR_ASMS}; ///< Current state of the checkup process.

    /**
     * @brief Constructor for the CheckupManager class.
     * @param system_data Pointer to the system data object.
     */
    explicit CheckupManager(SystemData *system_data) : _system_data_(system_data) {
                                                      };

    /**
     * @brief Resets the checkup state to the initial state
     */
    void reset_checkup_state();

    /**
     * @brief Performs a manual driving checkup.
     */
    [[nodiscard]] bool should_stay_manual_driving() const;

    /**
     * @brief Performs an off checkup.
     */
    bool should_stay_off(DigitalSender *digital_sender);

    /**
     * @brief Performs an initial checkup.
     */
    CheckupError initial_checkup_sequence(DigitalSender *digital_sender);

    /**
     * @brief Performs a last re-check for off to ready transition.
     */
    [[nodiscard]] bool should_go_ready_from_off() const;

    /**
     * @brief Performs a ready to drive checkup.
     */
    [[nodiscard]] bool should_stay_ready() const;

    /**
     * @brief Performs an emergency checkup.
     */
    [[nodiscard]] bool should_enter_emergency(State current_state) const;

    [[nodiscard]] bool should_stay_driving() const;

    /**
     * @brief Performs a mission finished checkup.
     */
    [[nodiscard]] bool should_stay_mission_finished() const;

    /**
     * @brief Checks if the emergency sequence is complete and the vehicle can
     * transition to AS_OFF.
     */
    [[nodiscard]] bool emergency_sequence_complete() const;

    /**
     * @brief Checks if the RES has been triggered.
     *
     * This function checks whether the RES has been triggered or not.
     *
     */
    [[nodiscard]] bool res_triggered() const;
};

inline void CheckupManager::reset_checkup_state()
{
    checkup_state_ = CheckupState::WAIT_FOR_ASMS;
    _system_data_->mission_finished = false;
}

inline bool CheckupManager::should_stay_manual_driving() const
{
    if (_system_data_->mission != MANUAL || _system_data_->digital_data_.pneumatic_line_pressure_ != 0 || _system_data_->digital_data_.asms_on_)
    {
        return false;
    }

    return true;
}

inline bool CheckupManager::should_stay_off(DigitalSender *digital_sender)
{
    CheckupError init_sequence_state = initial_checkup_sequence(digital_sender);

    if (init_sequence_state != CheckupError::SUCCESS)
    {
        return true;
    }
    return false;
}

inline CheckupManager::CheckupError CheckupManager::initial_checkup_sequence(DigitalSender *digital_sender)
{
    switch (checkup_state_)
    {
    case CheckupState::WAIT_FOR_ASMS:
        // ASMS Activated?
        if (_system_data_->digital_data_.asms_on_)
        {
            checkup_state_ = CheckupState::CLOSE_SDC;
        }
        break;
    case CheckupState::CLOSE_SDC:
        // Close SDC
        DigitalSender::close_sdc();
        checkup_state_ = CheckupState::WAIT_FOR_AATS;

        break;
    case CheckupState::WAIT_FOR_AATS:

        // AATS Activated?
        if (!_system_data_->digital_data_.sdc_open_)
        {
            // At this point, the emergency signal should be set to false, since it is
            // expected that the RES has already sent all initial emergency signals,
            // and if RES unexpectedly sends another emergency signal, it will be
            // set after the AATS button is pressed.
            _system_data_->failure_detection.emergency_signal = false;
            checkup_state_ = CheckupState::WAIT_FOR_TS;
        }
        break;
    case CheckupState::WAIT_FOR_TS:
        if (_system_data_->failure_detection.ts_on)
        {
            DEBUG_PRINT("TS activated");

            checkup_state_ = CheckupState::TOGGLE_VALVE;
        }
        break;
    case CheckupState::TOGGLE_VALVE:
        // Toggle EBS Valves
        checkup_state_ = CheckupState::CHECK_TIMESTAMPS;
        DEBUG_PRINT("EBS activated");
        DigitalSender::activate_ebs();

        break;
    case CheckupState::CHECK_PRESSURE:
        // Check hydraulic line pressure and pneumatic line pressure
        if (_system_data_->sensors._hydraulic_line_pressure >= HYDRAULIC_BRAKE_THRESHOLD && _system_data_->digital_data_.pneumatic_line_pressure_)
        {
            checkup_state_ = CheckupState::CHECK_TIMESTAMPS;
        }
        break;

    case CheckupState::CHECK_TIMESTAMPS:
    {

        // Check if all components have responded and no emergency signal has been sent
        checkup_state_ = CheckupState::CHECK_TIMESTAMPS;
        if (_system_data_->failure_detection.has_any_component_timed_out() || _system_data_->failure_detection.emergency_signal)
        {
            DEBUG_PRINT("Returning ERROR from CHECK_TIMESTAMPS")
            return CheckupError::ERROR;
        }
        checkup_state_ = CheckupState::CHECKUP_COMPLETE;
        DEBUG_PRINT("Checkup complete and returning success");
        return CheckupError::SUCCESS;
    }
    default:
        break;
    }
    return CheckupError::WAITING_FOR_RESPONSE;
}

inline bool CheckupManager::should_go_ready_from_off() const
{
    if (!_system_data_->digital_data_.asms_on_ || !_system_data_->failure_detection.ts_on || _system_data_->digital_data_.sdc_open_)
    {
        return false;
    }
    _system_data_->r2d_logics.enterReadyState();
    return true;
}

inline bool CheckupManager::should_stay_ready() const
{
    if (!_system_data_->r2d_logics.r2d)
    {
        return true;
    }
    _system_data_->r2d_logics.enterDrivingState();
    return false;
}

inline bool CheckupManager::should_enter_emergency(State current_state) const
{
    if (current_state == AS_READY)
    {
        return _system_data_->failure_detection.emergency_signal ||
               (_system_data_->digital_data_.pneumatic_line_pressure_ == 0 && _system_data_->r2d_logics.engageEbsTimestamp.checkWithoutReset()) || // 5 seconds have passed since ready state and line pressure is 0
               _system_data_->failure_detection.has_any_component_timed_out() ||
               !_system_data_->digital_data_.asms_on_ ||
               !_system_data_->failure_detection.ts_on ||
               (_system_data_->sensors._hydraulic_line_pressure < HYDRAULIC_BRAKE_THRESHOLD && _system_data_->r2d_logics.engageEbsTimestamp.checkWithoutReset()) ||
               _system_data_->digital_data_.sdc_open_;
    }
    else if (current_state == AS_DRIVING)
    {
        return _system_data_->failure_detection.has_any_component_timed_out() ||
               _system_data_->failure_detection.emergency_signal ||
               _system_data_->digital_data_.sdc_open_ ||
               (_system_data_->digital_data_.pneumatic_line_pressure_ == 0 && _system_data_->r2d_logics.releaseEbsTimestamp.checkWithoutReset()) ||                      // car has one second to make pneumatic pressure 1
               (_system_data_->sensors._hydraulic_line_pressure >= HYDRAULIC_BRAKE_THRESHOLD && _system_data_->r2d_logics.releaseEbsTimestamp.checkWithoutReset()) || // car has 1 second to reduce hydraulic pressure
               !_system_data_->digital_data_.asms_on_ ||
               !_system_data_->failure_detection.ts_on;
    }

    return false;
}

inline bool CheckupManager::should_stay_driving() const
{
    if (abs(_system_data_->sensors._left_wheel_rpm) < 0.1 && abs(_system_data_->sensors._right_wheel_rpm) < 0.1 && _system_data_->mission_finished)
    {
        return false;
    }
    return true;
}

inline bool CheckupManager::should_stay_mission_finished() const
{
    if (_system_data_->digital_data_.asms_on_)
    {
        return true;
    }
    return false;
}

inline bool CheckupManager::emergency_sequence_complete() const
{
    if (!_system_data_->digital_data_.asms_on_ && _ebs_sound_timestamp_.checkWithoutReset())
    {
        return true;
    }
    return false;
}

inline bool CheckupManager::res_triggered() const
{
    if (_system_data_->failure_detection.emergency_signal)
    {
        return true;
    }
    return false;
}
