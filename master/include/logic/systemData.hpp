#pragma once

#include <logic/sensors.hpp>
#include <logic/systemDiagnostics.hpp>
#include <logic/structure.hpp>
#include <embedded/digitalData.hpp>

struct SystemData {
    InternalLogics internalLogics;
    FailureDetection failureDetection;
    Sensors sensors;

    DigitalData digitalData;
    Mission mission{MANUAL};

    bool ready2Drive{false}, missionFinished{false};
    bool sdcState{false}, asmsState{false};
};