#include "comm/communicatorSettings.hpp"
#include "logic/checkupManager.hpp"
#include "model/systemData.hpp"
#include "unity.h"

void test_shouldStayManualDriving_true() {
  SystemData systemData;
  systemData.mission_ = Mission::MANUAL;
  systemData.digital_data_.pneumatic_line_pressure_ = false;
  systemData.digital_data_.asms_on_ = false;

  CheckupManager checkupManager(&systemData);

  TEST_ASSERT_TRUE(checkupManager.should_stay_manual_driving());
}

void test_shouldStayManualDriving_false() {
  SystemData systemData;
  CheckupManager checkupManager(&systemData);

  systemData.mission_ = Mission::INSPECTION;
  TEST_ASSERT_FALSE(checkupManager.should_stay_manual_driving());

  systemData.mission_ = Mission::MANUAL;
  systemData.digital_data_.pneumatic_line_pressure_ = true;
  TEST_ASSERT_FALSE(checkupManager.should_stay_manual_driving());

  systemData.digital_data_.pneumatic_line_pressure_ = false;
  systemData.digital_data_.asms_on_ = true;
  TEST_ASSERT_FALSE(checkupManager.should_stay_manual_driving());

  systemData.digital_data_.asms_on_ = false;
  TEST_ASSERT_TRUE(checkupManager.should_stay_manual_driving());
}

// can only test false since we can't mock the initial sequence :(
void test_shouldStayOff_whenInitialCheckupFails_false() {
  SystemData systemData;
  DigitalSender digitalSender;
  // default values shouldnt allow to pass test
  CheckupManager checkupManager(&systemData);

  TEST_ASSERT_TRUE(checkupManager.should_stay_off(&digitalSender));
}

void test_initialCheckupSequence_states() {
  SystemData sd;
  DigitalSender digitalSender;
  CheckupManager cm(&sd);

  cm.initial_checkup_sequence(&digitalSender);
  TEST_ASSERT_EQUAL(CheckupManager::CheckupState::WAIT_FOR_ASMS, cm.checkupState);

  sd.digital_data_.asms_on_ = true;
  cm.initial_checkup_sequence(&digitalSender);
  // TEST_ASSERT_EQUAL(CheckupManager::CheckupState::START_TOGGLING_WATCHDOG, cm.checkupState);

  // cm.initialCheckupSequence(&digitalSender);
  // TEST_ASSERT_EQUAL(CheckupManager::CheckupState::WAIT_FOR_WATCHDOG, cm.checkupState);

  // cm.getInitialCheckupTimestamp().reset();
  // sd.digital_data_.watchdog_state = true;
  // cm.initialCheckupSequence(&digitalSender);
  // TEST_ASSERT_EQUAL(CheckupManager::CheckupState::STOP_TOGGLING_WATCHDOG, cm.checkupState);

  // cm.initialCheckupSequence(&digitalSender);
  // TEST_ASSERT_EQUAL(CheckupManager::CheckupState::CHECK_WATCHDOG, cm.checkupState);

  // Metro waitForWatchdogExpiration{1000};
  // while (!waitForWatchdogExpiration.check()) {
  // }
  // sd.digital_data_.watchdog_state = false;
  // cm.initialCheckupSequence(&digitalSender);
  TEST_ASSERT_EQUAL(CheckupManager::CheckupState::CLOSE_SDC, cm.checkupState);

  cm.initial_checkup_sequence(&digitalSender);
  TEST_ASSERT_EQUAL(CheckupManager::CheckupState::WAIT_FOR_AATS, cm.checkupState);

  sd.digital_data_.sdc_open_ = false;
  cm.initial_checkup_sequence(&digitalSender);
  TEST_ASSERT_EQUAL(CheckupManager::CheckupState::WAIT_FOR_TS, cm.checkupState);

  sd.failure_detection_.ts_on_ = true;
  cm.initial_checkup_sequence(&digitalSender);
  TEST_ASSERT_EQUAL(CheckupManager::CheckupState::TOGGLE_VALVE, cm.checkupState);

  sd.digital_data_.pneumatic_line_pressure_ = true;
  sd.sensors_._hydraulic_line_pressure = HYDRAULIC_BRAKE_THRESHOLD;
  cm.initial_checkup_sequence(&digitalSender);
  TEST_ASSERT_EQUAL(CheckupManager::CheckupState::CHECK_PRESSURE, cm.checkupState);

  sd.failure_detection_.emergency_signal_ = false;
  sd.failure_detection_.inversor_alive_timestamp_.reset();
  sd.failure_detection_.pc_alive_timestamp_.reset();
  sd.failure_detection_.steer_alive_timestamp_.reset();
  // todo MISSING INVERSOR ALIVE TIMESTAMP

  TEST_ASSERT_EQUAL(CheckupManager::CheckupError::SUCCESS,
                    cm.initial_checkup_sequence(&digitalSender));
}

