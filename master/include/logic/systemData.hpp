
#include <logic/sensors.hpp>
#include <logic/systemDiagnostics.hpp>
#include <logic/structure.hpp>
#include <embedded/digitalData.hpp>

struct SystemData
{
    InternalLogics internalLogics;
    FailureDetection failureDetection;
    Sensors sensors;

    DigitalData digitalData;
    Mission mission{MANUAL};

    bool ready2Drive{0}, missionFinished{0};
    bool sdcState{0}, asmsState{0};
};