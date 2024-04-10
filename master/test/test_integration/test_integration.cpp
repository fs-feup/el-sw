#include "model/systemData.hpp"
#include "comm/communicator.hpp"
#include "embedded/digitalSender.hpp"
#include "logic/stateLogic.hpp"
#include "unity.h"

SystemData sd;

Communicator communicator = Communicator(&sd); // CAN
DigitalSender digitalSender = DigitalSender(); // Digital outputs

ASState as_state = ASState(&sd, &communicator, &digitalSender);

void test_off_to_ready(void){
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
    Metro time{100};
    while (!time.check()) {
        as_state.calculateState();
    }
    
    // Wait for wd timeout
    Metro time2{INITIAL_CHECKUP_STEP_TIMEOUT + 100};
    while (!time2.check()) {  
    }
    sd.digitalData.watchdog_state = false;
    
    Metro time3{100};
    while(!time3.check()) {
        as_state.calculateState();
        sd.failureDetection.inversorAliveTimestamp.reset();
        sd.failureDetection.pcAliveTimestamp.reset();
        sd.failureDetection.steerAliveTimestamp.reset();
    }

    //TEST_ASSERT_EQUAL(true, sd.failureDetection.emergencySignal);
    
    TEST_ASSERT_GREATER_THAN(HYDRAULIC_BRAKE_THRESHOLD, sd.sensors._hydraulic_line_pressure);
    
    TEST_ASSERT_EQUAL(true, sd.digitalData.pneumatic_line_pressure);
    TEST_ASSERT_EQUAL(true, sd.digitalData.asms_on);
    TEST_ASSERT_EQUAL(true, sd.failureDetection.ts_on);
    TEST_ASSERT_EQUAL(false, sd.failureDetection.emergencySignal);
    // TEST_ASSERT_EQUAL(true, sd.digitalData.watchdog_state);
    TEST_ASSERT_EQUAL(false, sd.digitalData.sdcState_OPEN);
    TEST_ASSERT_EQUAL(CheckupManager::CheckupState::CHECK_TIMESTAMPS, as_state._checkupManager.checkupState);
    
    TEST_ASSERT_EQUAL(State::AS_READY, as_state.state);
}

void setUp() {
    
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_off_to_ready);
    return UNITY_END();
}