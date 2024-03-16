#ifndef COMM_Communicator_HPP_
#define COMM_Communicator_HPP_

#include <string>
#include "comm/message.hpp"

CommunicationManager manager;

struct Code {
    int key;
    int code;
};

Code fifoCodes[] = {
    {0, C3_ID},
    {1, BMS_ID},
    {2, BAMO_RESPONSE_ID}
};
// TODO(andre): fill the rest of the map

/**
 * @brief Communication strategy class for communication
 * using Teensy CAN libraries.
*/
class Communicator {
    std::string can_network_name;
    FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

    public: 
    Communicator(std::string can_network);
    std::string get_network_name() const;

    static void parse_message(const CAN_message_t& msg);

    int publish_state(int state_id);
    int publish_mission(int mission_id);
    int publish_left_wheel_rpm(double value);
    int send_message(const unsigned int len, const unsigned char* buffer, const unsigned int id);
};

#ifdef EMBEDDED
#include <FlexCAN_T4.h>

Communicator::Communicator(std::string can_network) : can_network_name(can_network) {
    can1.begin();
    can1.setBaudRate(500000);
    can1.enableFIFO();
    can1.enableFIFOInterrupt();
    can1.setFIFOFilter(REJECT_ALL);
    for (unsigned i = 0; i < sizeof(fifoCodes) / sizeof(fifoCodes[0]); i++)
        can1.setFIFOFilter(fifoCodes[i].key, fifoCodes[i].code, STD);
    
    can1.onReceive(parse_message);
    //initMessages();
};

std::string Communicator::get_network_name() const {
    return this->can_network_name; 
}

void Communicator::parse_message(const CAN_message_t& msg) {
    switch(msg.id) {
        // pc messages
        case AS_CU_EMERGENCY_SIGNAL:
            manager.emergencyCallback();
            break;
        case MISSION_FINISHED:
            manager.missionFinishedCallback();
            break;
        case PC_ALIVE:
            manager.pcAliveCallback();
            break;

        case RES:
            manager.resCallback(msg.buf);
            break;

        // sensor messages
        case RIGHT_WHEEL:
            // see with barros É SÓ SANITY CHECK?
            break;
        case HYDRAULIC_LINE:
            // see with barros É SÓ SANITY CHECK?
            break;

        case BAMO_RESPONSE_ID:
            manager.bamocarCallback(msg.buf);
            break;
        case STEERING_ACTUATOR:
            manager.steeringCallback();
            break;
        default:
            break;
    }
}

int Communicator::publish_state(int state_id) {
    unsigned int id = STATE_MSG;
    unsigned char msg[] = {static_cast<unsigned char>(state_id)};

    this->send_message(1, msg, id);
}

int Communicator::publish_mission(int mission_id) {
    unsigned int id = MISSION_MSG;
    unsigned char msg[] = {static_cast<unsigned char>(mission_id)};

    this->send_message(1, msg, id);
}

int Communicator::publish_left_wheel_rpm(double value) {
    // TODO (andre): define ordem de grandeza para mandar
}

int Communicator::send_message(const unsigned int len, const unsigned char* buffer, const unsigned int id) {
    
    CAN_message_t can_message;
    can_message.id = id;
    can_message.len = len;
    for (unsigned int i = 0; i < len; i++) {
        can_message.buf[i] = buffer[i];
    }
    can1.begin();
    can1.write(can_message);
    
    return 0;
} 
#endif

#endif