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
float temperature, humidity;
float co2_ppm;
int tempThreshold = 30;
int humidityThreshold = 50;
int gasThreshold = 200;
int soilThreshold = 40;
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

void setup() {
  // put your setup code here, to run once:
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
  // put your main code here, to run repeatedly:
  readDHT(temperature, humidity);
  MQ2Reading();
  ControlIrrigation();
  controlVentilation(temperature, humidity);

  displayMenu();
  handleNavigation();
  handleEditMode();
  handleValueChange();

  delay(1000);  // Delay لتنظيم التكرار

}

void readDHT(float &temperature, float &humidity) {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.print(F("%  Temperature: "));
  Serial.print(temperature);
  Serial.println(F("°C"));
}

void MQ2Reading() {
  int sensorValue = analogRead(MQ2_PIN);         // Read raw value from MQ-2
  float voltage = sensorValue * (3.3 / 4095.0);  // Convert to voltage (ESP32 has 12-bit ADC)

  Serial.print("Raw Sensor Value: ");
  Serial.print(sensorValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage);
  Serial.println("V");

  co2_ppm = map(sensorValue, 0, 4095, 400, 5000);  // Example mapping

  Serial.print("Estimated CO2 Level: ");
  Serial.print(co2_ppm);
  Serial.println(" ppm");

  delay(2000);
}


void controlVentilation() {
  if (temperature > TEMP_THRESHOLD || humidity > HUMIDITY_THRESHOLD || co2_ppm > 1500) {
    myServo.write(90);
    digitalWrite(Fan, HIGH);
    Serial.println(F("Ventilation Opened!"));

  } else {
    myServo.write(0);
    digitalWrite(Fan, LOW);
    Serial.println(F("Ventilation Closed!"));
  }
}


void ControlIrrigation() {
  int SMvalue = analogRead(SoilMoisture);
  Serial.print("Soil Moisture Value: ");
  Serial.println(SMvalue);
  if (SMvalue > 2000) {
    digitalWrite(Pump, HIGH);
    Serial.println("Water pump Status: Open ");
  } else {
    Serial.println("Water pump Status: Close ");
    digitalWrite(Pump, LOW);
  }
}

void handleNavigation() {
  if (digitalRead(BTN_UP) == LOW) {
    currentSensor = (currentSensor - 1 + 4) % 4;
    delay(300);  // Debounce
  }
}

void handleEditMode() {
  if (digitalRead(BTN_SELECT) == LOW) {
    inEditMode = !inEditMode;
    delay(300);  // Debounce
  }
}

void handleValueChange() {
  if (inEditMode) {
    if (digitalRead(BTN_INC) == LOW) {
      changeValue(currentSensor, 1);
      delay(200);  // Debounce
    }
    if (digitalRead(BTN_DEC) == LOW) {
      changeValue(currentSensor, -1);
      delay(200);  // Debounce
    }
  }
}

void displayMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(sensorNames[currentSensor]);

  lcd.setCursor(0, 1);
  lcd.print("Val: ");
  lcd.print(getSensorValue(currentSensor));

  if (inEditMode) {
    lcd.print(" [Edit]");
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
