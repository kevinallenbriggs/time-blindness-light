#include <Arduino.h>
#include <Bounce2.h>
#include <arduino-timer.h>

// output pins
const int redPin = 9;
const int greenPin = 10;
const int bluePin = 11;

// input pins
const int buttonPin = 2;  // momentary switch

// debouncing
Bounce bounce = Bounce();

// LED management
int brightness = 0;  // how bright the LED is (0-255)

// timer management
auto timer = timer_create_default();
const unsigned int phaseTimeLimitMillis = 3000;   // ms
unsigned int currentPhase = 0;
const int phaseCount = 5;

// reference
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
 * Set an LED color and brightness using the constants defined in this script.
*/
void setColor(int color, int brightness = 255) {
  switch (color) {
    case RED: setrgb(brightness, 0, 0); break;
    case BLUE: setrgb(0, 0, brightness); break;
    case GREEN: setrgb(0, brightness, 0); break;
    case CYAN: setrgb(0, brightness, brightness); break;
    case PURPLE: setrgb(brightness, 0, brightness); break;
    case YELLOW: setrgb(brightness, brightness, 0); break;
    default: setrgb(0,0,0);
  }
}

/**
 * Blink the LEDs.
*/
void blink(int color, int count, int brightness) {
  for (int i = 1; i <= count; i++) {
    setrgb(0, 0, 0);
    delay(500);
    setColor(color, brightness);
    delay(500);
  }
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

  bounce.attach(buttonPin, INPUT);
  bounce.interval(5);
}

bool phaseChange(void *phase) {
  Serial.println("current phase: " + String(currentPhase));
  if (currentPhase == 0) {
    currentPhase = ++currentPhase;
  } else if (currentPhase > phaseCount) {
    currentPhase = 0;
  } else {
    currentPhase++;
  }

  Serial.println("new phase: " + String(currentPhase));
  return true;
} // phaseChange

/**
 * meat and potatoes. runs over and over until powered off.
*/
void loop() {
  timer.tick();
  bounce.update();

  // detect button push
  if (bounce.changed()) {
    int debouncedInput = bounce.read();

    if (debouncedInput == HIGH) {
      timer.every(phaseTimeLimitMillis, phaseChange);
    } /* else if (debouncedInput == HIGH) {
      timer.cancel();
    } */
  }

    const int ticks = timer.ticks();
    if (ticks && ticks % 1000 == 0) {
      Serial.println("timer: " + String(timer.ticks()));
      Serial.println("phase: " + String(currentPhase));
    }

    brightness = (currentPhase > 0) ? 255 : 0;   // placeholder for color & pentiometer stuff

    switch(currentPhase) {
      case 1: setColor(GREEN, brightness); break;
      case 2: setColor(CYAN, brightness); break;
      case 3: setColor(PURPLE, brightness); break;
      case 4: setColor(YELLOW, brightness); break;
      case 5: setColor(RED, brightness); break;
      case 6: blink(RED, 3, brightness); break;
      default: setrgb(0, 0, 0); break;
    }
}

// legacy pentiometer/brightness stuff
      //   int sensorValue = analogRead(pentPin);
      //   int newBrightness = 0;
      //   newBrightness = sensorValue / fadeInterval;

      //   // ensure limits
      //   if (newBrightness <= 5 ) newBrightness = 5;
      //   if ( newBrightness >= 255) newBrightness = 255;

      //   brightness = newBrightness;

      //   // set the brightness of the LED
      //   analogWrite(ledPin, newBrightness);
