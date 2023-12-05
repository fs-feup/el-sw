#ifndef TESTCOMM_TESTUTILS_HPP_
#define TESTCOMM_TESTUTILS_HPP_

#include <unity.h>

#include "logic/utils.hpp"

/**
 * @brief Test the sum operation of the Calculator class
*/
void test_calculator_addition(void) {
    // 
    Calculator calc;
    float res = calc.sum(1,2);
    TEST_ASSERT(res == 3);
}

void test_function_addition(void) {
    int res = summing(2,4);
    TEST_ASSERT_EQUAL_INT32(res, 6);
}

#endif 