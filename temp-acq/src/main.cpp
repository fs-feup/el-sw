#include <Adafruit_MCP3008.h>
#include <FlexCAN_T4.h>
#include <Wire.h>
#include <elapsedMillis.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

Adafruit_MCP3008 ADCs[8];

#define Reference 2.5
#define ERROR_TIME 900

#define N_ADCs 8
#define N_ADC_CHANNELS 8

// #define BMS_CAN_PERIOD 250
// #define BROADCAST_PERIOD 250
#define BROADCAST_ID 0x301

elapsedMillis tempErrorTimer;

CAN_message_t BMSInfoMsg;
CAN_message_t BMSErrorFlag;
CAN_message_t tempBroadcast;
CAN_message_t msg_1;

// elapsedMillis timeSinceLastBroadcast = 0;
// elapsedMillis timeSinceLastBMSMessage = 0;

int broadcastIndex = 0;
int broadcastEnabled = 0;
int count = 0;

float read = 0;
double voltage = 0;
double voltage1 = 0;
double voltage2 = 0;
double voltage3 = 0;
double voltage4 = 0;
double temperature = 0.0;
float maxTemp = 0.0;
float minTemp = 60.0;
float tempSum = 0.0;
float avgTemp = 0.0;

volatile bool BMSErr = 0;
volatile bool tempErr = 0;

int ADCRaw[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}};

float Temps[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}};

double ADCconversion(int raw) {
    voltage = (raw * Reference) / 1024.0;
    voltage2 = voltage * voltage;
    voltage3 = voltage2 * voltage;
    voltage4 = voltage3 * voltage;

    temperature = 2875.88397 - 5512.867802 * voltage + 4082.002758 * voltage2 - 1358.200746 * voltage3 + 168.841073 * voltage4;
    return temperature;
}

// write a function to read all the ADC values
void readRawADCData() {
    for (int adc = 0; adc < N_ADCs; adc++) {
        for (int channel = 0; channel < N_ADC_CHANNELS; channel++) {
            if (adc == 7 && channel > 3)
                continue;
            int tempID = adc * 8 + channel;
            switch (tempID) {
                case 18:
                case 29:
                case 33:
                    ADCRaw[adc][channel] = ADCRaw[1][1];
                    break;
                default:
                    ADCRaw[adc][channel] = ADCs[adc].readADC(channel);
                    break;
            }
            Temps[adc][channel] = ADCconversion(ADCRaw[adc][channel]);

            minTemp = min(minTemp, Temps[adc][channel]);
            maxTemp = max(maxTemp, Temps[adc][channel]);
            tempSum += Temps[adc][channel];
        }
    }
    avgTemp = tempSum / (N_ADCs * N_ADC_CHANNELS - 4);  // Only 60 ADC Channels are usable
}

void broadcastRawData() {
    tempBroadcast.id = BROADCAST_ID + broadcastIndex;  // para decidir
    tempBroadcast.len = N_ADC_CHANNELS + 1;
    for (int i = 0; i < N_ADCs; i++)
        tempBroadcast.buf[i] = (uint8_t)ADCRaw[broadcastIndex][i];

    can1.write(tempBroadcast);
    broadcastIndex = (broadcastIndex + 1) % N_ADCs;
}

