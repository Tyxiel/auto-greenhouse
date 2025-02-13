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

// Servos
Servo sLeft;
const byte servoLeftPin = 11;
Servo sRight;
const byte servoRightPin = 10;
unsigned short int position = 0;

// Humidity Sensor
const byte humVCC = 12;
const byte humSensor = A0;
unsigned short int humidity = 0;

// Brightness
const byte brightnessSensor = A1;
unsigned short int brightness = 0;

// Piezo
const byte piezo = 9;

// ESP32
const byte esp32 = 13;

// LED
const byte ledRed = A2;
const byte ledYellow = A3;
const byte ledGreen = A4;

void setup() {
  // LCD
  lcd.begin(16, 2);
  
  // Servos
  sLeft.attach(servoLeftPin);
  sRight.attach(servoRightPin);
  sLeft.write(0);
  sRight.write(0);
  
  // Humidity Sensor
  pinMode(humVCC, OUTPUT);
  pinMode(humSensor, INPUT);
  
  // Brigthness Sensor
  pinMode(brightnessSensor, INPUT);
  
  // Piezo
  pinMode(piezo, OUTPUT);
  
  // LED
  pinMode(ledRed, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  
  // ESP32
  pinMode(esp32, OUTPUT);
  
  // Serial
  Serial.begin(9600);
}

void loop() {
  reset();
  humRead();
  brightRead();
  delay(500);
  
  // Sun
  if (brightness > 90 && position != 45) {
    unsigned short int position = 45;
    sLeft.write(45);
    sRight.write(position);
    //avisar no esp
  } else {
  	unsigned short int position = 0;
    sLeft.write(position);
    sRight.write(position);
  }
  
  // Humidity
  if(humidity < 25){
    digitalWrite(ledRed, HIGH);
    // tone(piezo, 5000, 10000);
    //avisar pelo esp
  } else if(humidity < 60) {
     digitalWrite(ledYellow, HIGH);
  } else {
    digitalWrite(ledGreen, HIGH);
  }
  
  
  delay(250);
}

void humRead() {
  digitalWrite(humVCC, HIGH);
  
  delay(20);
  
  humidity = analogRead(humSensor);
  // Tinkercad
  // humidity = map(humidity, 0, 872, 0, 100);

  // Real Life
  humidity = map(humidity, 1023, 372, 0, 100);
  
  digitalWrite(humVCC, LOW);
  
  Serial.println("Humidity: " + String(humidity));
  lcd.setCursor(0, 0);
  lcd.print("Humidity: " + String(humidity));
}

void brightRead() {
  brightness = analogRead(brightnessSensor);
  // brightness = map(brightness, 54, 974, 0, 100);
  
  Serial.println("Brightness: " + String(brightness));
  lcd.setCursor(0, 1);
  lcd.print("Brightness: " + String(brightness));
}

// Reset LCD, Piezo and LED
void reset() {
  // LCD
  lcd.clear();
  
  // Piezo
  noTone(piezo);
  
  // LED
  digitalWrite(ledRed, LOW);
  digitalWrite(ledYellow, LOW);
  digitalWrite(ledGreen, LOW);
}
