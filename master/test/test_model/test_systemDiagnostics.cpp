#include "unity.h"
#include "model/systemDiagnostics.hpp"

void test_hasAnyComponentTimedOut(void) {
    FailureDetection fd;
    TEST_ASSERT_FALSE(fd.hasAnyComponentTimedOut());
    delay(COMPONENT_TIMESTAMP_TIMEOUT + 1);
    TEST_ASSERT_TRUE(fd.hasAnyComponentTimedOut());
}

void setUp(void) {
}

void tearDown(void) {
}


int main () {
    UNITY_BEGIN();
    RUN_TEST(test_hasAnyComponentTimedOut);
    return UNITY_END();
}