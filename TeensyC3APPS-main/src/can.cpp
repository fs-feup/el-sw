#include "can.h"
#include "debug.h"

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

CAN_message_t disable;
CAN_message_t BTBStatus;
CAN_message_t noDisable;
CAN_message_t clearErrors;
CAN_message_t BTBResponse;
CAN_message_t statusRequest;
CAN_message_t torqueRequest;
CAN_message_t enableResponse;
CAN_message_t DCVoltageRequest;
CAN_message_t actualSpeedRequest;
CAN_message_t transmissionRequestEnable;

CAN_message_t rpmRequest;
CAN_message_t speedRequest;
CAN_message_t currentMOTOR;
CAN_message_t tempMOTOR;
CAN_message_t tempBAMO;

int Ibat;
int Vbat;
int Mout;
int Nact;
int Vout;
int Tpeak;
int Iq_cmd;
int Imax_peak;
int I_con_eff;
int Iq_actual;
int I_lim_inuse;
int I_actual_filtered;

extern volatile bool BTBReady;
extern volatile bool transmissionEnabled;
extern volatile bool disabled;
extern volatile bool TSOn;
extern volatile bool R2DOverride;

extern int highTemp;
extern int soc;
extern int current;
extern int speedInt;
extern int packVoltage;
extern int lowTemp;

extern int current_BMS;

extern int powerStageTemp;
extern int motorTemp;

extern int rpm;
extern int ACCurrent;

extern volatile uint16_t brakeValue;

extern elapsedMillis R2DTimer;

elapsedMillis CANTimer;
const int CANTimeoutMS = 100;

#define DC_THRESHOLD 4328  // Threshold for DC voltage to be considered present for R2D

// Initialize CAN messages
/**
 * @brief Initialize CAN messages
 *
 */
void initCanMessages() {
    // APPS Message
    torqueRequest.id = BAMO_COMMAND_ID;
    torqueRequest.len = 3;
    torqueRequest.buf[0] = 0x90;

    enableResponse.id = BAMO_RESPONSE_ID;
    enableResponse.len = 4;
    enableResponse.buf[0] = 0xE8;
    enableResponse.buf[1] = 0x01;
    enableResponse.buf[2] = 0x00;
    enableResponse.buf[3] = 0x00;

    clearErrors.id = BAMO_COMMAND_ID;
    clearErrors.len = 3;
    clearErrors.buf[0] = 0x8E;
    clearErrors.buf[1] = 0x44;
    clearErrors.buf[2] = 0x4D;

    noDisable.id = BAMO_COMMAND_ID;
    noDisable.len = 3;
    noDisable.buf[0] = 0x51;
    noDisable.buf[1] = 0x00;
    noDisable.buf[2] = 0x00;

    BTBStatus.id = BAMO_COMMAND_ID;
    BTBStatus.len = 3;
    BTBStatus.buf[0] = 0x3D;
    BTBStatus.buf[1] = 0xE2;
    BTBStatus.buf[2] = 0x00;

    BTBResponse.id = BAMO_RESPONSE_ID;
    BTBResponse.len = 4;
    BTBResponse.buf[0] = 0xE2;
    BTBResponse.buf[1] = 0x01;
    BTBResponse.buf[2] = 0x00;
    BTBResponse.buf[3] = 0x00;

    transmissionRequestEnable.id = BAMO_COMMAND_ID;
    transmissionRequestEnable.len = 3;
    transmissionRequestEnable.buf[0] = 0x3D;
    transmissionRequestEnable.buf[1] = 0xE8;
    transmissionRequestEnable.buf[2] = 0x00;

    statusRequest.id = BAMO_COMMAND_ID;
    statusRequest.len = 3;
    statusRequest.buf[0] = 0x3D;
    statusRequest.buf[1] = 0x40;
    statusRequest.buf[2] = 0x00;

    disable.id = BAMO_COMMAND_ID;
    disable.len = 3;
    disable.buf[0] = 0x51;
    disable.buf[1] = 0x04;
    disable.buf[2] = 0x00;

    DCVoltageRequest.id = BAMO_COMMAND_ID;
    DCVoltageRequest.len = 3;
    DCVoltageRequest.buf[0] = 0x3D;
    DCVoltageRequest.buf[1] = 0xEB;
    DCVoltageRequest.buf[2] = 0x64;
}

