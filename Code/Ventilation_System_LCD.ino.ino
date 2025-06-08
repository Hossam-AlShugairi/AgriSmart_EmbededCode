#include <ESP32Servo.h>
#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// تعيين المنافذ
#define DHTPIN 12
#define SERVO_PIN 13
#define Fan 14
#define DHTTYPE DHT11

// إنشاء كائنات المستشعر والسيرفو
DHT dht(DHTPIN, DHTTYPE);
Servo myServo;
LiquidCrystal_I2C lcd(0x27, 16, 2); // عنوان I2C للشاشة، عرض 16، ارتفاع 2

// تعيين حدود التهوية
#define TEMP_THRESHOLD 30  // درجة الحرارة القصوى
#define HUMIDITY_THRESHOLD 50  // نسبة الرطوبة القصوى

void readDHT(float &temperature, float &humidity);
void controlServo(float temperature, float humidity);
void displayLCD(float temperature, float humidity);

void setup() {
  Serial.begin(115200);
  pinMode(Fan, OUTPUT);
  dht.begin();
  myServo.attach(SERVO_PIN);
  
  // تهيئة شاشة LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Init...");
  delay(2000);
  lcd.clear();

  Serial.println(F("System Initialized"));
}

void loop() {
  float temperature, humidity;
  
  readDHT(temperature, humidity);  // قراءة القيم من المستشعر
  controlServo(temperature, humidity);  // التحكم في السيرفو بناءً على القيم
  displayLCD(temperature, humidity); // عرض القيم على الشاشة

  delay(2000);  // تأخير 2 ثانية قبل القراءة التالية
}

// دالة لقراءة مستشعر الحرارة والرطوبة
void readDHT(float &temperature, float &humidity) {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error!");
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.print(F("%  Temperature: "));
  Serial.print(temperature);
  Serial.println(F("°C"));
}

// دالة للتحكم في محرك السيرفو بناءً على القيم المقروءة
void controlServo(float temperature, float humidity) {
  if (temperature > TEMP_THRESHOLD || humidity > HUMIDITY_THRESHOLD) {
    myServo.write(90);  // فتح النوافذ (تحريك السيرفو إلى 90°)
    digitalWrite(Fan, HIGH);
    Serial.println(F("Ventilation Opened!"));

  } else {
    myServo.write(0);   // غلق النوافذ (تحريك السيرفو إلى 0°)
    digitalWrite(Fan, LOW);
    Serial.println(F("Ventilation Closed!"));
  }
}

// دالة لعرض القيم على شاشة LCD
void displayLCD(float temperature, float humidity) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Humi: ");
  lcd.print(humidity);
  lcd.print(" %");
}