void CAN_msg() {
    msg_1.id = 0x301;  // para decidir
    msg_1.len = 8;

    msg_1.buf[0] = 0;
    msg_1.buf[1] = (uint8_t)ADCconversion(ADCRaw[0][0]);
    msg_1.buf[2] = (uint8_t)ADCconversion(ADCRaw[0][1]);
    msg_1.buf[3] = (uint8_t)ADCconversion(ADCRaw[0][2]);
    msg_1.buf[4] = (uint8_t)ADCconversion(ADCRaw[0][3]);
    msg_1.buf[5] = (uint8_t)ADCconversion(ADCRaw[0][4]);
    msg_1.buf[6] = (uint8_t)ADCconversion(ADCRaw[0][5]);
    msg_1.buf[7] = (uint8_t)ADCconversion(ADCRaw[0][6]);
    can1.write(msg_1);

    msg_1.buf[0] = 1;
    msg_1.buf[1] = (uint8_t)ADCconversion(ADCRaw[0][7]);
    msg_1.buf[2] = (uint8_t)ADCconversion(ADCRaw[1][0]);
    msg_1.buf[3] = (uint8_t)ADCconversion(ADCRaw[1][1]);
    msg_1.buf[4] = (uint8_t)ADCconversion(ADCRaw[1][2]);
    msg_1.buf[5] = (uint8_t)ADCconversion(ADCRaw[1][3]);
    msg_1.buf[6] = (uint8_t)ADCconversion(ADCRaw[1][4]);
    msg_1.buf[7] = (uint8_t)ADCconversion(ADCRaw[1][5]);
    can1.write(msg_1);

    msg_1.buf[0] = 2;
    msg_1.buf[1] = (uint8_t)ADCconversion(ADCRaw[1][6]);
    msg_1.buf[2] = (uint8_t)ADCconversion(ADCRaw[1][7]);
    msg_1.buf[3] = (uint8_t)ADCconversion(ADCRaw[2][0]);
    msg_1.buf[4] = (uint8_t)ADCconversion(ADCRaw[2][1]);
    msg_1.buf[5] = (uint8_t)ADCconversion(ADCRaw[2][2]);
    msg_1.buf[6] = (uint8_t)ADCconversion(ADCRaw[2][3]);
    msg_1.buf[7] = (uint8_t)ADCconversion(ADCRaw[2][4]);
    can1.write(msg_1);

    msg_1.buf[0] = 3;
    msg_1.buf[1] = (uint8_t)ADCconversion(ADCRaw[2][5]);
    msg_1.buf[2] = (uint8_t)ADCconversion(ADCRaw[2][6]);
    msg_1.buf[3] = (uint8_t)ADCconversion(ADCRaw[2][7]);
    msg_1.buf[4] = (uint8_t)ADCconversion(ADCRaw[3][0]);
    msg_1.buf[5] = (uint8_t)ADCconversion(ADCRaw[3][1]);
    msg_1.buf[6] = (uint8_t)ADCconversion(ADCRaw[3][2]);
    msg_1.buf[7] = (uint8_t)ADCconversion(ADCRaw[3][3]);
    can1.write(msg_1);

    msg_1.buf[0] = 4;
    msg_1.buf[1] = (uint8_t)ADCconversion(ADCRaw[3][4]);
    msg_1.buf[2] = (uint8_t)ADCconversion(ADCRaw[3][5]);
    msg_1.buf[3] = (uint8_t)ADCconversion(ADCRaw[3][6]);
    msg_1.buf[4] = (uint8_t)ADCconversion(ADCRaw[3][7]);
    msg_1.buf[5] = (uint8_t)ADCconversion(ADCRaw[4][0]);
    msg_1.buf[6] = (uint8_t)ADCconversion(ADCRaw[4][1]);
    msg_1.buf[7] = (uint8_t)ADCconversion(ADCRaw[4][2]);
    can1.write(msg_1);

    msg_1.buf[0] = 5;
    msg_1.buf[1] = (uint8_t)ADCconversion(ADCRaw[4][3]);
    msg_1.buf[2] = (uint8_t)ADCconversion(ADCRaw[4][4]);
    msg_1.buf[3] = (uint8_t)ADCconversion(ADCRaw[4][5]);
    msg_1.buf[4] = (uint8_t)ADCconversion(ADCRaw[4][6]);
    msg_1.buf[5] = (uint8_t)ADCconversion(ADCRaw[4][7]);
    msg_1.buf[6] = (uint8_t)ADCconversion(ADCRaw[5][0]);
    msg_1.buf[7] = (uint8_t)ADCconversion(ADCRaw[5][1]);
    can1.write(msg_1);

    msg_1.buf[0] = 6;
    msg_1.buf[1] = (uint8_t)ADCconversion(ADCRaw[5][2]);
    msg_1.buf[2] = (uint8_t)ADCconversion(ADCRaw[5][3]);
    msg_1.buf[3] = (uint8_t)ADCconversion(ADCRaw[5][4]);
    msg_1.buf[4] = (uint8_t)ADCconversion(ADCRaw[5][5]);
    msg_1.buf[5] = (uint8_t)ADCconversion(ADCRaw[5][6]);
    msg_1.buf[6] = (uint8_t)ADCconversion(ADCRaw[5][7]);
    msg_1.buf[7] = (uint8_t)ADCconversion(ADCRaw[6][0]);
    can1.write(msg_1);

    msg_1.buf[0] = 7;
    msg_1.buf[1] = (uint8_t)ADCconversion(ADCRaw[6][1]);
    msg_1.buf[2] = (uint8_t)ADCconversion(ADCRaw[6][2]);
    msg_1.buf[3] = (uint8_t)ADCconversion(ADCRaw[6][3]);
    msg_1.buf[4] = (uint8_t)ADCconversion(ADCRaw[6][4]);
    msg_1.buf[5] = (uint8_t)ADCconversion(ADCRaw[6][5]);
    msg_1.buf[6] = (uint8_t)ADCconversion(ADCRaw[6][6]);
    msg_1.buf[7] = (uint8_t)ADCconversion(ADCRaw[6][7]);
    can1.write(msg_1);

    msg_1.buf[0] = 8;
    msg_1.buf[1] = (uint8_t)ADCconversion(ADCRaw[7][0]);
    msg_1.buf[2] = (uint8_t)ADCconversion(ADCRaw[7][1]);
    msg_1.buf[3] = (uint8_t)ADCconversion(ADCRaw[7][2]);
    msg_1.buf[4] = (uint8_t)ADCconversion(ADCRaw[7][3]);
    can1.write(msg_1);
}

