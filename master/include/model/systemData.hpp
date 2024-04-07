#pragma once

#include <model/sensors.hpp>
#include <model/systemDiagnostics.hpp>
#include <model/structure.hpp>
#include <model/digitalData.hpp>

/**
 * @brief The whole model of the system:
 * holds all the data necessary
*/
struct SystemData {
    R2DLogics internalLogics;
    FailureDetection failureDetection;
    Sensors sensors;

    DigitalData digitalData;
    Mission mission{MANUAL};

    bool ready2Drive{false}, missionFinished{false};
};
