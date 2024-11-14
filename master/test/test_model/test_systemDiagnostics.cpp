#include "model/systemDiagnostics.hpp"
#include "unity.h"

void test_hasAnyComponentTimedOut(void) {
  FailureDetection fd;
  TEST_ASSERT_FALSE(fd.has_any_component_timed_out());
  delay(COMPONENT_TIMESTAMP_TIMEOUT + 1);
  TEST_ASSERT_TRUE(fd.has_any_component_timed_out());
}

void setUp(void) {}

void tearDown(void) {}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_hasAnyComponentTimedOut);
  return UNITY_END();
}