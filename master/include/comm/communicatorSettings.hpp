#pragma once

// IDS
constexpr auto MASTER_ID = 0x300;
constexpr auto BMS_ID = 0x666;
constexpr auto BAMO_RESPONSE_ID = 0x181;
constexpr auto C1_ID = 0x123;
constexpr auto C3_ID = 0x111;
constexpr auto AS_CU_ID = 0x400;
constexpr auto STEERING_ID = 0x295d;

// PC
constexpr auto AS_CU_EMERGENCY_SIGNAL = 0x43;
constexpr auto MISSION_FINISHED = 0x42;
constexpr auto PC_ALIVE = 0x41;

// Sensors
constexpr auto RIGHT_WHEEL_CODE = 0x11;
constexpr auto LEFT_WHEEL_CODE = 0x12;
constexpr auto HYDRAULIC_LINE = 0x90;
constexpr auto HYDRAULIC_BRAKE_THRESHOLD = 165;

constexpr auto WHEEL_PRECISION = 1e-2;

// Logging Status
constexpr auto DRIVING_STATE = 0x500;
constexpr auto DRIVING_CONTROL = 0x501;
constexpr auto SYSTEM_STATUS = 0x502;

// RES
constexpr auto NODE_ID = 0x011;  // Competition Defined
constexpr auto RES_STATE = (0x180 + NODE_ID);
constexpr auto RES_READY = (0x700 + NODE_ID);
constexpr auto RES_ACTIVATE = 0x000;

// Master State
constexpr auto STATE_MSG = 0x31;
constexpr auto MISSION_MSG = 0x32;
constexpr auto LEFT_WHEEL_MSG = 0x33;
constexpr auto DBG_LOG_MSG = 0x34;
constexpr auto DBG_LOG_MSG_2 = 0x35;

// Bamocar
constexpr auto BTB_READY = 0xE2;
constexpr auto VDC_BUS = 0xEB;
constexpr auto DC_THRESHOLD = 1890;  // same as 60 volts