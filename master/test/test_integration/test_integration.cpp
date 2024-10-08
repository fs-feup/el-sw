#include "model/systemData.hpp"
#include "comm/communicator.hpp"
#include "embedded/digitalSender.hpp"
#include "logic/stateLogic.hpp"
#include "unity.h"

#define HYDRAULIC_PRESSURE_HIGH 0xf8
#define HYDRAULIC_PRESSURE_LOW 0x01
#define BAMOCAR_VDC_HIGH 0x11
#define RES_GO 0x02

SystemData sd;
Communicator communicator = Communicator(&sd); // CAN
DigitalSender digitalSender = DigitalSender(); // Digital outputs
ASState as_state = ASState(&sd, &communicator, &digitalSender);

/**
 * @brief Auxiliary function to reset data values 
*/
void reset() {
    sd = SystemData();
    as_state = ASState(&sd, &communicator, &digitalSender);
}

/**
 * @brief Auxiliary function to set state as ready
*/
void to_ready() {
    sd.digitalData.asms_on = true;
    // sd.digitalData.watchdog_state = true;
    sd.digitalData.sdcState_OPEN = false;

    uint8_t bamo_msg[] = {VDC_BUS, 0x00, BAMOCAR_VDC_HIGH}; // VDC_BUS fill
    communicator.bamocarCallback(bamo_msg);
   
    sd.digitalData.pneumatic_line_pressure = true;
    uint8_t hydraulic_msg[] = {HYDRAULIC_LINE, HYDRAULIC_PRESSURE_HIGH, 0x00};
    communicator.c1Callback(hydraulic_msg);

    // Iterate a few times to go to check wd
    Metro time{1};
    while (!time.checkWithoutReset()) {
        as_state.calculateState();
    }
    
    // sd.digitalData.watchdog_state = false;
    // Wait for wd timeout
    Metro time2{INITIAL_CHECKUP_STEP_TIMEOUT};
    while (!time2.checkWithoutReset()) {  
        as_state.calculateState();
        sd.failureDetection.inversorAliveTimestamp.reset();
        sd.failureDetection.pcAliveTimestamp.reset();
        sd.failureDetection.steerAliveTimestamp.reset();
        sd.failureDetection.resSignalLossTimestamp.reset();
        // sd.digitalData.watchdogTimestamp.reset();
    }
}

/**
 * @brief Test function to validate AS_OFF to AS_Ready transition
 * normal and successful transition 
*/
void test_off_to_ready_success(void){
    reset();
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
    
    to_ready();
        
    TEST_ASSERT_EQUAL(false, sd.failureDetection.emergencySignal);

    TEST_ASSERT_EQUAL(CheckupManager::CheckupState::CHECK_TIMESTAMPS, as_state._checkupManager.checkupState);
    
    TEST_ASSERT_EQUAL(State::AS_READY, as_state.state);
}

/**
 * @brief Test function to validate AS_OFF to AS_Ready transition
 * with checks reverting to invalid mid transition 
*/
void test_off_to_ready_recheck() {
    reset();

    bool went_ready = false;
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
    
    sd.digitalData.asms_on = true;
    // sd.digitalData.watchdog_state = true;
    sd.digitalData.sdcState_OPEN = false;

    uint8_t bamo_msg[] = {VDC_BUS, 0x00, BAMOCAR_VDC_HIGH}; // VDC_BUS fill
    communicator.bamocarCallback(bamo_msg);
   
    sd.digitalData.pneumatic_line_pressure = true;
    uint8_t hydraulic_msg[] = {HYDRAULIC_LINE, HYDRAULIC_PRESSURE_HIGH, 0x00};
    communicator.c1Callback(hydraulic_msg);

    // // Iterate a few times to go to check wd
    // Metro time{1};
    // while (!time.checkWithoutReset()) {
    //     as_state.calculateState();
    // }
    
    // sd.digitalData.watchdog_state = false;
    sd.digitalData.asms_on = false; // switch previously checked condition
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
    // Wait for wd timeout

    Metro time2{INITIAL_CHECKUP_STEP_TIMEOUT};
    while (!time2.checkWithoutReset()) {  
        if (as_state.state == State::AS_READY)
            went_ready = true;
        
        as_state.calculateState();
        sd.failureDetection.inversorAliveTimestamp.reset();
        sd.failureDetection.pcAliveTimestamp.reset();
        sd.failureDetection.steerAliveTimestamp.reset();
        // sd.digitalData.watchdogTimestamp.reset(); // todo check 
    }

    TEST_ASSERT_EQUAL(false, went_ready);
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
}

