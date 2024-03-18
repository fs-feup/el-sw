#pragma once

#include <logic/timestamp.hpp>

#define READY_TIMEOUT_MS 5000

struct InternalLogics
{
    Timestamp readyTimestamp;
    bool goSignal = false;

    InternalLogics() = default;

    void enterReadyState()
    {
        readyTimestamp.update();
        goSignal = false;
    }

    /**
     * @brief Processes the go signal.
     *
     * This function is responsible for processing the go signal.
     * It performs the necessary actions based on the received signal.
     *
     * @return 0 if the go signal was successfully processed, 1 otherwise.
     */
    bool processGoSignal()
    {
        // If goSignal is not received or received before 5 seconds, return false
        if (goSignal && readyTimestamp.hasTimedOut(READY_TIMEOUT_MS))
        {
            goSignal = true;
            return 0;
        }
        // If goSignal is received after 5 seconds, return true
        goSignal = false;
        return 1;
    }
};

struct FailureDetection
{
    Timestamp pcAliveTimestamp, steerAliveTimestamp, inversorAliveTimestamp, bmsAliveTimestamp;
    bool emergencySignal = false;
    bool bamocarReady = true;
    double bamocarTension = 0;

    FailureDetection() = default;

    bool hasAnyComponentTimedOut(unsigned long timeout) const
    {
        return pcAliveTimestamp.hasTimedOut(timeout) ||
               steerAliveTimestamp.hasTimedOut(timeout) ||
               inversorAliveTimestamp.hasTimedOut(timeout) ||
               bmsAliveTimestamp.hasTimedOut(timeout);
    }
};