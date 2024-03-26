#include "logic/systemData.hpp"
#include <comm/communicator.hpp>
#include <embedded/digitalReceiver.hpp>
#include "logic/checkupManager.hpp"
#include <logic/stateLogic.hpp>

SystemData systemData;
Communicator communicator;
auto digitalData = DigitalReceiver(&systemData.digitalData, &systemData.mission);
auto as_state = ASState(CheckupManager(&systemData));
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Communicator::can1;

void setup() {
    Communicator::_systemData = &systemData;
}

void loop() {
    digitalData.digitalReads();
    as_state.calculateState();
    communicator.publish_state(as_state.state);
    communicator.publish_mission(systemData.mission);
    communicator.publish_left_wheel_rpm(systemData.mission);
}
