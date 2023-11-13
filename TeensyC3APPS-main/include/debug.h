
/**
 * THIS FILE DEFINES MACROS FOR INTERNAL LOGGING PURPOSES *
 */
#ifndef _LOG_H_
#define _LOG_H_

#include <Arduino.h>

/**
 * @brief Logs the given formatted message using the given prefix.
 *
 * @param prefix the prefix to use when logging this message
 */
#define _LOG(prefix, ...) Serial.printf("[" prefix "] " __VA_ARGS__)

/**
 * Prints the formatted message with level INFO
 */
#define INFO(...) _LOG("INFO", __VA_ARGS__)

/**
 * Prints the formatted message with level LOG
 */
#define LOG(...) _LOG("LOG", __VA_ARGS__)

/**
 * Prints the formatted message with level ALERT
 */
#define ALERT(...) _LOG("ALERT", __VA_ARGS__)

/**
 * Prints the formatted message with level ERROR
 */
#define ERROR(...) Serial.printf("[ERROR] " __VA_ARGS__)

// uncomment this line to enable debug messages
// #define APPS_DEBUG
// #define CAN_DEBUG
// #define DISPLAY_DEBUG
// #define MAIN_DEBUG
// #define R2D_DEBUG

// uncomment these lines to enable aditional data display and data logging
#define DATA_DISPLAY 0
// #define DATA_LOGGING

#endif  // _LOG_H_