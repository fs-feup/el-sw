#include <unity.h>

#include "comm/comms.hpp"

/**
 * @brief Test the communications module
 * Simply here to show the execution of 
 * CAN communication in desktop
*/
void test_comms(void) {
    example_usage_function();
    TEST_ASSERT(1 == 1);
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_comms);
    UNITY_END();

    return 0;
}