#include <comm/communicator.hpp>
#include "logic/checkupManager.hpp"
#include "logic/systemData.hpp"
#include <embedded/digitalReceiver.hpp>
#include <logic/stateLogic.hpp>

SystemData* systemData;
DigitalReceiver digitalData;
CheckupManager checkupManager;
Communicator communicator;
ASState as_state;

void setup() {
  systemData = SystemData();
  digitalData = DigitalReceiver(&systemData.digitalData, &systemData.mission);
  communicator = Communicator(systemData);
  as_state = ASState(CheckupManager(&systemData));
}

void loop() {
  digitalData.digitalReads();

  communicator.publish_state(as_state.state); // TODO(andre): fill with state
  // commManager->communicator->publish_mission(); // TODO(andre): fill with
  // mission
  communicator.publish_left_wheel_rpm(systemData->mission);
}
