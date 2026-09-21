/*
 * LADAKH-SHIELD: High-Altitude Electronic System Monitoring & Protection
 * ESP32 Universal Smart Module Firmware Prototype
 * 
 * Hardware Setup:
 * - ESP32 DevKit v1
 * - BME280 (I2C: SDA=21, SCL=22) -> Temp, Pressure, Humidity
 * - INA219/INA226 (I2C: 0x40)    -> Bus Voltage, Current, Power
 * - SSD1306 OLED (I2C: 0x3C)     -> Local cockpit status display
 * - LoRa SX1278 (SPI: SCK=18, MISO=19, MOSI=23, SS=5, RST=14, DIO0=2)
 * - MicroSD SPI (CS=15)          -> Offline fault & telemetry logging
 * - MOSFET 1 (GPIO 25)           -> PTC Heater Pad control
 * - MOSFET 2 (GPIO 26 / PWM)     -> Cooling Fan control
 * - MOSFET 3 (GPIO 27)           -> Solid-State Power Rail Isolation
 * - Buzzer & Strobe (GPIO 32)    -> Critical acoustic/optical alarm
 */

#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_INA219.h>
#include <LoRa.h>
#include <SD.h>

// WiFi Configuration
const char* ssid = "DEFENCE_MESH_NET";
const char* password = "SECURE_LADAKH_PASS";
const char* apiServer = "http://192.168.1.100:5000/api/sensor-data";

// Hardware Pin Definitions
#define PIN_HEATER_MOSFET 25
#define PIN_FAN_MOSFET    26
#define PIN_POWER_ISOLATE 27
#define PIN_ALARM_BUZZER  32
#define PIN_SD_CS         15
#define PIN_LORA_SS       5
#define PIN_LORA_RST      14
#define PIN_LORA_DIO0     2

// Thresholds for High Altitude Ladakh Operation
#define THRESHOLD_COLD_HEATER_ON   -20.0 // °C
#define THRESHOLD_COLD_HEATER_OFF  -15.0 // °C (Hysteresis)
#define THRESHOLD_HOT_FAN_ON        45.0 // °C
#define THRESHOLD_HOT_FAN_OFF       40.0 // °C
#define THRESHOLD_OVERCURRENT_MAX    6.0 // Amperes

Adafruit_BME280 bme;
Adafruit_INA219 ina219;

unsigned long lastTransmitTime = 0;
const unsigned long transmitInterval = 2000; // 2 seconds
bool mosfetTripped = false;
int packetSequence = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  pinMode(PIN_HEATER_MOSFET, OUTPUT);
  pinMode(PIN_FAN_MOSFET, OUTPUT);
  pinMode(PIN_POWER_ISOLATE, OUTPUT);
  pinMode(PIN_ALARM_BUZZER, OUTPUT);

  digitalWrite(PIN_POWER_ISOLATE, HIGH); // Armed/closed by default
  digitalWrite(PIN_HEATER_MOSFET, LOW);
  digitalWrite(PIN_FAN_MOSFET, LOW);
  digitalWrite(PIN_ALARM_BUZZER, LOW);

  // Initialize BME280
  if (!bme.begin(0x76, &Wire)) {
    Serial.println(F("[ERROR] BME280 Sensor not detected!"));
  } else {
    Serial.println(F("[OK] BME280 Initialized."));
  }

  // Initialize INA219
  if (!ina219.begin(&Wire)) {
    Serial.println(F("[ERROR] INA219 Sensor not detected!"));
  } else {
    Serial.println(F("[OK] INA219 Initialized."));
  }

  // Initialize LoRa SX1278
  LoRa.setPins(PIN_LORA_SS, PIN_LORA_RST, PIN_LORA_DIO0);
  if (!LoRa.begin(868E6)) {
    Serial.println(F("[WARN] LoRa SX1278 not found. Fallback to WiFi/SD."));
  } else {
    LoRa.setTxPower(20); // Max power for mountain terrain
    Serial.println(F("[OK] LoRa SX1278 868MHz Armed."));
  }

  // Initialize SD Card
  if (!SD.begin(PIN_SD_CS)) {
    Serial.println(F("[WARN] SD Card logging unavailable."));
  } else {
    Serial.println(F("[OK] MicroSD Telemetry Card Mounted."));
  }

  // Attempt WiFi Connection
  WiFi.begin(ssid, password);
  Serial.print(F("Connecting to Tactical Wi-Fi"));
}