void sendTempsToBMS() {
    if (minTemp > 0 or maxTemp < 58)
        tempErrorTimer = 0;

    if (tempErrorTimer >= 700)
        tempErr = 1;

    BMSInfoMsg.id = 0x1839F380;
    BMSInfoMsg.flags.extended = 1;
    BMSInfoMsg.len = 8;
    BMSInfoMsg.buf[0] = 0x00;
    BMSInfoMsg.buf[1] = minTemp;  // 60 is maximum allowed temperature before
    BMSInfoMsg.buf[2] = maxTemp;  // triggering an error on the BMS
    BMSInfoMsg.buf[3] = avgTemp;
    BMSInfoMsg.buf[4] = 0x01;
    BMSInfoMsg.buf[5] = 0x01;
    BMSInfoMsg.buf[6] = 0x00;
    BMSInfoMsg.buf[7] = BMSInfoMsg.buf[1] + BMSInfoMsg.buf[2] + BMSInfoMsg.buf[3] + BMSInfoMsg.buf[4] + BMSInfoMsg.buf[5] + BMSInfoMsg.buf[6] + 0x39 + 0x08;
    can1.write(BMSInfoMsg);

    BMSErrorFlag.id = 0x306;
    BMSErrorFlag.flags.extended = 1;
    BMSErrorFlag.len = 1;
    BMSErrorFlag.buf[0] = (BMSErr || tempErr);
    can1.write(BMSErrorFlag);
}

void canbusSniffer(const CAN_message_t& msg) {
    // if (Serial) {
    //     Serial.println("CAN message received");
    //     Serial.print("Message ID: ");
    //     Serial.println(msg.id, HEX);
    // }

    if (msg.id == 0x300)
        broadcastEnabled = 1;

    if (msg.id == 0x270) {
        BMSErr = msg.buf[0];  // atualiza flag erro BMS
    }
}

void setup() {
    // try to connect to the serial monitor
    Serial.begin(9600);

    if (Serial)
        Serial.println("Serial monitor connected");

    can1.begin();
    can1.setBaudRate(125000);

    can1.enableFIFO();
    can1.enableFIFOInterrupt();
    can1.onReceive(canbusSniffer);
    can1.setFIFOFilter(REJECT_ALL);
    (void)can1.setFIFOFilter(0, 0x111, STD);

    (void)ADCs[0].begin(13, 11, 12, 18);
    (void)ADCs[1].begin(13, 11, 12, 19);
    (void)ADCs[2].begin(13, 11, 12, 20);
    (void)ADCs[3].begin(13, 11, 12, 21);
    (void)ADCs[4].begin(13, 11, 12, 4);
    (void)ADCs[5].begin(13, 11, 12, 5);
    (void)ADCs[6].begin(13, 11, 12, 6);
    (void)ADCs[7].begin(13, 11, 12, 7);
}

void printTemp() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Serial.printf("ADC %d raw data: %d temp: %f\n", (i * 8) + j, ADCRaw[i][j], ADCconversion(ADCRaw[i][j]));
        }
    }
    Serial.printf("BMS Error: %d\n", BMSErr);
    Serial.printf("Temp Error: %d\n", tempErr);
}

void loop() {
    // reset measurements
    tempSum = 0;
    maxTemp = 0;
    minTemp = 999;

    readRawADCData();
    // broadcastRawData();
    CAN_msg();
    sendTempsToBMS();
    if (Serial)
        printTemp();

    delay(50);
}
