#include <Arduino.h>
#include "application_soft.h"

// Pins
const uint8_t LEFT_BUTTON_PIN = 2;
const uint8_t RIGHT_BUTTON_PIN = 3;
const uint8_t LEFT_LED_PIN = 9;
const uint8_t RIGHT_LED_PIN = 10;

void setup() {
  Serial.begin(9600);

  pinMode(LEFT_BUTTON_PIN, INPUT_PULLUP);  // Buttons active LOW
  pinMode(RIGHT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LEFT_LED_PIN, OUTPUT);
  pinMode(RIGHT_LED_PIN, OUTPUT);

  appInitialize(LEFT_BUTTON_PIN, RIGHT_BUTTON_PIN, LEFT_LED_PIN, RIGHT_LED_PIN);
}

void loop() {
  appUpdate();
  delay(100);  // 100ms scheduler
}

// Base software functions for Application to use (optional)
bool baseReadButton(uint8_t pin) {
  return digitalRead(pin) == LOW;  // Active LOW buttons
}

void baseWriteLed(uint8_t pin, uint8_t pwmValue) {
  analogWrite(pin, pwmValue);
}

void basePrint(const char* msg) {
  Serial.println(msg);
}
