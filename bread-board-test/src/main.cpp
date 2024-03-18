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

void loop() {

  if(digitalRead(BUTTON_1) == HIGH){
    Serial.println("Botão 1 pressionado");
    digitalWrite(GREEN_LED_1, HIGH);
  }
  if(digitalRead(BUTTON_2) == HIGH){
    Serial.println("Botão 2 pressionado");
    digitalWrite(GREEN_LED_2, HIGH);
  }
  if(digitalRead(BUTTON_3) == HIGH){
    Serial.println("Botão 3 pressionado");
    digitalWrite(GREEN_LED_3, HIGH);
  }
  if(digitalRead(BUTTON_4) == HIGH){
    Serial.println("Botão 4 pressionado");
    digitalWrite(RED_LED_1, HIGH);
  }
  if(digitalRead(BUTTON_5) == HIGH){
    Serial.println("Botão 5 pressionado");
    digitalWrite(RED_LED_2, HIGH);
  }
  if(digitalRead(BUTTON_6) == HIGH){
    Serial.println("Botão 6 pressionado");
    digitalWrite(YELLOW_LED_1, HIGH);
  }

  int potencio = analogRead(POTENCIO);
  Serial.println(potencio);


  if (potencio > 512) {
    digitalWrite(YELLOW_LED_2, HIGH);
  } else {
    digitalWrite(YELLOW_LED_2, LOW);
  }

  delay(1000);
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}