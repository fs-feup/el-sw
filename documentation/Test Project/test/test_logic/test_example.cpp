#include <unity.h>

#include "test_utils.hpp"

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_calculator_addition);
    RUN_TEST(test_function_addition);
    UNITY_END();

    return 0;
}