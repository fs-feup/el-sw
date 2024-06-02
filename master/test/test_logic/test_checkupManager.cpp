#include "unity.h"
#include "logic/checkupManager.hpp"
#include "model/systemData.hpp"
#include "comm/communicatorSettings.hpp"

void test_shouldStayManualDriving_true() {
    SystemData systemData;
    systemData.mission = MANUAL;
    systemData.digitalData.pneumatic_line_pressure = false;
    systemData.digitalData.asms_on = false;

    CheckupManager checkupManager(&systemData);

    TEST_ASSERT_TRUE(checkupManager.shouldStayManualDriving());
}

void test_shouldStayManualDriving_false() {
    SystemData systemData;
    CheckupManager checkupManager(&systemData);

    systemData.mission = INSPECTION;
    TEST_ASSERT_FALSE(checkupManager.shouldStayManualDriving());

    systemData.mission = MANUAL;
    systemData.digitalData.pneumatic_line_pressure = true;
    TEST_ASSERT_FALSE(checkupManager.shouldStayManualDriving());

    systemData.digitalData.pneumatic_line_pressure = false;
    systemData.digitalData.asms_on = true;
    TEST_ASSERT_FALSE(checkupManager.shouldStayManualDriving());

    systemData.digitalData.asms_on = false;
    TEST_ASSERT_TRUE(checkupManager.shouldStayManualDriving());
}

//can only test false since we can't mock the initial sequence :(
void test_shouldStayOff_whenInitialCheckupFails_false() {
    SystemData systemData;
    DigitalSender digitalSender;
    // default values shouldnt allow to pass test
    CheckupManager checkupManager(&systemData);

    TEST_ASSERT_TRUE(checkupManager.shouldStayOff(&digitalSender));
}

void test_initialCheckupSequence_states() {
    SystemData sd;
    DigitalSender digitalSender;
    CheckupManager cm(&sd);

    cm.initialCheckupSequence(&digitalSender);
    TEST_ASSERT_EQUAL(CheckupManager::CheckupState::WAIT_FOR_ASMS, cm.checkupState);

    sd.digitalData.asms_on = true;
    cm.initialCheckupSequence(&digitalSender);
    // TEST_ASSERT_EQUAL(CheckupManager::CheckupState::START_TOGGLING_WATCHDOG, cm.checkupState);

    cm.initialCheckupSequence(&digitalSender);
    // TEST_ASSERT_EQUAL(CheckupManager::CheckupState::WAIT_FOR_WATCHDOG, cm.checkupState);

    cm.getInitialCheckupTimestamp().reset();
    // sd.digitalData.watchdog_state = true;
    cm.initialCheckupSequence(&digitalSender);
    // TEST_ASSERT_EQUAL(CheckupManager::CheckupState::STOP_TOGGLING_WATCHDOG, cm.checkupState);

    cm.initialCheckupSequence(&digitalSender);
    // TEST_ASSERT_EQUAL(CheckupManager::CheckupState::CHECK_WATCHDOG, cm.checkupState);

    // Metro waitForWatchdogExpiration{1000};
    // while (!waitForWatchdogExpiration.check()) {
    // }
    // sd.digitalData.watchdog_state = false;
    cm.initialCheckupSequence(&digitalSender);
    TEST_ASSERT_EQUAL(CheckupManager::CheckupState::CLOSE_SDC, cm.checkupState);

    cm.initialCheckupSequence(&digitalSender);
    TEST_ASSERT_EQUAL(CheckupManager::CheckupState::WAIT_FOR_AATS, cm.checkupState);

    sd.digitalData.sdcState_OPEN = false;
    cm.initialCheckupSequence(&digitalSender);
    TEST_ASSERT_EQUAL(CheckupManager::CheckupState::WAIT_FOR_TS, cm.checkupState);

    sd.failureDetection.ts_on = true;
    cm.initialCheckupSequence(&digitalSender);
    TEST_ASSERT_EQUAL(CheckupManager::CheckupState::TOGGLE_VALVE, cm.checkupState);

    sd.digitalData.pneumatic_line_pressure = true;
    sd.sensors._hydraulic_line_pressure = HYDRAULIC_BRAKE_THRESHOLD;
    cm.initialCheckupSequence(&digitalSender);
    TEST_ASSERT_EQUAL(CheckupManager::CheckupState::CHECK_PRESSURE, cm.checkupState);

    cm.getInitialCheckupTimestamp().reset();
    cm.initialCheckupSequence(&digitalSender);
    TEST_ASSERT_EQUAL(CheckupManager::CheckupState::CHECK_TIMESTAMPS, cm.checkupState);

    sd.failureDetection.emergencySignal = false;
    sd.failureDetection.inversorAliveTimestamp.reset();
    sd.failureDetection.pcAliveTimestamp.reset();
    sd.failureDetection.steerAliveTimestamp.reset();
    //todo MISSING INVERSOR ALIVE TIMESTAMP

    TEST_ASSERT_EQUAL(CheckupManager::CheckupError::SUCCESS, cm.initialCheckupSequence(&digitalSender));

}

