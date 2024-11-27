#include <Arduino.h>
#include <Bounce2.h>
#include <FlexCAN_T4.h>

#include "apps.h"
#include "can.h"
#include "debug.h"
#include "display.h"
#include "statemachine.hpp"


int current_BMS = 0;
extern elapsedMillis ASEmergencyTimer;

volatile bool disabled = false;
volatile bool BTBReady = false;
volatile bool transmissionEnabled = false;

volatile bool TSOn = false;
volatile bool R2DOverride = false;

volatile bool ASReady = false; // true if ASState = ASReady

extern FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

extern CAN_message_t statusRequest;

extern CAN_message_t disable;

extern CAN_message_t DCVoltageRequest;
extern CAN_message_t actualSpeedRequest;

extern int speed;

extern status R2DStatus;

uint8_t current_byte1; // MSB
uint8_t current_byte2; // LSB
CAN_message_t current_msg;

extern status R2DStatus;
Bounce r2dButton = Bounce();

elapsedMillis R2DTimer;
extern elapsedMillis APPSTimer;
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

void checkASEmergencySound()
{
    // if (ASEmergencyTimer < ASBuzzer)
    //     digitalWrite(buzzerPin, HIGH);
    // else
    //     digitalWrite(buzzerPin, LOW);
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

    //Uncomment this to run the integration test on the Testing Board
    //testing_setup();
}

void loop()
{
    if (mainLoopPeriod < 10)
        return;
    statemachine();
    checkASEmergencySound();
    //Uncomment this to run the integration test on the Testing Board
    //integrationtest(R2DStatus);
}