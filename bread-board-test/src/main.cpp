#include <Arduino.h>

#define GREEN_LED_1 4
#define GREEN_LED_2 5
#define GREEN_LED_3 6
#define RED_LED_1 0
#define RED_LED_2 1
#define YELLOW_LED_1 2
#define YELLOW_LED_2 3
#define BUTTON_1 7
#define BUTTON_2 8
#define BUTTON_3 9
#define BUTTON_4 10
#define BUTTON_5 11
#define BUTTON_6 12
#define POTENCIO 14

void setup() {
  pinMode(GREEN_LED_1, OUTPUT);
  pinMode(GREEN_LED_2, OUTPUT);
  pinMode(GREEN_LED_3, OUTPUT);
  pinMode(RED_LED_1, OUTPUT);
  pinMode(RED_LED_2, OUTPUT);
  pinMode(YELLOW_LED_1, OUTPUT);
  pinMode(YELLOW_LED_2, OUTPUT);
  pinMode(BUTTON_1, INPUT);
  pinMode(BUTTON_2, INPUT);
  pinMode(BUTTON_3, INPUT);
  pinMode(BUTTON_4, INPUT);
  pinMode(BUTTON_5, INPUT);
  pinMode(BUTTON_6, INPUT);
  pinMode(POTENCIO, INPUT);
  Serial.begin(9600);
}

void reset() {
  Serial.println("Reseting");
  digitalWrite(GREEN_LED_1, LOW);
  digitalWrite(GREEN_LED_2, LOW);
  digitalWrite(GREEN_LED_3, LOW);
  digitalWrite(RED_LED_1, LOW);
  digitalWrite(RED_LED_2, LOW);
  digitalWrite(YELLOW_LED_1, LOW);
  digitalWrite(YELLOW_LED_2, LOW);
}

void loop() {

  if(digitalRead(BUTTON_1) == LOW){
    Serial.println("Botão 1 pressionado");
    digitalWrite(RED_LED_1, HIGH);
    delay(10);
    reset();
  }
  if(digitalRead(BUTTON_2) == LOW){
    Serial.println("Botão 2 pressionado");
    digitalWrite(RED_LED_2, HIGH);
    delay(10);
    reset();
  }
  if(digitalRead(BUTTON_3) == LOW){
    Serial.println("Botão 3 pressionado");
    digitalWrite(YELLOW_LED_1, HIGH);
    delay(10);
    reset();
  }
  if(digitalRead(BUTTON_4) == LOW){
    Serial.println("Botão 4 pressionado");
    digitalWrite(YELLOW_LED_2, HIGH);
    delay(10);
    reset();
  }
  if(digitalRead(BUTTON_5) == LOW){
    Serial.println("Botão 5 pressionado");
    digitalWrite(GREEN_LED_1, HIGH);
    delay(10);
    reset();
  }
  if(digitalRead(BUTTON_6) == LOW){
    Serial.println("Botão 6 pressionado");
    digitalWrite(GREEN_LED_2, HIGH);
    delay(10);
    reset();
  }

  int potencio = analogRead(POTENCIO);
  Serial.print("Potenciometro: ");
  Serial.println(potencio);

  // Turn last green led on if potencio is greater than 512
  if (potencio > 512) {
    digitalWrite(GREEN_LED_3, HIGH);
  } else {
    digitalWrite(GREEN_LED_3, LOW);
  }
}