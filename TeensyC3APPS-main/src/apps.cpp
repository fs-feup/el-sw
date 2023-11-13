#include "apps.h"
#include "debug.h"
#include "can.h"

#include <Arduino.h>
#include <elapsedMillis.h>

elapsedMillis appsImplausibilityTimer;
elapsedMillis appsBrakePlausibilityTimer;

volatile uint16_t brakeValue;
bool APPsTimeout = false;

int avgBuffer1[AVG_SAMPLES] = {0};
int avgBuffer2[AVG_SAMPLES] = {0};

int average(int* buffer, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += buffer[i];
    }
    return sum / n;
}

void bufferInsert(int* buffer, int n, int value) {
    for (int i = 0; i < n - 1; i++) {
        buffer[i] = buffer[i + 1];
    }
    buffer[n - 1] = value;
}

int scaleApps2(int apps2) {
    return apps2 + APPS_LINEAR_OFFSET;
}

bool plausibility(int v_apps1, int v_apps2) {
    if (v_apps1 < v_apps2)
        return false;

    if (v_apps1 > APPS_1_UPPER_BOUND || v_apps1 < APPS_1_LOWER_BOUND)
        return false;

    if (v_apps2 > APPS_2_UPPER_BOUND || v_apps2 < APPS_2_LOWER_BOUND)
        return false;

    if (v_apps1 >= APPS1_DEAD_THRESHOLD)
        return v_apps2 >= APPS1_DEADZONE_EQUIVALENT - APPS_MAX_ERROR_ABS && v_apps2 <= APPS_2_UPPER_BOUND;

    if (v_apps2 <= APPS2_DEAD_THRESHOLD)
        return v_apps1 >= APPS_1_LOWER_BOUND && v_apps1 <= APPS2_DEADZONE_EQUIVALENT + APPS_MAX_ERROR_ABS;

    int v2_expected = scaleApps2(v_apps2);
    int plausibility_value = abs(v2_expected - v_apps1) * 100 / v_apps1;

    return (plausibility_value < APPS_MAX_ERROR_PERCENT);
}

int convertToBamocarScale(int apps1, int apps2) {
    int torqueVal = 0;

    if (apps2 <= APPS2_DEAD_THRESHOLD)
        torqueVal = apps1;
    else
        torqueVal = scaleApps2(apps2);

    int bamoMax = BAMOCAR_MAX;
    int bamoMin = BAMOCAR_MIN;
    int appsMax = APPS_MAX;
    int appsMin = APPS_MIN;

    if (torqueVal > appsMax)
        torqueVal = appsMax;
    if (torqueVal < appsMin)
        torqueVal = appsMin;

    // maps sensor value to bamocar range
    torqueVal = map(torqueVal, appsMin, appsMax, bamoMin, bamoMax);

    return torqueVal >= BAMOCAR_MAX ? BAMOCAR_MAX : torqueVal;
}

int readApps() {
    int v_apps1 = analogRead(APPS_1_PIN);
    int v_apps2 = analogRead(APPS_2_PIN);

    bufferInsert(avgBuffer1, AVG_SAMPLES, v_apps1);
    bufferInsert(avgBuffer2, AVG_SAMPLES, v_apps2);

    v_apps1 = average(avgBuffer1, AVG_SAMPLES);
    v_apps2 = average(avgBuffer2, AVG_SAMPLES);
    sendAPPS(v_apps1,v_apps2);

#ifdef APPS_DEBUG
    INFO("APPS1: %d\tAPPS2: %d\t", v_apps1, v_apps2);
#endif  // APPS_DEBUG

    bool plausible = plausibility(v_apps1, v_apps2);

    if (!plausible and appsImplausibilityTimer > APPS_IMPLAUSIBLE_TIMEOUT_MS) {
#ifdef APPS_DEBUG
        ERROR("APPS Implausible\n");
#endif  // APPS_DEBUG
        return -1;
    }

    if (plausible)
        appsImplausibilityTimer = 0;

    int bamocarValue = convertToBamocarScale(v_apps1, v_apps2);

#ifdef APPS_DEBUG
    INFO("Plausible\t Torque Request:%d\t", bamocarValue);
#endif  // APPS_DEBUG

    if (APPsTimeout) {
        LOG("APPS Blocked\n");
        if (bamocarValue == 0) {
            APPsTimeout = false;
            LOG("APPS Available\n");
        } else
            return 0;
    }

    float pedalTravelPercent = ((float)bamocarValue / BAMOCAR_MAX) * 100.0;

#ifdef APPS_DEBUG
    INFO("Travel \%: %.2f\tBrake Val: %d\tBrake Timer: ", pedalTravelPercent, brakeValue);
    Serial.println(appsBrakePlausibilityTimer);
#endif  // APPS_DEBUG

    if (brakeValue >= BRAKE_BLOCK_THRESHOLD && pedalTravelPercent >= 25.0) {
        if (appsBrakePlausibilityTimer > APPS_BRAKE_PLAUSIBILITY_TIMEOUT_MS) {
            ERROR("APPS and Brake Implausible\n");
            APPsTimeout = true;
            return 0;
        }
    } else
        appsBrakePlausibilityTimer = 0;

    return bamocarValue;
}