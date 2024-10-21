#pragma once

#include <cstdlib>
#include "metro.h"
#include "Arduino.h"

#include "embedded/digitalSettings.hpp"
#include "debugUtils.hpp"

constexpr unsigned long READY_TIMEOUT_MS = 5000;
constexpr unsigned long RELEASE_EBS_TIMEOUT_MS = 1000;
constexpr unsigned long ENGAGE_EBS_TIMEOUT_MS = 5000;

struct R2DLogics {
    Metro readyTimestamp{READY_TIMEOUT_MS};

    /// Timestamp from when EBS is released on r2d, 
    /// used to tolerate a small delay before entering driving state
    Metro releaseEbsTimestamp{RELEASE_EBS_TIMEOUT_MS}; 

    /// Timestamp from when EBS is activate on entering ready state, 
    /// used to tolerate a small delay in which pneumatic line pressure is low
    Metro engageEbsTimestamp{ENGAGE_EBS_TIMEOUT_MS};
    bool r2d{false};

    /**
     * @brief resets timestamps for ready
     */
    void enterReadyState() {
        readyTimestamp.reset();
        engageEbsTimestamp.reset();
        r2d = false;
    }


    /**
     * @brief resets timestamps for driving
     */
    void enterDrivingState() {
        releaseEbsTimestamp.reset();
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
        // If r2d is not received or received before 5 seconds, return false (?_?)
        if (readyTimestamp.check()) {
            r2d = true;
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
    Metro resSignalLossTimestamp{RES_TIMESTAMP_TIMEOUT};
    bool steer_dead_{false};
    bool pc_dead_{false};
    bool inversor_dead_{false};
    bool res_dead_{false};
    bool emergencySignal{false};
    bool ts_on{false};
    double radio_quality{0};

    [[nodiscard]] bool has_any_component_timed_out() {//no discard makes return value non ignorable
        steer_dead_ = steerAliveTimestamp.checkWithoutReset();
        pc_dead_ = pcAliveTimestamp.checkWithoutReset();
        inversor_dead_ = inversorAliveTimestamp.checkWithoutReset();
        res_dead_ = resSignalLossTimestamp.checkWithoutReset();
        if (steer_dead_) {
            DEBUG_PRINT_VAR(steer_dead_);
        }
        if (pc_dead_) {
            DEBUG_PRINT_VAR(pc_dead_);
        }
        if (inversor_dead_) {
            DEBUG_PRINT_VAR(inversor_dead_);
        }
        if (res_dead_) {
            DEBUG_PRINT_VAR(res_dead_);
        }
        return steer_dead_ || pc_dead_ || inversor_dead_ || res_dead_;
            // pcAliveTimestamp.check() ||
            //    steerAliveTimestamp.check();
            //    inversorAliveTimestamp.check();
            //    resSignalLossTimestamp.check();
    }
};
