#pragma once

#define COMPONENT_TIMESTAMP_TIMEOUT 500
#define WATCHDOG_TOGGLE_TIMEOUT 100
#define EBS_BUZZER_TIMEOUT 8000
#define LED_BLINK_INTERVAL 500

#define WHEEL_MEASUREMENT_INTERVAL_MS 30 // 30ms // TODO(andre): change to adequate value
#define WHEEL_MEASUREMENT_INTERVAL_MIN (WHEEL_MEASUREMENT_INTERVAL_MS / 60000.0)
#define PULSES_PER_ROTATION 50 // TODO(andre): change to true value

#define WD_PULSE_INTERVAL_MS 10 // TODO(andre): change to adequate value
#define WD_WAIT_INTERVAL_MS 100 // TODO(andre): change to adequate value

/*
 * ===========
 * OUTPUT PINS
 * ===========
 */

#define ASSI_DRIVING_PIN 4
#define ASSI_READY_PIN 5
#define ASSI_FINISH_PIN 6
#define ASSI_EMERGENCY_PIN 7

#define SDC_LOGIC_CLOSE_SDC_PIN 8
#define SDC_LOGIC_WATCHDOG_OUT_PIN 10
#define EBS_VALVE_1_PIN 29
#define EBS_VALVE_2_PIN 30
#define MASTER_SDC_OUT_PIN 31

/*
 * ==========
 * INPUT PINS
 * ==========
 */

#define LWSS_PIN 32

//When pressure on the line passes the threshold defined, pin is set to HIGH
#define SENSOR_PRESSURE_2_PIN 22
#define SENSOR_PRESSURE_1_PIN 23

// Mission pins

#define MISSION_MANUAL_PIN 39
#define MISSION_ACCELERATION_PIN 41
#define MISSION_SKIDPAD_PIN 14
#define MISSION_AUTOCROSS_PIN 16
#define MISSION_TRACKDRIVE_PIN 18
#define MISSION_EBSTEST_PIN 20
#define MISSION_INSPECTION_PIN 21


//TODO: PROPERLY REPLACE HERE
#define AATS_SWITCH_PIN 0

#define SDC_LOGIC_WATCHDOG_IN_PIN 35
#define ASMS_IN_PIN 33
