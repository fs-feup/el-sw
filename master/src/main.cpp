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
}

