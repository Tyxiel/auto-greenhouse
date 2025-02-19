// Libraries
#include <LiquidCrystal.h>

// LCD
const byte rs = 2;
const byte e = 3;
const byte db4 = 4;
const byte db5 = 5;
const byte db6 = 6;
const byte db7 = 7;

LiquidCrystal lcd(rs, e, db4, db5, db6, db7);

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

// LED
const byte ledGreen = A4;

// ESP32
const byte esp32 = 13;

// Relays
const byte relayIrrigation = 9;
const byte relayDrainage = 10; 

void setup() {
  // LCD
  lcd.begin(16, 2);
  
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
  
  // ESP32
  pinMode(esp32, OUTPUT);

  // Relays
  pinMode(relayIrrigation, OUTPUT);
  pinMode(relayDrainage, OUTPUT);
  digitalWrite(relayIrrigation, LOW);
  digitalWrite(relayDrainage, LOW);

  // Serial
  Serial.begin(9600);
}

void loop() {
  reset();
  humRead();
  brightRead();
  tempRead();
  delay(500);
  
  // Humidity
  if (humidity < 25) {
    digitalWrite(relayIrrigation, HIGH);
    digitalWrite(relayDrainage, LOW);
    Serial.println("Irrigation pump ON");
  } else if (humidity > 60) {
    digitalWrite(relayIrrigation, LOW);
    digitalWrite(relayDrainage, HIGH);
    Serial.println("Drainage pump ON");
  } else {
    digitalWrite(relayIrrigation, LOW);
    digitalWrite(relayDrainage, LOW);
    Serial.println("Both pumps OFF");
  }

  // Temperature
  if(temperature > 30) {
    digitalWrite(fan, HIGH);
  } else {
    digitalWrite(fan, LOW);
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
  lcd.print("U: " + String(humidity));
}

void brightRead() {
  brightness = analogRead(brightnessSensor);
  // brightness = map(brightness, 54, 974, 0, 100);
  
  Serial.println("Brightness: " + String(brightness));
  lcd.setCursor(0, 1);
  lcd.print("Brightness: " + String(brightness));
}

void tempRead() {
  tempValue = analogRead(lm35);
  float voltage = tempValue * (5.0 / 1023.0);
  temperature = voltage * 100;
  
  Serial.println("Temperature: " + String(temperature) + " °C");
  // Exibição da temperatura na LCD, atualizando a posição desejada
  lcd.setCursor(7, 0);
  lcd.print(String(temperature) + "C");
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
