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
            if (_checkupManager.manualDrivingCheckup()) break;

            DigitalSender::enterOffState();

            state = AS_OFF;
            break;

        case AS_OFF:

            // If manual driving checkup fails, the car can't be in OFF state, so it goes back to MANUAL
            if (_checkupManager.manualDrivingCheckup()) {
                DigitalSender::enterManualState();
                state = AS_MANUAL;
                break;
            }

            if (_checkupManager.offCheckup()) break;

            DigitalSender::enterReadyState();
            state = AS_READY;
            break;

        case AS_READY:
            if (_checkupManager.emergencyCheckup()) {
                performEmergencyOperations();
                state = AS_EMERGENCY;
                break;
            }
            if (_checkupManager.r2dCheckup()) break;

            DigitalSender::enterDrivingState();
            state = AS_DRIVING;
            break;
        case AS_DRIVING:
            if (_checkupManager.emergencyCheckup()) {
                performEmergencyOperations();
                state = AS_EMERGENCY;
                break;
            }
            if (_checkupManager.drivingCheckup()) break;

            DigitalSender::enterFinishState();
            state = AS_FINISHED;
            break;
        case AS_FINISHED:
            if (_checkupManager.resTriggered()) {
                performEmergencyOperations();
                state = AS_EMERGENCY;
                break;
            }
            if (_checkupManager.missionFinishedCheckup()) break;

            DigitalSender::enterOffState();
            state = AS_OFF;
            break;
        case AS_EMERGENCY:
            if (_checkupManager.emergencySequenceComplete()) break;

            DigitalSender::enterOffState();
            state = AS_OFF;
            break;
        default:
            break;
    }
}

inline void ASState::performEmergencyOperations() {
    //TODO: SET CORRECT MESSAGE HERE
    // _communicator->send_message();
    DigitalSender::enterEmergencyState();
}
