#include "logging.h"
#include <iostream>
#include <string>

LogEntry globalLogEntry;
LogEntry *entry = &globalLogEntry;

extern FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
File myFile_setup;
File myFile2_setup;
File myFile;


constexpr size_t CANBUFSIZE = 100;
LogEntry lbuf[CANBUFSIZE];
volatile unsigned int lbuf_head = 0;
volatile unsigned int lbuf_tail = 0;
int t = 0;

char file_VD[16] = "VD_Test1.csv";
char file_powertrain[24] = "Powertrain_Test1.csv";

int count = 1;
int count2 = 1;

void getTimeStamp(LogEntry* logEntry)
{
    // read rtc (64bit, number of 32,768 kHz crystal periods)
    uint64_t periods;
    uint32_t hi1 = SNVS_HPRTCMR, lo1 = SNVS_HPRTCLR;
    while (true)
    {
        uint32_t hi2 = SNVS_HPRTCMR, lo2 = SNVS_HPRTCLR;
        if (lo1 == lo2 && hi1 == hi2)
        {
            periods = (uint64_t)hi2 << 32 | lo2;
            break;
        }
        hi1 = hi2;
        lo1 = lo2;
    }

    // calculate seconds and milliseconds
    uint32_t ms = (1000 * (periods % 32768)) / 32768;
    time_t sec  = periods / 32768;

    tm t                  = *gmtime(&sec); // calculate calendar data
    logEntry->second      = t.tm_sec;
    logEntry->minute      = t.tm_min;
    logEntry->hour        = t.tm_hour;
    logEntry->day         = t.tm_mday;
    logEntry->month       = t.tm_mon + 1;
    logEntry->year        = t.tm_year + 1900;
    logEntry->millisecond = ms;
}


void Logging::setup_log() {

    //SPI.setCS(PIN_SD_CS);    
    //SPI.setMISO(PIN_SPI_MISO);
    //SPI.setMOSI(PIN_SPI_MOSI);
    //SPI.setSCK(PIN_SPI_SCK);
    // Setup pinout
    pinMode(PIN_SPI_MOSI, OUTPUT);
    pinMode(PIN_SPI_MISO, INPUT);
    pinMode(PIN_SPI_SCK, OUTPUT);
    // Disable SPI devices
    pinMode(PIN_SD_CS, OUTPUT);
    digitalWrite(PIN_SD_CS, HIGH);


    // Setup serial
    Serial.begin(9600);
    
    // Setup SD card
    if (!SD.begin(BUILTIN_SDCARD)) {
        Serial.println("initialization failed!");
        return;
    }

    while(SD.exists(file_VD)) {
        snprintf(file_VD, sizeof(file_VD), "VD_Test%d.csv", count);
        count++;
    }
    
    myFile_setup = SD.open(file_VD, FILE_WRITE);
    myFile_setup.printf("TimeStamp, Current, Voltage, MinTmp, MaxTmp, AvgTmp, APPS1, APPS2, Brake \n");
    myFile_setup.close();

    while(SD.exists(file_powertrain)) {
        snprintf(file_powertrain, sizeof(file_powertrain), "Powertrain_Test%d.csv", count2);
        count2++;
    }
    
    myFile2_setup = SD.open(file_powertrain, FILE_WRITE);
    myFile2_setup.printf("TimeStamp, RPM, Motor Current, BAMO Temp, Motor Temp \n");
    myFile2_setup.close();
    

}
void Logging::write_to_file_VD(int current, int voltage, int mintmp, int maxtmp, int avgtmp, int apps1, int apps2, int brake) {
    
        //Serial.print("Starting to write...");

        myFile = SD.open(file_VD, FILE_WRITE);

        //getTimeStamp(entry);

        //myFile.printf("%d-%02d-%02d %02d:%02d:%02d.%03u \n", entry->year, entry->month, entry->day, entry->hour, entry->minute, entry->second, entry->millisecond);

        
        myFile.printf("%d, ", t);

        myFile.printf("%d, ",current);

        myFile.printf("%d, ",voltage);

        myFile.printf("%d, ",mintmp);

        myFile.printf("%d, ",maxtmp);

        myFile.printf("%d, ",avgtmp);

        myFile.printf("%d, ",apps1);
        
        myFile.printf("%d, ",apps2);

        myFile.printf("%d \n",brake);

        myFile.close();
}

void Logging::write_to_file_powertrain(int rpm, int I_actual, int powerStageTmp, int motorTmp) {
            //Serial.print("Starting to write...");

        myFile = SD.open(file_powertrain, FILE_WRITE);

        //getTimeStamp(entry);

        //myFile.printf("%d-%02d-%02d %02d:%02d:%02d.%03u \n", entry->year, entry->month, entry->day, entry->hour, entry->minute, entry->second, entry->millisecond);
        
        myFile.printf("%d, ", t);

        myFile.printf("%d, ",rpm);

        myFile.printf("%d, ",I_actual);

        myFile.printf("%d, ",powerStageTmp);

        myFile.printf("%d \n",motorTmp);

        myFile.close();

        t+=LOGGING_PERIOD;
}