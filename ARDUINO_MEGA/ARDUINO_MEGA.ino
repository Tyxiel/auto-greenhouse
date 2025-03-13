// Libraries
#include <LiquidCrystal.h>
#include <Servo.h>

// LCD
const byte rs = 2;
const byte e = 3;
const byte db4 = 4;
const byte db5 = 5;
const byte db6 = 6;
const byte db7 = 7;
LiquidCrystal lcd(rs, e, db4, db5, db6, db7);

// Servo CEILING
Servo myServo;
const byte servoRightPin = 44;

// Button for Servo
const byte buttonPin = 32;
bool lastButtonState = LOW;
bool currentState = LOW;
bool servoOpen = false;

// Button for Fan
const byte fanButton = 33;  // Button to toggle fan on/off
bool lastFanButtonState = HIGH;  // Using INPUT_PULLUP: button is HIGH when not pressed
bool currentFanButtonState = HIGH;
bool fanEnabled = false;         // Fan starts off

// Humidity Sensor
const byte humVCC = 12;
const byte humSensor = A0;
unsigned short int humidity = 0;

// Brightness Sensor
const byte brightnessSensor = A1;
unsigned short int brightness = 0;

// Fan (controlled by the new fan button)
const byte fan = 8;

// ESP32
const byte esp32 = 13;

// LED
const byte ledPin = 11;

// Relays
const byte relayIrrigation = 9;
const byte relayDrainage = 10; 

void setup() {
  // LCD
  lcd.begin(16, 2);
  
  // Servo CEILING
  myServo.attach(servoRightPin);
  myServo.write(0);
  
  // Humidity Sensor
  pinMode(humVCC, OUTPUT);
  pinMode(humSensor, INPUT);
  
  // Brightness Sensor
  pinMode(brightnessSensor, INPUT);

  // Fan
  pinMode(fan, OUTPUT);
  digitalWrite(fan, LOW);  // Explicitly start fan off

  // LED
  pinMode(ledPin, OUTPUT);

  // Relays
  pinMode(relayIrrigation, OUTPUT);
  pinMode(relayDrainage, OUTPUT);
  digitalWrite(relayIrrigation, LOW);
  digitalWrite(relayDrainage, LOW);

  // Button for servo
  pinMode(buttonPin, INPUT_PULLUP);
  
  // Button for fan
  pinMode(fanButton, INPUT_PULLUP);

  // ESP32
  pinMode(esp32, OUTPUT);

  // Serial
  Serial.begin(9600);
  
  // Print fan status to LCD on startup
  lcd.setCursor(10, 1);
  lcd.print("FAN OFF");
}

void loop() {
  reset();
  humRead();
  brightRead();
  delay(500);
  
  // Display humidity result on LCD and control irrigation/drainage relays
  lcd.setCursor(9, 0);
  if (humidity < 25) {
    digitalWrite(relayIrrigation, HIGH);
    digitalWrite(relayDrainage, LOW);
    Serial.println("Irrigation pump ON");
    lcd.print("Irr ON");
  } else if (humidity > 60) {
    digitalWrite(relayIrrigation, LOW);
    digitalWrite(relayDrainage, HIGH);
    Serial.println("Drainage pump ON");
    lcd.print("Drn ON");
  } else {
    digitalWrite(relayIrrigation, LOW);
    digitalWrite(relayDrainage, LOW);
    Serial.println("Both pumps OFF");
  }

  // Brightness management for LED
  if (brightness < 75) {
    digitalWrite(ledPin, HIGH);
    Serial.println("LEDs ON");
  } else {
    digitalWrite(ledPin, LOW);
  }

  // Button for servo (toggle ceiling servo and esp32 signal)
  currentState = digitalRead(buttonPin);
  if (currentState == LOW && lastButtonState == HIGH) {
    servoOpen = !servoOpen;
    Serial.println("Servo button pressed");
    if (servoOpen) {
      myServo.write(45);
      digitalWrite(esp32, HIGH);
    } else {
      myServo.write(0);
      digitalWrite(esp32, LOW);
    }
  }
  lastButtonState = currentState;
  
  // Fan toggle button logic
  currentFanButtonState = digitalRead(fanButton);
  
  // Debug: print current fan button state
  Serial.print("Fan Button State: ");
  Serial.println(currentFanButtonState);
  
  // Since using INPUT_PULLUP, a press is detected when the button reads LOW.
  if (currentFanButtonState == LOW && lastFanButtonState == HIGH) {
    fanEnabled = !fanEnabled;
    
    if (fanEnabled) {
      digitalWrite(fan, HIGH);
      Serial.println("Fan turned ON");
    } else {
      digitalWrite(fan, LOW);
      Serial.println("Fan turned OFF");
    }
  }
  lastFanButtonState = currentFanButtonState;
  
  // Always display the fan status on the LCD at a fixed position.
  lcd.setCursor(10, 1);
  if (fanEnabled) {
    lcd.print("FAN ON ");
  } else {
    lcd.print("FAN OFF");
  }
  
  delay(500);
}

void humRead() {
  digitalWrite(humVCC, HIGH);
  delay(20);
  
  humidity = analogRead(humSensor);
  // Tinkercad simulation:
  // humidity = map(humidity, 0, 872, 0, 100);

  // Real-life calibration (adjust as needed)
  humidity = map(humidity, 1023, 372, 0, 100);
  
  digitalWrite(humVCC, LOW);
  
  Serial.println("Humidity: " + String(humidity));
  lcd.setCursor(0, 0);
  lcd.print("U: " + String(humidity) + " -");
}

void brightRead() {
  brightness = analogRead(brightnessSensor);
  // brightness = map(brightness, 930, 1023, 0, 100);
  
  Serial.println("Brightness: " + String(brightness));
  lcd.setCursor(0, 1);
  lcd.print("LUM: " + String(brightness));
}

// Reset LCD and add a blank line in Serial (currently only resets the LCD)
void reset() {
  lcd.clear();
  Serial.println(" ");
}
