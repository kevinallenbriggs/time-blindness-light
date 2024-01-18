#include <Arduino.h>
#include <Bounce2.h>
#include <arduino-timer.h>

// pins
const int redPin = 9;
const int greenPin = 10;
const int bluePin = 11;
const int buttonPin = 2;  // momentary switch

// debouncing
Bounce bounce = Bounce();

// timer management
const unsigned long MINUTE_IN_MS = 60UL * 1000UL;
const unsigned long phaseTimeLimitMillis = 9UL * MINUTE_IN_MS;
unsigned int currentPhase = 0;
const int phaseCount = 6;
auto phaseTimer = timer_create_default();

// color constants for readability
const int RED = 0;
const int GREEN = 1;
const int BLUE = 2;
const int CYAN = 3;
const int PURPLE = 4;
const int YELLOW = 5;

/**
 * Validates data and writes it to the 3 LED analog pins.
*/
void setrgb(int red, int green, int blue) {
  red = red > 255 ? 255 : red;
  green = green > 255 ? 255 : green;
  blue = blue > 255 ? 255 : blue;

  red = red < 0 ? 0 : red;
  green = green < 0 ? 0 : green;
  blue = blue < 0 ? 0 : blue;

  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

/**
 * Set an LED color and value using the constants defined in this script.
*/
void setColor(int color, int value = 255) {
  switch (color) {
    case RED: setrgb(value, 0, 0); break;
    case BLUE: setrgb(0, 0, value); break;
    case GREEN: setrgb(0, value, 0); break;
    case CYAN: setrgb(0, value, value); break;
    case PURPLE: setrgb(value, 0, value); break;
    case YELLOW: setrgb(value, value, 0); break;
    default: setrgb(0,0,0);
  }
}

/**
 * Blink the LEDs.
*/
void blink(int color, int count = 6) {
  for (int i = 1; i <= count; i++) {
    setrgb(0, 0, 0);
    delay(500);
    setColor(color);
    delay(500);
  }
}

bool changePhase(void *arg) {
  Serial.println("current phase: " + String(currentPhase));
  currentPhase = (currentPhase > phaseCount) ? 0 : currentPhase + 1;
  Serial.println("new phase: " + String(currentPhase));

  return (currentPhase == 0) ? false : true;
}

void toggleTimer() {
  if (! phaseTimer.empty()) {
    currentPhase = 0;
    phaseTimer.cancel();
    return;
  }

  if (currentPhase == 0) {
    currentPhase = 1;
  }

  phaseTimer.every(phaseTimeLimitMillis, changePhase);
}

/**
 * define inputs and outputs, start serial connections, set state or anything
 * else that only need be run once.
*/
void setup() {
  pinMode(buttonPin, INPUT);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(9600);
  Serial.println("phase time limit: " + String(phaseTimeLimitMillis));

  bounce.attach(buttonPin, INPUT);
  bounce.interval(5);
}

/**
 * meat and potatoes. runs over and over until powered off.
*/
void loop() {
  phaseTimer.tick();
  bounce.update();

  // react to button push
  if (bounce.changed() && bounce.read() == HIGH) toggleTimer();

  // debugging
  const int ticks = phaseTimer.ticks();
  if (ticks && ticks % 1000 == 0) {
    Serial.println("timer: " + String(phaseTimer.ticks()));
    Serial.println("phase: " + String(currentPhase));
  }

  // set LED color
  switch(currentPhase) {
    case 1: setColor(GREEN); break;
    case 2: setColor(CYAN); break;
    case 3: setColor(PURPLE); break;
    case 4: setColor(YELLOW); break;
    case 5: setColor(RED); break;
    case 6: blink(RED, 5); break;
    default: setrgb(0, 0, 0); break;
  }
}