#include <Arduino.h>
#include <Bounce2.h>

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


extern volatile bool TSOn;
extern volatile bool R2DOverride;
extern Bounce r2dButton;
extern volatile bool ASReady;
extern elapsedMillis R2DTimer;
extern elapsedMillis APPSTimer;
extern CAN_message_t disable;

extern FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;


enum status
{
    IDLE,     // waiting for r2d && ATS off
    DRIVING,  // r2d pressed && ATS on
    ASDRIVING // TSon and ASReady (it doesnt mean that the car is driving; AS State = ASReady || Driving || Emergency)
};

status R2DStatus;

void playR2DSound()
{
    digitalWrite(buzzerPin, HIGH);
    delay(1000);
    digitalWrite(buzzerPin, LOW);
}

void statemachine(){
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
        else if (TSOn and ASReady)
        {
            initBamocarD3();
            request_dataLOG_messages();
            R2DStatus = ASDRIVING;
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
    case ASDRIVING:
        if (not TSOn)
        {
            R2DStatus = IDLE;
            can1.write(disable);
            break;
        }
        break;
    default:
        ERROR("Invalid r2d_status");
        break;
    }
}
