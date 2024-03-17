#ifndef COMM_COMMS_HPP_
#define COMM_COMMS_HPP_

#include "comm/communicator.hpp"
#include "logic/checkupManager.hpp"
#include "logic/sensors.hpp"

/**
 * @brief Class that contains definitions of typical messages to send via CAN
 * It serves only as an example of the usage of the strategy pattern,
 * where the communicator is the strategy
*/
class CommunicationManager {

    Communicator* communicator;
    CheckupManager* checkupManager;
    Sensors* sensors;

    public:
    CommunicationManager(CheckupManager* checkupManager, Sensors* sensors);

    void emergencySignalCallback();
    void missionFinishedCallback();
    void pcAliveCallback();
    void rlWheelCallback(double value);
    void hydraulicLineCallback(double value);
    void resCallback(const uint8_t *buf);
    void bamocarCallback(const uint8_t *buf);
    void steeringCallback();
};



CommunicationManager::CommunicationManager(CheckupManager* checkupManager, Sensors* sensors) 
    : checkupManager(checkupManager), sensors(sensors) {
    communicator = new Communicator(this, "can1");
};

void CommunicationManager::emergencySignalCallback() {
    checkupManager->_failureDetection.emergencySignal = true;
}

void CommunicationManager::missionFinishedCallback() {
    checkupManager->_missionFinished = true;
}

void CommunicationManager::pcAliveCallback() {
    checkupManager->_failureDetection.pcAliveTimestamp.update();
}

void CommunicationManager::rlWheelCallback(double value) {
    sensors->updateRL(value);
}

void CommunicationManager::hydraulicLineCallback(double value) {
    sensors->updateHydraulic(value);
}

void CommunicationManager::resCallback(const uint8_t *buf) {
    bool emg_stop1 = buf[0] & 0x01;
    bool emg_stop2 = buf[3] >> 7 & 0x01;
    bool go_switch = (buf[0] >> 1) & 0x01;
    bool go_button = (buf[0] >> 2) & 0x01;

    if (go_button || go_switch)
        checkupManager->_internalLogics.processGoSignal();
    else if (emg_stop1 || emg_stop2)
        checkupManager->_failureDetection.emergencySignal = true;
}

void CommunicationManager::bamocarCallback(const uint8_t *buf) {
    if (buf[0] == BTB_READY)
        bool alive = true;
        // TODO(andre): what to do with btb ready?
    else if (buf[0] == VDC_BUS){
        int battery_voltage = (buf[2] << 8) | buf[1];
        checkupManager->_failureDetection.bamocarTension = battery_voltage;
    }
}

void CommunicationManager::steeringCallback() {
    checkupManager->_failureDetection.steerAliveTimestamp.update();
}
#endif 