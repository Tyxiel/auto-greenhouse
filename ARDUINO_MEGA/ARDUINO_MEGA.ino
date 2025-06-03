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

// LED
const byte ledPin = 11;

// Relays
const byte relayIrrigation = 52;
const byte relayDrainage = 53;

void setup() {
  // LCD
  lcd.begin(16, 2);
  lcd.clear(); // Limpa o LCD apenas uma vez na inicialização

  // Servo CEILING
  myServo.attach(servoRightPin);
  myServo.write(0); // Garante que o servo comece fechado

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
  // Não chame reset() aqui para evitar o flickering do LCD
  Serial.println(" "); // Mantém a linha em branco no Serial, se desejado

  humRead();
  brightRead();
  delay(500); // Mantido para cadência de leitura dos sensores

  // Display humidity result on LCD and control irrigation/drainage relays
  lcd.setCursor(9, 0); // Posição para status de irrigação/drenagem
  if (humidity < 25) {
    digitalWrite(relayIrrigation, HIGH);
    digitalWrite(relayDrainage, LOW);
    Serial.println("Irrigation pump ON");
    lcd.print("Irr ON "); // Adicionado espaço para sobrescrever "Drn ON" se necessário
  } else if (humidity > 60) {
    digitalWrite(relayIrrigation, LOW);
    digitalWrite(relayDrainage, HIGH);
    Serial.println("Drainage pump ON");
    lcd.print("Drn ON "); // Adicionado espaço para sobrescrever "Irr ON" se necessário
  } else {
    digitalWrite(relayIrrigation, LOW);
    digitalWrite(relayDrainage, LOW);
    Serial.println("Both pumps OFF");
    lcd.print("       "); // Limpa o espaço se nenhum estiver ativo
  }

  // Brightness management for LED
  if (brightness < 75) {
    digitalWrite(ledPin, HIGH);
    Serial.println("LEDs ON");
  } else {
    digitalWrite(ledPin, LOW);
  }

  // Button for servo (toggle ceiling servo)
  currentState = digitalRead(buttonPin);
  // Detecta a transição de HIGH para LOW (botão pressionado)
  if (currentState == LOW && lastButtonState == HIGH) {
    servoOpen = !servoOpen;
    Serial.println("Servo button pressed");
    if (servoOpen) {
      myServo.write(45); // Abre o servo
    } else {
      myServo.write(0);  // Fecha o servo
    }
  }
  lastButtonState = currentState; // Atualiza o estado anterior do botão

  // Fan toggle button logic
  currentFanButtonState = digitalRead(fanButton);

  // Debug: print current fan button state (útil para depuração)
  Serial.print("Fan Button State: ");
  Serial.println(currentFanButtonState);

  // Since using INPUT_PULLUP, a press is detected when the button reads LOW.
  // Detecta a transição de HIGH para LOW (botão pressionado)
  if (currentFanButtonState == LOW && lastFanButtonState == HIGH) {
    fanEnabled = !fanEnabled; // Inverte o estado do ventilador

    if (fanEnabled) {
      digitalWrite(fan, HIGH);
      Serial.println("Fan turned ON");
    } else {
      digitalWrite(fan, LOW);
      Serial.println("Fan turned OFF");
    }
  }
  lastFanButtonState = currentFanButtonState; // Atualiza o estado anterior do botão

  // Always display the fan status on the LCD at a fixed position.
  lcd.setCursor(10, 1); // Posição para status do ventilador
  if (fanEnabled) {
    lcd.print("FAN ON "); // Adicionado espaço para sobrescrever "OFF"
  } else {
    lcd.print("FAN OFF");
  }

  delay(500); // Mantido para cadência geral do loop
}

void humRead() {
  digitalWrite(humVCC, HIGH); // Liga o VCC do sensor de umidade
  delay(20); // Pequeno delay para estabilização

  humidity = analogRead(humSensor);
  // Real-life calibration (adjust as needed)
  humidity = map(humidity, 1023, 372, 0, 100); // Mapeia o valor bruto para porcentagem de 0-100

  digitalWrite(humVCC, LOW); // Desliga o VCC do sensor para economizar energia/evitar corrosão

  Serial.println("Humidity: " + String(humidity));
  lcd.setCursor(3, 0); // Posição para o valor da umidade
  // Garante que o valor da umidade seja preenchido com espaços para limpar dígitos anteriores
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
  brightness = map(brightness, 100, 970, 100, 0); // Mapeamento correto para sua necessidade

  Serial.println("Brightness: " + String(brightness));
  lcd.setCursor(5, 1); // Posição para o valor da luminosidade
  // Garante que o valor da luminosidade seja preenchido com espaços para limpar dígitos anteriores
  if (brightness < 10) {
    lcd.print(String(brightness) + "   ");
  } else if (brightness < 100) {
    lcd.print(String(brightness) + "  ");
  } else if (brightness <= 100) { // Adicionado <= 100 para cobrir o valor máximo
    lcd.print(String(brightness) + " ");
  }
}
// A função reset() foi modificada para não limpar o LCD,
// apenas para imprimir uma linha em branco no Serial.
void reset() {
  // lcd.clear(); // Removido para evitar flickering
  Serial.println(" ");
}
