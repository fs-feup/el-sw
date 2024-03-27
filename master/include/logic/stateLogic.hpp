#pragma once

#include <Arduino.h>
#include <logic/checkupManager.hpp>
#include <logic/structure.hpp>
#include <embedded/digitalSender.hpp>
#include <utility>

class ASState {
private:
    CheckupManager _checkupManager;
    DigitalSender _digitalSender;

public:
    State state{AS_MANUAL};

    explicit ASState(CheckupManager checkupManager) : _checkupManager(std::move(checkupManager)) {
    }

    void calculateState();
};

inline void ASState::calculateState() {
    switch (state) {
        case AS_MANUAL:
            if (!_checkupManager.manualDrivingCheckup())
                break;

            DigitalSender::openSDC();
            DigitalSender::sendDigitalSignal(EBS_VALVE_1_PIN, LOW);
            DigitalSender::sendDigitalSignal(EBS_VALVE_2_PIN, LOW);
            state = AS_OFF;
            break;

        case AS_OFF:

            // If manual driving checkup fails, the car can't be in OFF state, so it goes back to MANUAL
            if (_checkupManager.manualDrivingCheckup()) {
                //SDC TO Shortcut ???
                //TODO: Nothing needed?
                state = AS_MANUAL;
                break;
            }

            if (_checkupManager.offCheckup())
                break;

            DigitalSender::sendDigitalSignal(ASSI_READY_PIN, HIGH);
            DigitalSender::closeSDC();
            DigitalSender::sendDigitalSignal(EBS_VALVE_1_PIN, HIGH);
            DigitalSender::sendDigitalSignal(EBS_VALVE_2_PIN, HIGH);
            state = AS_READY;
            break;

        case AS_READY:
            if (_checkupManager.emergencyCheckup()) {
                DigitalSender::enterEmergencyState();
                state = AS_EMERGENCY;
                break;
            }
            if (_checkupManager.r2dCheckup()) {
                // Not ready, do nothing
                break;
            }

            DigitalSender::sendDigitalSignal(ASSI_DRIVING_PIN, HIGH);
            DigitalSender::sendDigitalSignal(EBS_VALVE_1_PIN, LOW);
            DigitalSender::sendDigitalSignal(EBS_VALVE_2_PIN, LOW);
            state = AS_DRIVING;
            break;
        case AS_DRIVING:
            if (_checkupManager.emergencyCheckup()) {
                // Emergency, go to emergency state
                DigitalSender::enterEmergencyState();
                
                state = AS_EMERGENCY;
                break;
            }
            if (_checkupManager.drivingCheckup()) {
                // Mission not complete, do nothing
                break;
            }

            DigitalSender::sendDigitalSignal(ASSI_FINISH_PIN, HIGH);
            DigitalSender::openSDC();
            DigitalSender::sendDigitalSignal(EBS_VALVE_1_PIN, HIGH);
            DigitalSender::sendDigitalSignal(EBS_VALVE_2_PIN, HIGH);
            state = AS_FINISHED;
            break;
        case AS_FINISHED:
            if (_checkupManager.resTriggered()) {
                DigitalSender::enterEmergencyState();
                state = AS_EMERGENCY;
                break;
            }
            if (_checkupManager.missionFinishedCheckup()) {
                break;
            }

            DigitalSender::sendDigitalSignal(EBS_VALVE_1_PIN, LOW);
            DigitalSender::sendDigitalSignal(EBS_VALVE_2_PIN, LOW);
            DigitalSender::openSDC();
            state = AS_OFF;
            break;
        case AS_EMERGENCY:
            if (_checkupManager.emergencySequenceComplete()) {
                break;
            }
        // TODO: perform necessary actions to enter AS_OFF
            state = AS_OFF;
            break;
        default:
            break;
    }
}
