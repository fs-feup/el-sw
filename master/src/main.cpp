#pragma once

#include <Arduino.h>
#include "logic/checkupManager.hpp"
#include "comm/communicator.hpp"
#include "embedded/digitalData.hpp"
#include <logic/stateLogic.hpp>

DigitalData digitalData;
CheckupManager checkupManager;
Sensors sensors;
Communicator communicator;
CheckupManager* Communicator::checkupManager = nullptr;
Sensors* Communicator::sensors = nullptr;
ASState as_state;

void setup() {
  checkupManager = CheckupManager();
  sensors =  Sensors();
  communicator = Communicator(&checkupManager, &sensors);

  as_state = ASState(&checkupManager);

  digitalData = DigitalData(&as_state.mission);
  
}

void loop() {
  digitalData.digitalReads();

  communicator.publish_state(as_state.state); // TODO(andre): fill with state
  // commManager->communicator->publish_mission(); // TODO(andre): fill with mission
  communicator.publish_left_wheel_rpm(as_state.mission);
}

