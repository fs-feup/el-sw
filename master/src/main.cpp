#include <Arduino.h>
#include "logic/checkupManager.hpp"
#include "comm/manager.hpp"

void setup() {
  CheckupManager* checkupManager = new CheckupManager();
  Sensors* sensors = new Sensors();
  CommunicationManager* commManager = new CommunicationManager(checkupManager, sensors);
}

void loop() {
  // put your main code here, to run repeatedly:
}
