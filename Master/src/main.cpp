
#include <Arduino.h>
#include "logic/StateLogic.hpp"
#include "logic/CheckupManager.hpp"

// put function declarations here:
int myFunction(int, int);

void setup() {
  CheckupManager *checkupManager = new CheckupManager();
  ASState* asState = new ASState(checkupManager);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}


