#include <logic/SystemDiagnostics.hpp>

// Also known as Orchestrator
class CheckupManager
{
private:
    FailureDetection _failureDetection;
    InternalLogics _internalLogics;
    Timestamp _ebsSoundTimestamp;
    bool _ready2Drive, _emergency, _SDCState, _ASMSState;

public:
    CheckupManager(): _failureDetection(), _internalLogics() {}
    bool r2dCheckup();
    bool emergencyCheckup();
    bool missionFinishedCheckup();
    bool initialCheckup();
    bool manualDrivingCheckup();
    bool offCheckup();
};