void request_dataLOG_messages() {
    rpmRequest.id = BAMO_COMMAND_ID;
    rpmRequest.len = 3;
    rpmRequest.buf[0] = 0x3D;
    rpmRequest.buf[1] = 0xCE;
    rpmRequest.buf[2] = 0x0A;
    can1.write(rpmRequest);

    speedRequest.id = BAMO_COMMAND_ID;
    speedRequest.len = 3;
    speedRequest.buf[0] = 0x3D;
    speedRequest.buf[1] = 0x30;
    speedRequest.buf[2] = 0x0A;
    can1.write(speedRequest);
    
    currentMOTOR.id = BAMO_COMMAND_ID;
    currentMOTOR.len = 3;
    currentMOTOR.buf[0] = 0x3D;
    currentMOTOR.buf[1] = 0x5f;
    currentMOTOR.buf[2] = 0x0A;
    can1.write(currentMOTOR);
    
    tempMOTOR.id = BAMO_COMMAND_ID;
    tempMOTOR.len = 3;
    tempMOTOR.buf[0] = 0x3D;
    tempMOTOR.buf[1] = 0x49;
    tempMOTOR.buf[2] = 0x0A;
    can1.write(tempMOTOR);
    
    tempBAMO.id = BAMO_COMMAND_ID;
    tempBAMO.len = 3;
    tempBAMO.buf[0] = 0x3D;
    tempBAMO.buf[1] = 0x4A;
    tempBAMO.buf[2] = 0x0A;
    can1.write(tempBAMO);
}

void sendTorqueVal(int value_bamo) {
    uint8_t byte1 = (value_bamo >> 8) & 0xFF;  // MSB
    uint8_t byte2 = value_bamo & 0xFF;         // LSB

    torqueRequest.buf[1] = byte2;
    torqueRequest.buf[2] = byte1;

    can1.write(torqueRequest);
}
void sendAPPS(int val1, int val2) {
    uint8_t byte1 = (val1 >> 8) & 0xFF;  // MSB
    uint8_t byte2 = val1 & 0xFF;         // LSB
    uint8_t byte3 = (val2 >> 8) & 0xFF;  // MSB
    uint8_t byte4 = val2 & 0xFF;         // LSB

    CAN_message_t msg;
    msg.id = 0x111;
    msg.len = 4;
    msg.buf[0] = byte2;
    msg.buf[1] = byte1;
    msg.buf[2] = byte4;
    msg.buf[3] = byte3;

    can1.write(msg);
}

void initBamocarD3() {
    can1.write(clearErrors);

    while (not transmissionEnabled and CANTimer > CANTimeoutMS) {
        can1.write(transmissionRequestEnable);
        CANTimer = 0;
    }

    while (not BTBReady and CANTimer > CANTimeoutMS) {
        can1.write(BTBStatus);
        CANTimer = 0;
    }

    can1.write(noDisable);
}

