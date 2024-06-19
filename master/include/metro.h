#pragma once

#include "Arduino.h"
#include <inttypes.h>

/**
 * @brief Our own implementation of Metro class
 * @details This class provides functionality to check if a certain amount of time has passed
 * since the last time the check was true. It adds to the public implementation of the Metro class
 * the possibility to check if the time has passed without resetting the timer.
 */
class Metro
{

public:
    Metro(unsigned long interval_millis);
    Metro(unsigned long interval_millis, uint8_t autoreset);
    void interval(unsigned long interval_millis);
    bool check();
    bool checkWithoutReset();
    void reset();

private:
    uint8_t autoreset;
    unsigned long previous_millis, interval_millis;
};

inline Metro::Metro(unsigned long interval_millis)
{
    this->autoreset = 0;
    interval(interval_millis);
    reset();
}

// New creator so I can use either the original check behavior or benjamin.soelberg's
// suggested one (see below).
// autoreset = 0 is benjamin.soelberg's check behavior
// autoreset != 0 is the original behavior

inline Metro::Metro(unsigned long interval_millis, uint8_t autoreset)
{
    this->autoreset = autoreset; // Fix by Paul Bouchier
    interval(interval_millis);
    reset();
}

/**
 * @brief Sets the interval of the timer
 * 
 * @param interval_millis The interval in milliseconds
 */
inline void Metro::interval(unsigned long interval_millis)
{
    this->interval_millis = interval_millis;
}

// Benjamin.soelberg's check behavior:
// When a check is true, add the interval to the internal counter.
// This should guarantee a better overall stability.

// Original check behavior:
// When a check is true, add the interval to the current millis() counter.
// This method can add a certain offset over time.

/**
 * @brief Checks if the interval has passed
 * @details This function checks if the interval has passed since 
 * the last time the function was called and resets the timer if 
 * the interval had already passed.
 * 
 * @return true if the interval has passed, false otherwise
 */
inline bool Metro::check()
{
    if (millis() - this->previous_millis >= this->interval_millis)
    {
        // As suggested by benjamin.soelberg@gmail.com, the following line
        // this->previous_millis = millis();
        // was changed to
        // this->previous_millis += this->interval_millis;

        // If the interval is set to 0 we revert to the original behavior
        if (this->interval_millis <= 0 || this->autoreset)
        {
            this->previous_millis = millis();
        }
        else
        {
            this->previous_millis += this->interval_millis;
        }

        return 1;
    }

    return 0;
}
/**
 * @brief Checks if the interval has passed without resetting the timer
 * 
 * @return true if the interval has passed, false otherwise
 */
inline bool Metro::checkWithoutReset()
{
    if (millis() - this->previous_millis >= this->interval_millis)
    {
        return 1;
    }
    return 0;
}

/**
 * @brief Resets the timer
 */
void Metro::reset()
{

    this->previous_millis = millis();
}
