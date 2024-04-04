
#include "unity.h"

void setUp(void) {
    // This is run before EACH test

}

void tearDown(void) {
    // This is run after EACH test
}

void teste_sem_nada(void) {
    // Call the loop function

}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(teste_sem_nada);
    return UNITY_END();
}