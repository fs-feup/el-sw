#ifndef _CAN_H_
#define _CAN_H_

#include <Arduino.h>

#define BMS_ID_CCL 0x101
#define BMS_ID_ERR 0x123
#define CH_ID 0x02207446
#define TA_ID 0x301

struct PARAMETERS {
    uint32_t setVoltage = 0;
    uint32_t currVoltage = 0;
    uint32_t allowedCurrent = 0;
    uint32_t setCurrent = 0;
    uint32_t currCurrent = 0;
    uint32_t ccl = 0;
    int16_t temp[60];
};

#endif  // _CAN_H_