#ifndef LOGGING_H
#define LOGGING_H

#include <SD.h>
#include <FlexCAN_T4.h>
#include <SPI.h>

#define PIN_SPI_CLK 45
#define PIN_SPI_MOSI 43
#define PIN_SPI_MISO 42
#define PIN_SD_CS 44
// If you have connected other SPI device then
// put here number of pin for disable its.
// Provide -1 if you don't have other devices.
#define PIN_OTHER_DEVICE_CS -1
// Change this value if you have problems with SD card
// Available values: SPI_QUARTER_SPEED //SPI_HALF_SPEED
// It is enum from SdFat
#define SD_CARD_SPEED SPI_FULL_SPEED

#define BMS_ID 0x666
#define BAMO_RESPONSE_ID 0x181

#define LOGGING_PERIOD 10


struct LogEntry
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    uint32_t millisecond;
};


class Logging {

 public:
  void write_to_file_VD(int current, int voltage, int mintmp, int maxtmp, int avgtmp, int apps1, int apps2, int brake);
  void write_to_file_powertrain(int rpm, int I_actual, int powerStageTmp, int motorTm);
  void setup_log();
 
 private:

};

#endif