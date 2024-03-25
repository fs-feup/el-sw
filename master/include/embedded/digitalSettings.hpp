#pragma once

constexpr auto WHEEL_MEASUREMENT_INTERVAL_MS = 30.0; // 30ms // TODO(andre): change to adequate value
constexpr auto WHEEL_MEASUREMENT_INTERVAL_MIN = WHEEL_MEASUREMENT_INTERVAL_MS / 60000.0;
constexpr auto PULSES_PER_ROTATION = 50; // TODO(andre): change to true value

constexpr auto WD_PULSE_INTERVAL_MS = 10; // TODO(andre): change to adequate value
constexpr auto WD_WAIT_INTERVAL_MS = 100; // TODO(andre): change to adequate value

constexpr auto LWSS_PIN = A9; // TODO(andre): change pin

// Missions TODO(andre): confirm order
constexpr auto MISSION_MANUAL_PIN = A1;
constexpr auto MISSION_ACCELERATION_PIN = A2;
constexpr auto MISSION_SKIDPAD_PIN = A3;
constexpr auto MISSION_AUTOCROSS_PIN = A4;
constexpr auto MISSION_TRACKDRIVE_PIN = A5;
constexpr auto MISSION_EBSTEST_PIN = A6;
constexpr auto MISSION_INSPECTION_PIN = A7;

// Switches TODO(andre): change to real
constexpr auto ASMS_SWITCH_PIN = A11;
constexpr auto AATS_SWITCH_PIN = A12;

#define PNEUMATIC_PIN A13 // TODO(andre): change to real

// Watchdog TODO(andre): change to real
constexpr auto WD_OUT = A13; // d1 not working
constexpr auto WD_IN = A14;