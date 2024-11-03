#pragma once

#include <model/sensors.hpp>
#include <model/systemDiagnostics.hpp>
#include <model/structure.hpp>
#include <model/digitalData.hpp>

/**
 * @brief The whole model of the system:
 * holds all the data necessary
 */
struct SystemData
{
    R2DLogics r2d_logics;
    FailureDetection failure_detection;
    Sensors sensors;

    DigitalData digital_data_;
    Mission mission{MANUAL};

    bool ready_2_drive{false}, mission_finished{false};
};
