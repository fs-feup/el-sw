#ifndef COMM_CANCOMMUNICATOR_HPP_
#define COMM_CANCOMMUNICATOR_HPP_


#include "comm/communicator.hpp"

/**
 * @brief Communication strategy class for communication
 * using Teensy CAN libraries.
*/
class CANCommunicator : public Communicator {

    public: 
    CANCommunicator(std::string can_network);
    int send_message(const unsigned int len, const unsigned char* buffer, const unsigned int id);
};

#ifdef EMBEDDED
#include <FlexCAN_T4.h>

CANCommunicator::CANCommunicator(std::string can_network) : Communicator(can_network) {};

int CANCommunicator::send_message(const unsigned int len, const unsigned char* buffer, const unsigned int id) {
    if (this->get_network_name() == "can1") {
        FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
        CAN_message_t can_message;
        can_message.id = id;
        can_message.len = len;
        for (unsigned int i = 0; i < len; i++) {
            can_message.buf[i] = buffer[i];
        }
        can1.begin();
        can1.write(can_message);
    }

    return 0;
} 
#endif

#endif