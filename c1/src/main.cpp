#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <elapsedMillis.h>
#include <logging.h>

#define AVG_SAMPLES 20

#define BRAKE_LIGHT 2
#define BRAKE_LIGHT_LOWER_THRESH 165       // 165/1023 * 3.3V = 0.532V
#define BRAKE_LIGHT_UPPER_THRESH 510       // /1023 * 3.3V = 0.78V
#define BRAKE_LIGHT_BRIGHTNESS 150         // 0-255
#define BRAKE_LIGHT_MIN_ACTIVE_PERIOD 200  // ms

#define BRAKE_SENSOR_PIN A5
#define CURRENT_SENSOR_PIN A4

#define SENSOR_SAMPLE_PERIOD 20  // ms

#define CAN_BAUD_RATE 500000
#define CAN_TRANSMISSION_PERIOD 100  // ms

#define LOGGING_PERIOD 10


uint16_t brake_val = 0;

elapsedMillis canTimer;
elapsedMillis brake_sensor_timer;
elapsedMillis brake_light_active_timer;
elapsedMillis writeTIMER;
elapsedMillis CURRENTtimer;


Logging loggingInstance;


int current = 0;
int voltage = 0;
int mintmp = 0;
int maxtmp = 0;
int avgtmp = 0;
int apps1 = 0;
int apps2 = 0;
int brake = 0;

int speed = 0;
int rpm_max = 0;
int I_actual = 0;
int powerStageTemp = 0;
int motorTemp = 0;
int lemos = 0;
int motorTemp2 = 0;
int powerStageTemp2 = 0;
int torque = 0;
int motor_voltage = 0;
int battery_voltage = 0;

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

CAN_message_t brake_sensor_c3;
CAN_message_t current_controll;

int8_t current_byte1; // MSB
int8_t current_byte2; // LSB
CAN_message_t current_message_bamo;

bool R2D = false;

int avgBuffer1[AVG_SAMPLES] = {0};

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

void initMessages() {
    current_controll.id = 0x201;
    current_controll.len = 3;
    current_controll.buf[0] = 0xfb;

    brake_sensor_c3.id = 0x123;
    brake_sensor_c3.len = 3;
    brake_sensor_c3.buf[0] = 0x90;
}

void canbusSniffer(const CAN_message_t& msg) {
    //Serial.println("CAN message received");
    //Serial.print("Message ID: ");
    //Serial.println(msg.id, HEX);
    switch(msg.id) {
        case BMS_ID:
        current = ((msg.buf[0] << 8) | msg.buf[1]);
        voltage = ((msg.buf[5] << 8) | msg.buf[6]) / 10;
        mintmp = msg.buf[2];
        maxtmp = msg.buf[3];
        avgtmp = msg.buf[4];
        break;
        
        case 0x111:
        apps1 = ((msg.buf[1] << 8) | msg.buf[0]);
        apps2 = ((msg.buf[3] << 8) | msg.buf[2]);
        break;

        case BAMO_RESPONSE_ID:
            if(msg.buf[0] == 0x30) {
                speed = (msg.buf[2] << 8) | msg.buf[1];
                //if(speed < 0) speed *= -1;
                //if (rpm < 0)
                    //rpm *= -1;
                //rpm = (rpm * 6500) / 32760;
            }
            if(msg.buf[0] == 0xCE) {
                rpm_max = (msg.buf[2] << 8) | msg.buf[1];
                //speed = rpm_max * (speed/32767);
            }            
            if(msg.buf[0] == 0x5f) {
                I_actual = (msg.buf[2] << 8) | msg.buf[1];
            }
            if(msg.buf[0] == 0x49) {
                motorTemp = (msg.buf[2] << 8) | msg.buf[1];
                //motorTemp2 = (msg.buf[1] << 8) | msg.buf[2];
                //if(motorTemp2 > motorTemp) motorTemp = motorTemp2;

                //motorTemp = motorTemp * 0.0194 - 160;
            }
            if(msg.buf[0] == 0x4A) {
                powerStageTemp = (msg.buf[2] << 8) | msg.buf[1];
                //powerStageTemp2 = (msg.buf[1] << 8) | msg.buf[2];
                //if(powerStageTemp2 > powerStageTemp) powerStageTemp = powerStageTemp2;
                //powerStageTemp = (int)(powerStageTemp / 103.969 - 158.29);
            }
            if(msg.buf[0] == 0x8a) {
                motor_voltage = (msg.buf[2] << 8) | msg.buf[1];
            }
            if(msg.buf[0] == 0xa0) {
                torque = (msg.buf[2] << 8) | msg.buf[1];
            }
            if(msg.buf[0] == 0xeb) {
                battery_voltage = (msg.buf[2] << 8) | msg.buf[1];
            }

            break;
    }
}

