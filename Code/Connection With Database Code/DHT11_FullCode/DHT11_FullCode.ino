#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

// WiFi
const char* ssid = "Hossam";
const char* password = "12345678";

// Server Base
String server = "http://192.168.126.16/SmartAgriculture/sensors";

// Temperature PHP Files
String insertTempURL = server + "temperature/temperature_insert.php";
String showTempThresholdsURL = server + "temperature/temperature_min_max_show.php";
String showModifiedTempThresholdsURL = server + "temperature/temperature_modified_value_show.php";

// Humidity PHP Files
String insertHumidityURL = server + "humidity/humidity_insert.php";
String showHumidityThresholdsURL = server + "humidity/humidity_min_max_show.php";
String showModifiedHumidityThresholdsURL = server + "humidity/humidity_modified_value_show.php";

// Pins
#define DHTPIN 12
#define DHTTYPE DHT11
#define FAN_PIN 14
#define SERVO_PIN 13

// Variables
float currentTemperature = 0.0;
float currentHumidity = 0.0;
int tempMin = 40, tempMax = 60;
int humMin = 40, humMax = 70;

unsigned long lastSensorRead = 0;
unsigned long lastThresholdFetch = 0;
const unsigned long sensorInterval = 300000;
const unsigned long thresholdInterval = 300000;

// Objects
DHT dht(DHTPIN, DHTTYPE);
Servo ventServo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);
  dht.begin();
  ventServo.attach(SERVO_PIN);
  pinMode(FAN_PIN, OUTPUT);
  lcd.init();
  lcd.backlight();

  WiFi.begin(ssid, password);
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
  delay(2000);
  lcd.clear();
}

void loop() {
  unsigned long now = millis();

  if (now - lastSensorRead >= sensorInterval) {
    lastSensorRead = now;
    readDHTSensor();
    controlVentilation();
    sendTempToServer();
    sendHumidityToServer();
    updateLCD();
  }

  if (now - lastThresholdFetch >= thresholdInterval) {
    lastThresholdFetch = now;
    fetchTempThresholds();
    fetchHumidityThresholds();
  }
}

void readDHTSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (!isnan(h) && !isnan(t)) {
    currentHumidity = h;
    currentTemperature = t;
  }
}

void controlVentilation() {
  if (currentTemperature > tempMax || currentHumidity > humMax) {
    digitalWrite(FAN_PIN, LOW);
    ventServo.write(90);  // open
  } else if (currentTemperature < tempMin && currentHumidity < humMin) {
    digitalWrite(FAN_PIN, HIGH);
    ventServo.write(0);  // close
  }
}

void sendTempToServer() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(insertTempURL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    //String postData = "sensorID=" + int('1') + "&value=" + float(currentTemperature);
    String postData = "sensorID&value==" + float(1,currentTemperature);
    http.POST(postData);
    http.end();
  }
}

void sendHumidityToServer() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(insertHumidityURL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "sensorID&value=" + float(1,currentHumidity);
    // String postData = "sensorID=" + int('1') + "&value" + float(currentHumidity);
    http.POST(postData);
    http.end();
  }
}

void fetchTempThresholds() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(showModifiedTempThresholdsURL);
    int code = http.GET();

    if (code == 200) {
      String payload = http.getString();
      StaticJsonDocument<256> doc;
      if (!deserializeJson(doc, payload)) {
        tempMin = doc["min"];
        tempMax = doc["max"];
        http.end();
        return;
      }
    }
    http.end();

    http.begin(showTempThresholdsURL);
    code = http.GET();
    if (code == 200) {
      String payload = http.getString();
      StaticJsonDocument<256> doc;
      if (!deserializeJson(doc, payload)) {
        tempMin = doc["min"];
        tempMax = doc["max"];
      }
    }
    http.end();
  }
}

void fetchHumidityThresholds() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(showModifiedHumidityThresholdsURL);
    int code = http.GET();

    if (code == 200) {
      String payload = http.getString();
      StaticJsonDocument<256> doc;
      if (!deserializeJson(doc, payload)) {
        humMin = doc["min"];
        humMax = doc["max"];
        http.end();
        return;
      }
    }
    http.end();

    http.begin(showHumidityThresholdsURL);
    code = http.GET();
    if (code == 200) {
      String payload = http.getString();
      StaticJsonDocument<256> doc;
      if (!deserializeJson(doc, payload)) {
        humMin = doc["min"];
        humMax = doc["max"];
      }
    }
    http.end();
  }
}

void updateLCD() {
  static float lastTemp = -1, lastHum = -1;

  if (abs(lastTemp - currentTemperature) > 0.1 || abs(lastHum - currentHumidity) > 0.1) {
    lastTemp = currentTemperature;
    lastHum = currentHumidity;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(currentTemperature);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("Hum: ");
    lcd.print(currentHumidity);
    lcd.print("%");
  }
}
