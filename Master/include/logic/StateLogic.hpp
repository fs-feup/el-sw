#pragma once

#include <Arduino.h>
#include <elapsedMillis.h>
#include <embedded/Timestamp.hpp>
#include <logic/SystemDiagnostics.hpp>
#include <logic/CheckupManager.hpp>
#include "CheckupManager.hpp"


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
    CheckupManager* _checkupManager;

public:
    ASState(CheckupManager* checkupManager) : _checkupManager(checkupManager), state(AS_MANUAL) {};
    State getState() { return state; };
    void calculateState();
};

void ASState::calculateState()
{
    switch (state)
    {
    case AS_MANUAL:
        // do something
        break;
    case AS_OFF:
        // do something
        break;
    case AS_READY:
        // do something
        break;
    case AS_DRIVING:
        // do something
        break;
    case AS_FINISHED:
        // do something
        break;
    case AS_EMERGENCY:
        // do something
        break;
    default:
        break;
    }
}