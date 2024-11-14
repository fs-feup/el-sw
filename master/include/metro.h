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
    /**
     * @brief Constructor to initialize the timer with a given interval and autoreset behavior
     * @param interval_millis The interval in milliseconds for the timer
     * @param autoreset If set to non-zero, the timer will reset automatically upon a successful check
     *
     * This constructor allows the user to specify both the interval and the autoreset behavior.
     * If `autoreset` is set to zero, the timer will follow Benjamin Soelberg's check behavior
     * where the timer will update based on the interval instead of resetting to the current time.
     */
    Metro(unsigned long interval_millis, uint8_t autoreset = 0);

    /**
     * @brief Sets a new interval for the timer
     * @param interval_millis The new interval in milliseconds
     *
     * This method allows the user to change the interval dynamically at runtime. The interval
     * will be used for subsequent checks.
     */
    void interval(unsigned long interval_millis);

    /**
     * @brief Checks if the interval has passed and resets the timer if true
     * @return true if the interval has passed, false otherwise
     *
     * This method checks whether the specified interval has passed since the last reset. If the
     * interval has passed, the timer is reset according to the autoreset behavior. If autoreset
     * is enabled, the timer resets to the current time; otherwise, it increments by the interval.
     */
    bool check();

    /**
     * @brief Checks if the interval has passed without resetting the timer
     * @return true if the interval has passed, false otherwise
     *
     * This method performs a check similar to `check()`, but it does not reset the timer upon
     * a successful check. It is useful when you want to verify the passage of time without
     * affecting the internal state.
     */
    bool checkWithoutReset() const;

    /**
     * @brief Resets the timer to the current time
     *
     * This method resets the timer, updating the internal state to the current time. After
     * calling this method, the timer will begin counting from zero again.
     */
    void reset();

private:
    uint8_t autoreset;             ///< Controls whether the timer resets automatically or not
    unsigned long previous_millis; ///< Stores the last recorded time in milliseconds
    unsigned long interval_millis; ///< The interval duration in milliseconds
};

inline Metro::Metro(unsigned long interval_millis, uint8_t autoreset)
{
    this->autoreset = autoreset; // Fix by Paul Bouchier
    interval(interval_millis);
    reset();
}

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

inline bool Metro::checkWithoutReset() const
{
    if (millis() - this->previous_millis >= this->interval_millis)
    {
        return 1;
    }
    return 0;
}

void Metro::reset()
{

    this->previous_millis = millis();
}
