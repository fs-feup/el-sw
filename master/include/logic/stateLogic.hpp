#pragma once

#include <logic/checkupManager.hpp>
#include <model/structure.hpp>
#include <embedded/digitalSender.hpp>

class ASState {
private:
    DigitalSender *_digitalSender;
    Communicator *_communicator;

public:
    CheckupManager _checkupManager;
    State state{AS_OFF};

    explicit ASState(SystemData *system_data, Communicator *communicator, 
    DigitalSender *digital_sender) : 
    _checkupManager(system_data), _digitalSender(digital_sender), _communicator(communicator) {}

    /**
     * @brief Calculates the state of the vehicle.
     */
    void calculateState();
};

inline void ASState::calculateState() {
    switch (state) {
        case AS_MANUAL:
            if (_checkupManager.shouldStayManualDriving()) break;

            DigitalSender::enterOffState();

            state = AS_OFF;
            break;

        case AS_OFF:

            // If manual driving checkup fails, the car can't be in OFF state, so it goes back to MANUAL
            if (_checkupManager.shouldStayManualDriving()) {
                DigitalSender::enterManualState();
                state = AS_MANUAL;
                break;
            }

            if (_checkupManager.shouldStayOff(_digitalSender)) break;
            if (!_checkupManager.shouldGoReadyFromOff()) break; // recheck all states

            systemData.r2dLogics.enterReadyState();
            DigitalSender::enterReadyState();
            state = AS_READY;
            break;

        case AS_READY:
            _digitalSender->toggleWatchdog();

            if (_checkupManager.shouldEnterEmergency(state)) {
                _digitalSender->enterEmergencyState();
                state = AS_EMERGENCY;
                break;
            }
            if (_checkupManager.shouldStayReady()) break;

            _digitalSender->enterDrivingState();
            state = AS_DRIVING;
            break;
        case AS_DRIVING:
            _digitalSender->toggleWatchdog();
            _digitalSender->blinkLED(ASSI_YELLOW_PIN);

            if (_checkupManager.shouldEnterEmergency(state)) {
                _digitalSender->enterEmergencyState();
                state = AS_EMERGENCY;
                break;
            }
            if (_checkupManager.shouldStayDriving()) break;

            DigitalSender::enterFinishState();
            state = AS_FINISHED;
            break;
        case AS_FINISHED:
            if (_checkupManager.resTriggered()) {
                // Buzzer is automatically triggered by state
                _digitalSender->enterEmergencyState();
                state = AS_EMERGENCY;
                break;
            }
            if (_checkupManager.shouldStayMissionFinished())
                break;

            DigitalSender::enterOffState();
            _checkupManager.resetCheckupState();
            state = AS_OFF;
            break;
        case AS_EMERGENCY:
            _digitalSender->blinkLED(ASSI_BLUE_PIN);

            if (_checkupManager.emergencySequenceComplete()) {
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