void test_shouldStayReady() {
    SystemData systemData;
    systemData.r2dLogics.r2d = false;

    CheckupManager checkupManager(&systemData);

    TEST_ASSERT_TRUE(checkupManager.shouldStayReady());

    systemData.r2dLogics.r2d = true;
    TEST_ASSERT_FALSE(checkupManager.shouldStayReady());
}

void test_shouldEnterEmergency() {
    SystemData sd;
    sd.digitalData.sdcState_OPEN = false;
    sd.digitalData.pneumatic_line_pressure = true;
    sd.digitalData.asms_on = true;
    sd.sensors._hydraulic_line_pressure = HYDRAULIC_BRAKE_THRESHOLD + 1;
    sd.failureDetection.inversorAliveTimestamp.reset();
    sd.failureDetection.pcAliveTimestamp.reset();
    sd.failureDetection.steerAliveTimestamp.reset();
    // sd.digitalData.watchdogTimestamp.reset();
    sd.failureDetection.emergencySignal = false;
    sd.failureDetection.ts_on = true;

    CheckupManager checkupManager(&sd);

    TEST_ASSERT_FALSE(checkupManager.shouldEnterEmergency(State::AS_READY));
    sd.digitalData.sdcState_OPEN = true;
    TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_READY));
    sd.digitalData.sdcState_OPEN = false;
    sd.digitalData.pneumatic_line_pressure = false;
    TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_READY));
    sd.digitalData.pneumatic_line_pressure = true;
    sd.digitalData.asms_on = false;
    TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_READY));
    sd.digitalData.asms_on = true;
    sd.failureDetection.ts_on = false;
    TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_READY));
    sd.failureDetection.ts_on = true;
    sd.failureDetection.emergencySignal = true;
    TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_READY));
    sd.failureDetection.emergencySignal = false;
    sd.sensors._hydraulic_line_pressure = 1;
    TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_READY));

    TEST_ASSERT_FALSE(checkupManager.shouldEnterEmergency(State::AS_DRIVING));
    sd.digitalData.sdcState_OPEN = true;
    TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_DRIVING));
    sd.digitalData.sdcState_OPEN = false;
    sd.digitalData.pneumatic_line_pressure = false;
    TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_DRIVING));
    sd.digitalData.pneumatic_line_pressure = true;
    sd.digitalData.asms_on = false;
    TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_DRIVING));
    sd.digitalData.asms_on = true;
    sd.failureDetection.ts_on = false;
    TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_READY));
    sd.failureDetection.ts_on = true;
    sd.failureDetection.emergencySignal = true;
    TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_DRIVING));
    sd.failureDetection.emergencySignal = false;
    sd.sensors._hydraulic_line_pressure = HYDRAULIC_BRAKE_THRESHOLD + 1;

    // // Metro wait{WATCHDOG_TIMEOUT};
    // while (!wait.check()) {
    // }
    TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_READY));
    sd.sensors._hydraulic_line_pressure = 1;
    TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_DRIVING));
}

