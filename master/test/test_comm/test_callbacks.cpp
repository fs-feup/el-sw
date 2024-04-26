#include <unity.h>
#include "comm/communicator.hpp"
#include "model/systemDiagnostics.hpp"

SystemData sd;
Communicator communicator(&sd);
CAN_message_t msg;

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

// TODO(andre): check lengths and bytes
void test_c1(void) {
    msg.id = C1_ID;
    msg.len = 5;
    msg.buf[0] = HYDRAULIC_LINE;
    msg.buf[1] = 0x01;
    msg.buf[2] = 0x01;
    msg.buf[3] = 0x00;
    msg.buf[4] = 0x00;
    communicator.parse_message(msg);
    TEST_ASSERT_EQUAL(257, sd.sensors._hydraulic_line_pressure);

    msg.buf[0] = RIGHT_WHEEL;
    msg.buf[1] = 0x00;
    msg.buf[2] = 0x08;
    communicator.parse_message(msg);

    if (sd.sensors._right_wheel_rpm == 20.48)
        TEST_PASS();
    else
        TEST_FAIL();
}

// TODO(andre): check lengths and bytes
void test_bamocar(void) {
    sd = SystemData(); // reset ready timestamp
    sd.failureDetection.ts_on = true;

    msg.id = BAMO_RESPONSE_ID;
    msg.len = 4;
    msg.buf[0] = BTB_READY;
    msg.buf[1] = 0x00;
    msg.buf[2] = 0x00;
    msg.buf[3] = 0x00;
    communicator.parse_message(msg);
    TEST_ASSERT_EQUAL(false, sd.failureDetection.ts_on);

    msg.buf[1] = 0x01;
    communicator.parse_message(msg);
    TEST_ASSERT_EQUAL(false, sd.failureDetection.ts_on);

    msg.buf[0] = VDC_BUS;
    msg.buf[2] = 0xf0;
    communicator.parse_message(msg);
    TEST_ASSERT_EQUAL(true, sd.failureDetection.ts_on);

    msg.buf[2] = 0x00;
    communicator.parse_message(msg);
    TEST_ASSERT_EQUAL(false, sd.failureDetection.ts_on);
}
// You can similarly write tests for other callback functions

void test_resState(void) {

    msg.id = RES_STATE;
    msg.len = 8;
    msg.buf[0] = 0x02; // Assuming emergency signals are set
    msg.buf[1] = 0x00;
    msg.buf[2] = 0x00;
    msg.buf[3] = 0x80;
    msg.buf[4] = 0x00;
    msg.buf[5] = 0x00;
    msg.buf[6] = 0x00;
    msg.buf[7] = 0x00;
    communicator.parse_message(msg);
    TEST_ASSERT_EQUAL(false, sd.r2dLogics.r2d);
    TEST_ASSERT_EQUAL(0, sd.failureDetection.radio_quality);

    // wait ready timeout
    Metro time{READY_TIMEOUT_MS};
    while (!time.check()){
    }

    msg.buf[6] = 0x64; // def radio quality 100
    communicator.parse_message(msg);
    TEST_ASSERT_EQUAL(true, sd.r2dLogics.r2d);
    TEST_ASSERT_EQUAL(100, sd.failureDetection.radio_quality);

    msg.buf[0] = 0x01;
    msg.buf[3] = 0x80; // def emg bits
    communicator.parse_message(msg);
    TEST_ASSERT_EQUAL(true, sd.failureDetection.emergencySignal);
    // switch order emg after
}


void setUp() {
    
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(wss_message_zero_rpm);
    RUN_TEST(wss_message_high_rpm);
    RUN_TEST(wss_message_negative_rpm);
    RUN_TEST(wss_message_small_rpm);
    RUN_TEST(test_c1);
    RUN_TEST(test_bamocar);
    RUN_TEST(test_resState);
    return UNITY_END();
}