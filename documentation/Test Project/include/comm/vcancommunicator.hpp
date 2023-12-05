#ifndef COMM_VCANCOMMUNICATOR_HPP_
#define COMM_VCANCOMMUNICATOR_HPP_

#include "comm/communicator.hpp"


/**
 * @brief Communication strategy class for communication
 * using the Socket CAN library for desktop CAN.
*/
class VCANCommunicator : public Communicator {

    public:
    VCANCommunicator(std::string can_network);
    int send_message(const unsigned int len, const unsigned char* buffer, const unsigned int id);
};

#ifndef EMBEDDED

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include "comm/vcancommunicator.hpp"

VCANCommunicator::VCANCommunicator(std::string can_network) : Communicator(can_network) {};

int VCANCommunicator::send_message(const unsigned int len, const unsigned char* buffer, const unsigned int id) {
    int s;
	int nbytes;
	struct sockaddr_can addr;
	struct can_frame frame;
	struct ifreq ifr;

	const char *ifname = "vcan0";

	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) == -1) {
		perror("Error while opening socket");
		return -1;
	}

	strcpy(ifr.ifr_name, ifname);
	ioctl(s, SIOCGIFINDEX, &ifr);
	
	addr.can_family  = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	printf("%s at index %d\n", ifname, ifr.ifr_ifindex);

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		perror("Error in socket bind");
		return -2;
	}

	frame.can_id  = 0x123;
	frame.can_dlc = len;
    for (unsigned int i = 0; i < len; i++) {
        frame.data[i] = buffer[i];
    }

	nbytes = write(s, &frame, sizeof(struct can_frame));

	return 0;
}

#endif

#endif 