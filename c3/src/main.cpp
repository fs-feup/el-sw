#include <Arduino.h>
#include <Bounce2.h>
#include <FlexCAN_T4.h>

#include "apps.h"
#include "can.h"
#include "debug.h"
#include "display.h"

#define buzzerPin 4 //! trocar para pino 2 no shield novo

#define R2D_PIN 32
#define R2D_TIMEOUT 500

#define APPS_1_PIN 41
#define APPS_2_PIN 40

#define STARTUP_DELAY_MS 10000

#define APPS_READ_PERIOD_MS 20
#define BAMOCAR_ATTENUATION_FACTOR 1

#define ASBuzzer 8000

int current_BMS = 0;
extern elapsedMillis ASEmergencyTimer;

volatile bool disabled = false;
volatile bool BTBReady = false;
volatile bool transmissionEnabled = false;

volatile bool TSOn = false;
volatile bool R2DOverride = false;

extern FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

extern CAN_message_t statusRequest;

extern CAN_message_t disable;

extern CAN_message_t DCVoltageRequest;
extern CAN_message_t actualSpeedRequest;

extern int speed;

uint8_t current_byte1; // MSB
uint8_t current_byte2; // LSB
CAN_message_t current_msg;

enum status
{
    IDLE,   // waiting for r2d && ATS off
    DRIVING // r2d pressed && ATS on
};

status R2DStatus;
Bounce r2dButton = Bounce();

elapsedMillis R2DTimer;
elapsedMillis APPSTimer;
elapsedMillis CURRENTtimer;
elapsedMicros mainLoopPeriod;

void sendMout(int value)
{
    uint8_t byte1 = (value >> 8) & 0xFF;
    uint8_t byte2 = value & 0xFF;

    CAN_message_t msg;
    msg.id = BAMO_COMMAND_ID;
    msg.len = 3;
    msg.buf[0] = 0xA0;
    msg.buf[1] = byte2;
    msg.buf[2] = byte1;
    can1.write(msg);
}

void playR2DSound()
{
    digitalWrite(buzzerPin, HIGH);
    delay(1000);
    digitalWrite(buzzerPin, LOW);
}

void checkASEmergencySound()
{
    if (ASEmergencyTimer < ASBuzzer)
        digitalWrite(buzzerPin, HIGH);
    else
        digitalWrite(buzzerPin, LOW);
}

void setup()
{
    Serial.begin(9600);
    pinMode(APPS_1_PIN, INPUT);
    pinMode(APPS_2_PIN, INPUT);

    pinMode(buzzerPin, OUTPUT);
    canSetup();

    r2dButton.attach(R2D_PIN, INPUT);
    r2dButton.interval(0.1);

    R2DStatus = IDLE;
    R2DTimer = 0;

    // Init the timer higher than the timeout (ASBuzzer)
    ASEmergencyTimer = 100000;

    delay(STARTUP_DELAY_MS);

    can1.write(disable);
    can1.write(statusRequest);
    can1.write(DCVoltageRequest);

#if DATA_DISPLAY > 0
    // can1.write(actualSpeedRequest);
    displaySetup();
#endif
#ifdef MAIN_DEBUG
    LOG("Setup complete, Waiting for R2D\n");
#endif
}

void loop()
{
    if (mainLoopPeriod < 10)
        return;

    switch (R2DStatus)
    {
    case IDLE:
        r2dButton.update();
        if ((r2dButton.fell() and TSOn and R2DTimer < R2D_TIMEOUT) or R2DOverride)
        {
            playR2DSound();
            initBamocarD3();
            request_dataLOG_messages();
            R2DStatus = DRIVING;
            break;
        }
        break;

    case DRIVING:
        if (not TSOn and not R2DOverride)
        {
            R2DStatus = IDLE;
            can1.write(disable);
            break;
        }

        if (APPSTimer > APPS_READ_PERIOD_MS)
        {
            APPSTimer = 0;
            int apps_value = readApps();

            if (apps_value >= 0)
                sendTorqueVal(apps_value);
            else
                sendTorqueVal(0);
            break;
        }

        break;
    default:
        ERROR("Invalid r2d_status");
        break;
    }
    checkASEmergencySound();
}