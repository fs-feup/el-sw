#pragma once

#include <logic/checkupManager.hpp>
#include <model/structure.hpp>
#include <embedded/digitalSender.hpp>

/**
 * @brief The ASState class manages and transitions between different states of the vehicle system.
 *
 * The ASState class uses the CheckupManager to perform system checks and determine the appropriate state transitions
 * based on the vehicle's status and operational conditions. It interacts with hardware components via the DigitalSender
 * and Communicator interfaces to update the system state and handle transitions.
 */
class ASState
{
private:
    DigitalSender *_digital_sender_; ///< Pointer to the DigitalSender object for hardware interactions.
    Communicator *_communicator_;   ///< Pointer to the Communicator object for communication operations.

public:
    CheckupManager _checkup_manager_; ///< CheckupManager object for handling various checkup operations.
    State state_{AS_OFF};            ///< Current state of the vehicle system, initialized to OFF.

    /**
     * @brief Constructor for the ASState class.
     * @param system_data Pointer to the SystemData object containing system status and sensor information.
     * @param communicator Pointer to the Communicator object.
     * @param digital_sender Pointer to the DigitalSender object.
     */
    explicit ASState(SystemData *system_data, Communicator *communicator,
                     DigitalSender *digital_sender) : _digital_sender_(digital_sender), _communicator_(communicator), _checkup_manager_(system_data) {}

    /**
     * @brief Calculates the state of the vehicle.
     */
    void calculate_state();
};

inline void ASState::calculate_state()
{
    switch (state_)
    {
    case AS_MANUAL:
        if (_checkup_manager_.should_stay_manual_driving())
            break;

        DEBUG_PRINT("Entering OFF state from MANUAL");
        DigitalSender::enter_off_state();

        state_ = AS_OFF;
        break;

    case AS_OFF:

        // If manual driving checkup fails, the car can't be in OFF state, so it goes back to MANUAL
        if (_checkup_manager_.should_stay_manual_driving())
        {
            DEBUG_PRINT("Entering MANUAL state from OFF");
            DigitalSender::enter_manual_state();
            state_ = AS_MANUAL;
            break;
        }

        if (_checkup_manager_.should_stay_off(_digital_sender_))
            break;
        if (!_checkup_manager_.should_go_ready_from_off())
            break; // recheck all states

        DEBUG_PRINT("Entering READY state from OFF");
        DigitalSender::enter_ready_state();
        state_ = AS_READY;
        DEBUG_PRINT("READY state entered...");
        break;

    case AS_READY:
        if (_checkup_manager_.should_enter_emergency(state_))
        {
            DEBUG_PRINT("Entering EMERGENCY state from READY");
            _digital_sender_->enter_emergency_state();
            _checkup_manager_._ebs_sound_timestamp_.reset();
            state_ = AS_EMERGENCY;
            break;
        }
        if (_checkup_manager_.should_stay_ready())
        {
            break;
        }

        DEBUG_PRINT("Entering DRIVING state from READY");
        _digital_sender_->enter_driving_state();
        state_ = AS_DRIVING;
        break;
    case AS_DRIVING:
        _digital_sender_->blink_led(ASSI_YELLOW_PIN);

        if (_checkup_manager_.should_enter_emergency(state_))
        {
            DEBUG_PRINT("Entering EMERGENCY state from DRIVING");
            _digital_sender_->enter_emergency_state();
            _checkup_manager_._ebs_sound_timestamp_.reset();
            state_ = AS_EMERGENCY;
            break;
        }
        if (_checkup_manager_.should_stay_driving())
            break;

        DEBUG_PRINT("Entering FINISHED state from DRIVING");
        DigitalSender::enter_finish_state();
        state_ = AS_FINISHED;
        break;
    case AS_FINISHED:
        if (_checkup_manager_.res_triggered())
        {
            DEBUG_PRINT("Entering EMERGENCY state from FINISHED");

            _digital_sender_->enter_emergency_state();
            _checkup_manager_._ebs_sound_timestamp_.reset();
            state_ = AS_EMERGENCY;
            break;
        }
        if (_checkup_manager_.should_stay_mission_finished())
            break;

        DEBUG_PRINT("Entering OFF state from FINISHED");
        DigitalSender::enter_off_state();
        _checkup_manager_.reset_checkup_state();
        state_ = AS_OFF;
        break;
    case AS_EMERGENCY:
        _digital_sender_->blink_led(ASSI_BLUE_PIN);

        if (_checkup_manager_.emergency_sequence_complete())
        {
            DEBUG_PRINT("Entering OFF state from EMERGENCY");
            DigitalSender::enter_off_state();
            _checkup_manager_.reset_checkup_state();
            state_ = AS_OFF;
            break;
        }
        break;
    default:
        break;
    }
}