void loop() {
  if (millis() - lastTransmitTime >= transmitInterval) {
    lastTransmitTime = millis();
    packetSequence++;

    // 1. READ BME280
    float temperature = bme.readTemperature();
    float pressureKPa = bme.readPressure() / 1000.0F; // Pa to kPa
    float humidity    = bme.readHumidity();

    // 2. READ INA219
    float busVoltage  = ina219.getBusVoltage_V();
    float current_mA  = ina219.getCurrent_mA();
    float current_A   = current_mA / 1000.0F;
    float power_W     = (busVoltage * current_A);

    // 3. ADAPTIVE PROTECTION RULES
    // Cold protection: PTC Heater
    if (temperature < THRESHOLD_COLD_HEATER_ON) {
      digitalWrite(PIN_HEATER_MOSFET, HIGH);
      Serial.println(F("[HEATER] Cold threshold reached. Heater ON."));
    } else if (temperature >= THRESHOLD_COLD_HEATER_OFF) {
      digitalWrite(PIN_HEATER_MOSFET, LOW);
    }

    // Heat protection: Cooling Fan
    if (temperature > THRESHOLD_HOT_FAN_ON) {
      digitalWrite(PIN_FAN_MOSFET, HIGH);
      Serial.println(F("[FAN] Overheating detected. Fan ON."));
    } else if (temperature <= THRESHOLD_HOT_FAN_OFF) {
      digitalWrite(PIN_FAN_MOSFET, LOW);
    }

    // Overcurrent protection: MOSFET Trip
    if (current_A >= THRESHOLD_OVERCURRENT_MAX && !mosfetTripped) {
      digitalWrite(PIN_POWER_ISOLATE, LOW); // Open circuit breaker
      digitalWrite(PIN_ALARM_BUZZER, HIGH); // Alarm sounds
      mosfetTripped = true;
      Serial.println(F("[CRITICAL] Overcurrent trip! MOSFET Isolation Active."));
    }

    // 4. TRANSMIT OVER LORA SX1278
    LoRa.beginPacket();
    LoRa.print("LSH;");
    LoRa.print(temperature); LoRa.print(";");
    LoRa.print(pressureKPa); LoRa.print(";");
    LoRa.print(humidity);    LoRa.print(";");
    LoRa.print(busVoltage);  LoRa.print(";");
    LoRa.print(current_A);   LoRa.print(";");
    LoRa.print(packetSequence);
    LoRa.endPacket();

    // 5. TRANSMIT OVER HTTP POST TO LADAKH-SHIELD DASHBOARD
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(apiServer);
      http.addHeader("Content-Type", "application/json");

      String jsonPayload = String("{") +
        "\"deviceId\":\"LSH-24-05-0017\"," +
        "\"temperature\":" + String(temperature, 2) + "," +
        "\"pressure\":" + String(pressureKPa, 2) + "," +
        "\"humidity\":" + String(humidity, 1) + "," +
        "\"voltage\":" + String(busVoltage, 2) + "," +
        "\"current\":" + String(current_A, 2) + "," +
        "\"dataSource\":\"LIVE_HARDWARE\"," +
        "\"mosfetTripped\":" + (mosfetTripped ? "true" : "false") +
      "}";

      int httpResponseCode = http.POST(jsonPayload);
      Serial.print(F("[HTTP] Status: "));
      Serial.println(httpResponseCode);
      http.end();
    } else {
      // 6. OFFLINE SD CARD LOGGING (Fallback)
      File logFile = SD.open("/telemetry.csv", FILE_APPEND);
      if (logFile) {
        logFile.printf("%lu,%0.2f,%0.2f,%0.1f,%0.2f,%0.2f\n", 
          millis(), temperature, pressureKPa, humidity, busVoltage, current_A);
        logFile.close();
      }
    }
  }
}
