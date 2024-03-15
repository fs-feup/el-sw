#ifndef COMM_COMMUNICATOR_HPP_
#define COMM_COMMUNICATOR_HPP_

#include <string>
#include "comm/message.hpp"

/**
 * @brief General communicator. Represents the 
 * abstract class of the communication strategy.
*/
class Communicator {

    std::string can_network_name;

    public:
    Communicator(std::string can_network);

    /**
     * @brief Function to send message. One of the function to
     * be defined by each specific strategy (can be more).
    */
    virtual int send_message(const unsigned int len, const unsigned char* buffer, const unsigned int id) = 0;

    std::string get_network_name() const;
};

Communicator::Communicator(std::string can_network) : can_network_name(can_network) {};

std::string Communicator::get_network_name() const { return this->can_network_name; }

#endif