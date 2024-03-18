#include <Arduino.h>
#include "logic/checkupManager.hpp"
#include "comm/communicator.hpp"
#include "embedded/digitalData.hpp"

DigitalData* digitalData;
CheckupManager* checkupManager;
Sensors* sensors;
Communicator* communicator;
CheckupManager* Communicator::checkupManager = nullptr;
Sensors* Communicator::sensors = nullptr;


void setup() {
  digitalData = new DigitalData();
  sensors = new Sensors();

  checkupManager = new CheckupManager();
  
  communicator = new Communicator(checkupManager, sensors);
}

void loop() {
  digitalData->digitalReads();

  // commManager->communicator->publish_state(); // TODO(andre): fill with state
  // commManager->communicator->publish_mission(); // TODO(andre): fill with mission
  communicator->publish_left_wheel_rpm(digitalData->_left_wheel_rpm);
}

