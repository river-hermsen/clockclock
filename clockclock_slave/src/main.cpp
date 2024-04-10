#include <Arduino.h>
// #include <Stepper.h>

#include <AsyncStepper.hpp>

// BLINK LED VAR
#define LED_PIN 9

unsigned long previousMillis = 0;
const long interval = 250;
int ledState = LOW;

const int stepsPerRevolution = 720;
// 4, 3, 16, 17
// 5, 6, 7, 8
const int hourMotorPins[4] = {2, 3, 4, 5};
int stepCounterHour = 0;
const int numStepsHour = 8;
const int stepsLookup[8] = {B1000, B1100, B0100, B0110,
                            B0010, B0011, B0001, B1001};

// Code to drive the NoDrive stepper
void setOutput(int step) {
  Serial.println(step);
  digitalWrite(hourMotorPins[0], bitRead(stepsLookup[step], 0));
  digitalWrite(hourMotorPins[1], bitRead(stepsLookup[step], 1));
  digitalWrite(hourMotorPins[2], bitRead(stepsLookup[step], 2));
  digitalWrite(hourMotorPins[3], bitRead(stepsLookup[step], 3));
}

void clockwise() {
  stepCounterHour++;
  if (stepCounterHour >= numStepsHour) stepCounterHour = 0;
  setOutput(stepCounterHour);
}

void anticlockwise() {
  stepCounterHour--;
  if (stepCounterHour < 0) stepCounterHour = numStepsHour - 1;
  setOutput(stepCounterHour);
}

const int stepper_steps = 200;

AsyncStepper stepperHour(
    stepper_steps, []() { clockwise(); }, []() { anticlockwise(); });

// CALIBRATION SETUP
#define HALL_HOUR_PIN A0
#define HALL_MIN_PIN A1
const int change_threshold = 3;
bool isMinCalibrated = false;
bool isHourCalibrated = false;

void setup() {
  Serial.begin(115200);
  Serial.println("STARTING...");
  // LED SETUP
  pinMode(LED_PIN, OUTPUT);
  pinMode(HALL_HOUR_PIN, INPUT);
  pinMode(HALL_MIN_PIN, INPUT);
  // STEPPER SETUP
  pinMode(hourMotorPins[0], OUTPUT);
  pinMode(hourMotorPins[1], OUTPUT);
  pinMode(hourMotorPins[2], OUTPUT);
  pinMode(hourMotorPins[3], OUTPUT);
  stepperHour.SetSpeedRpm(60);
  // stepperHour.RotateContinuous(AsyncStepper::CW);
  stepperHour.RotateAngle(270.0f, AsyncStepper::CW);
  // stepperHour.OnFinish = []() { Serial.println("Stopped"); };
  // digitalWrite(hourMotorPins[0], HIGH);
  // digitalWrite(hourMotorPins[1], LOW);
  // digitalWrite(hourMotorPins[2], HIGH);
  // digitalWrite(hourMotorPins[3], LOW);
}

bool hourHallChanges(int value) {
  static int oldValHour;
  bool changed = abs(value - oldValHour) >= change_threshold;
  if (changed) oldValHour = value;
  return changed;
}

bool minHallChanges(int value) {
  static int oldValMin;
  bool changed = abs(value - oldValMin) >= change_threshold;
  if (changed) oldValMin = value;
  return changed;
}

void calibrateHands() {
  int valHour = analogRead(HALL_HOUR_PIN);
  int valMin = analogRead(HALL_MIN_PIN);

  if (hourHallChanges(valHour)) {
    isHourCalibrated = true;
  }

  if (minHallChanges(valMin)) {
    digitalWrite(LED_PIN, HIGH);
    isMinCalibrated = true;
  }
}

void loop() {
  stepperHour.Update();

  // unsigned long currentMillis = millis();
  // if (currentMillis - previousMillis >= interval) {
  //   previousMillis = currentMillis;
  //   if (ledState == LOW)
  //     ledState = HIGH;
  //   else
  //     ledState = LOW;

  //   digitalWrite(LED_PIN, ledState);
  // }

  // int value = analogRead(REED_HOUR_PIN);
  // if (changed_significantly(value)) {
  //   digitalWrite(LED_PIN, HIGH);
  //   delay(100);
  //   digitalWrite(LED_PIN, LOW);
  // };

  // hourHand.step(stepsPerRevolution);
  // minuteHand.step(stepsPerRevolution);
}