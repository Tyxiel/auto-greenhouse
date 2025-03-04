# Auto Greenhouse
An automatic greenhouse with wifi connection based on arduino and ESP32

## How to use:

### 1. Install Arduino IDE
[Arduino](https://www.arduino.cc/en/software)

### 2. Download Modules (Inside Arduino IDE)

#### Download and install
[CP210x USB to UART Bridge VCP Drivers](https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers)

#### Add this link to your settings (Board Manager URL)
https://dl.espressif.com/dl/package_esp32_index.json

#### Boards
<ol>
  <li>ESP32</li>
</ol>

#### Libraries
<ol>
  <li>Arduino Uno WiFi Dev Ed Library</li>
  <li>LiquidCrystal</li>
  <li>Servo</li>
</ol> 

### 3. Resources needed

#### Arduino MEGA (2560)
<ul>
  <li>1 12V AC/DC (Used on the fan)</li>
  <li>4 resistors (2 220 ohns, 1 10k ohns, 1 1k ohns)</li>
  <li>1 LCD</li>
  <li>1 button</li>
  <li>1 servo</li>
  <li>1 DHT11 (Humidity Sensor)</li>
  <li>1 LDR resistor (Brightness Sensor)</li>
  <li>1 LM35 (Temperature Sensor)</li>
  <li>1 micro-fan (12V, connect to Vin)</li>
  <li>3 LEDs (1 green, 2 others any color)</li>
  <li>1 double 10A (5V) relay</li>
  <li>2 water bombs</li>
</ul>

#### ESP32

<br>
