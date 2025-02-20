// Libraries
#include <Servo.h>

// Servos
Servo sLeft;
const byte servoLeftPin = 11;
Servo sRight;
const byte servoRightPin = 10;

// Button
const byte buttonPin = 2;
bool lastButtonState = LOW;
bool currentState = LOW;
bool servoOpen = false;

void setup() {
  // Servos
  sLeft.attach(servoLeftPin);
  sRight.attach(servoRightPin);
  sLeft.write(0);
  sRight.write(0);

  // Button
  pinMode(buttonPin, INPUT_PULLUP);

  // Serial
  Serial.begin(9600);
}

void loop() {
  currentState = digitalRead(buttonPin);

  if (currentState == LOW && lastButtonState == HIGH) {
    servoOpen = !servoOpen;

    if (servoOpen) {
      sLeft.write(45);
      sRight.write(45);
    } else {
      sLeft.write(0);
      sRight.write(0);
    }
    
    delay(250);
  }

  lastButtonState = currentState;

  // Optional: Add a small delay to avoid rapid toggling
  delay(50);
}
