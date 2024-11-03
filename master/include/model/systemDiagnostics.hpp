#pragma once

#include <cstdlib>
#include "metro.h"
#include "Arduino.h"

#include "embedded/digitalSettings.hpp"
#include "debugUtils.hpp"

constexpr unsigned long READY_TIMEOUT_MS = 5000;
constexpr unsigned long RELEASE_EBS_TIMEOUT_MS = 1000;
constexpr unsigned long ENGAGE_EBS_TIMEOUT_MS = 5000;

struct R2DLogics
{
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
    void enter_ready_state()
    {
        readyTimestamp.reset();
        engageEbsTimestamp.reset();
        r2d = false;
    }

    /**
     * @brief resets timestamps for driving
     */
    void enter_driving_state()
    {
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

    bool process_go_signal()
    {
        // If r2d is not received or received before 5 seconds, return false (?_?)
        if (readyTimestamp.check())
        {
            r2d = true;
            return EXIT_SUCCESS;
        }
        // If r2d is received after the timeout duration, return true
        r2d = false;
        return EXIT_FAILURE;
    }
};

struct FailureDetection
{
    Metro pc_alive_timestamp{COMPONENT_TIMESTAMP_TIMEOUT};
    Metro steer_alive_timestamp{COMPONENT_TIMESTAMP_TIMEOUT};
    Metro inversor_alive_timestamp{COMPONENT_TIMESTAMP_TIMEOUT};
    Metro res_signal_loss_timestamp{RES_TIMESTAMP_TIMEOUT};
    Metro dc_voltage_drop_timestamp{DC_VOLTAGE_TIMEOUT}; // timer to check if dc voltage drops below threshold for more than 150ms
    Metro dc_voltage_hold_timestamp{DC_VOLTAGE_HOLD};    // timer for ts on, only after enough voltage for 1 sec
    bool steer_dead_{true};
    bool pc_dead_{true};
    bool inversor_dead_{true};
    bool res_dead_{true};
    bool emergency_signal{false};
    bool ts_on{false};
    double radio_quality{0};
    unsigned dc_voltage{0};

    [[nodiscard]] bool has_any_component_timed_out()
    { // no discard makes return value non ignorable
        steer_dead_ = steer_alive_timestamp.checkWithoutReset();
        pc_dead_ = pc_alive_timestamp.checkWithoutReset();
        inversor_dead_ = inversor_alive_timestamp.checkWithoutReset();
        res_dead_ = res_signal_loss_timestamp.checkWithoutReset();
        return steer_dead_ || pc_dead_ || inversor_dead_ || res_dead_;
    }
};
