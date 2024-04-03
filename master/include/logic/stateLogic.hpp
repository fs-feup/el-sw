#pragma once

#include <logic/checkupManager.hpp>
#include <logic/structure.hpp>
#include <embedded/digitalSender.hpp>

class ASState {
private:
    CheckupManager _checkupManager;
    DigitalSender _digitalSender;
    Communicator *_communicator;

public:
    State state{AS_MANUAL};

    explicit ASState(SystemData *system_data, Communicator *communicator) : _checkupManager(system_data),
                                                                            _communicator(communicator) {
    }

    void calculateState();

    void performEmergencyOperations();
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

            DigitalSender::enterReadyState();
            state = AS_READY;
            break;

        case AS_READY:
            _digitalSender.toggleWatchdog();

            if (_checkupManager.shouldRevertToOffFromReady()) {
                DigitalSender::enterOffState();
                state = AS_OFF;
                break;
            }

            if (_checkupManager.shouldEnterEmergency()) {
                performEmergencyOperations();
                state = AS_EMERGENCY;
                break;
            }
            if (_checkupManager.shouldStayR2D()) break;

            _digitalSender.enterDrivingState();
            state = AS_DRIVING;
            break;
        case AS_DRIVING:
            _digitalSender.toggleWatchdog();
            _digitalSender.blinkLED(ASSI_DRIVING_PIN);

            if (_checkupManager.shouldEnterEmergency()) {
                performEmergencyOperations();
                state = AS_EMERGENCY;
                break;
            }
            if (_checkupManager.shouldStayDriving()) break;

            DigitalSender::enterFinishState();
            state = AS_FINISHED;
            break;
        case AS_FINISHED:
            if (_checkupManager.resTriggered()) {
                performEmergencyOperations();
                state = AS_EMERGENCY;
                break;
            }
            if (_checkupManager.shouldStayMissionFinished())
                break;

            DigitalSender::enterOffState();
            state = AS_OFF;
            break;
        case AS_EMERGENCY:
            _digitalSender.blinkLED(ASSI_EMERGENCY_PIN);

            if (_checkupManager.emergencySequenceComplete()) {
                DigitalSender::enterOffState();
                state = AS_OFF;
                break;
            }
            break;
        default:
            break;
    }
}

inline void ASState::performEmergencyOperations() {
    //TODO: SET CORRECT MESSAGE HERE
    // _communicator->send_message();
    _digitalSender.enterEmergencyState();
}
