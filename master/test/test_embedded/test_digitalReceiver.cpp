#include "../../include/embedded/digitalData.hpp"

#undef WHEEL_MEASUREMENT_INTERVAL_MS
#undef WHEEL_MEASUREMENT_INTERVAL_MIN
#undef PULSES_PER_ROTATION
#undef ASMS_SWITCH_PIN
#undef AATS_SWITCH_PIN
#undef PNEUMATIC_PIN
#undef WD_IN
#undef LWSS_PIN

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

#define ASMS_SWITCH_PIN BUTTON_1
#define AATS_SWITCH_PIN BUTTON_2
#define WD_IN BUTTON_3
#define PNEUMATIC_PIN BUTTON_4
#define LWSS_PIN BUTTON_6

#define WHEEL_MEASUREMENT_INTERVAL_MS 1000 // clicks measured every second 
#define WHEEL_MEASUREMENT_INTERVAL_MIN (WHEEL_MEASUREMENT_INTERVAL_MS / 60000.0)
#define PULSES_PER_ROTATION 60 // 60 pulses per rotation - if 1 pulse/click per sec will give 1 rpm
// Constants defined to meassure number of clicks in 1s as rpm

#include "../../include/embedded/digitalReceiver.hpp"
#include "../../include/logic/systemData.hpp"


SystemData systemData;
auto digitalRecv = DigitalReceiver(&systemData.digitalData, &systemData.mission);

/**
 * @attention PROTOCOL TEST
 * @brief Test the ASMS Switch
 * Check the ASMS Switch High Low Read 
 * CAN communication in desktop
 * PROTOCOL:
 * 1. Click Button 1
 * 2. Verify LED1 Red Turning Off (Low)
*/
void test_asms() {
    if (systemData.digitalData.asms_on){
        digitalWrite(RED_LED_1, HIGH);
    }
    else{
        digitalWrite(RED_LED_1, LOW);
    }
}

/**
 * @attention PROTOCOL TEST
 * @brief Test the AATS Switch
 * Check the AATS Switch High Low Read 
 * CAN communication in desktop
 * PROTOCOL:
 * 1. Click Button 2
 * 2. Verify LED2 Red Turning Off (Low)
*/
void test_aats() {
    if (systemData.digitalData.aats_on){
        digitalWrite(RED_LED_2, HIGH);
    }
    else{
        digitalWrite(RED_LED_2, LOW);
    }
}

/**
 * @attention PROTOCOL TEST
 * @brief Test the Watchdog Status Input
 * Check Whether watchdog pin is High or LOw
 * PROTOCOL:
 * 1. Click Button 3
 * 2. Verify LED1 Turning Off (Low)
*/
void test_wd() {
    if (systemData.digitalData.watchdog_state){
        digitalWrite(YELLOW_LED_1, HIGH);
        delay(10);
    }
    else{
        digitalWrite(YELLOW_LED_1, LOW);
    }
}

/**
 * @attention PROTOCOL TEST
 * @brief Test the Pneumatic Brake Pressure
 * Check the Brake Pressure pins are both High
 * PROTOCOL:
 * 1. Click Button 4 or 5
 * 2. Verify LED2 Yellow Turning Off (One or Both Pins are Low)
*/
void test_pneumatic() {
    if (systemData.digitalData.pneumatic_line_pressure){
        digitalWrite(YELLOW_LED_2, HIGH);
        delay(10);
    }
    else{
        digitalWrite(YELLOW_LED_2, LOW);
    }
}

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
*/
void test_lwss() {
    if (systemData.digitalData._left_wheel_rpm > 2)
        digitalWrite(GREEN_LED_3, HIGH);
    else 
        digitalWrite(GREEN_LED_3, LOW);

    if (systemData.digitalData._left_wheel_rpm > 1)
        digitalWrite(GREEN_LED_2, HIGH);
    else 
        digitalWrite(GREEN_LED_2, LOW);

    if (systemData.digitalData._left_wheel_rpm > 0)
        digitalWrite(GREEN_LED_1, HIGH);
    else
        digitalWrite(GREEN_LED_1, LOW);
}

/**
  * For Arduino framework
  */
void setup() {
    pinMode(GREEN_LED_1, OUTPUT);
    pinMode(GREEN_LED_2, OUTPUT);
    pinMode(GREEN_LED_3, OUTPUT);
    pinMode(YELLOW_LED_1, OUTPUT);
    pinMode(YELLOW_LED_2, OUTPUT);
    pinMode(RED_LED_1, OUTPUT);
    pinMode(RED_LED_2, OUTPUT);
}
void loop() {
    digitalRecv.digitalReads();
    test_asms();
    test_aats();
    test_wd();
    test_pneumatic();
    test_lwss();
}

