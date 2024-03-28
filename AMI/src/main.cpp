#include <Arduino.h>
#include <Bounce2.h>

#define NUM_PINS 7
#define DEBOUNCE_INTERVAL 5
#define PRESSED_STATE LOW

// Use ATtiny pin identifiers
int inputPins[NUM_PINS] = {PA2, PA3, PA4, PA5, PA6, PA7, PB0};
int outputPins[NUM_PINS] = {PB1, PB2, PB3, PB4, PB5, PA0, PA1};
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

/**
 * @brief AMI main loop: 
 * Button gets picked and lights up the corresponding LED
 * LAST BUTTON CHECKED WINS
*/
void loop() {
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