#include "unity.h"
#include "embedded/digitalSettings.hpp"

//undefine before all the pins defined after
#undef SDC_LOGIC_CLOSE_SDC_PIN
#undef SDC_LOGIC_WATCHDOG_OUT_PIN
#undef EBS_VALVE_1_PIN
#undef EBS_VALVE_2_PIN
#undef MASTER_SDC_OUT_PIN

#define SDC_LOGIC_CLOSE_SDC_PIN 2
#define MASTER_SDC_OUT_PIN 3
#define EBS_VALVE_1_PIN 4
#define EBS_VALVE_2_PIN 5
#define SDC_LOGIC_WATCHDOG_OUT_PIN 6

#define UNALLOWED_PIN 0

#include "metro.h"
#include "embedded/digitalSender.hpp"

void setUp(void) {
    for (int pin = 0; pin < 7; pin++) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }
}

void tearDown(void) {
    // This is run after EACH test
}


/**
 * @attention PROTOCOL TEST
 * @brief Test the SDC Functions
 * PROTOCOL:
 * 1. ASSERT - Observe that pin 1 and 2 turn on for 3 seconds then turn off
*/
void test_openSDC_then_closeSDC(void) {
    Metro test{5000};
    DigitalSender::openSDC();
    while (!test.check()) {
    }
    DigitalSender::closeSDC();
    while (!test.check()) {
    };
    TEST_PASS();
}

/**
 * @attention PROTOCOL TEST
 * @brief Test the enterEmergency state function
 * PROTOCOL:
 * 1. ASSERT - Observe that after only LEDS 2, 3 return on
*/
void test_enterEmergencyState_and_turn_off_LEDS(void) {
    Metro test{5000};
    DigitalSender ds;
    ds.enterEmergencyState();
    while (!test.check()) {
    }
    TEST_PASS();
}

/**
 * @attention PROTOCOL TEST
 * @brief Test the blinkLED function
 * The toggle watchdog is the same function as blink LED so we can afirm
 * that passing this test implies that the toggleWatchdog function works
 * PROTOCOL:
 * 1. ASSERT - Observer that LED correctly blinks at 1hz for 10 seconds
*/
void test_blinkLED(void) {
    Metro test{10000};
    DigitalSender ds;

    while (!test.check()) { 
        ds.blinkLED(1);
    }
    TEST_PASS();
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_openSDC_then_closeSDC);
    RUN_TEST(test_enterEmergencyState_and_turn_off_LEDS);
    RUN_TEST(test_blinkLED);
    return UNITY_END();
}

