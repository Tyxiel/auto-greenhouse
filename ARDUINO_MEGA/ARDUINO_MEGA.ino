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

// Servo FAN
const byte servoPin = 22;
Servo myServo;
int servoPosition = 0;

// Servos CELLING
Servo sLeft;
const byte servoLeftPin = 11;
Servo sRight;
const byte servoRightPin = 10;

// Button
const byte buttonPin = 2;
bool lastButtonState = LOW;
bool currentState = LOW;
bool servoOpen = false;

// Humidity Sensor
const byte humVCC = 12;
const byte humSensor = A0;
unsigned short int humidity = 0;

// Brightness Sensor
const byte brightnessSensor = A1;
unsigned short int brightness = 0;

// Temperature Sensor (LM35)
const byte lm35 = A5;
const byte powerSupply = A3;
unsigned short int tempValue = 0;
float temperature = 0;

// Fan
const byte fan = 8;

// ESP32
const byte esp32 = 13;

// LED
const byte ledGreen = A4;
const byte ledPin = 11;

// Relays
const byte relayIrrigation = 9;
const byte relayDrainage = 10; 

void setup() {
  // LCD
  lcd.begin(16, 2);

  // Servo FAN
  myServo.attach(servoPin);
  myServo.write(servoPosition);
  
  // Servo CELLING
  sLeft.attach(servoLeftPin);
  sRight.attach(servoRightPin);
  sLeft.write(0);
  sRight.write(0);
  
  // Humidity Sensor
  pinMode(humVCC, OUTPUT);
  pinMode(humSensor, INPUT);
  
  // Brigthness Sensor
  pinMode(brightnessSensor, INPUT);

  // Temperature Sensor
  pinMode(lm35, INPUT);
  pinMode(powerSupply, OUTPUT);
  digitalWrite(powerSupply, HIGH);

  // Fan
  pinMode(fan, OUTPUT);
  
  // LED
  pinMode(ledGreen, OUTPUT);
  pinMode(ledPin, OUTPUT);

  // Relays
  pinMode(relayIrrigation, OUTPUT);
  pinMode(relayDrainage, OUTPUT);
  digitalWrite(relayIrrigation, LOW);
  digitalWrite(relayDrainage, LOW);

  // Button
  pinMode(buttonPin, INPUT_PULLUP);

  // ESP32
  pinMode(esp32, OUTPUT);

  // Serial
  Serial.begin(9600);
}

void loop() {
  reset();
  humRead();
  brightRead();
  tempRead();
  delay(500);
  
  lcd.setCursor(9, 0);
  // Humidity
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

  // Brightness
  if(brightness < 75) {
    digitalWrite(ledPin, HIGH);
    Serial.println("LEDs ON");
  } else {
    digitalWrite(ledPin, LOW);
  }

  // Temperature
  if(temperature > 30) {
    digitalWrite(fan, HIGH);
    servoPosition = 180;
  } else {
    digitalWrite(fan, LOW);
    servoPosition = 0;
  }

  myServo.write(servoPosition);

  if (currentState == LOW && lastButtonState == HIGH) {
    servoOpen = !servoOpen;

    if (servoOpen) {
      sLeft.write(45);
      sRight.write(45);
      digitalWrite(esp32, HIGH);
    } else {
      sLeft.write(0);
      sRight.write(0);
      digitalWrite(esp32, LOW);
    }
    
    delay(250);
  }

  lastButtonState = currentState;
  
  checkResources();
  
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
  lcd.print("U: " + String(humidity) + " -");
}

void brightRead() {
  brightness = analogRead(brightnessSensor);
  // brightness = map(brightness, 930, 1023, 0, 100);
  
  Serial.println("Brightness: " + String(brightness));
  lcd.setCursor(0, 1);
  lcd.print("LUM: " + String(brightness));
}

void tempRead() {
  tempValue = analogRead(lm35);
  temperature = (tempValue - 100.2) / 2.05;
  Serial.println(temperature);

  Serial.println("Temperature: " + String(temperature) + " °C");
  // Exibição da temperatura na LCD, atualizando a posição desejada
  char tempString[6];
  dtostrf(temperature, 5, 1, tempString);
  lcd.setCursor(7, 1);
  lcd.print("T: ");
  lcd.setCursor(9, 1);
  lcd.print(tempString);
  lcd.print(" C");
}

void checkResources() {
  bool allResourcesWorking = true;

  // Verifica a umidade
  if (humidity < 0 || humidity > 100) {
    allResourcesWorking = false;
    Serial.println("Humidity sensor error");
  }

  // Verifica a luminosidade
  if (brightness < 0 || brightness > 100) {
    allResourcesWorking = false;
    Serial.println("Brightness sensor error");
  }

  // Verifica a temperatura
  if (temperature < -40 || temperature > 125) {
    allResourcesWorking = false;
    Serial.println("Temperature sensor error");
  }

  // Se todos os recursos estão funcionando, liga o LED verde
  if (allResourcesWorking) {
    digitalWrite(ledGreen, HIGH);
  } else {
    digitalWrite(ledGreen, LOW);
  }
}

// Reset LCD, Piezo and LED
void reset() {
  // LCD
  lcd.clear();
  
  // LED
  digitalWrite(ledGreen, LOW);

  // Serial
  Serial.println(" ");
}
