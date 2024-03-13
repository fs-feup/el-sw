#include <embedded/Timestamp.hpp>

struct InternalLogics
{
    Timestamp readyTimestamp;
    bool goSignal;

    InternalLogics() : goSignal(false) {}

    void enterReadyState()
    {
        readyTimestamp.update();
        goSignal = false;
    }

    bool canProcessGoSignal()
    {
        // If goSignal is not received or received before 5 seconds, return false
        if (!goSignal || !readyTimestamp.hasTimedOut(5000))
        {
            return false;
        }
        // If goSignal is received after 5 seconds, return true
        return true;
    }
};

struct FailureDetection
{
    Timestamp pcAliveTimestamp, steerAliveTimestamp, inversorAliveTimestamp, bmsAliveTimestamp;
    bool emergencySignal;
    double bamocarTension;

    FailureDetection() : emergencySignal(false), bamocarTension(0.0) {}

    bool hasAnyComponentTimedOut(unsigned long timeout)
    {
        return pcAliveTimestamp.hasTimedOut(timeout) ||
               steerAliveTimestamp.hasTimedOut(timeout) ||
               inversorAliveTimestamp.hasTimedOut(timeout) ||
               bmsAliveTimestamp.hasTimedOut(timeout);
    }
};