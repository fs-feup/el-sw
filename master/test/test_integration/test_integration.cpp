#include "model/systemData.hpp"
#include "comm/communicator.hpp"
#include "embedded/digitalSender.hpp"
#include "logic/stateLogic.hpp"
#include "unity.h"

void test_off_to_ready_success(void){
    SystemData sd;
    Communicator communicator = Communicator(&sd); // CAN
    DigitalSender digitalSender = DigitalSender(); // Digital outputs
    ASState as_state = ASState(&sd, &communicator, &digitalSender);

    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
    
    sd.digitalData.asms_on = true;
    sd.digitalData.watchdog_state = true;
    sd.digitalData.sdcState_OPEN = false;

    uint8_t bamo_msg[] = {VDC_BUS, 0x94, 0x11}; // VDC_BUS fill
    communicator.bamocarCallback(bamo_msg);
   
    sd.digitalData.pneumatic_line_pressure = true;
    uint8_t hydraulic_msg[] = {HYDRAULIC_LINE, 0xf8, 0x00};
    communicator.c1Callback(hydraulic_msg);

    // Iterate a few times to go to check wd
    Metro time{1};
    while (!time.check()) {
        as_state.calculateState();
    }
    
    sd.digitalData.watchdog_state = false;
    // Wait for wd timeout
    Metro time2{INITIAL_CHECKUP_STEP_TIMEOUT};
    while (!time2.check()) {  
        as_state.calculateState();
        sd.failureDetection.inversorAliveTimestamp.reset();
        sd.failureDetection.pcAliveTimestamp.reset();
        sd.failureDetection.steerAliveTimestamp.reset();
        sd.digitalData.watchdogTimestamp.reset();
    }
        
    TEST_ASSERT_EQUAL(false, sd.failureDetection.emergencySignal);

    TEST_ASSERT_EQUAL(CheckupManager::CheckupState::CHECK_TIMESTAMPS, as_state._checkupManager.checkupState);
    
    TEST_ASSERT_EQUAL(State::AS_READY, as_state.state);
}

void test_off_to_ready_recheck() {
    SystemData sd;
    Communicator communicator = Communicator(&sd); // CAN
    DigitalSender digitalSender = DigitalSender(); // Digital outputs
    ASState as_state = ASState(&sd, &communicator, &digitalSender);
    Serial.begin(9600);

    bool went_ready = false;
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
    
    sd.digitalData.asms_on = true;
    sd.digitalData.watchdog_state = true;
    sd.digitalData.sdcState_OPEN = false;

    uint8_t bamo_msg[] = {VDC_BUS, 0x94, 0x11}; // VDC_BUS fill
    communicator.bamocarCallback(bamo_msg);
   
    sd.digitalData.pneumatic_line_pressure = true;
    uint8_t hydraulic_msg[] = {HYDRAULIC_LINE, 0xf8, 0x00};
    communicator.c1Callback(hydraulic_msg);

    // Iterate a few times to go to check wd
    Metro time{1};
    while (!time.check()) {
        as_state.calculateState();
    }
    
    sd.digitalData.watchdog_state = false;
    sd.digitalData.asms_on = false; // switch previously checked condition
    // Wait for wd timeout

    Metro time2{INITIAL_CHECKUP_STEP_TIMEOUT};
    while (!time2.check()) {  
        if (as_state.state == State::AS_READY)
            went_ready = true;
        
        as_state.calculateState();
        sd.failureDetection.inversorAliveTimestamp.reset();
        sd.failureDetection.pcAliveTimestamp.reset();
        sd.failureDetection.steerAliveTimestamp.reset();
        sd.digitalData.watchdogTimestamp.reset();
    }

    TEST_ASSERT_EQUAL(false, went_ready);
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
}

void test_off_to_ready_wayback() {
    SystemData sd;
    Communicator communicator = Communicator(&sd); // CAN
    DigitalSender digitalSender = DigitalSender(); // Digital outputs
    ASState as_state = ASState(&sd, &communicator, &digitalSender);

    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
    
    sd.digitalData.asms_on = true;
    sd.digitalData.watchdog_state = true;
    sd.digitalData.sdcState_OPEN = false;

    uint8_t bamo_msg[] = {VDC_BUS, 0x94, 0x11}; // VDC_BUS fill
    communicator.bamocarCallback(bamo_msg);
   
    sd.digitalData.pneumatic_line_pressure = true;
    uint8_t hydraulic_msg[] = {HYDRAULIC_LINE, 0xf8, 0x00};
    communicator.c1Callback(hydraulic_msg);

    // Iterate a few times to go to check wd
    Metro time{1};
    while (!time.check()) {
        as_state.calculateState();
    }
    
    sd.digitalData.watchdog_state = false;
    // Wait for wd timeout
    Metro time2{INITIAL_CHECKUP_STEP_TIMEOUT};
    while (!time2.check()) {  
        as_state.calculateState();
        sd.failureDetection.inversorAliveTimestamp.reset();
        sd.failureDetection.pcAliveTimestamp.reset();
        sd.failureDetection.steerAliveTimestamp.reset();
        sd.digitalData.watchdogTimestamp.reset();
    }
    TEST_ASSERT_EQUAL(State::AS_READY, as_state.state);

    // Procedure to go back to AS_OFF state
    uint8_t bamo_msg2[] = {VDC_BUS, 0x23, 0x00}; // cd voltage set to below threshold
    communicator.bamocarCallback(bamo_msg2);

    Metro time3{1};
    while (!time3.check()) {
        as_state.calculateState();
    }
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);

    // Procedure to go back to AS_READY state
    uint8_t bamo_msg3[] = {VDC_BUS, 0x94, 0x11}; // cd voltage set to above threshold
    communicator.bamocarCallback(bamo_msg3);
    sd.digitalData.watchdog_state = true;

    Metro time4{1};
    while (!time4.check()) {
        as_state.calculateState();
    }
    
    sd.digitalData.watchdog_state = false;
    // Wait for wd timeout
    Metro time5{INITIAL_CHECKUP_STEP_TIMEOUT};
    while (!time5.check()) {  
        as_state.calculateState();
        sd.failureDetection.inversorAliveTimestamp.reset();
        sd.failureDetection.pcAliveTimestamp.reset();
        sd.failureDetection.steerAliveTimestamp.reset();
        sd.digitalData.watchdogTimestamp.reset();
    }

    TEST_ASSERT_EQUAL(State::AS_READY, as_state.state);
}

void test_ready_to_emg_success() {

}

void test_driving_to_emg_success() {
    
}

void setUp() {
    
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_off_to_ready_success);
    RUN_TEST(test_off_to_ready_recheck);
    RUN_TEST(test_off_to_ready_wayback);
    return UNITY_END();
}
