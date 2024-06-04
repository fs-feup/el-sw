#include <Metro.h>
#include "embedded/digitalSender.hpp"
#include "unity.h"
#include "embedded/digitalSettings.hpp"



void setUp(void) {
    DigitalSender ds = DigitalSender();
}


/**
 * @attention PROTOCOL TEST IN CAR
 * @brief Test the enterEmergency state function
 * PROTOCOL:
 * 1. ASSERT - Observe that after only LEDS 2, 3 return on
*/
void test_enterEmergencyState_and_turn_off_LEDS(void) {
    Metro test{5000};
    DigitalSender ds;
    ds.enterEmergencyState();
    while (!test.check()) {
        ds.blinkLED(ASSI_BLUE_PIN);
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
    ds.enterDrivingState();
    while (!test.check()) {
        ds.blinkLED(ASSI_YELLOW_PIN);
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
    ds.enterFinishState();
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
    ds.enterReadyState();
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