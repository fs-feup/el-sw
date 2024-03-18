#include "display.h"
#include "apps.h"

#include <EasyNextionLibrary.h>
#include <FlexCAN_T4.h>

#define NEXTION_PORT Serial1

EasyNex myNex(NEXTION_PORT);

int rpm = 0;
int soc = 0;
int current = 0;
int speedInt = 0;
int switchPin = 14;
int ACCurrent = 0;
int motorTemp = 0;
int lowTemp = 0;
int sensorValue = 0;
int highTemp = 0;
int currentPage = 0;
int packVoltage = 0;
int powerStageTemp = 0;

int mapSensorValueToSwitchNumber(int sensorValue) {
    int rotswitchNumber = 0;

    float posicao = sensorValue;

    posicao /= 93;

    rotswitchNumber = round(posicao);
    return rotswitchNumber;
}

void displaySetup() {
    myNex.begin(9600);
    pinMode(switchPin, INPUT);
}

void displayUpdate() {
    myNex.NextionListen();

    int sensorValue = analogRead(switchPin);

    // Map the sensor value to the switch position
    int switchPosition = mapSensorValueToSwitchNumber(sensorValue);

    char mode[28];

    sprintf(mode, "MODE %d", switchPosition);

    myNex.writeStr("t3.txt", mode);

    myNex.writeNum("n0.val", speedInt);
    myNex.writeNum("x0.val", soc * 10);
    myNex.writeNum("x1.val", highTemp * 10);
    myNex.writeNum("x2.val", packVoltage);
    myNex.writeNum("x3.val", current);
    myNex.writeNum("x4.val", motorTemp);
    myNex.writeNum("x5.val", powerStageTemp);
    myNex.writeNum("x7.val", ACCurrent);
    myNex.writeNum("x8.val", rpm);
    myNex.writeNum("x9.val", highTemp);
    myNex.writeNum("x10.val", lowTemp);
}