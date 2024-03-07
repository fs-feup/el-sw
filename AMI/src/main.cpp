#include <Arduino.h>

#define NUM_PINS 7

int inputPins[NUM_PINS] = {2, 3, 4, 5, 6, 7, 8};
int outputPins[NUM_PINS] = {9, 10, 11, 12, 13, 14, 15};

void setup() {
  for (int i = 0; i < NUM_PINS; i++) {
    pinMode(inputPins[i], INPUT);
    pinMode(outputPins[i], OUTPUT);
  }
}

// LAST BUTTON CHECKED WINS
void loop() {
  int currentButtonPressed = -1;

  for (int i = 0; i < NUM_PINS; i++) {
    if (digitalRead(inputPins[i]) == LOW) {
      currentButtonPressed = i;
    }
  }

  if (currentButtonPressed != -1) {
    for (int i = 0; i < NUM_PINS; i++) {
      digitalWrite(outputPins[i], LOW);
    }

    digitalWrite(outputPins[currentButtonPressed], HIGH);
  }
}

// SIMPLEST WAY TO DO IT
// void loop() {
//   for (int i = 0; i < NUM_PINS; i++) {
//     if (digitalRead(inputPins[i]) == LOW) {
//       digitalWrite(outputPins[i], HIGH);
//     }
//     else {
//       digitalWrite(outputPins[i], LOW);
//     }
//   }
// }
