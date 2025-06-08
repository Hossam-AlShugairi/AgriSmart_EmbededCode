//Library
#include <ESP32Servo.h>
#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Sensors
#define DHTPIN 12
#define MQ2_PIN 27
#define SoilMoisture 33

//Actuators
#define Pump 18
#define SERVO_PIN 13
#define FAN_PIN 14
#define BTN_UP 15
#define BTN_SELECT 4
#define BTN_INC 16
#define BTN_DEC 17

//Variables
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

//Objects
Servo myServo;
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

//Functions
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

  lcd.setCursor(0, 0);
  lcd.print("Control Panel");
  delay(1000);
  lcd.clear();
}

void loop() {
  readDHT(currentTemperature, currentHumidity);
  MQ2Reading();
  ControlIrrigation();
  controlVentilation();

  displayMenu();
  handleNavigation();
  handleEditMode();
  handleValueChange();

  delay(1000);
}

void readDHT(float &temperature, float &humidity) {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  currentTemperature = temperature;
  currentHumidity = humidity;

  Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.print(F("%  Temperature: "));
  Serial.print(temperature);
  Serial.println(F("°C"));
}

void MQ2Reading() {
  int sensorValue = analogRead(MQ2_PIN);
  if (isnan(sensorValue)) {
    Serial.println(F("Failed to read from Gas " MQ - 2 " sensor!"));
  }
  float voltage = sensorValue * (3.3 / 4095.0);
  co2_ppm = map(sensorValue, 0, 4095, 400, 5000);
  currentGas = co2_ppm;

  Serial.print("CO2: ");
  Serial.print(co2_ppm);
  Serial.println(" ppm");
}

void controlVentilation() {
  if (currentTemperature > tempThreshold || currentHumidity > humidityThreshold || co2_ppm > gasThreshold) {
    myServo.write(90);
    digitalWrite(FAN_PIN, HIGH);
    Serial.println(F("Ventilation Opened!"));
  } else {
    myServo.write(0);
    digitalWrite(FAN_PIN, LOW);
    Serial.println(F("Ventilation Closed!"));
  }
}

void ControlIrrigation() {
  int SMvalue = analogRead(SoilMoisture);
  if (isnan(SMvalue)) {
    Serial.println(F("Failed to read from SoilMoisture sensor!"));
  }
  currentSoil = SMvalue;

  Serial.print("Soil Moisture Value: ");
  Serial.println(SMvalue);

  if (SMvalue > soilThreshold) {
    digitalWrite(Pump, HIGH);
    Serial.println("Water pump Status: Open ");
  } else {
    digitalWrite(Pump, LOW);
    Serial.println("Water pump Status: Close ");
  }
}

void handleNavigation() {
  if (digitalRead(BTN_UP) == LOW) {
    currentSensor = (currentSensor - 1 + 4) % 4;
    delay(300);
  }
}

void handleEditMode() {
  if (digitalRead(BTN_SELECT) == LOW) {
    inEditMode = !inEditMode;
    delay(300);
  }
}

void handleValueChange() {
  if (inEditMode) {
    if (digitalRead(BTN_INC) == LOW) {
      changeValue(currentSensor, 1);
      delay(200);
    }
    if (digitalRead(BTN_DEC) == LOW) {
      changeValue(currentSensor, -1);
      delay(200);
    }
  }
}

void displayMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(sensorNames[currentSensor]);

  lcd.setCursor(0, 1);
  lcd.print("Now:");
  lcd.print(getSensorCurrent(currentSensor));
  lcd.print("/");

  lcd.print(getSensorValue(currentSensor));

  if (inEditMode) {
    lcd.setCursor(13, 1);
    lcd.print("[E]");
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
