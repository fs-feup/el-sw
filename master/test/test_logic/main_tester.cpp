#include <unity.h>
#include "test_checkupManager.hpp"
#include "test_stateLogic.hpp"

void setUp(void) {
}

void tearDown(void) {
}

int main() {
    UNITY_BEGIN();
    run_tests_checkupManager();
    run_tests_stateLogic();
    return UNITY_END();
}