void test_shouldStayReady() {
  SystemData systemData;
  systemData.r2d_logics_.r2d = false;

  CheckupManager checkupManager(&systemData);

  TEST_ASSERT_TRUE(checkupManager.shouldStayReady());

  systemData.r2d_logics_.r2d = true;
  TEST_ASSERT_FALSE(checkupManager.shouldStayReady());
}

void test_shouldEnterEmergency() {
  SystemData sd;
  sd.digital_data_.sdc_open_ = false;
  sd.digital_data_.pneumatic_line_pressure_ = true;
  sd.digital_data_.asms_on_ = true;
  sd.sensors_._hydraulic_line_pressure = HYDRAULIC_BRAKE_THRESHOLD + 1;
  sd.failure_detection_.inversor_alive_timestamp_.reset();
  sd.failure_detection_.pc_alive_timestamp_.reset();
  sd.failure_detection_.steer_alive_timestamp_.reset();
  sd.failure_detection_.res_signal_loss_timestamp_.reset();
  // sd.digital_data_.watchdogTimestamp.reset();
  sd.failure_detection_.emergency_signal_ = false;
  sd.failure_detection_.ts_on_ = true;

  CheckupManager checkupManager(&sd);

  TEST_ASSERT_FALSE(checkupManager.shouldEnterEmergency(State::AS_READY));
  sd.digital_data_.sdc_open_ = true;
  TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_READY));
  sd.digital_data_.sdc_open_ = false;
  sd.digital_data_.pneumatic_line_pressure_ = false;
  while (!sd.r2d_logics_.releaseEbsTimestamp.checkWithoutReset());
  TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_READY));
  sd.digital_data_.pneumatic_line_pressure_ = true;
  sd.digital_data_.asms_on_ = false;
  TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_READY));
  sd.digital_data_.asms_on_ = true;
  sd.failure_detection_.ts_on_ = false;
  TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_READY));
  sd.failure_detection_.ts_on_ = true;
  sd.failure_detection_.emergency_signal_ = true;
  TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_READY));
  sd.failure_detection_.emergency_signal_ = false;
  sd.sensors_._hydraulic_line_pressure = 1;
  TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_READY));

  sd.failure_detection_.inversor_alive_timestamp_.reset();
  sd.failure_detection_.pc_alive_timestamp_.reset();
  sd.failure_detection_.steer_alive_timestamp_.reset();
  sd.failure_detection_.res_signal_loss_timestamp_.reset();
  TEST_ASSERT_FALSE(checkupManager.shouldEnterEmergency(State::AS_DRIVING));
  sd.digital_data_.sdc_open_ = true;
  TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_DRIVING));
  sd.digital_data_.sdc_open_ = false;
  sd.digital_data_.pneumatic_line_pressure_ = false;
  TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_DRIVING));
  sd.digital_data_.pneumatic_line_pressure_ = true;
  sd.digital_data_.asms_on_ = false;
  TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_DRIVING));
  sd.digital_data_.asms_on_ = true;
  sd.failure_detection_.ts_on_ = false;
  TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_READY));
  sd.failure_detection_.ts_on_ = true;
  sd.failure_detection_.emergency_signal_ = true;
  TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_DRIVING));
  sd.failure_detection_.emergency_signal_ = false;
  sd.sensors_._hydraulic_line_pressure = HYDRAULIC_BRAKE_THRESHOLD + 1;

  // // Metro wait{WATCHDOG_TIMEOUT};
  // while (!wait.check()) {
  // }
  // TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_READY));
  sd.sensors_._hydraulic_line_pressure = HYDRAULIC_BRAKE_THRESHOLD + 1;
  TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_DRIVING));
}

