#pragma once

// IDS
#define MASTER_ID 0x300

#define BMS_ID 0x666
#define BAMO_RESPONSE_ID 0x181
#define C1_ID 0x123
#define C3_ID 0x111
#define PC_ID 0x400
#define STEERING_ID 0x700 // TODO(andre): change or confirm code

// PC
#define AS_CU_EMERGENCY_SIGNAL 0x43 // TODO(andre): change or confirm code
#define MISSION_FINISHED 0x42       // TODO(andre): change or confirm code
#define PC_ALIVE 0x41               // TODO(andre): change or confirm code

// Sensors
#define RIGHT_WHEEL 0x11    // TODO(andre): change or confirm code
#define HYDRAULIC_LINE 0x12 // TODO(andre): change or confirm code

#define WHEEL_PRECISION 1e-2          // TODO(andre): change or confirm value
#define HYDRAULIC_LINE_PRECISION 1e-1 // TODO(andre): change or confirm value

// Logging Status
#define DRIVING_STATE 0x500
#define DRIVING_CONTROL 0x501
#define SYSTEM_STATUS 0x502

// RES
#define NODE_ID 0x011 // Competition Defined
#define RES (0x180 + NODE_ID)

// Master State
#define STATE_MSG 0x31
#define MISSION_MSG 0x32
#define LEFT_WHEEL_MSG 0x33

// Bamocar
#define BTB_READY 0xE2
#define VDC_BUS 0xEB
