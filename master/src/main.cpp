#include "model/systemData.hpp"
#include "comm/communicator.hpp"
#include "embedded/digitalReceiver.hpp"
#include "embedded/digitalSender.hpp"
#include "logic/stateLogic.hpp"
#include "timings.hpp"

SystemData systemData; // Model
Communicator communicator = Communicator(); // CAN
DigitalReceiver digitalReceiver = DigitalReceiver(&systemData.digitalData, &systemData.mission); // Digital inputs
DigitalSender digitalSender = DigitalSender(); // Digital outputs
ASState as_state = ASState(&systemData, &communicator, &digitalSender);
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Communicator::can1;

Metro rl_rpm_timer = Metro{LEFT_WHEEL_PUBLISH_INTERVAL};
Metro mission_timer = Metro(MISSION_PUBLISH_INTERVAL);
Metro state_timer = Metro(STATE_PUBLISH_INTERVAL);
IntervalTimer state_calculation_timer;

void setup() {
    Communicator::_systemData = &systemData;

    state_calculation_timer.begin([]() {
        noInterrupts();
        digitalReceiver.digitalReads();
        as_state.calculateState();
        interrupts();
    }, STATE_CALCULATION_INTERVAL); // Ensuring 50ms intervals beween state calculations

    rl_rpm_timer.reset();
    mission_timer.reset();
    state_timer.reset();
}

void loop() {
    digitalReceiver.digitalReads();
    as_state.calculateState();
    
    if (rl_rpm_timer.check()) {
        Communicator::publish_left_wheel_rpm(systemData.mission);
        rl_rpm_timer.reset();
    }
    if (mission_timer.check()) {
        Communicator::publish_mission(systemData.mission);
        mission_timer.reset();
    }
    if (state_timer.check()) {
        Communicator::publish_state(as_state.state);
        state_timer.reset();
    }
}
