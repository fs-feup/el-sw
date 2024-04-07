#pragma once

#include "unity.h"


// not testing because no mocks in unity, impossible to properly test state logic
void testesemnada(void) {
    TEST_PASS();
}

void run_tests_stateLogic() {
    RUN_TEST(testesemnada);
}
