#include "model/systemData.hpp"
#include "comm/communicator.hpp"
#include "embedded/digitalReceiver.hpp"
#include "embedded/digitalSender.hpp"
#include "logic/stateLogic.hpp"
#include "timings.hpp"
#include "enum_utils.hpp"
#include "debugUtils.hpp"

SystemData system_data;                                                                           // Model
Communicator communicator = Communicator(&system_data);                                           // CAN
DigitalReceiver digital_receiver = DigitalReceiver(&system_data.digital_data_, &system_data.mission); // Digital inputs
DigitalSender digital_sender = DigitalSender();                                                   // Digital outputs
ASState as_state = ASState(&system_data, &communicator, &digital_sender);

Metro rl_rpm_timer = Metro{LEFT_WHEEL_PUBLISH_INTERVAL};
Metro mission_timer = Metro(MISSION_PUBLISH_INTERVAL);
Metro state_timer = Metro(STATE_PUBLISH_INTERVAL);
IntervalTimer state_calculation_timer;
// only publih debug log if there is a change in one of the states
uint8_t master_state_helper = static_cast<uint8_t>(15);
uint8_t checkup_state_helper = static_cast<uint8_t>(15);
uint8_t mission_helper = static_cast<uint8_t>(15);

void setup()
{
    Serial.begin(9600);
    Communicator::_systemData = &system_data;
    communicator.init();
    rl_rpm_timer.reset();
    mission_timer.reset();
    state_timer.reset();
    DEBUG_PRINT("Starting up...");
}

void loop()
{
    digital_receiver.digital_reads();
    as_state.calculate_state();
    if (master_state_helper != static_cast<uint8_t>(as_state.state_) || checkup_state_helper != static_cast<uint8_t>(as_state._checkup_manager_.checkup_state_) || mission_helper != static_cast<uint8_t>(system_data.mission))
    {
        master_state_helper = static_cast<uint8_t>(as_state.state_);
        checkup_state_helper = static_cast<uint8_t>(as_state._checkup_manager_.checkup_state_);
        mission_helper = static_cast<uint8_t>(system_data.mission);
        Communicator::publish_debug_log(system_data, to_underlying(as_state.state_), static_cast<uint8_t>(as_state._checkup_manager_.checkup_state_)); // mudar pointer se problemas de memória
    }
    if (mission_timer.check())
    {
        Communicator::publish_mission(to_underlying(system_data.mission));
        mission_timer.reset();
    }
    if (state_timer.check())
    {
        Communicator::publish_state(to_underlying(as_state.state_));
        state_timer.reset();
    }
}