/**
 * @brief Test function to validate AS_READY doesn't revert to AS_Ready 
*/
void test_off_to_ready_wayback_impossible() {
    bool reverted_to_off = false;
    reset();
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
    to_ready();
    TEST_ASSERT_EQUAL(State::AS_READY, as_state.state);

    // Procedure to go back to AS_OFF state
    uint8_t bamo_msg2[] = {VDC_BUS, 0x00, 0x00}; // cd voltage set to below threshold
    communicator.bamocarCallback(bamo_msg2);

    Metro time3{1};
    while (!time3.checkWithoutReset()) {
        as_state.calculateState();
        if (as_state.state == AS_OFF)
            reverted_to_off = true;
    }

    TEST_ASSERT_EQUAL(false, reverted_to_off);
    TEST_ASSERT_EQUAL(State::AS_EMERGENCY, as_state.state);
}

/**
 * @brief Test function to validate AS_READY to AS_Emergency
 * and the AS_EMERGENCY to AS_OFF transition 
*/
void test_ready_to_emg_to_off() {
    reset();
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
    to_ready();
    TEST_ASSERT_EQUAL(State::AS_READY, as_state.state);

    Metro time{COMPONENT_TIMESTAMP_TIMEOUT + 10};
    while (!time.checkWithoutReset()){
        as_state.calculateState();
    }

    TEST_ASSERT_EQUAL(State::AS_EMERGENCY, as_state.state);

    Metro time2{EBS_BUZZER_TIMEOUT / 2};
    while (!time2.checkWithoutReset()){
        as_state.calculateState();
    }
    TEST_ASSERT_EQUAL(State::AS_EMERGENCY, as_state.state);

    Metro time3{EBS_BUZZER_TIMEOUT / 2};
    while (!time3.checkWithoutReset()){
        as_state.calculateState();
    }
    TEST_ASSERT_EQUAL(State::AS_EMERGENCY, as_state.state);
    sd.digitalData.asms_on = false;
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
}

/**
 * @brief Test function to validate AS_READY to AS_Driving transition
 * and AS_DRIVING to AS_EMERGENCY if brakes still active after timeout
*/
void test_ready_to_driving_to_emg() {
    reset();
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
    to_ready();
    TEST_ASSERT_EQUAL(State::AS_READY, as_state.state);
    
    Metro time{READY_TIMEOUT_MS / 2};
    while (!time.checkWithoutReset()){
        as_state.calculateState();
        sd.failureDetection.inversorAliveTimestamp.reset();
        sd.failureDetection.pcAliveTimestamp.reset();
        sd.failureDetection.steerAliveTimestamp.reset();
        sd.failureDetection.resSignalLossTimestamp.reset();
        // sd.digitalData.watchdogTimestamp.reset();
    }

    uint8_t msg[8] = {RES_GO, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    communicator.resStateCallback(msg);

    TEST_ASSERT_EQUAL(State::AS_READY, as_state.state);

    Metro time2{READY_TIMEOUT_MS / 2};
    while (!time2.checkWithoutReset()){
        as_state.calculateState();
        sd.failureDetection.inversorAliveTimestamp.reset();
        sd.failureDetection.pcAliveTimestamp.reset();
        sd.failureDetection.steerAliveTimestamp.reset();
        sd.failureDetection.resSignalLossTimestamp.reset();
        // sd.digitalData.watchdogTimestamp.reset();
    }
    
    communicator.resStateCallback(msg);
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_DRIVING, as_state.state);

    // brake pressure has a threshold to be updated, otherwise emergency
    Metro time3{RELEASE_EBS_TIMEOUT_MS / 2};
    while (!time3.checkWithoutReset()){
        sd.failureDetection.inversorAliveTimestamp.reset();
        sd.failureDetection.pcAliveTimestamp.reset();
        sd.failureDetection.steerAliveTimestamp.reset();
        sd.failureDetection.resSignalLossTimestamp.reset();
        // sd.digitalData.watchdogTimestamp.reset();
        as_state.calculateState();
    }

    TEST_ASSERT_EQUAL(State::AS_DRIVING, as_state.state); // still within threshold, okay

    Metro time4{RELEASE_EBS_TIMEOUT_MS + 10};
    while (!sd.r2dLogics.releaseEbsTimestamp.checkWithoutReset()){
        sd.failureDetection.inversorAliveTimestamp.reset();
        sd.failureDetection.pcAliveTimestamp.reset();
        sd.failureDetection.steerAliveTimestamp.reset();
        sd.failureDetection.resSignalLossTimestamp.reset();
        // sd.digitalData.watchdogTimestamp.reset();
        as_state.calculateState();
    }
    // threshold over, still with brake pressure, emergency
    TEST_ASSERT_TRUE(sd.r2dLogics.releaseEbsTimestamp.checkWithoutReset());
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_EMERGENCY, as_state.state);
}

