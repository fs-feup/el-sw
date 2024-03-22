#pragma once

#include <logic/systemData.hpp>

// Also known as Orchestrator
/**
 * @brief The CheckupManager class handles various checkup operations.
 */
class CheckupManager {
private:
  SystemData *_systemData;
  Timestamp _ebsSoundTimestamp;

public:
  CheckupManager(SystemData *systemData) : _systemData(systemData){};

  /**
   * @brief Performs a manual driving checkup.
   * @return 0 if success, else 1.
   */
  bool manualDrivingCheckup();

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
  bool r2dCheckup();

  /**
   * @brief Performs an emergency checkup.
   * @return 0 if success, else 1.
   */
  bool emergencyCheckup();

  /**
   * @brief Performs a mission finished checkup.
   * @return 0 if success, else 1.
   */
  bool missionFinishedCheckup();

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
  bool resTriggered();
};

bool CheckupManager::manualDrivingCheckup() {
  if (/*TS OFF | DRIVERLESS MISSION SELECTED | EBS MANUAL ENABLE | EBS IS DEACTIVATED*/)
    {
    return false;
  }
}

bool CheckupManager::r2dCheckup() {
  if (!_systemData->internalLogics.goSignal) {
    return 1;
  }
  return 0;
}

bool CheckupManager::emergencySequenceComplete() {

  // TODO: If the emergency sequence is complete (buzzer done & ASMS OFF),
  // return 0, else 1
  if (/* BUZZER.hastimeout(8-9s) | ASMS_STATE == OFF*/)
    return 0;
}

bool CheckupManager::resTriggered() {
  // TODO
}

// TODO: don't forget check se batteryvoltage(aka vdc) > 60 e failure->
// bamocar-ready false emergency