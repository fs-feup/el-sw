#include <Arduino.h>

#define NUM_PINS 7
#define PRESSED_STATE LOW

// Use ATtiny pin identifiers
int inputPins[NUM_PINS] = {2, 1, 0, 3, 4, 5, 8};
int outputPins[NUM_PINS] = {15, 10, 12, 11, 13, 14, 9}; //funcional

void setup()
{
  for (int i = 0; i < NUM_PINS; i++) {
    pinMode(outputPins[i], OUTPUT);
    pinMode(inputPins[i], INPUT_PULLUP);  
  }
}

/**
 * @brief AMI main loop: 
 * Button gets picked and lights up the corresponding LED
 * LAST BUTTON CHECKED WINS
*/
void loop() {
  int currentButtonPressed = -1;

  for (int i = 0; i < NUM_PINS; i++)
  {
    if (digitalRead(inputPins[i])==PRESSED_STATE)
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