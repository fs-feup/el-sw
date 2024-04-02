#pragma once

// IDS
constexpr auto MASTER_ID = 0x300;

constexpr auto BMS_ID = 0x666;
constexpr auto BAMO_RESPONSE_ID = 0x181;
constexpr auto C1_ID = 0x123;
constexpr auto C3_ID = 0x111;
constexpr auto PC_ID = 0x400;
constexpr auto STEERING_ID = 0x700; // TODO(andre): change or confirm code

// PC
constexpr auto AS_CU_EMERGENCY_SIGNAL = 0x43; // TODO(andre): change or confirm code
constexpr auto MISSION_FINISHED = 0x42;       // TODO(andre): change or confirm code
constexpr auto PC_ALIVE = 0x41;               // TODO(andre): change or confirm code

// Sensors
constexpr auto RIGHT_WHEEL = 0x11;    // TODO(andre): change or confirm code
constexpr auto HYDRAULIC_LINE = 0x12; // TODO(andre): change or confirm code

constexpr auto WHEEL_PRECISION = 1e-2;          // TODO(andre): change or confirm value
constexpr auto HYDRAULIC_LINE_PRECISION = 1e-1; // TODO(andre): change or confirm value

// Logging Status
constexpr auto DRIVING_STATE = 0x500;
constexpr auto DRIVING_CONTROL = 0x501;
constexpr auto SYSTEM_STATUS = 0x502;

// RES
constexpr auto NODE_ID = 0x011; // Competition Defined
constexpr auto RES_STATE = (0x180 + NODE_ID);
constexpr auto RES_READY = (0x700 + NODE_ID);
constexpr auto RES_ACTIVATE = 0x000;

// Master State
constexpr auto STATE_MSG = 0x31;
constexpr auto MISSION_MSG = 0x32;
constexpr auto LEFT_WHEEL_MSG = 0x33;

// Bamocar
constexpr auto BTB_READY = 0xE2;
constexpr auto VDC_BUS = 0xEB;
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

#define WHEEL_MEASUREMENT_INTERVAL_MS 1000 // clicks measured every second 
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