/**
 * @brief Test function to validate AS_READY to AS_Driving transition
 * and AS_DRIVING to AS_EMERGENCY if failure occurs
*/
void test_ready_to_driving_to_emg2() {
    reset();
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
    to_ready();
    TEST_ASSERT_EQUAL(State::AS_READY, as_state.state);
    
    Metro time{READY_TIMEOUT_MS};
    while (!time.checkWithoutReset()){
        as_state.calculateState();
        sd.failureDetection.inversorAliveTimestamp.reset();
        sd.failureDetection.pcAliveTimestamp.reset();
        sd.failureDetection.steerAliveTimestamp.reset();
        sd.failureDetection.resSignalLossTimestamp.reset();
        // sd.digitalData.watchdogTimestamp.reset();
    }

    uint8_t msg[8] = {RES_GO, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    communicator.resStateCallback(msg);
    as_state.calculateState();

    TEST_ASSERT_EQUAL(State::AS_DRIVING, as_state.state);
    
    uint8_t hydraulic_msg[] = {HYDRAULIC_LINE, HYDRAULIC_PRESSURE_LOW, 0x00}; // loose brake activation
    communicator.c1Callback(hydraulic_msg);
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_DRIVING, as_state.state);

    sd.digitalData.sdcState_OPEN = true;
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_EMERGENCY, as_state.state);
}

/**
 * @brief Test function to validate AS_DRIVING to AS_FINISHED transition
 * and AS_FINISHED to AS_OFF after
*/
void test_driving_to_finished_to_off() {
    reset();
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
    to_ready();
    TEST_ASSERT_EQUAL(State::AS_READY, as_state.state);
    sd.r2dLogics.r2d = true;
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_DRIVING, as_state.state);

    uint8_t pc_msg[] = {MISSION_FINISHED};
    communicator.pcCallback(pc_msg);

    uint8_t c1_msg[] = {RIGHT_WHEEL_CODE, 0x00, 0x01, 0x00, 0x00}; // value not 0
    communicator.c1Callback(c1_msg); // right wheel = msg
    sd.sensors._left_wheel_rpm = 0;

    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_DRIVING, as_state.state);

    uint8_t c1_msg2[] = {RIGHT_WHEEL_CODE, 0x00, 0x00, 0x00, 0x00};
    communicator.c1Callback(c1_msg2); // right wheel = msg

    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_FINISHED, as_state.state);

    sd.digitalData.asms_on = false;
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
}

/**
 * @brief Test function to validate AS_FINISHED to AS_EMERGENCY transition
 * if RES is activated after mission finished
*/
void test_finished_to_emg() {
    reset();
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
    to_ready();
    TEST_ASSERT_EQUAL(State::AS_READY, as_state.state);
    sd.r2dLogics.r2d = true;
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_DRIVING, as_state.state);
    sd.sensors._left_wheel_rpm = 0;
    sd.sensors._right_wheel_rpm = 0;
    sd.missionFinished = true;
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_FINISHED, as_state.state);
    sd.failureDetection.emergencySignal = true;
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_EMERGENCY, as_state.state);
}

/**
 * @brief Test function to validate AS_OFF to AS_Manual transition
 * and the other way around
*/
void test_off_to_manual_wayback() {
    reset();
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
    sd.digitalData.asms_on = false;
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);

    sd.mission = Mission::MANUAL;
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);

    sd.digitalData.pneumatic_line_pressure = false;
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_MANUAL, as_state.state);

    sd.digitalData.pneumatic_line_pressure = true;
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
}

/**
 * @brief Test function to validate flow diagram conditions to go AS_DRIVING
*/
void test_flow_driving() {
    reset();
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
    uint8_t hydraulic_msg[] = {HYDRAULIC_LINE, HYDRAULIC_PRESSURE_HIGH, 0x00};
    communicator.c1Callback(hydraulic_msg);
    as_state.calculateState();
    
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
    sd.r2dLogics.r2d = true;

    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
    to_ready(); // will validate asms is on and ts is active
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_READY, as_state.state);

    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_READY, as_state.state); // r2d reset after transition to ready

    sd.r2dLogics.r2d = true;
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_DRIVING, as_state.state);
}