void canbusSetup() {
    can1.begin();
    can1.setBaudRate(500000);
    can1.enableFIFO();
    can1.enableFIFOInterrupt();
    can1.setFIFOFilter(REJECT_ALL);
    can1.setFIFOFilter(0, 0x111, STD);
    can1.setFIFOFilter(1, BMS_ID, STD);
    can1.setFIFOFilter(2, BAMO_RESPONSE_ID, STD);
    can1.onReceive(canbusSniffer);
    initMessages();
}

void sendMsg(uint16_t brake_value) {
    brake_sensor_c3.buf[2] = (brake_value >> 8) & 0xFF;  // MSB
    brake_sensor_c3.buf[1] = brake_value & 0xFF;         // LSB

    can1.write(brake_sensor_c3);
}

bool brakeLightControl(int brake_val) {
    if (brake_val >= BRAKE_LIGHT_LOWER_THRESH and brake_val <= BRAKE_LIGHT_UPPER_THRESH) {
        brake_light_active_timer = 0;
        analogWrite(BRAKE_LIGHT, BRAKE_LIGHT_BRIGHTNESS);
        return true;
    } else if (brake_light_active_timer > BRAKE_LIGHT_MIN_ACTIVE_PERIOD) {
        analogWrite(BRAKE_LIGHT, 0);
        return false;
    }
    return false;
}

void setup() {
    Logging loggingInstance;

    canbusSetup();
    loggingInstance.setup_log();
    pinMode(BRAKE_SENSOR_PIN, INPUT);
    pinMode(BRAKE_LIGHT, OUTPUT);
}

void loop() {
    if (brake_sensor_timer > SENSOR_SAMPLE_PERIOD) {
        brake_sensor_timer = 0;
        brake_val = analogRead(BRAKE_SENSOR_PIN);
        bufferInsert(avgBuffer1, AVG_SAMPLES, brake_val);
        brake_val = average(avgBuffer1, AVG_SAMPLES);
        brake = brake_val;
        //Serial.println(brake_val);
        if (brakeLightControl(brake_val)) {
            //Serial.println("Brake Light ON");
            if (canTimer > CAN_TRANSMISSION_PERIOD) {
                //Serial.println("Message sent");
                sendMsg(brake_val);
                canTimer = 0;
            }
        }
    }
    if(writeTIMER > LOGGING_PERIOD) {
        loggingInstance.write_to_file(current, voltage, mintmp, maxtmp, avgtmp, apps1, apps2, brake, speed, I_actual, powerStageTemp, motorTemp, torque, motor_voltage, battery_voltage);
        //current = 0; voltage = 0; mintmp = 0; maxtmp = 0; avgtmp = 0; apps1 = 0; apps2 = 0; brake = 0;
        //speed = 0; I_actual = 0; powerStageTemp = 0; motorTemp = 0;
        writeTIMER = 0;
    }
    if(CURRENTtimer > 8) {
        CURRENTtimer = 0;
            
        current_byte1 = (I_actual >> 8) & 0xFF;  // MSB
        current_byte2 = I_actual & 0xFF;         // LSB

        current_message_bamo.id = 0x201;
        current_message_bamo.len = 5;
        current_message_bamo.buf[0] = 0xfb;
        current_message_bamo.buf[1] = current_byte2;
        current_message_bamo.buf[2] = current_byte1;
        current_message_bamo.buf[3] = 0x00;
        current_message_bamo.buf[4] = 0x00;

        can1.write(current_message_bamo);
    }

}