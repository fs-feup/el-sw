#include "r2d.h"
#include <Arduino.h>
#include <FlexCAN_T4.h>
#include "Bounce2.h"
#include "can.h"

extern CAN_message_t request_bamo;
extern CAN_message_t bamo_apps;
// FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

// int pin_brake_sensor = 20;
int pin_current_sensor = 21;
int pin_selector_1 = 26;
int pin_selector_2 = 19;
int pin_shutdown_circuit = 9;
int pin_precharge = 33;

bool sound = false;
r2d_mode next_state;

Bounce pushbutton = Bounce(pin_R2Dbutton, 10);
// 10 ms debounce
/*
int check_bamocar() {
    can1.write(request_bamo);

    CAN_message_t msg;
    if (can1.read(msg)) {
        if (msg.buf[0] == 0x8F) {
            uint32_t data = msg.buf[1] + msg.buf[2] + msg.buf[3] + msg.buf[4];
            switch (data) {  // all possible errors. O que fazer quando são detetados??
                case 1:
                    // Parameter damaged
                    Serial.print("Parameter damaged");
                    return 1;
                    break;
                case 2:
                    // Hardware error
                    Serial.print("Hardware error");
                    return 1;
                    break;
                case 4:
                    // Safety circuit faulty (only active with RUN)
                    Serial.print("Safety circuit faulty (only active with RUN)");
                    return 1;
                    break;
                case 8:
                    // CAN TimeOut Time exceeded
                    Serial.print("CAN Timeout time exceeded");
                    return 1;
                    break;
                case 16:
                    // Bad or wrong encoder signal
                    Serial.print("Bad or wrong encoder signal");
                    return 1;
                    break;
                case 32:
                    // Power voltage missing
                    Serial.print("Power voltage missing");
                    return 1;
                    break;
                case 64:
                    // Engine temperature too high
                    Serial.print("Engine temperature too high");
                    return 1;
                    break;
                case 128:
                    // Unit temperature too high
                    Serial.print("Unit temperature too high");
                    return 1;
                    break;
                case 256:
                    // Overvoltage > 1.8 x UN reached
                    Serial.print("Overvoltage > 1.8 x UN reached");
                    return 1;
                    break;
                case 512:
                    // Overcurrent or strong oscillating current detected
                    Serial.print("Overcurrent or strong oscillating current detected");
                    return 1;
                    break;
                case 1024:
                    // Spinning (without setpoint, wrong direction)
                    Serial.print("Spinning (without setpoint, wrong direction)");
                    return 1;
                    break;
                case 2048:
                    // User - Error selection
                    Serial.print("User - Error selection");
                    return 1;
                    break;
                case 16384:
                    // Current - Measurement error
                    Serial.print("Current - Measurement error");
                    return 1;
                    break;
                case 32768:
                    // Ballast circuit overloaded
                    Serial.print("Ballast circuit overloaded");
                    return 1;
                    break;
            }
        }
    }
    return 0;
}
*/

/*
void send_to_bamocar(int value_bamo) {
    uint8_t byte1 = (value_bamo >> 8) & 0xFF;  // MSB
    uint8_t byte2 = value_bamo & 0xFF;         // LSB

     bamo_apps.buf[1] = byte2;
     bamo_apps.buf[2] = byte1;

     can1.write(bamo_apps);
 }

 int check_BMS() {
     return 0;
 }
*/
void play_r2d_sound() {
    digitalWrite(buzzerPin, HIGH);  // Turn off the buzzer for the other half of the period
    delay(1000);
    digitalWrite(buzzerPin, LOW);
    delay(1000);
}

void r2d_state_update(r2d_mode* state) {
    return;
}

void check_val(const CAN_message_t& msg) {
    if (msg.id == 0x123) {
        int val = msg.buf[0];
        if (pushbutton.update() && val > 100) {
            if (pushbutton.fallingEdge())
                next_state = R2D_MODE_DRIVE;
        }
    } else
        return;
}

r2d_mode r2d_state_machine(r2d_mode cur_state, int apps_value) {
    r2d_mode next_state = cur_state;
    switch (cur_state) {
        case R2D_MODE_STARTUP:
            delay(POWER_ON_DELAY_MS);
            next_state = R2D_MODE_IDLE;
            sound = false;
            break;
        case R2D_MODE_IDLE:

            // if(check_bamocar() != 0) next_state = R2D_MODE_ERROR;

            // if(check_BMS()) {}

            // check apps
            // check modo dash
            // check modo volante

            // check fim precharge
            // check r2d button
            // check brake
            if (digitalRead(pin_precharge) == LOW && digitalRead(pin_shutdown_circuit) == LOW) {
                CAN_message_t msg;
                int val = 0;
                // can1.onReceive(check_val);
            }
            // update display
            break;
        case R2D_MODE_DRIVE:

            if (!sound) {
                play_r2d_sound();
                sound = true;
            }

            // if (check_bamocar() != 0){
            // next_state = R2D_MODE_ERROR;
            //}
            // if(check_BMS()) {}

            // check apps
            // check modo dash
            // check modo volante
            // else {
            // ler seletor do volante referente aos modos (para limitar o bamocar) -> usar código do Bernardo mas enviamos como parâmetros os valores lidos da BMS

            // send_to_bamocar(apps_value);

            // if(o carro desligar temos de atualizar o estado para o básico) next_state = R2D_MODE_STARTUP;
            //}
            break;
        case R2D_MODE_ERROR:
            next_state = R2D_MODE_IDLE;
            sound = false;
            break;

        default:
            next_state = R2D_MODE_ERROR;
            sound = false;
            break;
    }
    return next_state;
}
