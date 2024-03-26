#pragma once

#include <logic/systemData.hpp>
#include <cstdlib>
#include <x10.h>
#include <XBee.h>

// Also known as Orchestrator
/**
 * @brief The CheckupManager class handles various checkup operations.
 */
class CheckupManager {
private:
    SystemData *_systemData;
    Timestamp _ebsSoundTimestamp;

public:
    explicit CheckupManager(SystemData *systemData) : _systemData(systemData) {
    };

    /**
     * @brief Performs a manual driving checkup.
     * @return 0 if success, else 1.
     */
    [[nodiscard]] bool manualDrivingCheckup() const;

    /**
     * @brief Performs an off checkup.
     * @return 0 if success, else 1.
     */
    bool offCheckup();

    /**
     * @brief Performs an initial checkup.
     * @return 0 if success, else 1.
     */
    bool initialCheckup();

    /**
     * @brief Performs a ready to drive checkup.
     * @return 0 if success, else 1.
     */
    bool r2dCheckup() const;

    /**
     * @brief Performs an emergency checkup.
     * @return 0 if success, else 1.
     */
    bool emergencyCheckup() const;

    /**
     * @brief Performs a mission finished checkup.
     * @return 0 if success, else 1.
     */
    bool missionFinishedCheckup() const;

    /**
     * @brief Checks if the emergency sequence is complete and the vehicle can
     * transition to AS_OFF.
     * @return 0 if success, else 1.
     */
    bool emergencySequenceComplete();

    /**
     * @brief Checks if the RES has been triggered.
     *
     * This function checks whether the RES has been triggered or not.
     *
     * @return 1 if the RES has been triggered, 0 otherwise.
     */
    bool resTriggered() const;

    CheckupManager();
};

inline bool CheckupManager::manualDrivingCheckup() const {
    /* AATS OFF | MISSION NOT MANUAL | EBS IS DISABLED --> Transition to AS_OFF
     * AATS ON  & MISSION MANUAL     & EBS INACTIVE    --> Transition to AS_READY
     *
     * In EXIT_SUCCESS, the vehicle can transition to AS_OFF.
     * IN EXIT_FAILURE, the vehicle can transition to AS_MANUAL.
     */

    if (!_systemData->digitalData.aats_on || _systemData->mission != MANUAL || _systemData->digitalData.
        pneumatic_line_pressure != 0) {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

inline bool CheckupManager::offCheckup() {
    if (!(_systemData->digitalData.asms_on && _systemData->digitalData.aats_on)) {
        return EXIT_FAILURE;
    }
    if (initialCheckup())
        return EXIT_FAILURE;

    _systemData->internalLogics.enterReadyState();
    return EXIT_SUCCESS;
}

inline bool CheckupManager::initialCheckup() {
    // TODO: Refer to initial checkup flowchart
    return EXIT_SUCCESS;
}

inline bool CheckupManager::r2dCheckup() const {
    if (!_systemData->internalLogics.goSignal) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

inline bool CheckupManager::emergencyCheckup() const {
    //TODO Continuous monitoring sequence
    return EXIT_SUCCESS;
}

inline bool CheckupManager::missionFinishedCheckup() const {
    if (_systemData->digitalData.asms_on) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

inline bool CheckupManager::emergencySequenceComplete() {
    // // TODO: If the emergency sequence is complete (buzzer done & ASMS OFF),
    // // return 0, else 1
    // if (/* BUZZER.hastimeout(8-9s) | ASMS_STATE == OFF*/)
    //   return 0;

    return 1;
}

inline bool CheckupManager::resTriggered() const {
    if (_systemData->failureDetection.emergencySignal) {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

// TODO: don't forget check se batteryvoltage(aka vdc) > 60 e failure->
// bamocar-ready false emergency
