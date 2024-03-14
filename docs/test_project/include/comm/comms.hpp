#ifndef COMM_COMMS_HPP_
#define COMM_COMMS_HPP_

#ifdef EMBEDDED
#include "comm/cancommunicator.hpp"
#else
#include "comm/vcancommunicator.hpp"
#endif

/**
 * @brief Class that contains definitions of typical messages to send via CAN
 * It serves only as an example of the usage of the strategy pattern,
 * where the communicator is the strategy
*/
class Messager {

    Communicator* communicator;
    public:
    Messager(Communicator* communicator);

    void send_typical_message();
};

Messager::Messager(Communicator* communicator) : communicator(communicator) {};

/**
 * @brief Sends typical message (test message)
*/
void Messager::send_typical_message() {
    unsigned char message[] = {0x11, 0x22};
    this->communicator->send_message(2, message, 1);
}

/**
 * @brief Just a test function to examplify a part of the code where
 * the Messager is utilized.
*/
void example_usage_function() {
    Communicator* communicator;
    #ifdef EMBEDDED /* These compiler flags allow the usage of one strategy or the
        other depending on the environment being native (DESKTOP) or teensy (EMBEDDED) */
    communicator = new CANCommunicator("can1");
    #else
    communicator = new VCANCommunicator("vcan1");
    #endif
    Messager messager = Messager(communicator);
    messager.send_typical_message();
}

#endif 