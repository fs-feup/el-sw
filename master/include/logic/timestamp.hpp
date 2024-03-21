#pragma once

#include <elapsedMillis.h>

class Timestamp {
private:
    elapsedMillis _timeElapsed;

public:
    /**
     * @brief Updates the timestamp by resetting the time elapsed to zero.
     *
     * This function should be called whenever the CAN sends a signal or any other event that requires updating the timestamp.
     */
    void update() {
        _timeElapsed = 0;
    }

    /**
     * @brief Checks if the timestamp has timed out.
     *
     * @param timeout The timeout in milliseconds.
     * @return true if the timestamp has timed out, false otherwise.
     */
    [[nodiscard]] bool hasTimedOut(unsigned long timeout) const {
        return _timeElapsed > timeout;
    }
};