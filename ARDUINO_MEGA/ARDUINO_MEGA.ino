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
const byte relayIrrigation = 52; // Mantendo apenas a bomba de irrigação

// ====================================================================
// Variáveis para o controle temporizado da bomba de irrigação
// ====================================================================
enum PumpState { OFF, RUNNING_TIMED }; // Estados da bomba: desligada, rodando por tempo
static PumpState currentPumpState = OFF; // Estado inicial da bomba
static unsigned long pumpActionTime = 0; // Armazena o tempo da última mudança de estado da bomba

// DURAÇÃO QUE A BOMBA FICARÁ LIGADA POR CICLO (em milissegundos)
// Ajuste este valor cuidadosamente para a sua estufa e bomba de 3000L/h.
// Para simular o enchimento de 1 litro com uma bomba de 3000L/h (aprox. 0.833 L/s),
// o tempo necessário é de 1L / 0.833 L/s = 1.2 segundos.
const unsigned long PUMP_RUN_DURATION_MS = 1200; // 1.2 segundos para 1 litro

// TEMPO MÍNIMO QUE A BOMBA DEVE FICAR DESLIGADA ANTES DE UM NOVO CICLO (em milissegundos)
// Isso evita ligar a bomba muito rapidamente após um ciclo.
// Reduzido para 10 segundos para simulação e exibição no LCD.
const unsigned long PUMP_MIN_OFF_TIME_MS = 10000; // 10 segundos
// ====================================================================


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
  digitalWrite(relayIrrigation, LOW); // Garante que a bomba comece desligada

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
  Serial.println(" "); // Mantém a linha em branco no Serial, se desejado

  humRead();
  brightRead();
  delay(500); // Mantido para cadência de leitura dos sensores e atualização do LCD

  // ====================================================================
  // Controle da bomba de irrigação usando máquina de estados
  // ====================================================================
  lcd.setCursor(9, 0); // Posição para status de irrigação

  switch (currentPumpState) {
    case OFF:
      // Se a umidade estiver baixa E o tempo mínimo de desligamento já passou
      if (humidity < 25 && (millis() - pumpActionTime >= PUMP_MIN_OFF_TIME_MS)) {
        digitalWrite(relayIrrigation, HIGH); // Liga a bomba
        Serial.println("Irrigation pump ON (timed cycle)");
        lcd.print("Irr ON "); // Exibe status no LCD
        currentPumpState = RUNNING_TIMED; // Muda para o estado de "rodando por tempo"
        pumpActionTime = millis(); // Registra o tempo de início deste ciclo
      } else if (humidity < 25 && (millis() - pumpActionTime < PUMP_MIN_OFF_TIME_MS)) {
        // Umidade baixa, mas ainda no período de resfriamento
        digitalWrite(relayIrrigation, LOW); // Garante que esteja desligada
        Serial.println("Irrigation needed, but in cooldown.");

        unsigned long remainingTime = PUMP_MIN_OFF_TIME_MS - (millis() - pumpActionTime);
        unsigned int remainingSeconds = remainingTime / 1000;

        lcd.print("CD:"); // Cooldown (3 caracteres)
        if (remainingSeconds < 10) {
          lcd.print(" "); // Adiciona um espaço para alinhar dígitos únicos
        }
        lcd.print(remainingSeconds); // 1 ou 2 caracteres
        lcd.print("s "); // "s" + espaço (2 caracteres)
        // Total: 3 + (1 ou 0) + (1 ou 2) + 2 = 6 ou 7 caracteres
        // Isso se encaixa no espaço de 7 caracteres disponível.
      } else {
        // Umidade OK, garante que a bomba esteja desligada e limpa o display
        digitalWrite(relayIrrigation, LOW);
        Serial.println("Irrigation pump OFF (humidity OK)");
        lcd.print("       "); // Limpa o status de irrigação no LCD (7 espaços)
      }
      break;

    case RUNNING_TIMED:
      // Se o tempo de execução da bomba já passou
      if (millis() - pumpActionTime >= PUMP_RUN_DURATION_MS) {
        digitalWrite(relayIrrigation, LOW); // Desliga a bomba
        Serial.println("Irrigation pump OFF (time limit reached)");
        lcd.print("       "); // Limpa o status de irrigação no LCD
        currentPumpState = OFF; // Volta para o estado "desligada"
        pumpActionTime = millis(); // Registra o tempo em que a bomba foi desligada (início do resfriamento)
      } else {
        // A bomba ainda está rodando, mantém ligada
        // Serial.println("Irrigation pump still running..."); // Pode ser comentado para menos spam no Serial
        lcd.print("Irr ON "); // Mantém o status no LCD (7 caracteres)
      }
      break;
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

  // O delay(500) aqui ajuda a cadenciar as leituras e atualizações do LCD,
  // mas o controle da bomba usa millis() para ser não-bloqueante.
  delay(500);
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
