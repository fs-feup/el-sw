#pragma once

#include <Arduino.h>
#include <logic/checkupManager.hpp>
#include <logic/structure.hpp>
#include <embedded/digitalSender.hpp>
#include <utility>

class ASState {
private:
    CheckupManager _checkupManager;
    // DigitalSender _digitalSender;

public:
    State state{AS_MANUAL};

    explicit ASState(CheckupManager checkupManager) : _checkupManager(std::move(checkupManager)) {};

    void calculateState();

    ASState();
};

void ASState::calculateState() {
    switch (state) {
        case AS_MANUAL:
            if (_checkupManager.manualDrivingCheckup())
                break;

            // TODO: EBS from disabled to inactive
            // TODO: Open SDC circuit

            state = AS_OFF;
            break;

        case AS_OFF:
            if (_checkupManager.offCheckup())
                break;

            state = AS_READY;
            break;

        case AS_READY:
            if (_checkupManager.emergencyCheckup()) {
                // Emergency, go to emergency state
                /* TODO:
                ASSI to blue flashing
                EBS to Active
                TS to OFF (open sdc)
                Buzzer to on for 8-10s
                */
                state = AS_EMERGENCY;
                break;
            }
            if (_checkupManager.r2dCheckup()) {
                // Not ready, do nothing
                break;
            }
            /* TODO:
            Ready to drive, go to driving state
            ASSI to yellow flashing
            EBS to inactive
            */
            state = AS_DRIVING;
            break;
        case AS_DRIVING:
            if (_checkupManager.emergencyCheckup()) {
                // Emergency, go to emergency state
                state = AS_EMERGENCY;
                break;
            }
            if (_checkupManager.missionFinishedCheckup()) {
                break;
            }
            break;
        case AS_FINISHED:
            if (_checkupManager.resTriggered()) {
                // TODO: perform necessary actions to enter AS_EMERGENCY
                state = AS_EMERGENCY;
                break;
            }
            if (_checkupManager.offCheckup()) {
                break;
            }
            // TODO: perform necessary actions to enter AS_OFF
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
