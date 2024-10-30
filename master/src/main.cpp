#include "model/systemData.hpp"
#include "comm/communicator.hpp"
#include "embedded/digitalReceiver.hpp"
#include "embedded/digitalSender.hpp"
#include "logic/stateLogic.hpp"
#include "timings.hpp"
#include "debugUtils.hpp"

SystemData systemData; // Model
Communicator communicator = Communicator(&systemData); // CAN
DigitalReceiver digitalReceiver = DigitalReceiver(&systemData.digitalData, &systemData.mission); // Digital inputs
DigitalSender digitalSender = DigitalSender(); // Digital outputs
ASState as_state = ASState(&systemData, &communicator, &digitalSender);

Metro rl_rpm_timer = Metro{LEFT_WHEEL_PUBLISH_INTERVAL};
Metro mission_timer = Metro(MISSION_PUBLISH_INTERVAL);
Metro state_timer = Metro(STATE_PUBLISH_INTERVAL);
IntervalTimer state_calculation_timer;
//only publih debug log if there is a change in one of the states
uint8_t master_state_helper = static_cast<uint8_t>(15);
uint8_t checkup_state_helper = static_cast<uint8_t>(15);
uint8_t mission_helper = static_cast<uint8_t>(15);


void setup() {
    Serial.begin(9600);
    Communicator::_systemData = &systemData;
    communicator.init();
    // state_calculation_timer.begin([]() {
    //     noInterrupts();
    //     digitalReceiver.digitalReads();
    //     as_state.calculateState();
    //     interrupts();
    // }, STATE_CALCULATION_INTERVAL); // Ensuring 50ms intervals beween state calculations

    rl_rpm_timer.reset();
    mission_timer.reset();
    state_timer.reset();
    DEBUG_PRINT("Starting up...");
}

// Metro testing_timestamp{5000};

void loop() {   
    digitalReceiver.digitalReads();
    as_state.calculateState();
    
    if(master_state_helper!=static_cast<uint8_t>(as_state.state) || checkup_state_helper!=static_cast<uint8_t>(as_state._checkupManager.checkupState) || mission_helper!=static_cast<uint8_t>(systemData.mission)){
        master_state_helper=static_cast<uint8_t>(as_state.state);
        checkup_state_helper=static_cast<uint8_t>(as_state._checkupManager.checkupState);
        mission_helper=static_cast<uint8_t>(systemData.mission); 
        Communicator::publish_debug_log(systemData, as_state.state, static_cast<uint8_t>(as_state._checkupManager.checkupState));//mudar pointer se problemas de memória e incluir timer se demasiadas mensagens
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
