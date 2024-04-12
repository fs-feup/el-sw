#include "model/systemData.hpp"
#include "comm/communicator.hpp"
#include "embedded/digitalSender.hpp"
#include "logic/stateLogic.hpp"
#include "unity.h"

SystemData sd;
Communicator communicator = Communicator(&sd); // CAN
DigitalSender digitalSender = DigitalSender(); // Digital outputs
ASState as_state = ASState(&sd, &communicator, &digitalSender);

void reset() {
    sd = SystemData();
    as_state = ASState(&sd, &communicator, &digitalSender);
}

void to_ready() {
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
}

void test_off_to_ready_success(void){
    reset();
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
    
    to_ready();
        
    TEST_ASSERT_EQUAL(false, sd.failureDetection.emergencySignal);

    TEST_ASSERT_EQUAL(CheckupManager::CheckupState::CHECK_TIMESTAMPS, as_state._checkupManager.checkupState);
    
    TEST_ASSERT_EQUAL(State::AS_READY, as_state.state);
}

void test_off_to_ready_recheck() {
    reset();

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
        sd.digitalData.watchdogTimestamp.reset(); // todo check
    }

    TEST_ASSERT_EQUAL(false, went_ready);
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
}

void test_off_to_ready_wayback() {
    reset();
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
    to_ready();
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
    to_ready();

    TEST_ASSERT_EQUAL(State::AS_READY, as_state.state);
}

void test_ready_to_emg_to_off() {
    reset();
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
    to_ready();
    TEST_ASSERT_EQUAL(State::AS_READY, as_state.state);

    Metro time{COMPONENT_TIMESTAMP_TIMEOUT};
    while (!time.check()){
        as_state.calculateState();
    }

    TEST_ASSERT_EQUAL(State::AS_EMERGENCY, as_state.state);

    Metro time2{EBS_BUZZER_TIMEOUT / 2};
    while (!time2.check()){
        as_state.calculateState();
    }
    TEST_ASSERT_EQUAL(State::AS_EMERGENCY, as_state.state);

    Metro time3{EBS_BUZZER_TIMEOUT / 2};
    while (!time3.check()){
        as_state.calculateState();
    }
    TEST_ASSERT_EQUAL(State::AS_EMERGENCY, as_state.state);
    sd.digitalData.asms_on = false;
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
}

void test_ready_to_driving_to_emg() {
    reset();
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
    to_ready();
    TEST_ASSERT_EQUAL(State::AS_READY, as_state.state);
    
    Metro time{READY_TIMEOUT_MS / 2};
    while (!time.check()){
        as_state.calculateState();
        sd.failureDetection.inversorAliveTimestamp.reset();
        sd.failureDetection.pcAliveTimestamp.reset();
        sd.failureDetection.steerAliveTimestamp.reset();
        sd.digitalData.watchdogTimestamp.reset();
    }

    uint8_t msg[8] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    communicator.resStateCallback(msg);

    TEST_ASSERT_EQUAL(State::AS_READY, as_state.state);

    Metro time2{READY_TIMEOUT_MS / 2};
    while (!time2.check()){
        as_state.calculateState();
        sd.failureDetection.inversorAliveTimestamp.reset();
        sd.failureDetection.pcAliveTimestamp.reset();
        sd.failureDetection.steerAliveTimestamp.reset();
        sd.digitalData.watchdogTimestamp.reset();
    }
    
    communicator.resStateCallback(msg);
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_DRIVING, as_state.state);
    // if brake pressure not updated, immediatly to emergency
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_EMERGENCY, as_state.state);
}

void test_ready_to_driving_to_emg2() {
    reset();
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
    to_ready();
    TEST_ASSERT_EQUAL(State::AS_READY, as_state.state);
    
    Metro time{READY_TIMEOUT_MS};
    while (!time.check()){
        as_state.calculateState();
        sd.failureDetection.inversorAliveTimestamp.reset();
        sd.failureDetection.pcAliveTimestamp.reset();
        sd.failureDetection.steerAliveTimestamp.reset();
        sd.digitalData.watchdogTimestamp.reset();
    }

    uint8_t msg[8] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    communicator.resStateCallback(msg);
    as_state.calculateState();

    TEST_ASSERT_EQUAL(State::AS_DRIVING, as_state.state);
    
    uint8_t hydraulic_msg[] = {HYDRAULIC_LINE, 0x10, 0x00}; // loose brake activation
    communicator.c1Callback(hydraulic_msg);
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_DRIVING, as_state.state);

    sd.digitalData.sdcState_OPEN = true;
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_EMERGENCY, as_state.state);
}

void setUp() {
    
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_off_to_ready_success);
    RUN_TEST(test_off_to_ready_recheck);
    RUN_TEST(test_off_to_ready_wayback);
    RUN_TEST(test_ready_to_driving_to_emg);
    RUN_TEST(test_ready_to_driving_to_emg2);
    RUN_TEST(test_ready_to_emg_to_off);
    return UNITY_END();
}
