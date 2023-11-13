
/**
 *	Write CSV fields feature example.
 */

#include <CSVFile.h>
#include <SdFat.h>

#include "can.h"
#include "debug.h"
#include "elapsedMillis.h"
#include "write_data.h"

extern FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

extern CAN_message_t Nact_filtered;
extern CAN_message_t Vout_msg;
extern CAN_message_t Iq_cmd_msg;
extern CAN_message_t Iq_actual_msg;
extern CAN_message_t Mout_msg;
extern CAN_message_t I_lim_inuse_msg;
extern CAN_message_t I_actual_filtered_msg;
extern CAN_message_t Tpeak_msg;
extern CAN_message_t Imax_peak_msg;
extern CAN_message_t I_con_eff_msg;

extern int soc;
extern int current;
extern int packVoltage;
extern int Nact;
extern int Vout;
extern int Iq_cmd;
extern int Iq_actual;
extern int Mout;
extern int I_lim_inuse;
extern int I_actual_filtered;
extern int Tpeak;
extern int Imax_peak;
extern int I_con_eff;
extern int motorTemp;
extern int powerStageTemp;

SdFat sd;
CSVFile csv;

int t = 0;

void setup_csv() {
    // Setup pinout
    pinMode(PIN_SPI_MOSI, OUTPUT);
    pinMode(PIN_SPI_MISO, INPUT);
    pinMode(PIN_SPI_SCK, OUTPUT);
    // Disable SPI devices
    pinMode(PIN_SD_CS, OUTPUT);
    digitalWrite(PIN_SD_CS, HIGH);

#if PIN_OTHER_DEVICE_CS > 0
    pinMode(PIN_OTHER_DEVICE_CS, OUTPUT);
    digitalWrite(PIN_OTHER_DEVICE_CS, HIGH);
#endif  // PIN_OTHER_DEVICE_CS > 0

    // Setup serial
    Serial.begin(9600);
    while (!Serial) { /* wait for Leonardo */
    }
    // Setup SD card
    if (!sd.begin(PIN_SD_CS, SD_CARD_SPEED)) {
        Serial.println("SD card begin error");
        return;
    }

    can1.write(Nact_filtered);
    can1.write(Vout_msg);
    can1.write(Iq_cmd_msg);
    can1.write(Iq_actual_msg);
    can1.write(Mout_msg);
    can1.write(I_lim_inuse_msg);
    can1.write(I_actual_filtered_msg);
    can1.write(Tpeak_msg);
    can1.write(Imax_peak_msg);
    can1.write(I_con_eff_msg);
}

void initSdFile(char* filename) {
    if (sd.exists(filename) && !sd.remove(filename)) {
        Serial.println("Failed init remove file");
        return;
    }
    // Important note!
    // You should use flag O_RDWR even if you use CSV File
    // only for writting.
    if (!csv.open(filename, O_WRONLY | O_APPEND | O_CREAT)) {
        Serial.println("Failed open file");
    }
}

void write() {
    // Data in CSV file is stored in lines.
    // Each line have some (or zero) fields.
    // First you should add line and next
    // add fields. After you can add next line.

    // Each line is ended by end line character '\n',
    // (UNIX style - without '\r').
    // You shouldn't use "println" method (and similars).
    // The fields are separated by delimiter ';'.
    // You can change this character in source file.
    // Your CSV file shouldn't contain this characters.

    // Important note!
    // You should use flag O_RDWR for initialize CSV File even if you use CSV File
    // only for writting.

    initSdFile("dataLogging.csv");

    // At the begin of file we don't need
    // add new line.

    // 2. Number field with non-fixed size.
    //    Use this field if you don't need
    //    edit field's value later.
    //    Support only positive integers.
    //    It is function designed for write
    //    line numbers.

    csv.addField(t);

    // N act (filt) - 0xA8
    csv.addField(Nact);

    // Vout - 0x8A
    csv.addField(Vout);

    // Iq cmd - 0x26
    csv.addField(Iq_cmd);

    // Iq actual - 0x27
    csv.addField(Iq_actual);

    // M out - 0xA0
    csv.addField(Mout);

    // I lim inuse - 0x48
    csv.addField(I_lim_inuse);

    // I act (filt) - 0x5F
    csv.addField(I_actual_filtered);

    // T-peak - 0xF0
    csv.addField(Tpeak);

    // Imax pk - 0xC4
    csv.addField(Imax_peak);

    // I con eff - 0xC5
    csv.addField(I_con_eff);

    // T-motor - 0x49
    csv.addField(motorTemp);

    // T-igbt - 0x4A
    csv.addField(powerStageTemp);

    // SoC
    csv.addField(soc);

    // V bat
    csv.addField(packVoltage);

    // I bat
    csv.addField(current);

    csv.addLine();

    // We don't add empty line at the end of file.
    // CSV file shouldn't end by '\n' char.

    // Don't forget close the file.
    csv.close();

    t++;

    // After this operations your CSV file should look like this
    // ('\0' is null character):

    /*0;65535;3444\n
     */
}
