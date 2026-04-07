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
bool fanEnabled = false;          // Fan starts off

// Humidity Sensor
const byte humVCC = 12;
const byte humSensor = A0;
unsigned short int humidity = 0;

// Brightness Sensor
const byte brightnessSensor = A1;
unsigned short int brightness = 0;

// Fan (controlled by the new fan button)
const byte fan = 8;

// LED
const byte ledPin = 11;

// Relays
const byte relayIrrigation = 52; // Maintaining only the irrigation pump

// ====================================================================
// Variáveis para o controle da bomba de irrigação (contínuo)
// ====================================================================
// A bomba agora liga e permanece ligada enquanto a umidade estiver abaixo do limiar.
// Ela desliga assim que a umidade sobe acima do limiar.
// ====================================================================


void setup() {
  // LCD
  lcd.begin(16, 2);
  lcd.clear(); // Clears the LCD only once at initialization

  // Servo CEILING
  myServo.attach(servoRightPin);
  myServo.write(0); // Ensures the servo starts closed

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
  digitalWrite(relayIrrigation, HIGH); // Ensures the pump starts physically off (inverted logic)

  // Button for servo
  pinMode(buttonPin, INPUT_PULLUP);

  // Button for fan
  pinMode(fanButton, INPUT_PULLUP);

  // Serial
  Serial.begin(9600);

  // Print initial fan status to LCD on startup
  lcd.setCursor(10, 1);
  lcd.print("FAN OFF");

  // Print initial labels for humidity and brightness
  lcd.setCursor(0, 0);
  lcd.print("U: ");
  lcd.setCursor(0, 1);
  lcd.print("LUM: ");
}

void loop() {
  Serial.println(" "); // Keeps the line blank in Serial, if desired

  humRead();
  brightRead();
  delay(500); // Maintained for sensor reading cadence and LCD update

  // ====================================================================
  // Controle da bomba de irrigação (contínuo baseado na umidade)
  // Lógica invertida para o relé: HIGH = OFF, LOW = ON
  // ====================================================================
  lcd.setCursor(9, 0); // Position for irrigation status

  if (humidity < 33) { // Changed threshold from 25 to 33
    digitalWrite(relayIrrigation, LOW); // Turns the pump physically ON (LOW in code)
    Serial.println("Irrigation pump ON (continuous)");
    lcd.print("Irr ON "); // Displays status on LCD (7 characters)
  } else {
    digitalWrite(relayIrrigation, HIGH); // Turns the pump physically OFF (HIGH in code)
    Serial.println("Irrigation pump OFF (humidity OK)");
    lcd.print("       "); // Clears the irrigation status on the LCD (7 spaces)
  }
  // ====================================================================

  // Brightness management for LED
  if (brightness < 75) {
    digitalWrite(ledPin, HIGH);
    Serial.println("LEDs ON");
  } else {
    digitalWrite(ledPin, LOW);
  }

  // Button for servo (toggle ceiling servo)
  currentState = digitalRead(buttonPin);
  // Detects the transition from HIGH to LOW (button pressed)
  if (currentState == LOW && lastButtonState == HIGH) {
    servoOpen = !servoOpen;
    Serial.println("Servo button pressed");
    if (servoOpen) {
      myServo.write(45); // Opens the servo
    } else {
      myServo.write(0);  // Closes the servo
    }
  }
  lastButtonState = currentState; // Updates the previous button state

  // Fan toggle button logic
  currentFanButtonState = digitalRead(fanButton);

  // Debug: print current fan button state (useful for debugging)
  Serial.print("Fan Button State: ");
  Serial.println(currentFanButtonState);

  // Since using INPUT_PULLUP, a press is detected when the button reads LOW.
  // Detects the transition from HIGH to LOW (button pressed)
  if (currentFanButtonState == LOW && lastFanButtonState == HIGH) {
    fanEnabled = !fanEnabled; // Inverts the fan state

    if (fanEnabled) {
      digitalWrite(fan, HIGH);
      Serial.println("Fan turned ON");
    } else {
      digitalWrite(fan, LOW);
      Serial.println("Fan turned OFF");
    }
  }
  lastFanButtonState = currentFanButtonState; // Updates the previous button state

  // Always display the fan status on the LCD at a fixed position.
  lcd.setCursor(10, 1); // Position for fan status
  if (fanEnabled) {
    lcd.print("FAN ON "); // Added space to overwrite "OFF"
  } else {
    lcd.print("FAN OFF");
  }

  // The delay(500) here helps to cadence readings and LCD updates.
  delay(500);
}

void humRead() {
  digitalWrite(humVCC, HIGH); // Turns on the VCC of the humidity sensor
  delay(20); // Small delay for stabilization

  humidity = analogRead(humSensor);
  // Real-life calibration (adjust as needed)
  humidity = map(humidity, 1023, 372, 0, 100); // Maps the raw value to a percentage from 0-100

  digitalWrite(humVCC, LOW); // Turns off the sensor VCC to save energy/prevent corrosion

  Serial.println("Humidity: " + String(humidity));
  lcd.setCursor(3, 0); // Position for the humidity value
  // Ensures the humidity value is padded with spaces to clear previous digits
  if (humidity < 10) {
    lcd.print(String(humidity) + "  ");
  } else if (humidity < 100) {
    lcd.print(String(humidity) + " ");
  } else {
    lcd.print(String(humidity));
  }
}

void brightRead() {
  brightness = analogRead(brightnessSensor);
  brightness = map(brightness, 100, 970, 100, 0); // Correct mapping for your needs

  Serial.println("Brightness: " + String(brightness));
  lcd.setCursor(5, 1); // Position for the brightness value
  // Ensures the brightness value is padded with spaces to clear previous digits
  if (brightness < 10) {
    lcd.print(String(brightness) + "   ");
  } else if (brightness < 100) {
    lcd.print(String(brightness) + "  ");
  } else if (brightness <= 100) { // Added <= 100 to cover the maximum value
    lcd.print(String(brightness) + " ");
  }
}

// The reset() function was modified not to clear the LCD,
// only to print a blank line in Serial.
void reset() {
  // lcd.clear(); // Removed to avoid flickering
  Serial.println(" ");
}
