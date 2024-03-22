#include <comm/communicator.hpp>
#include <logic/systemData.hpp>
#include <embedded/digitalReceiver.hpp>
#include <logic/stateLogic.hpp>
#include <logic/checkupManager.hpp>

SystemData systemData;
Communicator communicator;
auto digitalData = DigitalReceiver(&systemData.digitalData, &systemData.mission);
auto as_state = ASState(CheckupManager(&systemData));

void setup() {
    Communicator::_systemData = &systemData;
}

void loop() {
    digitalData.digitalReads();
    as_state.calculateState();
    communicator.publish_state(as_state.state); // TODO(andre): fill with state
    // commManager->communicator->publish_mission(); // TODO(andre): fill with
    // mission
    communicator.publish_left_wheel_rpm(systemData.mission);
}
