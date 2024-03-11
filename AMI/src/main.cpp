#include <Arduino.h>
#include <Bounce2.h>

#define NUM_PINS 7
#define DEBOUNCE_INTERVAL 5
#define PRESSED_STATE LOW

int inputPins[NUM_PINS] = {2, 3, 4, 5, 6, 7, 8};
int outputPins[NUM_PINS] = {9, 10, 11, 12, 13, 14, 15};
Bounce2::Button buttons[NUM_PINS];

void setup()
{
  for (int i = 0; i < NUM_PINS; i++) {
    pinMode(outputPins[i], OUTPUT);
    buttons[i].attach(inputPins[i], INPUT_PULLUP);
    buttons[i].interval(DEBOUNCE_INTERVAL);
    buttons[i].setPressedState(PRESSED_STATE);
  }
}

void loop()
{
  int currentButtonPressed = -1;

  for (int i = 0; i < NUM_PINS; i++)
  {
    buttons[i].update();
    if (buttons[i].pressed())
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