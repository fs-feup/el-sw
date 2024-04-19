#include "unity.h"
#include "rpm.h"

/*
* @brief Test case for sendRPM()
* check if the conversion is done correctly
*/

void test_RRRPM_2_BYTE() {
    float rr_rpm=126.23;
    char *rr_rpm_byte=0;
    rpm_2_byte(rr_rpm, rr_rpm_byte);

    int32_t RPMAux = (rr_rpm_byte[4] << 24) | (rr_rpm_byte[3] << 16) | (rr_rpm_byte[2] << 8) | rr_rpm_byte[1];
    
    TEST_ASSERT_EQUAL(12623, RPMAux);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_RRRPM_2_BYTE);
    UNITY_END();

    return 0;
}