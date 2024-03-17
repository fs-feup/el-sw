#include <Arduino.h>
#include "logic/checkupManager.hpp"
#include "comm/manager.hpp"
#include "embedded/digitalData.hpp"

DigitalData* digitalData;
CheckupManager* checkupManager;
Sensors* sensors;
CommunicationManager* commManager;


void setup() {
  digitalData = new DigitalData();
  sensors = new Sensors();

  checkupManager = new CheckupManager();
  
  commManager = new CommunicationManager(checkupManager, sensors);
}

void loop() {
  digitalData->digitalReads();

  // commManager->communicator->publish_state(); // TODO(andre): fill with state
  // commManager->communicator->publish_mission(); // TODO(andre): fill with mission
  commManager->communicator->publish_left_wheel_rpm(digitalData->_left_wheel_rpm);
}
