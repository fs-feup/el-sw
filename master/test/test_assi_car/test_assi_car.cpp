#include "embedded/digitalSender.hpp"
#include "embedded/digitalSettings.hpp"
#include "metro.h"
#include "unity.h"

void setUp(void) { DigitalSender ds = DigitalSender(); }

/**
 * @attention PROTOCOL TEST IN CAR
 * @brief Test the enterEmergency state function
 * PROTOCOL:
 * 1. ASSERT - Observe that after only LEDS 2, 3 return on
 */
void test_enterEmergencyState_and_turn_off_LEDS(void) {
  Metro test{5000};
  DigitalSender ds;
  ds.enter_emergency_state();
  while (!test.check()) {
    ds.blink_led(ASSI_BLUE_PIN);
  }
  TEST_PASS();
}

/**
 * @attention PROTOCOL TEST IN CAR
 * @brief Test the enterDrivingState function
 * PROTOCOL:
 * 1. ASSERT - Observe that after only LEDS 2, 3 return on
 */
void test_enterDrivingState_and_turn_off_LEDS(void) {
  Metro test{5000};
  DigitalSender ds;
  ds.enter_driving_state();
  while (!test.check()) {
    ds.blink_led(ASSI_YELLOW_PIN);
  }
  TEST_PASS();
}

/**
 * @attention PROTOCOL TEST IN CAR
 * @brief Test the enterFinishState function
 * PROTOCOL:
 * 1. ASSERT - Observe that after only LEDS 2, 3 return on
 */
void test_enterFinishState_and_turn_off_LEDS(void) {
  Metro test{5000};
  DigitalSender ds;
  ds.enter_finish_state();
  while (!test.check()) {
  }
  TEST_PASS();
}

/**
 * @attention PROTOCOL TEST IN  CAR
 * @brief Test the enterReadyState function
 * PROTOCOL:
 * 1. ASSERT - Observe that after only LEDS 2, 3 return on
 */
void test_enterReadyState_and_turn_off_LEDS(void) {
  Metro test{5000};
  DigitalSender ds;
  ds.enter_ready_state();
  while (!test.check()) {
  }
  TEST_PASS();
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_enterReadyState_and_turn_off_LEDS);
  RUN_TEST(test_enterDrivingState_and_turn_off_LEDS);
  RUN_TEST(test_enterFinishState_and_turn_off_LEDS);
  RUN_TEST(test_enterEmergencyState_and_turn_off_LEDS);
  return UNITY_END();
}