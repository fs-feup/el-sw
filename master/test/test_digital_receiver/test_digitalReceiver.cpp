#include "model/digitalData.hpp"
#include "unity.h"

#define GREEN_LED_1 4
#define GREEN_LED_2 5
#define GREEN_LED_3 6
#define RED_LED_1 0
#define RED_LED_2 1
#define YELLOW_LED_1 2
#define YELLOW_LED_2 3
#define BUTTON_1 7
#define BUTTON_2 8
#define BUTTON_3 9
#define BUTTON_4 10
#define BUTTON_5 11
#define BUTTON_6 12

#define ASMS_IN_PIN BUTTON_1
#define SDC_STATE_PIN BUTTON_2
#define SDC_LOGIC_WATCHDOG_IN_PIN BUTTON_3
#define SENSOR_PRESSURE_1_PIN BUTTON_4
#define SENSOR_PRESSURE_2_PIN BUTTON_5
#define LWSS_PIN BUTTON_6

#define WHEEL_MEASUREMENT_INTERVAL_MS 1000  // clicks measured every second
#define WHEEL_MEASUREMENT_INTERVAL_MIN (WHEEL_MEASUREMENT_INTERVAL_MS / 60000.0)
#define PULSES_PER_ROTATION 60  // 60 pulses per rotation - if 1 pulse/click per sec will give 1 rpm
// Constants defined to meassure number of clicks in 1s as rpm

#include "embedded/digitalReceiver.hpp"
#include "model/systemData.hpp"

SystemData system_data;
auto digitalRecv = DigitalReceiver(&system_data.digital_data_, &system_data.mission_);

/**
 * @attention PROTOCOL TEST
 * @brief Test the Left Wheel Speed Sensor
 * Check the Left wheel Speed Sensor is being well calculated
 * PROTOCOL:
 * 1. Click Button 6 once
 * 2. Wait 1 second
 * 3. Verify LED1 Green Turning On (High) = 1 click (1 rpm with test settings)
 * 4. Click Button 6 twice
 * 5. Wait 1 second
 * 6. Verify LED1 & LED2 Green Turning On (High) = 2 click (2 rpm with test settings)
 * 7. Click Button 6 twice
 * 8. Wait 1 second
 * 9. Verify LED1 & LED2 & LED3 Green Turning On (High) = 3+ click (3+ rpm with test settings)
 * NOTE: do this inside 10 seconds
 * NOTE: moved to C1 Teensy
 */
// void test_lwss() {
//     Metro test{10000};
//     bool one_rpm = false, two_rpm = false, three_rpm = false, test_pass = false;
//     systemData.digitalData._left_wheel_rpm = 0;
//     while (!test.check() && !test_pass) {
//         digitalRecv.digitalReads();
//         if (systemData.digitalData._left_wheel_rpm > 2) {
//             three_rpm = true;
//             digitalWrite(GREEN_LED_3, HIGH);
//         } else {
//             digitalWrite(GREEN_LED_3, LOW);
//         }

//         if (systemData.digitalData._left_wheel_rpm > 1) {
//             two_rpm = true;
//             digitalWrite(GREEN_LED_2, HIGH);
//         } else {
//             digitalWrite(GREEN_LED_2, LOW);
//         }

//         if (systemData.digitalData._left_wheel_rpm > 0) {
//             one_rpm = true;
//             digitalWrite(GREEN_LED_1, HIGH);
//         } else {
//             digitalWrite(GREEN_LED_1, LOW);
//         }

//         test_pass = one_rpm && two_rpm && three_rpm;
//     }
//     TEST_ASSERT_TRUE(test_pass);
// }

void setUp() {
  pinMode(GREEN_LED_1, OUTPUT);
  pinMode(GREEN_LED_2, OUTPUT);
  pinMode(GREEN_LED_3, OUTPUT);
  pinMode(YELLOW_LED_1, OUTPUT);
  pinMode(YELLOW_LED_2, OUTPUT);
  pinMode(RED_LED_1, OUTPUT);
  pinMode(RED_LED_2, OUTPUT);
}

int main() {
  UNITY_BEGIN();
  // RUN_TEST(test_lwss);
  return UNITY_END();
}