void test_shouldEnterEmergencyAsDrivingEBSValves() {
  SystemData sd;
  sd.digital_data_.sdc_open_ = false;
  sd.digital_data_.pneumatic_line_pressure_ = true;
  sd.digital_data_.asms_on_ = true;
  sd.sensors_._hydraulic_line_pressure = 1;
  sd.failure_detection_.inversor_alive_timestamp_.reset();
  sd.failure_detection_.pc_alive_timestamp_.reset();
  sd.failure_detection_.steer_alive_timestamp_.reset();
  sd.failure_detection_.res_signal_loss_timestamp_.checkWithoutReset();
  // sd.digital_data_.watchdogTimestamp.reset();
  sd.r2d_logics_.releaseEbsTimestamp.reset();
  sd.failure_detection_.emergency_signal_ = false;
  sd.failure_detection_.ts_on_ = true;

  CheckupManager checkupManager(&sd);

  TEST_ASSERT_FALSE(checkupManager.shouldEnterEmergency(State::AS_DRIVING));
  sd.sensors_._hydraulic_line_pressure = HYDRAULIC_BRAKE_THRESHOLD + 1;
  TEST_ASSERT_FALSE(checkupManager.shouldEnterEmergency(State::AS_DRIVING));

  Metro time3{RELEASE_EBS_TIMEOUT_MS + 10};
  while (!time3.check()) {
    sd.failure_detection_.inversor_alive_timestamp_.reset();
    sd.failure_detection_.pc_alive_timestamp_.reset();
    sd.failure_detection_.steer_alive_timestamp_.reset();
    sd.failure_detection_.res_signal_loss_timestamp_.reset();
    // sd.digital_data_.watchdogTimestamp.reset();
  }

  TEST_ASSERT_TRUE(checkupManager.shouldEnterEmergency(State::AS_DRIVING));
  sd.sensors_._hydraulic_line_pressure = 1;
  TEST_ASSERT_FALSE(checkupManager.shouldEnterEmergency(State::AS_DRIVING));
}

void test_shouldStayDriving() {
  SystemData systemData;
  systemData.sensors_._left_wheel_rpm = 0;
  systemData.mission_finished_ = true;

  CheckupManager checkupManager(&systemData);

  TEST_ASSERT_FALSE(checkupManager.shouldStayDriving());

  systemData.sensors_._left_wheel_rpm = 1;
  TEST_ASSERT_TRUE(checkupManager.shouldStayDriving());

  systemData.sensors_._left_wheel_rpm = 0;
  systemData.mission_finished_ = false;
  TEST_ASSERT_TRUE(checkupManager.shouldStayDriving());
}

void test_shouldStayMissionFinished() {
  SystemData systemData;
  systemData.digital_data_.asms_on_ = false;

  CheckupManager checkupManager(&systemData);

  TEST_ASSERT_FALSE(checkupManager.shouldStayMissionFinished());

  systemData.digital_data_.asms_on_ = true;
  TEST_ASSERT_TRUE(checkupManager.shouldStayMissionFinished());
}

void test_emergencySequenceComplete() {
  SystemData systemData;
  systemData.digital_data_.asms_on_ = true;

  CheckupManager checkupManager(&systemData);

  TEST_ASSERT_FALSE(checkupManager.emergencySequenceComplete());

  systemData.digital_data_.asms_on_ = false;
  Metro waitForEbsSound{8500};
  while (!waitForEbsSound.check()) {
  }
  TEST_ASSERT_TRUE(checkupManager.emergencySequenceComplete());
}

void test_resTriggered() {
  SystemData systemData;
  systemData.failure_detection_.emergency_signal_ = true;

  CheckupManager checkupManager(&systemData);

  TEST_ASSERT_TRUE(checkupManager.resTriggered());

  systemData.failure_detection_.emergency_signal_ = false;
  TEST_ASSERT_FALSE(checkupManager.resTriggered());
}

void setUp(void) {}

void tearDown(void) {}

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