#pragma once

#include <cstdlib>
#include <Metro.h>
#include <embedded/digitalSettings.hpp>

constexpr unsigned long READY_TIMEOUT_MS = 5000;

struct InternalLogics {
    Metro readyTimestamp{READY_TIMEOUT_MS};
    bool goSignal{false};

    void enterReadyState() {
        readyTimestamp.reset();
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

    bool processGoSignal() {
        // If goSignal is not received or received before 5 seconds, return false
        if (goSignal && readyTimestamp.check()) {
            goSignal = true;
            return EXIT_SUCCESS;
        }
        // If goSignal is received after the timeout duration, return true
        goSignal = false;
        return EXIT_FAILURE;
    }
};

struct FailureDetection {
    Metro pcAliveTimestamp{COMPONENT_TIMESTAMP_TIMEOUT};
    Metro steerAliveTimestamp{COMPONENT_TIMESTAMP_TIMEOUT};
    Metro inversorAliveTimestamp{COMPONENT_TIMESTAMP_TIMEOUT};
    Metro bmsAliveTimestamp{COMPONENT_TIMESTAMP_TIMEOUT};
    bool emergencySignal{false};
    double bamocarTension{0.0}; // Add default member initializer
    bool bamocarReady{true};
    double radio_quality{0};

    FailureDetection() = default;

    [[nodiscard]] bool hasAnyComponentTimedOut() {
        return pcAliveTimestamp.check() ||
               steerAliveTimestamp.check() ||
               inversorAliveTimestamp.check() ||
               bmsAliveTimestamp.check();
    }
};
