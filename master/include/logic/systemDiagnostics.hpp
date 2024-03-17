#include <logic/timestamp.hpp>

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
        if (goSignal && readyTimestamp.hasTimedOut(5000))
        {
            goSignal = false;
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