#include <OneWire.h>
#include <DallasTemperature.h>
// #include <BLEDevice.h>
// #include <BLEUtils.h>
// #include <BLEServer.h>

// Pin assignments
#define ONE_WIRE_BUS 4      // DS18B20 data pin
#define RELAY_PIN 26        // Relay control pin
#define LED_WARN 2          // Warning LED pin

// Temperature thresholds (°C)
const float TARGET_TEMP = 60.0;
const float STABILIZE_TOLERANCE = 2.0;
const float OVERHEAT_TEMP = 75.0;
const float HYSTERESIS = 1.5;

// OneWire and DallasTemperature setup
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Heater control states
enum State { IDLE, HEATING, STABILIZING, TARGET_REACHED, OVERHEAT };
State currentState = IDLE;

// Variables for LED blinking in OVERHEAT state
unsigned long previousBlinkMillis = 0;
const unsigned long blinkInterval = 500;  // Blink every 500 ms
bool ledState = LOW;

/*
// BLE
BLEServer* pServer = nullptr;
BLECharacteristic* pCharacteristic = nullptr;
const char* serviceUUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
const char* charUUID    = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E";

String stateToString(State s) {
  switch (s) {
    case IDLE: return "IDLE";
    case HEATING: return "HEATING";
    case STABILIZING: return "STABILIZING";
    case TARGET_REACHED: return "TARGET_REACHED";
    case OVERHEAT: return "OVERHEAT";
  }
  return "UNKNOWN";
}

void setupBLE() {
  Serial.println("Initializing BLE...");
  BLEDevice::init("Heater-ESP32");
  pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(serviceUUID);
  pCharacteristic = pService->createCharacteristic(
    charUUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );
  pCharacteristic->setValue("IDLE");
  pService->start();
  pServer->getAdvertising()->start();
  Serial.println("BLE advertising started");
}
*/

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Setup started...");

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_WARN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_WARN, LOW);

  Serial.println("Starting sensor...");
  sensors.begin();
  Serial.println("Sensor initialized");

  // setupBLE();

  Serial.println("Heater Control System starting...");
}

float readTemperatureC() {
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);
  if (temp == DEVICE_DISCONNECTED_C) {
    Serial.println("Error: DS18B20 disconnected!");
    return NAN;
  }
  return temp;
}

unsigned long lastLog = 0;
const unsigned long LOG_INTERVAL = 1000; // Log every 1 second


void setHeater(bool on) {
  digitalWrite(RELAY_PIN, on ? HIGH : LOW);
  Serial.print("Heater turned ");
  Serial.println(on ? "ON" : "OFF");
}

void loop() {
  Serial.println("Loop start");
  float temp = readTemperatureC();
  Serial.print("Read temperature: ");
  Serial.println(temp);

  if (isnan(temp)) {
    Serial.println("Temperature read failed, going to OVERHEAT state");
    currentState = OVERHEAT;
  } else {
    switch (currentState) {
      case IDLE:
        setHeater(false);
        digitalWrite(LED_WARN, LOW);
        if (temp < TARGET_TEMP - HYSTERESIS - STABILIZE_TOLERANCE) {
          currentState = HEATING;
          Serial.println("State change: IDLE -> HEATING");
        }
        break;

      case HEATING:
        setHeater(true);
        if (temp >= TARGET_TEMP - STABILIZE_TOLERANCE) {
          currentState = STABILIZING;
          Serial.println("State change: HEATING -> STABILIZING");
        }
        if (temp >= OVERHEAT_TEMP) {
          currentState = OVERHEAT;
          Serial.println("State change: HEATING -> OVERHEAT");
        }
        break;

      case STABILIZING:
        if (temp >= TARGET_TEMP + HYSTERESIS) {
          setHeater(false);
          currentState = TARGET_REACHED;
          Serial.println("State change: STABILIZING -> TARGET_REACHED");
        } else if (temp < TARGET_TEMP - HYSTERESIS) {
          setHeater(true);
          currentState = HEATING;
          Serial.println("State change: STABILIZING -> HEATING");
        }
        if (temp >= OVERHEAT_TEMP) {
          currentState = OVERHEAT;
          Serial.println("State change: STABILIZING -> OVERHEAT");
        }
        break;

      case TARGET_REACHED:
        setHeater(false);
        if (temp < TARGET_TEMP - (HYSTERESIS + 1.0)) {
          currentState = HEATING;
          Serial.println("State change: TARGET_REACHED -> HEATING");
        }
        if (temp >= OVERHEAT_TEMP) {
          currentState = OVERHEAT;
          Serial.println("State change: TARGET_REACHED -> OVERHEAT");
        }
        break;

      case OVERHEAT:
        setHeater(false);

        // Blink LED_WARN every 500 ms
        unsigned long currentMillis = millis();
        if (currentMillis - previousBlinkMillis >= blinkInterval) {
          previousBlinkMillis = currentMillis;
          ledState = !ledState;                 // Toggle LED state
          digitalWrite(LED_WARN, ledState);    // Write the toggled state
        }

        if (temp < (TARGET_TEMP - 5.0)) {
          digitalWrite(LED_WARN, LOW);
          currentState = IDLE;
          Serial.println("State change: OVERHEAT -> IDLE");
        }
        break;
    }
  }

  // logStatus(temp);

  Serial.println("Loop end\n");
  delay(500);
}
