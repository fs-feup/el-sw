#ifndef _WRITE_DATA_H_
#define _WRITE_DATA_H_

// =*= CONFIG =*=
// SPI pinout
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

void setup_csv();
void write();

#endif  // WRITE_DATA_H