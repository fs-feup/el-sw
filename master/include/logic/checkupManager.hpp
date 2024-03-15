#include <logic/systemDiagnostics.hpp>

// Also known as Orchestrator
class CheckupManager
{
private:
    
    
    Timestamp _ebsSoundTimestamp;
    bool _emergency, _SDCState, _ASMSState;

public:
    InternalLogics _internalLogics;
    FailureDetection _failureDetection;

    bool _ready2Drive, _missionFinished;

    CheckupManager(): _failureDetection(), _internalLogics() {}
    bool r2dCheckup();
    bool emergencyCheckup();
    bool missionFinishedCheckup();
    bool initialCheckup();
    bool manualDrivingCheckup();
    bool offCheckup();
};
