#ifndef COMM_COMMS_HPP_
#define COMM_COMMS_HPP_

#ifdef EMBEDDED
#include "comm/cancommunicator.hpp"
#else
#include "comm/vcancommunicator.hpp"
#endif

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

    void send_typical_message();

    void emergencySignalCallback(std::string msg);
    void missionFinishedCallback();
    void pcAliveCallback();
    void rlWheelCallback(double value);
    void hydraulicLineCallback(double value);
    void r2dCallback();
    void goCallback();
    void bamocarTensionCallback(float value);
    void steeringCallback();
};



CommunicationManager::CommunicationManager(CheckupManager* checkupManager, Sensors* sensors) 
    : checkupManager(checkupManager), sensors(sensors) {
    #ifdef EMBEDDED /* These compiler flags allow the usage of one strategy or the
        other depending on the environment being native (DESKTOP) or teensy (EMBEDDED) */
    communicator = new CANCommunicator("can1");
    #else
    communicator = new VCANCommunicator("vcan1");
    #endif
};

void CommunicationManager::emergencySignalCallback(std::string msg) {
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

void CommunicationManager::r2dCallback() {
    checkupManager->_ready2Drive = true;
}

void CommunicationManager::goCallback(){
    checkupManager->_internalLogics.processGoSignal();
}

void CommunicationManager::bamocarTensionCallback(float value) {
    checkupManager->_failureDetection.bamocarTension = value;
}

void CommunicationManager::steeringCallback() {
    checkupManager->_failureDetection.steerAliveTimestamp.update();
}
#endif 