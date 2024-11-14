#pragma once

constexpr int COMPONENT_TIMESTAMP_TIMEOUT = 500;
constexpr int RES_TIMESTAMP_TIMEOUT = 200;
constexpr int DC_VOLTAGE_TIMEOUT = 150;
constexpr int DC_VOLTAGE_HOLD = 1000;
constexpr int EBS_BUZZER_TIMEOUT = 8000;
constexpr int LED_BLINK_INTERVAL = 500;
constexpr int INITIAL_CHECKUP_STEP_TIMEOUT = 500;
constexpr unsigned long READY_TIMEOUT_MS = 5000;
constexpr unsigned long RELEASE_EBS_TIMEOUT_MS = 1000;
constexpr unsigned long ENGAGE_EBS_TIMEOUT_MS = 5000;
// #define WATCHDOG_TOGGLE_FREQUENCY 100
// #define WATCHDOG_TIMEOUT 500

constexpr int WHEEL_MEASUREMENT_INTERVAL_MS = 30;  // 30ms // TODO(andre): change to adequate value
constexpr int WHEEL_MEASUREMENT_INTERVAL_MIN = (WHEEL_MEASUREMENT_INTERVAL_MS / 60000.0);
constexpr int PULSES_PER_ROTATION = 48;

// Number of consecutive different values of a digital input to consider change
// (to avoid noise)
constexpr int DIGITAL_INPUT_COUNTER_LIMIT = 5;
constexpr int WD_PULSE_INTERVAL_MS = 10;
// TODO(andre): confirm wd timer values

/*
 * ===========
 * OUTPUT PINS
 * ===========
 */

constexpr int ASSI_BLUE_PIN = 4;
constexpr int ASSI_YELLOW_PIN = 7;

constexpr int SDC_LOGIC_CLOSE_SDC_PIN = 8;
// #define SDC_LOGIC_WATCHDOG_OUT_PIN 10
constexpr int EBS_VALVE_1_PIN = 25;
constexpr int EBS_VALVE_2_PIN = 30;
constexpr int MASTER_SDC_OUT_PIN = 31;

/*
 * ==========
 * INPUT PINS
 * ==========
 */

constexpr int LWSS_PIN = 32;

// When pressure on the line passes the threshold defined, pin is set to HIGH
constexpr int SENSOR_PRESSURE_2_PIN = 6;
constexpr int SENSOR_PRESSURE_1_PIN = 5;

// Mission pins

constexpr int MISSION_MANUAL_PIN = 39;
constexpr int MISSION_AUTOCROSS_PIN = 41;
constexpr int MISSION_TRACKDRIVE_PIN = 14;
constexpr int MISSION_SKIDPAD_PIN = 16;
constexpr int MISSION_INSPECTION_PIN = 18;
constexpr int MISSION_ACCELERATION_PIN = 20;
constexpr int MISSION_EBSTEST_PIN = 21;

constexpr int SDC_STATE_PIN = 37;
// #define SDC_LOGIC_WATCHDOG_IN_PIN 35
constexpr int ASMS_IN_PIN = 33;
