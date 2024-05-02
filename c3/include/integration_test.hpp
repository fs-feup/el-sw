#include "Arduino.h"

#define GREEN_LED_1 4
#define GREEN_LED_2 5
#define GREEN_LED_3 6
#define RED_LED_1 0
#define RED_LED_2 1
#define YELLOW_LED_1 2
#define YELLOW_LED_2 3
#define BUTTON_1 7
#define BUTTON_2 8
#define BUTTON_3 9
#define BUTTON_4 10
#define BUTTON_5 11
#define BUTTON_6 12
#define POTENCIO 14

#undef R2D_PIN

#define BK_SENSOR_PIN POTENCIO
#define R2D_PIN BUTTON_1
#define TSON_PIN BUTTON_2
#define AS_READY_PIN BUTTON_3
#define STOP_PIN BUTTON_4
#define STATE_IDLE_GREEN_LED_1_PIN GREEN_LED_1
#define STATE_IDLE_GREEN_LED_2_PIN GREEN_LED_2
#define STATE_IDLE_GREEN_LED_3_PIN GREEN_LED_3
#define STATE_DRIVING_RED_LED_1_PIN RED_LED_1
#define STATE_DRIVING_RED_LED_2_PIN RED_LED_2
#define STATE_ASDRIVING_YELLOW_LED_1_PIN YELLOW_LED_1
#define STATE_ASDRIVING_YELLOW_LED_2_PIN YELLOW_LED_2

extern volatile bool TSOn;
extern volatile bool ASReady;
extern volatile uint16_t brakeValue;

elapsedMillis print_timer;

/**
 * @brief Integration test
 *
 * @details
 * This should be used as integration testing together with the testing board.
 * It simulates the behavior of:
 * - Brake sensor
 * - TSOn
 * - ASReady
 * - R2D
 * It can be use to test the state machine logic.
 * The LEDs represent the current state:
 * - Green: Idle
 * - Red: Driving
 * - Yellow: ASDriving
 * The serial monitor shows the current values of the variables.
 */

void brakeValueUpdate()
{
    uint16_t brakeValue = analogRead(BK_SENSOR_PIN);
    if (brakeValue > 165)
        R2DTimer = 0;
}

void updateTS()
{
    TSOn = not digitalRead(TSON_PIN);
}

void updateASReady()
{
    ASReady = not digitalRead(AS_READY_PIN);
}

void testing_setup()
{
    pinMode(BK_SENSOR_PIN, INPUT);
    pinMode(R2D_PIN, INPUT);
    pinMode(TSON_PIN, INPUT);
    pinMode(AS_READY_PIN, INPUT);
    pinMode(STATE_IDLE_GREEN_LED_1_PIN, OUTPUT);
    pinMode(STATE_IDLE_GREEN_LED_2_PIN, OUTPUT);
    pinMode(STATE_IDLE_GREEN_LED_3_PIN, OUTPUT);
    pinMode(STATE_DRIVING_RED_LED_1_PIN, OUTPUT);
    pinMode(STATE_DRIVING_RED_LED_2_PIN, OUTPUT);
    pinMode(STATE_ASDRIVING_YELLOW_LED_1_PIN, OUTPUT);
    pinMode(STATE_ASDRIVING_YELLOW_LED_2_PIN, OUTPUT);

    // attachInterrupt(digitalPinToInterrupt(BK_SENSOR_PIN),brakeValueUpdate,CHANGE);
    attachInterrupt(digitalPinToInterrupt(TSON_PIN), updateTS, CHANGE);
    attachInterrupt(digitalPinToInterrupt(AS_READY_PIN), updateASReady, CHANGE);

    print_timer = 0;
}

void state_light(int state)
{
    switch (state)
    {
    case 0: // idle
        digitalWrite(STATE_IDLE_GREEN_LED_1_PIN, HIGH);
        digitalWrite(STATE_IDLE_GREEN_LED_2_PIN, HIGH);
        digitalWrite(STATE_IDLE_GREEN_LED_3_PIN, HIGH);
        digitalWrite(STATE_DRIVING_RED_LED_1_PIN, LOW);
        digitalWrite(STATE_DRIVING_RED_LED_2_PIN, LOW);
        digitalWrite(STATE_ASDRIVING_YELLOW_LED_1_PIN, LOW);
        digitalWrite(STATE_ASDRIVING_YELLOW_LED_2_PIN, LOW);
        break;
    case 1: // Driving
        digitalWrite(STATE_IDLE_GREEN_LED_1_PIN, LOW);
        digitalWrite(STATE_IDLE_GREEN_LED_2_PIN, LOW);
        digitalWrite(STATE_IDLE_GREEN_LED_3_PIN, LOW);
        digitalWrite(STATE_DRIVING_RED_LED_1_PIN, HIGH);
        digitalWrite(STATE_DRIVING_RED_LED_2_PIN, HIGH);
        digitalWrite(STATE_ASDRIVING_YELLOW_LED_1_PIN, LOW);
        digitalWrite(STATE_ASDRIVING_YELLOW_LED_2_PIN, LOW);
        break;
    case 2: // ASDriving
        digitalWrite(STATE_IDLE_GREEN_LED_1_PIN, LOW);
        digitalWrite(STATE_IDLE_GREEN_LED_2_PIN, LOW);
        digitalWrite(STATE_IDLE_GREEN_LED_3_PIN, LOW);
        digitalWrite(STATE_DRIVING_RED_LED_1_PIN, LOW);
        digitalWrite(STATE_DRIVING_RED_LED_2_PIN, LOW);
        digitalWrite(STATE_ASDRIVING_YELLOW_LED_1_PIN, HIGH);
        digitalWrite(STATE_ASDRIVING_YELLOW_LED_2_PIN, HIGH);
        break;
    default:
        break;
    }
}

void printVariables(int state)
{
    printf("TSOn: %d\n", TSOn);
    printf("ASReady: %d\n", ASReady);
    printf("BrakeVal: %d\n", brakeValue);
    switch (state)
    {
    case 0:
        printf("State light -> IDLE\n");
        break;
    case 1:
        printf("State light -> DRIVING\n");
        break;
    case 2:
        printf("State light -> ASDRIVING\n");
        break;
    default:
        break;
    }
}

void integrationtest(int R2DStatus)
{
    state_light(R2DStatus);
    brakeValueUpdate();
    if (print_timer > 1000)
    {
        print_timer = 0;
        printf("---------------\n");
        printVariables(R2DStatus);
        printf("TSON READ: %d\n", digitalRead(TSON_PIN));
        printf("Brake sensor: %d\n", analogRead(BK_SENSOR_PIN));
    }
}