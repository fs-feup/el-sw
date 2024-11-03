#pragma once
#include <Arduino.h>
#include "comm/communicatorSettings.hpp"

/**
 * @brief Function to create left wheel msg
 */
void create_left_wheel_msg(std::array<uint8_t, 5> &msg, double value)
{
    value /= WHEEL_PRECISION; // take precision off to send integer value
    if (value < 0)
        value = 0;

    msg[0] = LEFT_WHEEL_MSG;
    // Copy the bytes of the double value to msg[1] to msg[4]
    for (int i = 0; i < 4; i++)
        msg[i + 1] = static_cast<int>(value) >> (8 * i); // shift 8(byte) to msb each time
}