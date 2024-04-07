
#include <unity.h>

#include "comm/utils.hpp"

void setUp(void) {
    // This is run before EACH test

}

void tearDown(void) {
    // This is run after EACH test
}

/**
 * @brief Test if the message wheel rpm message is
 * created correctly for a 0.0 rpm value
 * 
*/
void wss_message_zero_rpm(void) {
    uint8_t msg[5];
    create_left_wheel_msg(msg, 0.0);
    TEST_ASSERT_EQUAL_HEX8(0x00, msg[1]);
    TEST_ASSERT_EQUAL_HEX8(0x00, msg[2]);
    TEST_ASSERT_EQUAL_HEX8(0x00, msg[3]);
    TEST_ASSERT_EQUAL_HEX8(0x00, msg[4]);
}

/**
 * @brief Test if the message wheel rpm message is
 * created correctly for a large rpm value
 * 
*/
void wss_message_high_rpm(void) {
    uint8_t msg[5];
    create_left_wheel_msg(msg, 1002.1231213);
    TEST_ASSERT_EQUAL_HEX8(0x74, msg[1]);
    TEST_ASSERT_EQUAL_HEX8(0x87, msg[2]);
    TEST_ASSERT_EQUAL_HEX8(0x01, msg[3]);
    TEST_ASSERT_EQUAL_HEX8(0x00, msg[4]);
}

/**
 * @brief Test if the message wheel rpm message is
 * created correctly for a negative rpm value
 * 
*/
void wss_message_negative_rpm(void) {
    uint8_t msg[5];
    create_left_wheel_msg(msg, -10.324);
    TEST_ASSERT_EQUAL_HEX8(0x00, msg[1]);
    TEST_ASSERT_EQUAL_HEX8(0x00, msg[2]);
    TEST_ASSERT_EQUAL_HEX8(0x00, msg[3]);
    TEST_ASSERT_EQUAL_HEX8(0x00, msg[4]);
}

/**
 * @brief Test if the message wheel rpm message is
 * created correctly for a small rpm value
 * 
*/
void wss_message_small_rpm(void) {
    uint8_t msg[5];
    create_left_wheel_msg(msg, 0.324235235);
    TEST_ASSERT_EQUAL_HEX8(0x20, msg[1]);
    TEST_ASSERT_EQUAL_HEX8(0x00, msg[2]);
    TEST_ASSERT_EQUAL_HEX8(0x00, msg[3]);
    TEST_ASSERT_EQUAL_HEX8(0x00, msg[4]);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(wss_message_zero_rpm);
    RUN_TEST(wss_message_high_rpm);
    RUN_TEST(wss_message_negative_rpm);
    RUN_TEST(wss_message_small_rpm);
    return UNITY_END();
}