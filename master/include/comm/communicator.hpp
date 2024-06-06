#pragma once

#include <FlexCAN_T4.h>
#include <Arduino.h>
#include <string>

#include "comm/communicatorSettings.hpp"
#include "model/systemData.hpp"
#include "comm/utils.hpp"
#include "debugUtils.hpp"

inline Code fifoCodes[] = {
    {0, C1_ID},
    {1, BMS_ID},
    {2, BAMO_RESPONSE_ID},
    {3, AS_CU_EMERGENCY_SIGNAL},
    {4, MISSION_FINISHED},
    {5, PC_ALIVE},
    {6, STEERING_ID},
    {7, RES_STATE},
    {8, RES_READY}
};

/**
 * @brief Class that contains definitions of typical messages to send via CAN
 * It serves only as an example of the usage of the strategy pattern,
 * where the communicator is the strategy
 */
class Communicator {
private:
    inline static FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

public:
    inline static SystemData *_systemData = nullptr;

    Communicator(SystemData* systemdata);

    /**
     * @brief Parses the message received from the CAN bus
    */
    static void parse_message(const CAN_message_t& msg);

    /**
     * @brief Sends a message to the CAN bus
     * @param len Length of the message
     * @param buffer Buffer containing the message
     * @param id ID of the message
     * @return 0 if successful
    */
    static int send_message(unsigned len, const unsigned char* buffer, unsigned id);


    /**
     * @brief Callback for message from AS CU
    */
    static void pcCallback(const uint8_t *buf);
    
    /**
     * @brief Callback for data from C1 Teensy
    */
    static void c1Callback(const uint8_t *buf);

    /**
     * @brief Callback RES default callback
    */
    static void resStateCallback(const uint8_t *buf);

    /**
     * @brief Callback for RES activation
    */
    static void resReadyCallback();

    /**
     * @brief Callback from inversor, for alive signal and data
    */
    static void bamocarCallback(const uint8_t *buf);

    /**
     * @brief Callback for steering actuator information
    */
    static void steeringCallback();


    /**
     * @brief Publish AS State to CAN
    */
    static int publish_state(int state_id);

    /**
     * @brief Publish AS Mission to CAN
    */
    static int publish_mission(int mission_id);

    /**
     * @brief Publish rl wheel rpm to CAN
    */
    static int publish_left_wheel_rpm(double value);
};

inline Communicator::Communicator(SystemData* systemData) {
    _systemData = systemData;

    can1.begin();
    can1.setBaudRate(500000);
    can1.enableFIFO();
    can1.enableFIFOInterrupt();
    // can1.setFIFOFilter(REJECT_ALL);
    for (auto &fifoCode: fifoCodes)
        can1.setFIFOFilter(fifoCode.key, fifoCode.code, STD);

    can1.onReceive(parse_message);
}

inline void Communicator::c1Callback(const uint8_t *buf) {
  if (buf[0] == HYDRAULIC_LINE) {
    _systemData->sensors._hydraulic_line_pressure = (buf[2] << 8) | buf[1];
    DEBUG_PRINT_VAR(_systemData->sensors._hydraulic_line_pressure);
  } else if (buf[0] == RIGHT_WHEEL_CODE) {
    double right_wheel_rpm = (buf[4] << 24) | (buf[3] << 16) | (buf[2] << 8) | buf[1];
    right_wheel_rpm *= WHEEL_PRECISION; // convert back adding decimal part
    _systemData->sensors._right_wheel_rpm = right_wheel_rpm;
    DEBUG_PRINT_VAR(_systemData->sensors._right_wheel_rpm);
  } else if (buf[0] == LEFT_WHEEL_CODE) {
    double left_wheel_rpm = (buf[4] << 24) | (buf[3] << 16) | (buf[2] << 8) | buf[1];
    left_wheel_rpm *= WHEEL_PRECISION; // convert back adding decimal part
    _systemData->sensors._left_wheel_rpm = left_wheel_rpm;
    DEBUG_PRINT_VAR(_systemData->sensors._left_wheel_rpm);
  }
  DEBUG_PRINT("RECEIVED SOMETHING");

}