/**
 * @brief Test function to validate flow diagram conditions to go AS_READY
*/
void test_flow_ready() {
    reset();
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
    
    sd.digitalData.asms_on = true;
    // sd.digitalData.watchdog_state = true;
    sd.digitalData.sdcState_OPEN = false;

    uint8_t bamo_msg[] = {VDC_BUS, 0x00, BAMOCAR_VDC_HIGH};
    communicator.bamocarCallback(bamo_msg);
   
    sd.digitalData.pneumatic_line_pressure = true;
    uint8_t hydraulic_msg[] = {HYDRAULIC_LINE, HYDRAULIC_PRESSURE_HIGH, 0x00};
    // uint8_t hydraulic_msg2[] = {HYDRAULIC_LINE, HYDRAULIC_PRESSURE_LOW, 0x00}; // loose brake activation
    communicator.c1Callback(hydraulic_msg);

    // Iterate a few times to go to check wd
    // Metro time{1};
    // while (!time.checkWithoutReset()) {
    //     as_state.calculateState();
    // }
    
    // sd.digitalData.watchdog_state = false;
    // Wait for wd timeout
    // Metro time2{INITIAL_CHECKUP_STEP_TIMEOUT};
    // while (!time2.checkWithoutReset()) {
    //     communicator.c1Callback(hydraulic_msg2);
    //     as_state.calculateState();
    //     sd.failureDetection.inversorAliveTimestamp.reset();
    //     sd.failureDetection.pcAliveTimestamp.reset();
    //     sd.failureDetection.steerAliveTimestamp.reset();
        // sd.digitalData.watchdogTimestamp.reset();
    // }
    // TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);

    Metro time3{INITIAL_CHECKUP_STEP_TIMEOUT};
    while (!time3.checkWithoutReset()) {
        communicator.c1Callback(hydraulic_msg);
        as_state.calculateState();
        sd.failureDetection.inversorAliveTimestamp.reset();
        sd.failureDetection.pcAliveTimestamp.reset();
        sd.failureDetection.steerAliveTimestamp.reset();
        sd.failureDetection.resSignalLossTimestamp.reset();
        // sd.digitalData.watchdogTimestamp.reset();
    }

    TEST_ASSERT_EQUAL(State::AS_READY, as_state.state);
}

/**
 * @brief Test function to validate flow diagram conditions to go AS_EMERGENCY
*/
void test_flow_emergency() {
    reset();
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
    to_ready();
    TEST_ASSERT_EQUAL(State::AS_READY, as_state.state);
    sd.r2dLogics.r2d = true;
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_DRIVING, as_state.state);

    sd.digitalData.sdcState_OPEN = true; // ebs before finished checks

    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_EMERGENCY, as_state.state);

    uint8_t pc_msg[] = {MISSION_FINISHED};
    communicator.pcCallback(pc_msg);
    sd.sensors._left_wheel_rpm = 0;
    uint8_t c1_msg[] = {RIGHT_WHEEL_CODE, 0x00, 0x00, 0x00, 0x00};
    communicator.c1Callback(c1_msg); // right wheel = msg
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_EMERGENCY, as_state.state);
}

/**
 * @brief Test function to validate flow diagram conditions to go AS_FINISHED
*/
void test_flow_finished() {
    reset();
    TEST_ASSERT_EQUAL(State::AS_OFF, as_state.state);
    to_ready();
    TEST_ASSERT_EQUAL(State::AS_READY, as_state.state);
    sd.r2dLogics.r2d = true;
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_DRIVING, as_state.state);

    uint8_t pc_msg[] = {MISSION_FINISHED};
    communicator.pcCallback(pc_msg);
    sd.sensors._left_wheel_rpm = 0;
    uint8_t c1_msg[] = {RIGHT_WHEEL_CODE, 0x00, 0x00, 0x00, 0x00};
    communicator.c1Callback(c1_msg); // right wheel = msg
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_FINISHED, as_state.state);

    sd.digitalData.sdcState_OPEN = true;
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_FINISHED, as_state.state);

    sd.failureDetection.emergencySignal = true;
    as_state.calculateState();
    TEST_ASSERT_EQUAL(State::AS_EMERGENCY, as_state.state);
}

void setUp() {
    
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_off_to_ready_success);
    RUN_TEST(test_off_to_ready_recheck);
    RUN_TEST(test_off_to_ready_wayback_impossible);
    RUN_TEST(test_ready_to_driving_to_emg);
    RUN_TEST(test_ready_to_driving_to_emg2);
    RUN_TEST(test_ready_to_emg_to_off);
    RUN_TEST(test_driving_to_finished_to_off);
    RUN_TEST(test_finished_to_emg);
    RUN_TEST(test_off_to_manual_wayback);
    RUN_TEST(test_flow_driving);
    RUN_TEST(test_flow_ready);
    RUN_TEST(test_flow_emergency);
    RUN_TEST(test_flow_finished);
    return UNITY_END();
}
