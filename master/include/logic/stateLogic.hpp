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
    DigitalSender *_digitalSender; ///< Pointer to the DigitalSender object for hardware interactions.
    Communicator *_communicator;   ///< Pointer to the Communicator object for communication operations.

public:
    CheckupManager _checkupManager; ///< CheckupManager object for handling various checkup operations.
    State state{AS_OFF};            ///< Current state of the vehicle system, initialized to OFF.

    /**
     * @brief Constructor for the ASState class.
     * @param system_data Pointer to the SystemData object containing system status and sensor information.
     * @param communicator Pointer to the Communicator object.
     * @param digital_sender Pointer to the DigitalSender object.
     */
    explicit ASState(SystemData *system_data, Communicator *communicator,
                     DigitalSender *digital_sender) : _digitalSender(digital_sender), _communicator(communicator), _checkupManager(system_data) {}

    /**
     * @brief Calculates the state of the vehicle.
     */
    void calculateState();
};

inline void ASState::calculateState()
{
    switch (state)
    {
    case AS_MANUAL:
        if (_checkupManager.shouldStayManualDriving())
            break;

        DEBUG_PRINT("Entering OFF state from MANUAL");
        DigitalSender::enterOffState();

        state = AS_OFF;
        break;

    case AS_OFF:

        // If manual driving checkup fails, the car can't be in OFF state, so it goes back to MANUAL
        if (_checkupManager.shouldStayManualDriving())
        {
            DEBUG_PRINT("Entering MANUAL state from OFF");
            DigitalSender::enterManualState();
            state = AS_MANUAL;
            break;
        }

        if (_checkupManager.shouldStayOff(_digitalSender))
            break;
        if (!_checkupManager.shouldGoReadyFromOff())
            break; // recheck all states

        DEBUG_PRINT("Entering READY state from OFF");
        DigitalSender::enterReadyState();
        state = AS_READY;
        DEBUG_PRINT("READY state entered...");
        break;

    case AS_READY:
        if (_checkupManager.shouldEnterEmergency(state))
        {
            DEBUG_PRINT("Entering EMERGENCY state from READY");
            _digitalSender->enterEmergencyState();
            _checkupManager._ebsSoundTimestamp.reset();
            state = AS_EMERGENCY;
            break;
        }
        if (_checkupManager.shouldStayReady())
        {
            break;
        }

        DEBUG_PRINT("Entering DRIVING state from READY");
        _digitalSender->enterDrivingState();
        state = AS_DRIVING;
        break;
    case AS_DRIVING:
        _digitalSender->blinkLED(ASSI_YELLOW_PIN);

        if (_checkupManager.shouldEnterEmergency(state))
        {
            DEBUG_PRINT("Entering EMERGENCY state from DRIVING");
            _digitalSender->enterEmergencyState();
            _checkupManager._ebsSoundTimestamp.reset();
            state = AS_EMERGENCY;
            break;
        }
        if (_checkupManager.shouldStayDriving())
            break;

        DEBUG_PRINT("Entering FINISHED state from DRIVING");
        DigitalSender::enterFinishState();
        state = AS_FINISHED;
        break;
    case AS_FINISHED:
        if (_checkupManager.resTriggered())
        {
            DEBUG_PRINT("Entering EMERGENCY state from FINISHED");

            _digitalSender->enterEmergencyState();
            _checkupManager._ebsSoundTimestamp.reset();
            state = AS_EMERGENCY;
            break;
        }
        if (_checkupManager.shouldStayMissionFinished())
            break;

        DEBUG_PRINT("Entering OFF state from FINISHED");
        DigitalSender::enterOffState();
        _checkupManager.resetCheckupState();
        state = AS_OFF;
        break;
    case AS_EMERGENCY:
        _digitalSender->blinkLED(ASSI_BLUE_PIN);

        if (_checkupManager.emergencySequenceComplete())
        {
            DEBUG_PRINT("Entering OFF state from EMERGENCY");
            DigitalSender::enterOffState();
            _checkupManager.resetCheckupState();
            state = AS_OFF;
            break;
        }
        break;
    default:
        break;
    }
}
