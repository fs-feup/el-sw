#include <Arduino.h>
#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

CAN_message_t msg2;

long //Parameters
const int aisPin  = 15;
const int numReadings  = 10;
int readings [numReadings];
int readIndex  = 0;
long total  = 0;
long average = 0;

long smooth() { /* function smooth */
  ////Perform average on sensor readings

  // subtract the last reading:
  total = total - readings[readIndex];
  // read the sensor:
  readings[readIndex] = analogRead(aisPin);
  // add value to total:
  total = total + readings[readIndex];
  // handle index
  readIndex = readIndex + 1;
  if (readIndex >= numReadings) {
    readIndex = 0;
  }
  // calculate the average:
  average = total / numReadings;

  return average;
}

void setup() {
  Serial.begin(9600);

  can1.begin();
  can1.setBaudRate(500000);

  //CAN_message_t msg3;
  //msg3.id = 0x201;
  //msg3.len = 3;
  //msg3.buf[0] = 0x51;
  //msg3.buf[1] = 0x00; 
  //msg3.buf[2] = 0x00;
  //can1.write(msg3);
}

void loop() {
  long val = smooth();
  
  uint16_t value_bamo = val * 32767.0 / 1023.0;
  
  uint8_t byte1 = (value_bamo >> 8) & 0xFF; //MSB
  uint8_t byte2 = value_bamo & 0xFF; //LSB

  Serial.print("byte1: ");
  Serial.print(byte1,HEX);
  Serial.print("\n byte2: ");
  Serial.print(byte2,HEX);
  //definir a mensagem de acordo com o que o BAMOCAR pede
  //speed command value

  Serial.printf("\n Value sent: %d",value_bamo);
  

  CAN_message_t msg;
  msg.id = 0x201;
  msg.len=3;
  msg.buf[0] = 0x31;
  msg.buf[1] = byte2;
  msg.buf[2] = byte1;

  Serial.print("Sent message with ID 0x");
  Serial.print(msg.id, HEX);
  Serial.print(": ");
  for (int i = 0; i < msg.len; i++) {
    Serial.print(msg.buf[i]);
  }

  can1.write(msg);
  
  Serial.println("\n Message sent!");
  delay(10);
  
  /*

  if (can1.read(msg2)) {
    Serial.print("Received message with ID 0x");
    Serial.print(msg2.id, HEX);
    Serial.print(": ");
    for (int i = 0; i < msg2.len; i++) {
      Serial.print(msg2.buf[i]);
    }
    Serial.print('\n');
  }
  */
  
}