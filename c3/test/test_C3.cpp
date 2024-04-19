#include "unity.h"
#include "statemachine.hpp"
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

#undef R2D_PIN 32

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


void Setup(){
    Serial.begin(9600);

    pinMode(BK_SENSOR_PIN, INPUT);
    pinMode(R2D_PIN, INPUT);
    //pinMode(TSON_PIN, INPUT);
    //pinMode(AS_READY_PIN, INPUT);
    //pinMode(STOP_PIN, INPUT);
    pinMode(STATE_IDLE_GREEN_LED_1_PIN, OUTPUT);
    pinMode(STATE_IDLE_GREEN_LED_2_PIN, OUTPUT);
    pinMode(STATE_IDLE_GREEN_LED_3_PIN, OUTPUT);
    pinMode(STATE_DRIVING_RED_LED_1_PIN, OUTPUT);
    pinMode(STATE_DRIVING_RED_LED_2_PIN, OUTPUT);
    pinMode(STATE_ASDRIVING_YELLOW_LED_1_PIN, OUTPUT);
    pinMode(STATE_ASDRIVING_YELLOW_LED_2_PIN, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(BK_SENSOR_PIN),brakeValueUpdate,CHANGE);
    attachInterrupt(digitalPinToInterrupt(TSON_PIN),updateTS,CHANGE);
    attachInterrupt(digitalPinToInterrupt(AS_READY_PIN),updateASReady,CHANGE);
    attachInterrupt(digitalPinToInterrupt(STOP_PIN),stopTest,CHANGE);


    r2dButton.attach(R2D_PIN, INPUT);
    r2dButton.interval(0.1);
}

void brakeValueUpdate(){
    uint16_t brakeValue = analogRead(BK_SENSOR_PIN);
    if (brakeValue > 165)
                R2DTimer = 0;
}

void updateTS(){
    TSOn = not digitalRead(TSON_PIN);
}

void updateASReady(){
    ASReady = not digitalRead(AS_READY_PIN);
}

void stopTest(){
    if (not digitalRead(STOP_PIN))
        TEST_ASSERT(1 == 1);

}

void test_state_machine() {
    while(1){
        statemachine();
    }
}

void test_IDLE_2_DRIVING() {
    R2DStatus = IDLE;
    TSOn = true;
    while(R2DStatus != DRIVING){
        statemachine();
        Serial.println(R2DStatus);
    }
    TEST_ASSERT_EQUAL(DRIVING, R2DStatus);
}

void test_IDLE_2_ASDRIVING() {
    R2DStatus = IDLE;
    TSOn = true;
    ASReady = true;
    statemachine();
    TEST_ASSERT_EQUAL(ASDRIVING, R2DStatus);
}

void test_ASDRIVING_2_IDLE() {
    R2DStatus = ASDRIVING;
    TSOn = false;
    statemachine();
    TEST_ASSERT_EQUAL(IDLE, R2DStatus);
}

void test_DRIVING_2_IDLE() {
    R2DStatus = DRIVING;
    TSOn = false;
    statemachine();
    TEST_ASSERT_EQUAL(IDLE, R2DStatus);
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_IDLE_2_DRIVING);
    RUN_TEST(test_DRIVING_2_IDLE);
    RUN_TEST(test_IDLE_2_ASDRIVING);
    RUN_TEST(test_ASDRIVING_2_IDLE);
    UNITY_END();

    return 0;
}