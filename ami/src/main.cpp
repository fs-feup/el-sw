#include <Arduino.h>

#define NUM_PINS 7
#define PRESSED_STATE LOW

// Use ATtiny pin identifiers
int inputPins[NUM_PINS] = {0, 1, 2, 3, 4, 5, 8};
int outputPins[NUM_PINS] = {15, 14, 13, 12, 11, 10, 9};

void setup()
{
  for (int i = 0; i < NUM_PINS; i++) {
    pinMode(outputPins[i], OUTPUT);
    pinMode(inputPins[i], INPUT_PULLUP);
  }
  digitalWrite(outputPins[6], HIGH);
}

/**
 * @brief AMI main loop: 
 * Button gets picked and lights up the corresponding LED
 * LAST BUTTON CHECKED WINS
*/
void loop() {
  int currentButtonPressed = 6;

  for (int i = 0; i < NUM_PINS; i++)
  {
    if (digitalRead(inputPins[i]) == PRESSED_STATE)
    {
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