void test_shouldEnterEmergencyAsDrivingEBSValves() {
    SystemData sd;
    sd.digitalData.sdcState_OPEN = false;
    sd.digitalData.pneumatic_line_pressure = true;
    sd.digitalData.asms_on = true;
    sd.sensors._hydraulic_line_pressure = 1;
    sd.failureDetection.inversorAliveTimestamp.reset();
    sd.failureDetection.pcAliveTimestamp.reset();
    sd.failureDetection.steerAliveTimestamp.reset();
    // sd.digitalData.watchdogTimestamp.reset();
    sd.r2dLogics.releaseEbsTimestamp = millis();
    sd.failureDetection.emergencySignal = false;
    sd.failureDetection.ts_on = true;

    CheckupManager checkupManager(&sd);

    TEST_ASSERT_FALSE(checkupManager.shouldEnterEmergency(State::AS_DRIVING));
    sd.sensors._hydraulic_line_pressure = HYDRAULIC_BRAKE_THRESHOLD + 1;
    TEST_ASSERT_FALSE(checkupManager.shouldEnterEmergency(State::AS_DRIVING));

    Metro time3{RELEASE_EBS_TIMEOUT_MS + 10};
    while (!time3.check()){
        sd.failureDetection.inversorAliveTimestamp.reset();
        sd.failureDetection.pcAliveTimestamp.reset();
        sd.failureDetection.steerAliveTimestamp.reset();
        // sd.digitalData.watchdogTimestamp.reset();
    }

    TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_DRIVING));
    sd.sensors._hydraulic_line_pressure = 1;
    TEST_ASSERT_FALSE(checkupManager.shouldEnterEmergency(State::AS_DRIVING));
}

void test_shouldStayDriving() {
    SystemData systemData;
    systemData.sensors._left_wheel_rpm = 0;
    systemData.missionFinished = true;

    CheckupManager checkupManager(&systemData);

    TEST_ASSERT_FALSE(checkupManager.shouldStayDriving());

    systemData.sensors._left_wheel_rpm = 1;
    TEST_ASSERT_TRUE(checkupManager.shouldStayDriving());

    systemData.sensors._left_wheel_rpm = 0;
    systemData.missionFinished = false;
    TEST_ASSERT_TRUE(checkupManager.shouldStayDriving());
}

void test_shouldStayMissionFinished() {
    SystemData systemData;
    systemData.digitalData.asms_on = false;

    CheckupManager checkupManager(&systemData);

    TEST_ASSERT_FALSE(checkupManager.shouldStayMissionFinished());

    systemData.digitalData.asms_on = true;
    TEST_ASSERT_TRUE(checkupManager.shouldStayMissionFinished());
}

void test_emergencySequenceComplete() {
    SystemData systemData;
    systemData.digitalData.asms_on = true;

    CheckupManager checkupManager(&systemData);

    TEST_ASSERT_FALSE(checkupManager.emergencySequenceComplete());


    systemData.digitalData.asms_on = false;
    Metro waitForEbsSound{8500};
    while (!waitForEbsSound.check()) {
    }
    TEST_ASSERT_TRUE(checkupManager.emergencySequenceComplete());
}

void test_resTriggered() {
    SystemData systemData;
    systemData.failureDetection.emergencySignal = true;

    CheckupManager checkupManager(&systemData);

    TEST_ASSERT_TRUE(checkupManager.resTriggered());

    systemData.failureDetection.emergencySignal = false;
    TEST_ASSERT_FALSE(checkupManager.resTriggered());
}

void setUp(void) {
}

void tearDown(void) {
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_shouldEnterEmergencyAsDrivingEBSValves);
    RUN_TEST(test_initialCheckupSequence_states);
    RUN_TEST(test_shouldStayManualDriving_true);
    RUN_TEST(test_shouldStayManualDriving_false);
    RUN_TEST(test_shouldStayOff_whenInitialCheckupFails_false);
    RUN_TEST(test_shouldStayReady);
    RUN_TEST(test_shouldEnterEmergency);
    RUN_TEST(test_shouldStayDriving);
    RUN_TEST(test_shouldStayMissionFinished);
    RUN_TEST(test_emergencySequenceComplete);
    RUN_TEST(test_resTriggered);
    return UNITY_END();
}