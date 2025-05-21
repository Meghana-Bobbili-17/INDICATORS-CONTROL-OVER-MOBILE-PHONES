#include "application_soft.h"

// Store pins
static uint8_t LEFT_BUTTON_PIN;
static uint8_t RIGHT_BUTTON_PIN;
static uint8_t LEFT_LED_PIN;
static uint8_t RIGHT_LED_PIN;

// Forward declare base functions implemented in .ino
extern bool baseReadButton(uint8_t pin);
extern void baseWriteLed(uint8_t pin, uint8_t pwmValue);
extern void basePrint(const char* msg);

// Timing constants
#define PRESS_TIME_MS 1000
#define BLINK_INTERVAL_MS 300

// State variables
static bool leftIndicatorOn = false;
static bool rightIndicatorOn = false;
static bool hazardOn = false;

static unsigned long leftPressStart = 0;
static unsigned long rightPressStart = 0;
static unsigned long lastBlinkTime = 0;
static bool ledBlinkState = false;

void appInitialize(uint8_t leftBtn, uint8_t rightBtn, uint8_t leftLed, uint8_t rightLed) {
  LEFT_BUTTON_PIN = leftBtn;
  RIGHT_BUTTON_PIN = rightBtn;
  LEFT_LED_PIN = leftLed;
  RIGHT_LED_PIN = rightLed;

  basePrint("Application initialized");
}

static void updateButtonPress(unsigned long &pressStart, bool pressed) {
  if (pressed) {
    if (pressStart == 0) {
      pressStart = millis();
    }
  } else {
    pressStart = 0;
  }
}

static bool isLongPress(unsigned long pressStart) {
  return (pressStart != 0) && ((millis() - pressStart) >= PRESS_TIME_MS);
}

static void toggleLeftIndicator() {
  if (rightIndicatorOn) {
    rightIndicatorOn = false;
    basePrint("Right indicator OFF");
  }
  leftIndicatorOn = !leftIndicatorOn;
  basePrint(leftIndicatorOn ? "Left indicator ON" : "Left indicator OFF");
}

static void toggleRightIndicator() {
  if (leftIndicatorOn) {
    leftIndicatorOn = false;
    basePrint("Left indicator OFF");
  }
  rightIndicatorOn = !rightIndicatorOn;
  basePrint(rightIndicatorOn ? "Right indicator ON" : "Right indicator OFF");
}

static void turnOffAllIndicators() {
  leftIndicatorOn = false;
  rightIndicatorOn = false;
  hazardOn = false;
  basePrint("All indicators OFF");
}

static void handleButtons() {
  bool leftPressed = baseReadButton(LEFT_BUTTON_PIN);
  bool rightPressed = baseReadButton(RIGHT_BUTTON_PIN);

  updateButtonPress(leftPressStart, leftPressed);
  updateButtonPress(rightPressStart, rightPressed);

  // Hazard activation: both buttons long pressed
  if (isLongPress(leftPressStart) && isLongPress(rightPressStart)) {
    if (!hazardOn) {
      hazardOn = true;
      leftIndicatorOn = false;
      rightIndicatorOn = false;
      basePrint("Hazard ON");
    }
    leftPressStart = 0;
    rightPressStart = 0;
    return;
  }

  // Hazard deactivation: any button long press when hazard active
  if (hazardOn && (isLongPress(leftPressStart) || isLongPress(rightPressStart))) {
    hazardOn = false;
    basePrint("Hazard OFF");
    leftPressStart = 0;
    rightPressStart = 0;
    return;
  }

  // Toggle left indicator
  if (isLongPress(leftPressStart) && !hazardOn) {
    toggleLeftIndicator();
    leftPressStart = 0;
  }

  // Toggle right indicator
  if (isLongPress(rightPressStart) && !hazardOn) {
    toggleRightIndicator();
    rightPressStart = 0;
  }
}

static void updateLEDs() {
  unsigned long now = millis();
  if (now - lastBlinkTime >= BLINK_INTERVAL_MS) {
    lastBlinkTime = now;
    ledBlinkState = !ledBlinkState;

    if (hazardOn) {
      baseWriteLed(LEFT_LED_PIN, ledBlinkState ? 255 : 0);
      baseWriteLed(RIGHT_LED_PIN, ledBlinkState ? 255 : 0);
    } else {
      baseWriteLed(LEFT_LED_PIN, (leftIndicatorOn && ledBlinkState) ? 255 : 0);
      baseWriteLed(RIGHT_LED_PIN, (rightIndicatorOn && ledBlinkState) ? 255 : 0);
    }
  }
}

void appUpdate() {
  handleButtons();
  updateLEDs();
}
