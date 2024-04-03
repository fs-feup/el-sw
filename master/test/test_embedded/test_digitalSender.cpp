#include "unity.h"
#include "embedded/digitalSettings.hpp"

//undefine before all the pins defined after
#undef ASSI_DRIVING_PIN
#undef ASSI_READY_PIN
#undef ASSI_FINISH_PIN
#undef ASSI_EMERGENCY_PIN
#undef SDC_LOGIC_CLOSE_SDC_PIN
#undef SDC_LOGIC_WATCHDOG_OUT_PIN
#undef EBS_VALVE_1_PIN
#undef EBS_VALVE_2_PIN
#undef MASTER_SDC_OUT_PIN


#define ASSI_EMERGENCY_PIN 0
#define ASSI_DRIVING_PIN 1
#define ASSI_READY_PIN 2
#define ASSI_FINISH_PIN 3

#define SDC_LOGIC_CLOSE_SDC_PIN 2
#define MASTER_SDC_OUT_PIN 3
#define EBS_VALVE_1_PIN 4
#define EBS_VALVE_2_PIN 5
#define SDC_LOGIC_WATCHDOG_OUT_PIN 6

#define UNALLOWED_PIN 0

#include <Metro.h>
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

// Test Completion:
// Observe than pin 1 and 2 turn on for 3 seconds then turn off
void test_openSDC_then_closeSDC(void) {
    Metro test{3000};
    DigitalSender::openSDC();
    while (!test.check()) {}
    DigitalSender::closeSDC();
    while (!test.check()) {};
    TEST_PASS();
}

// Test Completion:
// Observe that after only LEDS 2, 3 rturn on
void test_enterEmergencyState_and_turn_off_LEDS(void) {
    Metro test{3000};
    DigitalSender ds;
    ds.enterEmergencyState();
    while (!test.check()) {}
    TEST_PASS();
}

//
// void test_DigitalSender_activateEBS_sets_correct_pins(void) {
//     DigitalSender ds;
//     ds.activateEBS();
//     TEST_ASSERT_EQUAL_INT(LOW, digitalRead(EBS_VALVE_1_PIN));
//     TEST_ASSERT_EQUAL_INT(LOW, digitalRead(EBS_VALVE_2_PIN));
// }
//
// void test_DigitalSender_deactivateEBS_sets_correct_pins(void) {
//     DigitalSender ds;
//     ds.deactivateEBS();
//     TEST_ASSERT_EQUAL_INT(HIGH, digitalRead(EBS_VALVE_1_PIN));
//     TEST_ASSERT_EQUAL_INT(HIGH, digitalRead(EBS_VALVE_2_PIN));
// }
//
// void test_DigitalSender_blinkLED_toggles_pin_state(void) {
//     DigitalSender ds;
//     int initial_state = digitalRead(LED_BUILTIN);
//     ds.blinkLED(LED_BUILTIN);
//     TEST_ASSERT_EQUAL_INT(!initial_state, digitalRead(LED_BUILTIN));
// }
//
// void test_DigitalSender_toggleWatchdog_toggles_watchdog_state(void) {
//     DigitalSender ds;
//     int initial_state = digitalRead(SDC_LOGIC_WATCHDOG_OUT_PIN);
//     ds.toggleWatchdog();
//     TEST_ASSERT_EQUAL_INT(!initial_state, digitalRead(SDC_LOGIC_WATCHDOG_OUT_PIN));
// }
//
int main(void) {
    UNITY_BEGIN();
    // RUN_TEST(test_openSDC_then_closeSDC);
    RUN_TEST(test_enterEmergencyState_and_turn_off_LEDS);
    // RUN_TEST(test_DigitalSender_openSDC_sets_correct_pins);
    // RUN_TEST(test_DigitalSender_closeSDC_sets_correct_pins);
    // RUN_TEST(test_DigitalSender_activateEBS_sets_correct_pins);
    // RUN_TEST(test_DigitalSender_deactivateEBS_sets_correct_pins);
    // RUN_TEST(test_DigitalSender_blinkLED_toggles_pin_state);
    // RUN_TEST(test_DigitalSender_toggleWatchdog_toggles_watchdog_state);
    return UNITY_END();
}