inline void Communicator::resStateCallback(const uint8_t *buf) {
    bool emg_stop1 = buf[0] & 0x01;
    bool emg_stop2 = buf[3] >> 7 & 0x01;
    bool go_switch = (buf[0] >> 1) & 0x01;
    bool go_button = (buf[0] >> 2) & 0x01;

    DEBUG_PRINT_VAR(emg_stop1);
    DEBUG_PRINT_VAR(emg_stop2);
    DEBUG_PRINT_VAR(go_switch);
    DEBUG_PRINT_VAR(go_button);

    if (go_button || go_switch)
        _systemData->r2dLogics.processGoSignal();
    else if (emg_stop1 || emg_stop2)
        _systemData->failureDetection.emergencySignal = true;

    _systemData->failureDetection.radio_quality =  buf[6];
    bool signal_loss = (buf[7] >> 6) & 0x01;
    if (signal_loss) {
        _systemData->failureDetection.emergencySignal = true;
    }

    DEBUG_PRINT_VAR(_systemData->r2dLogics.r2d);
    DEBUG_PRINT_VAR(_systemData->failureDetection.radio_quality);
    DEBUG_PRINT_VAR(_systemData->failureDetection.emergencySignal);
}

inline void Communicator::resReadyCallback() {
    // If res sends boot message, activate it
    unsigned id = RES_ACTIVATE;
    uint8_t msg[] = {0x01, NODE_ID}; // 0x00 in byte 2 for all nodes

    send_message(2, msg, id);
}

inline void Communicator::bamocarCallback(const uint8_t *buf) {
    _systemData->failureDetection.inversorAliveTimestamp.reset();
    DEBUG_PRINT("Received Bamocar Alive");

    if (buf[0] == BTB_READY) {
        if (buf[1] == false)
            _systemData->failureDetection.ts_on = false;
     
    } else if (buf[0] == VDC_BUS) {
        
        int dc_voltage = (buf[2] << 8) | buf[1];

        if (dc_voltage < DC_THRESHOLD)
            _systemData->failureDetection.ts_on = false;
        else 
            _systemData->failureDetection.ts_on = true;       
        DEBUG_PRINT_VAR(_systemData->failureDetection.ts_on);
    }   

}

inline void Communicator::pcCallback(const uint8_t *buf) {
    if (buf[0] == PC_ALIVE) {
        _systemData->failureDetection.pcAliveTimestamp.reset();
        DEBUG_PRINT("Received AS CU Alive");
    } else if (buf[0] == MISSION_FINISHED) {
        _systemData->missionFinished = true;
        DEBUG_PRINT_VAR(_systemData->missionFinished);
    } else if (buf[0] == AS_CU_EMERGENCY_SIGNAL) {
        _systemData->failureDetection.emergencySignal = true;
        DEBUG_PRINT_VAR(_systemData->failureDetection.emergencySignal);
    }
    
}

inline void Communicator::steeringCallback() {
    _systemData->failureDetection.steerAliveTimestamp.reset();
    DEBUG_PRINT("Received Steering Alive");
}

inline void Communicator::parse_message(const CAN_message_t& msg) {
    DEBUG_PRINT_VAR(msg.id);
    switch(msg.id) {
        case PC_ID:
            pcCallback(msg.buf);
        case RES_STATE:
            resStateCallback(msg.buf);
            break;
        case RES_READY:
            resReadyCallback();
            break;
        case C1_ID:
            c1Callback(msg.buf); // rwheel and hydraulic line
            break;
        case BAMO_RESPONSE_ID:
            bamocarCallback(msg.buf);
            break;
        case STEERING_ID:
            steeringCallback();
            break;
        default:
            break;
    }
}

inline int Communicator::publish_state(const int state_id) {
    const uint8_t msg[] = {STATE_MSG, static_cast<uint8_t>(state_id)};

    send_message(2, msg, MASTER_ID);
    return 0;
}

inline int Communicator::publish_mission(int mission_id) {
    const uint8_t msg[] = {MISSION_MSG, static_cast<uint8_t>(mission_id)};

    send_message(2, msg, MASTER_ID);
    return 0;
}

inline int Communicator::publish_left_wheel_rpm(double value) {
    uint8_t msg[5];
    create_left_wheel_msg(msg, value);
    
    send_message(5, msg, MASTER_ID);
    return 0;
}

inline int Communicator::send_message(const unsigned len, const unsigned char* buffer, const unsigned id) {
    CAN_message_t can_message;
    can_message.id = id;
    can_message.len = len;
    for (unsigned i = 0; i < len; i++) {
        can_message.buf[i] = buffer[i];
    }
    can1.begin();
    can1.write(can_message);

    return 0;
}
