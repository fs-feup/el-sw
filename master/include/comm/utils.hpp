#pragma once
#include <Arduino.h>

#include "comm/communicatorSettings.hpp"
#include "model/systemData.hpp"
#include "enum_utils.hpp"

/**
 * @brief Function to create left wheel msg
 */
void create_left_wheel_msg(std::array<uint8_t, 5> &msg, double value) {
  value /= WHEEL_PRECISION;  // take precision off to send integer value
  if (value < 0) value = 0;

  msg[0] = LEFT_WHEEL_MSG;
  // Copy the bytes of the double value to msg[1] to msg[4]
  for (int i = 0; i < 4; i++)
    msg[i + 1] = static_cast<int>(value) >> (8 * i);  // shift 8(byte) to msb each time
}

inline std::array<uint8_t, 8> create_debug_message_1(const SystemData& system_data, uint8_t state, uint8_t state_checkup) {
    return {
        DBG_LOG_MSG,
        (system_data.sensors_._hydraulic_line_pressure >> 24) & 0xFF,
        (system_data.sensors_._hydraulic_line_pressure >> 16) & 0xFF,
        (system_data.sensors_._hydraulic_line_pressure >> 8) & 0xFF,
        system_data.sensors_._hydraulic_line_pressure & 0xFF,
        (system_data.failure_detection_.emergency_signal_ & 0x01) << 7 | 
        (system_data.digital_data_.pneumatic_line_pressure_ & 0x01) << 6 |
        (system_data.r2d_logics_.engageEbsTimestamp.checkWithoutReset() & 0x01) << 5 | 
        (system_data.r2d_logics_.releaseEbsTimestamp.checkWithoutReset() & 0x01) << 4 |
        (system_data.failure_detection_.steer_dead_ & 0x01) << 3 | 
        (system_data.failure_detection_.pc_dead_ & 0x01) << 2 |
        (system_data.failure_detection_.inversor_dead_ & 0x01) << 1 | 
        (system_data.failure_detection_.res_dead_ & 0x01),
        (system_data.digital_data_.asms_on_ & 0x01) << 7 | 
        (system_data.failure_detection_.ts_on_ & 0x01) << 6 | 
        (system_data.digital_data_.sdc_open_ & 0x01) << 5 |
        (state_checkup & 0x0F),
        (to_underlying(system_data.mission_) & 0x0F) | ((state & 0x0F) << 4)
    };
}

inline std::array<uint8_t, 7> create_debug_message_2(const SystemData& system_data) {
    return {
        DBG_LOG_MSG_2,
        (system_data.failure_detection_.dc_voltage_ >> 24) & 0xFF,
        (system_data.failure_detection_.dc_voltage_ >> 16) & 0xFF,
        (system_data.failure_detection_.dc_voltage_ >> 8) & 0xFF,
        system_data.failure_detection_.dc_voltage_ & 0xFF,
        system_data.digital_data_.pneumatic_line_pressure_1_ & 0x01,
        system_data.digital_data_.pneumatic_line_pressure_2_ & 0x01
    };
}