#pragma once

#include <model/sensors.hpp>
#include <model/systemDiagnostics.hpp>
#include <model/structure.hpp>
#include <model/digitalData.hpp>

struct SystemData {
    InternalLogics internalLogics;
    FailureDetection failureDetection;
    Sensors sensors;

    DigitalData digitalData;
    Mission mission{MANUAL};

    bool ready2Drive{false}, missionFinished{false};
    bool sdcState_OPEN{false};
};
