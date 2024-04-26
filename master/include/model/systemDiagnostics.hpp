#pragma once

#include <cstdlib>
#include <Metro.h>
#include "Arduino.h"

#include "embedded/digitalSettings.hpp"

constexpr unsigned long READY_TIMEOUT_MS = 5000;
constexpr unsigned long RELEASE_EBS_TIMEOUT_MS = 1000;

struct R2DLogics {
    Metro readyTimestamp{READY_TIMEOUT_MS};
    unsigned long releaseEbsTimestamp = millis();
    bool r2d{false};

    void enterReadyState() {
        readyTimestamp.reset();
        r2d = false;
    }

    /**
     * @brief Processes the go signal.
     *
     * This function is responsible for processing the go signal.
     * It performs the necessary actions based on the received signal.
     *
     * @return 0 if the go signal was successfully processed, 1 otherwise.
     */

    bool processGoSignal() {
        // If r2d is not received or received before 5 seconds, return false
        if (readyTimestamp.check()) {
            r2d = true;
            releaseEbsTimestamp = millis(); // starts ebs timeout to release when going as driving
            return EXIT_SUCCESS;
        }
        // If r2d is received after the timeout duration, return true
        r2d = false;
        return EXIT_FAILURE;
    }
};

struct FailureDetection {
    Metro pcAliveTimestamp{COMPONENT_TIMESTAMP_TIMEOUT};
    Metro steerAliveTimestamp{COMPONENT_TIMESTAMP_TIMEOUT};
    Metro inversorAliveTimestamp{COMPONENT_TIMESTAMP_TIMEOUT};
    bool emergencySignal{false};
    bool ts_on{false};
    double radio_quality{0};

    [[nodiscard]] bool hasAnyComponentTimedOut() {
        return pcAliveTimestamp.check() ||
               steerAliveTimestamp.check() ||
               inversorAliveTimestamp.check();
    }
};