void REGIDHandler(const CAN_message_t& msg) {
    switch (msg.buf[0]) {
        /*
        case REGID_VOUT:
            Vout = (msg.buf[2] << 8) | msg.buf[1];
            break;

        case REGID_ACTUAL_IQ:
            Iq_actual = (msg.buf[2] << 8) | msg.buf[1];
            break;

        case REGID_CMD_IQ:
            Iq_cmd = (msg.buf[2] << 8) | msg.buf[1];
            break;

        case REGID_MOUT:
            Mout = (msg.buf[2] << 8) | msg.buf[1];
            break;

        case REGID_I_LIM_INUSE:
            I_lim_inuse = (msg.buf[2] << 8) | msg.buf[1];
            break;

        case REGID_I_ACT_FILTERED:
            I_actual_filtered = (msg.buf[2] << 8) | msg.buf[1];
            break;

        case REGID_T_PEAK:
            Tpeak = (msg.buf[2] << 8) | msg.buf[1];
            break;

        case REGID_I_MAX_PEAK:
            Imax_peak = (msg.buf[2] << 8) | msg.buf[1];
            break;

        case REGID_I_CON_EFF:
            I_con_eff = (msg.buf[2] << 8) | msg.buf[1];
            break;

        case REGID_ACTUAL_SPEED: {
            double speed = 0;
            speed = (msg.buf[2] << 8) | msg.buf[1];
            if (speed < 0)
                speed *= -1;
            rpm = speed;
            rpm = (rpm * 6500) / 32760;
            speed = (speed / 5.04) * 0.02394;
            speedInt = (int)speed;
            break;
        }
        */

        case REGID_DC_VOLTAGE: {
            long dc_voltage = 0;
            dc_voltage = (msg.buf[2] << 8) | msg.buf[1];
#ifdef CAN_DEBUG
            LOG("DC Voltage: %d\n", dc_voltage);
#endif
            TSOn = (dc_voltage >= DC_THRESHOLD);
            break;
        }

        default:
            break;
    }
}

void canSniffer(const CAN_message_t& msg) {
#ifdef CAN_DEBUG
    LOG("CAN message received\n");
    INFO("Message ID: %x\n", msg.id);
    INFO("Message length: %d\n", msg.len);
    INFO("Message data: ");
    for (int i = 0; i < msg.len; i++)
        Serial.printf("%x ", msg.buf[i]);
    Serial.println();
#endif  // CAN_DEBUG

    switch (msg.id) {
        case 0x666:
            current_BMS = ((msg.buf[1] << 8) | msg.buf[0]);
            //Serial.printf("Message data received: ");
            //for (int i = 0; i < 2; i++)
                //Serial.printf("%x ", msg.buf[i]);
            //Serial.println();
            break;

        case C3_ID:
#ifdef R2D_DEBUG
            INFO("Braking signal received\n");
#endif  // R2D_DEBUG
            brakeValue = (msg.buf[2] << 8) | msg.buf[1];
            if (brakeValue > 165)
                R2DTimer = 0;
            break;

        case R2D_ID:
            R2DOverride = true;
            break;

        case BAMO_RESPONSE_ID:
            if (msg.len == 4) {
                BTBReady = (msg.buf[0] == BTBResponse.buf[0] and msg.buf[1] == BTBResponse.buf[1] and msg.buf[2] == BTBResponse.buf[2] and msg.buf[3] == BTBResponse.buf[3]);
                if (BTBReady)
                    Serial.println("BTB ready");
                else
                    REGIDHandler(msg);
                break;
            }
            if (msg.len == 3) {
                transmissionEnabled = (msg.buf[0] == enableResponse.buf[0] and msg.buf[1] == enableResponse.buf[1] and msg.buf[2] == enableResponse.buf[2]);
                if (transmissionEnabled)
                    Serial.println("Transmission enabled");
                break;
            }
            break;

        default:
            break;
    }
}

void canSetup() {
    can1.begin();
    can1.setBaudRate(500000);
    can1.enableFIFO();
    can1.enableFIFOInterrupt();
    can1.setFIFOFilter(REJECT_ALL);
    can1.setFIFOFilter(0, C3_ID, STD);
    can1.setFIFOFilter(1, R2D_ID, STD);
    can1.setFIFOFilter(2, BMS_ID, STD);
    can1.setFIFOFilter(3, BAMO_RESPONSE_ID, STD);
    can1.onReceive(canSniffer);

    initCanMessages();
}