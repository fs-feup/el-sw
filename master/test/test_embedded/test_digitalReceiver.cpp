#pragma once

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
#define PNEUMATIC_PIN BUTTON_3
#define WD_IN BUTTON_4
#define LWSS_PIN BUTTON_5

#define WHEEL_MEASUREMENT_INTERVAL_MS 1e3 // clicks measured every second 
#define WHEEL_MEASUREMENT_INTERVAL_MIN WHEEL_MEASUREMENT_INTERVAL_MS / 60000.0
#define PULSES_PER_ROTATION 1 // 1 click = 1 rpm

// #regular defines shouldn't override test ones
#include "../../include/embedded/digitalReceiver.hpp"
#include "../../include/logic/systemData.hpp"

// #undef WHEEL_MEASUREMENT_INTERVAL_MS
// #undef WHEEL_MEASUREMENT_INTERVAL_MIN
// #undef PULSES_PER_ROTATION
// #undef ASMS_SWITCH_PIN
// #undef AATS_SWITCH_PIN
// #undef PNEUMATIC_PIN
// #undef WD_IN
// #undef LWSS_PIN

// Constants defined to meassure number of clicks in 5s as rpm

SystemData systemData;
auto digitalRecv = DigitalReceiver(&systemData.digitalData, &systemData.mission);

void test_asms() {
    if (systemData.digitalData.asms_on){
        digitalWrite(RED_LED_1, HIGH);
        delay(10);
    }
    else{
        digitalWrite(RED_LED_1, LOW);
    }
}

void test_aats() {
    if (systemData.digitalData.aats_on){
        digitalWrite(RED_LED_2, HIGH);
        delay(10);
    }
    else{
        digitalWrite(RED_LED_2, LOW);
    }
}

void test_pneumatic() {
    if (systemData.digitalData.pneumatic_line_pressure){
        digitalWrite(YELLOW_LED_1, HIGH);
        delay(10);
    }
    else{
        digitalWrite(YELLOW_LED_1, LOW);
    }
}

void test_wd() {
    if (systemData.digitalData.watchdog_state){
        digitalWrite(YELLOW_LED_2, HIGH);
        delay(10);
    }
    else{
        digitalWrite(YELLOW_LED_2, LOW);
    }
}

void test_lwss() {
    // In next second:
    // 2 lights 2+ rpm-click
    // 1 light 1 rpm-clicks
    // 0 lights 0 rpm-clicks
    if (systemData.digitalData._left_wheel_rpm > 1){
        digitalWrite(GREEN_LED_1, HIGH);
        digitalWrite(GREEN_LED_2, HIGH);
    } else if (systemData.digitalData._left_wheel_rpm == 1) {
        digitalWrite(GREEN_LED_1, HIGH);
        digitalWrite(GREEN_LED_2, LOW);
    } else {
        digitalWrite(GREEN_LED_1, LOW);
        digitalWrite(GREEN_LED_2, LOW);
    }
}

/**
  * For Arduino framework
  */
void setup() {
    pinMode(GREEN_LED_1, OUTPUT);
    pinMode(GREEN_LED_2, OUTPUT);
    pinMode(GREEN_LED_3, OUTPUT);
    pinMode(RED_LED_1, OUTPUT);
    pinMode(RED_LED_2, OUTPUT);
    pinMode(YELLOW_LED_1, OUTPUT);
    pinMode(YELLOW_LED_2, OUTPUT);
}
void loop() {
    digitalRecv.digitalReads();
    test_asms();
    test_aats();
    test_pneumatic();
    test_pneumatic();
    test_lwss();
}

