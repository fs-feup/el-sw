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
constexpr auto HYDRAULIC_LINE = 0x90; // TODO(andre): change or confirm code
constexpr auto HYDRAULIC_BRAKE_THRESHOLD = 165; // 0 to 1024, 165 considered braking

constexpr auto WHEEL_PRECISION = 1e-2;          // TODO(andre): change or confirm value

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
constexpr auto DC_THRESHOLD = 4328; // same as 60 volts