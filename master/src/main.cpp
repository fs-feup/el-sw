#include "logic/systemData.hpp"
#include <comm/communicator.hpp>
#include <embedded/digitalReceiver.hpp>
#include "logic/checkupManager.hpp"
#include <logic/stateLogic.hpp>


SystemData systemData;
Communicator communicator;
DigitalReceiver digitalData = DigitalReceiver(&systemData.digitalData, &systemData.mission);
ASState as_state = ASState(CheckupManager(&systemData));

void setup() {
    Communicator::_systemData = &systemData;
}

void loop() {
  digitalData.digitalReads();

  communicator.publish_state(as_state.state);
  communicator.publish_mission(systemData.mission);
  communicator.publish_left_wheel_rpm(systemData.mission);
}
