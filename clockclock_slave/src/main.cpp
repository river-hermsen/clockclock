#include <Arduino.h>
#include <Stepper.h>

// BLINK LED VAR
#define LED_PIN 9
unsigned long previousMillis = 0;
const long interval = 250;
int ledState = LOW;

const int stepsPerRevolution = 720;
Stepper hourHand(stepsPerRevolution, 5, 6, 7, 8);
Stepper minuteHand(stepsPerRevolution, 14, 15, 16, 17);

void setup() {
  // LED SETUP
  pinMode(LED_PIN, OUTPUT);
  // STEPPER SETUP
  hourHand.setSpeed(20);
  minuteHand.setSpeed(20);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;

    digitalWrite(LED_PIN, ledState);
  }

  hourHand.step(stepsPerRevolution);
  minuteHand.step(stepsPerRevolution);
}