#pragma once

#define WHEEL_MEASUREMENT_INTERVAL_MS 30.0 // 30ms // TODO(andre): change to adequate value
#define WHEEL_MEASUREMENT_INTERVAL_MIN WHEEL_MEASUREMENT_INTERVAL_MS / 60000.0
#define PULSES_PER_ROTATION 50 // TODO(andre): change to true value

#define WD_PULSE_INTERVAL_MS 10 // TODO(andre): change to adequate value
#define WD_WAIT_INTERVAL_MS 100 // TODO(andre): change to adequate value

#define LWSS_PIN A9 // TODO(andre): change pin

// Missions TODO(andre): confirm order
#define MISSION_MANUAL_PIN A1
#define MISSION_ACCELERATION_PIN A2
#define MISSION_SKIDPAD_PIN A3
#define MISSION_AUTOCROSS_PIN A4
#define MISSION_TRACKDRIVE_PIN A5
#define MISSION_EBSTEST_PIN A6
#define MISSION_INSPECTION_PIN A7

// Switches TODO(andre): change to real
#define ASMS_SWITCH_PIN A11
#define AATS_SWITCH_PIN A12

#define PNEUMATIC_PIN A13 // TODO(andre): change to real

// Watchdog TODO(andre): change to real
#define WD_OUT A13 // d1 not working
#define WD_IN A8