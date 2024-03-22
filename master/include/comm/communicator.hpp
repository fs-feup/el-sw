#pragma once

#include "comm/message.hpp"
#include "logic/checkupManager.hpp"
#include "logic/sensors.hpp"
#include <FlexCAN_T4.h>
#include <string>

Code fifoCodes[] = {
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
    static FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

public:
    inline static SystemData *_systemData = nullptr;

    Communicator();

    static void parse_message(const CAN_message_t& msg);
    static int send_message(unsigned len, const unsigned char* buffer, unsigned id);

    static void emergencySignalCallback();
    static void missionFinishedCallback();
    static void pcAliveCallback();
    static void pcCallback(const uint8_t *buf);
    static void c1Callback(const uint8_t *buf);
    static void resStateCallback(const uint8_t *buf);
    static void resReadyCallback();
    static void bamocarCallback(const uint8_t *buf);
    static void steeringCallback();

    int publish_state(int state_id);
    int publish_mission(int mission_id);
    int publish_left_wheel_rpm(double value);
    static int activateRes();
};

Communicator::Communicator() {
  can1.begin();
  can1.setBaudRate(500000);
  can1.enableFIFO();
  can1.enableFIFOInterrupt();
  can1.setFIFOFilter(REJECT_ALL);
  for (unsigned i = 0; i < sizeof(fifoCodes) / sizeof(fifoCodes[0]); i++)
    can1.setFIFOFilter(fifoCodes[i].key, fifoCodes[i].code, STD);

  can1.onReceive(parse_message);
}

void Communicator::emergencySignalCallback() {
  _systemData->failureDetection.emergencySignal = true;
}

void Communicator::missionFinishedCallback() {
  _systemData->missionFinished = true;
}

void Communicator::pcAliveCallback() {
  _systemData->failureDetection.pcAliveTimestamp.update();
}

void Communicator::c1Callback(const uint8_t *buf) {
  if (buf[0] == HYDRAULIC_LINE) {
    double break_pressure = (buf[2] << 8) | buf[1];
    break_pressure *=
        HYDRAULIC_LINE_PRECISION; // convert back adding decimal part
    _systemData->sensors.updateHydraulic(break_pressure);
  } else if (buf[0] == RIGHT_WHEEL) {
    double right_wheel_rpm = (buf[2] << 8) | buf[1];
    right_wheel_rpm *= WHEEL_PRECISION; // convert back adding decimal part
    _systemData->sensors.updateRL(right_wheel_rpm);
  }
}

void Communicator::resStateCallback(const uint8_t *buf) {
    bool emg_stop1 = buf[0] & 0x01;
    bool emg_stop2 = buf[3] >> 7 & 0x01;
    bool go_switch = (buf[0] >> 1) & 0x01;
    bool go_button = (buf[0] >> 2) & 0x01;

    if (go_button || go_switch)
        _systemData->internalLogics.processGoSignal();
    else if (emg_stop1 || emg_stop2)
        _systemData->failureDetection.emergencySignal = true;

    _systemData->failureDetection.radio_quality =  buf[6];
    bool signal_loss = (buf[7] >> 6) & 0x01;
    Communicator::emergencySignalCallback();
}

void Communicator::resReadyCallback() {
    // If res sends boot message, activate it
    Communicator::activateRes();
}

void Communicator::bamocarCallback(const uint8_t *buf) {
  // TODO(andré): inversor timestamp
  if (buf[0] == BTB_READY) {
    if (buf[1] == false)
      _systemData->failureDetection.bamocarReady = false;
  } else if (buf[0] == VDC_BUS) {
    int battery_voltage = (buf[2] << 8) | buf[1];
    _systemData->failureDetection.bamocarTension = battery_voltage;
  }
}

void Communicator::pcCallback(const uint8_t *buf) {
  if (buf[0] == PC_ALIVE) {
    _systemData->failureDetection.pcAliveTimestamp.update();
  } else if (buf[0] == MISSION_FINISHED) {
    _systemData->missionFinished = true;
  } else if (buf[0] == AS_CU_EMERGENCY_SIGNAL) {
    _systemData->failureDetection.emergencySignal = true;
  }
}

void Communicator::steeringCallback() {
  _systemData->failureDetection.steerAliveTimestamp.update();
}

void Communicator::parse_message(const CAN_message_t& msg) {
    switch(msg.id) {
        case PC_ID:
            Communicator::pcCallback(msg.buf);
        case RES_STATE:
            Communicator::resStateCallback(msg.buf);
            break;
        case RES_READY:
            Communicator::resReadyCallback();
        case C1_ID:
            Communicator::c1Callback(msg.buf); // rwheel and hydraulic line
            break;
        case BAMO_RESPONSE_ID:
            Communicator::bamocarCallback(msg.buf);
            break;
        case STEERING_ID:
            Communicator::steeringCallback();
            break;
        default:
            break;
    }
}

int Communicator::publish_state(int state_id) {
  unsigned id = STATE_MSG;
  uint8_t msg[] = {static_cast<unsigned char>(state_id)};

  this->send_message(1, msg, id);
}

int Communicator::publish_mission(int mission_id) {
  unsigned id = MISSION_MSG;
  uint8_t msg[] = {static_cast<unsigned char>(mission_id)};

  this->send_message(1, msg, id);
}

int Communicator::publish_left_wheel_rpm(double value) {
  unsigned id = LEFT_WHEEL_MSG;

  value /= WHEEL_PRECISION; // take precision off to send interger value
  uint8_t *msg = reinterpret_cast<uint8_t *>(&value);

  this->send_message(2, msg, id);
}

int Communicator::activateRes() {
    unsigned id = RES_ACTIVATE;
    uint8_t msg[] = {0x01, NODE_ID}; // 0x00 in byte 2 for all nodes

    Communicator::send_message(2, msg, id);
    return 0;
}

int Communicator::send_message(unsigned len, const unsigned char* buffer, unsigned id) {
    
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
