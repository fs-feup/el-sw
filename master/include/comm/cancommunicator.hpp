#ifndef COMM_CANCOMMUNICATOR_HPP_
#define COMM_CANCOMMUNICATOR_HPP_

#include "comm/communicator.hpp"

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
class CANCommunicator : public Communicator {
    FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

    public: 
    CANCommunicator(std::string can_network);

    static void parse_message(const CAN_message_t& msg);

    int publish_state(int state_id);
    int publish_mission(int mission_id);
    int publish_rr_wheel_rpm(double value);
    int send_message(const unsigned int len, const unsigned char* buffer, const unsigned int id);
};

#ifdef EMBEDDED
#include <FlexCAN_T4.h>

CANCommunicator::CANCommunicator(std::string can_network) : Communicator(can_network) {
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

void CANCommunicator::parse_message(const CAN_message_t& msg) {
    switch(msg.id) {
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
            bool emg_stop1 = msg.buf[0] && 0x01;
            bool emg_stop2 = msg.buf[3] >> 7 && 0x01;
            bool go_switch = (msg.buf[0] >> 1) && 0x01;
            bool go_button = (msg.buf[0] >> 2) && 0x01;

            if (go_button || go_switch)
                manager.goCallback();
            else if (emg_stop1 || emg_stop2)
                manager.emergencyCallback();
            break;
        case RL_WHEEL:
            // see with barros É SÓ SANITY CHECK?
            break;
        case HYDRAULIC_LINE:
            // see with barros É SÓ SANITY CHECK?
            break;
        case BAMO_RESPONSE_ID:
            // see with barros
            // one always receiving
            // one on ask
            break;
        case STEERING_ANGLE:
            // check documentation ak70-10
            // only alive
            manager.steeringCallback();
            break;
        default:
            break;
    }
}

int CANCommunicator::publish_state(int state_id) {
    unsigned int id = STATE_MSG;
    unsigned char msg[] = {static_cast<unsigned char>(state_id)};

    this->send_message(1, msg, id);
}

int CANCommunicator::publish_mission(int mission_id) {
    unsigned int id = MISSION_MSG;
    unsigned char msg[] = {static_cast<unsigned char>(mission_id)};

    this->send_message(1, msg, id);
}

int CANCommunicator::publish_rr_wheel_rpm(double value) {
    // TODO (andre): define ordem de grandeza para mandar
}

int CANCommunicator::send_message(const unsigned int len, const unsigned char* buffer, const unsigned int id) {
    
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