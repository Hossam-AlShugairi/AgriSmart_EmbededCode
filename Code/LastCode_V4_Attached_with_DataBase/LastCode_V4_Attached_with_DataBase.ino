// Libraries
#include <ESP32Servo.h>
#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Wi-Fi credentials
const char *ssid = "Hossam";
const char *password = "12345678";
String server = "http://192.168.1.100/agrismart";

// Sensors
#define DHTPIN 12
#define MQ2_PIN 27
#define SoilMoisture 33

// Actuators
#define Pump 18
#define SERVO_PIN 13
#define FAN_PIN 14
#define BTN_UP 15
#define BTN_SELECT 4
#define BTN_INC 16
#define BTN_DEC 17

// Variables
float co2_ppm;
float currentTemperature = 0.0;
float currentHumidity = 0.0;
float currentGas = 0.0;
float currentSoil = 0.0;

int tempThreshold = 30;
int humidityThreshold = 60;
int gasThreshold = 1000;
int soilThreshold = 2000;

int currentSensor = 0;
bool inEditMode = false;

#define DHTTYPE DHT11
String sensorNames[4] = { "Temp", "Humidity", "Gas", "Soil" };

// Timing
unsigned long lastSensorRead = 0;
unsigned long lastButtonCheck = 0;
unsigned long lastThresholdFetch = 0;
const unsigned long sensorInterval = 1000;
const unsigned long thresholdFetchInterval = 10000;
const unsigned long buttonDebounce = 200;
const unsigned long WIFI_TIMEOUT_MS = 10000;

// Objects
Servo myServo;
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// LCD Flicker reduction
String lastDisplayLine0 = "";
String lastDisplayLine1 = "";

// Function Declarations
void ControlIrrigation();
void MQ2Reading();
void controlVentilation();
void readDHT(float &temperature, float &humidity);
void displayMenu();
void handleNavigation();
void handleEditMode();
void handleValueChange();
float getSensorCurrent(int index);
int getSensorValue(int index);
void changeValue(int index, int delta);
void sendSensorData(String type, float value);
void fetchThresholds();

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  dht.begin();
  myServo.attach(SERVO_PIN);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(Pump, OUTPUT);

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(BTN_INC, INPUT_PULLUP);
  pinMode(BTN_DEC, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  unsigned long wifiStartAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStartAttemptTime < WIFI_TIMEOUT_MS) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi Connected!");
  } else {
    Serial.println("WiFi Connection Failed.");
  }

  lcd.setCursor(0, 0);
  lcd.print("Control Panel");
  delay(1000);
  lcd.clear();
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastSensorRead >= sensorInterval) {
    lastSensorRead = currentMillis;
    readDHT(currentTemperature, currentHumidity);
    MQ2Reading();
    ControlIrrigation();
    controlVentilation();

    sendSensorData("temp", currentTemperature);
    sendSensorData("humidity", currentHumidity);
    sendSensorData("gas", currentGas);
    sendSensorData("soil", currentSoil);
  }

  if (currentMillis - lastThresholdFetch >= thresholdFetchInterval) {
    lastThresholdFetch = currentMillis;
    fetchThresholds();
  }

  displayMenu();

  if (currentMillis - lastButtonCheck >= buttonDebounce) {
    lastButtonCheck = currentMillis;
    handleNavigation();
    handleEditMode();
    handleValueChange();
  }
}

void readDHT(float &temperature, float &humidity) {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) return;

  currentTemperature = temperature;
  currentHumidity = humidity;
}

void MQ2Reading() {
  int sensorValue = analogRead(MQ2_PIN);
  float voltage = sensorValue * (3.3 / 4095.0);
  float ratio = sensorValue / 4095.0;
  co2_ppm = 400 + (ratio * (5000 - 400));
  currentGas = co2_ppm;
}

void controlVentilation() {
  if (currentTemperature > tempThreshold || currentHumidity > humidityThreshold || co2_ppm > gasThreshold) {
    myServo.write(90);
    digitalWrite(FAN_PIN, HIGH);
  } else {
    myServo.write(0);
    digitalWrite(FAN_PIN, LOW);
  }
}

void ControlIrrigation() {
  int SMvalue = analogRead(SoilMoisture);
  currentSoil = SMvalue;
  if (SMvalue < soilThreshold) {
    digitalWrite(Pump, HIGH);
  } else {
    digitalWrite(Pump, LOW);
  }
}

void handleNavigation() {
  if (digitalRead(BTN_UP) == LOW) {
    currentSensor = (currentSensor - 1 + 4) % 4;
  }
}

void handleEditMode() {
  if (digitalRead(BTN_SELECT) == LOW) {
    inEditMode = !inEditMode;
  }
}

void handleValueChange() {
  if (inEditMode) {
    if (digitalRead(BTN_INC) == LOW) {
      changeValue(currentSensor, 1);
    }
    if (digitalRead(BTN_DEC) == LOW) {
      changeValue(currentSensor, -1);
    }
  }
}

void displayMenu() {
  String line0 = sensorNames[currentSensor];
  String line1 = "Now:" + String(getSensorCurrent(currentSensor)) + "/" + String(getSensorValue(currentSensor));
  if (inEditMode) line1 += "[E]";

  if (line0 != lastDisplayLine0 || line1 != lastDisplayLine1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line0);
    lcd.setCursor(0, 1);
    lcd.print(line1);
    lastDisplayLine0 = line0;
    lastDisplayLine1 = line1;
  }
}

float getSensorCurrent(int index) {
  switch (index) {
    case 0: return currentTemperature;
    case 1: return currentHumidity;
    case 2: return currentGas;
    case 3: return currentSoil;
    default: return 0.0;
  }
}

int getSensorValue(int index) {
  switch (index) {
    case 0: return tempThreshold;
    case 1: return humidityThreshold;
    case 2: return gasThreshold;
    case 3: return soilThreshold;
    default: return 0;
  }
}

void changeValue(int index, int delta) {
  switch (index) {
    case 0: tempThreshold += delta; break;
    case 1: humidityThreshold += delta; break;
    case 2: gasThreshold += delta; break;
    case 3: soilThreshold += delta; break;
  }
}

void sendSensorData(String type, float value) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(server + "/insert_data.php");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "sensor_type=" + type + "&value=" + String(value);
    http.POST(postData);
    http.end();
  }
}

void fetchThresholds() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(server + "/get_thresholds.php");
    int httpResponseCode = http.GET();

    if (httpResponseCode == 200) {
      String payload = http.getString();
      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        tempThreshold = doc["temp"] | tempThreshold;
        humidityThreshold = doc["humidity"] | humidityThreshold;
        gasThreshold = doc["gas"] | gasThreshold;
        soilThreshold = doc["soil"] | soilThreshold;
      } else {
        Serial.print("JSON Error: ");
        Serial.println(error.c_str());
      }
    } else {
      Serial.print("HTTP Error: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }
}
