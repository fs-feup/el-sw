#ifndef _CAN_H_
#define _CAN_H_

#include <FlexCAN_T4.h>
#include <elapsedMillis.h>

#define C3_ID 0x123
#define R2D_ID 0x665
#define BMS_ID 0x675
#define MASTER_ID 0x300

#define BAMO_COMMAND_ID 0x201
#define BAMO_RESPONSE_ID 0x181

#define REGID_MOUT 0xA0
#define REGID_IGBT 0x4A
#define REGID_NACT 0xA8
#define REGID_VOUT 0x8A
#define REGID_T_PEAK 0xF0
#define REGID_CMD_IQ 0x26
#define REGID_I_CON_EFF 0xC5
#define REGID_ACTUAL_IQ 0x27
#define REGID_I_MAX_PEAK 0xC4
#define REGID_DC_VOLTAGE 0xEB
#define REGID_AC_CURRENT 0x20
#define REGID_MOTOR_TEMP 0x49
#define REGID_I_LIM_INUSE 0x48
#define REGID_ACTUAL_SPEED 0x30
#define REGID_I_ACT_FILTERED 0x5F

#define MAX_I 250
#define ADC_MAX 65536


void canSetup();
void sendTorqueVal(int value_bamo);
void initBamocarD3();
void request_dataLOG_messages();
void sendAPPS(int val1, int val2);
#endif