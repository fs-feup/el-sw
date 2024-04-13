#include "gtest/gtest.h"
#include "rpm.h"

/*
* @brief Test case for sendRPM()
* check if the conversion is done correctly
*/
TEST(C1Test, SendRPM1) {
    // Arrange
    // Set up any necessary variables or state.
    rr_rpm=126.23;

    // Act
    sendRPM();

    int32_t RPMAux = (rr_rpm_msg.buf[4] << 24) | (rr_rpm_msg.buf[3] << 16) | (rr_rpm_msg.buf[2] << 8) | rr_rpm_msg.buf[1];
    
    EXPECT_EQ(RPMAux, data.input);
}