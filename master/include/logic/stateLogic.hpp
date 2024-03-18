#pragma once

#include <Arduino.h>
#include <elapsedMillis.h>
#include <logic/timestamp.hpp>
#include <logic/systemDiagnostics.hpp>
#include <logic/checkupManager.hpp>

enum State
{
    AS_MANUAL,
    AS_OFF,
    AS_READY,
    AS_DRIVING,
    AS_FINISHED,
    AS_EMERGENCY
};

class ASState
{
private:
    State state;
    CheckupManager *_checkupManager;

public:
    ASState(CheckupManager *checkupManager) : _checkupManager(checkupManager), state(AS_MANUAL){};
    State getState() { return state; };
    void calculateState();
};

void ASState::calculateState()
{
    switch (state)
    {
    case AS_MANUAL:
        if (_checkupManager->manualDrivingCheckup())
        {
            return;
        }
        // TODO: EBS from disabled to inactive
        // TODO: Open SDC circuit
        state = AS_OFF;
        break;

    case AS_OFF:
        if (_checkupManager->offCheckup())
        {
            break;
        }
        state = AS_READY;
        break;

    case AS_READY:
        if (_checkupManager->emergencyCheckup())
        {
            // Emergency, go to emergency state
            /* TODO:
            ASSI to blue flashing
            EBS to Active
            SA to disable
            TS to OFF
            Buzzer to on for 8-10s
            */
            state = AS_EMERGENCY;
            break;
        }
        if (_checkupManager->r2dCheckup())
        {
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
        if (_checkupManager->emergencyCheckup())
        {
            // Emergency, go to emergency state
            state = AS_EMERGENCY;
            break;
        }
        if (_checkupManager->missionFinishedCheckup())
        {
            break;
        }
        break;
    case AS_FINISHED:
        if (_checkupManager->resTriggered())
        {
            //TODO: perform necessary actions to enter AS_EMERGENCY
            state = AS_EMERGENCY;
            break;
        }
        if (_checkupManager->offCheckup())
        {
            break;
        }
        // TODO: perform necessary actions to enter AS_OFF
        state = AS_OFF;
        break;
    case AS_EMERGENCY:
        if (_checkupManager->emergencySequenceComplete())
        {
            break;
        }
        // TODO: perform necessary actions to enter AS_OFF
        state = AS_OFF;
        break;
    default:
        break;
    }
}