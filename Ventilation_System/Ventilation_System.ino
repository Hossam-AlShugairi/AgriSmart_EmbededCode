#include <ESP32Servo.h>
#include "DHT.h"

// تعيين المنافذ
#define DHTPIN 12
#define SERVO_PIN 13
#define Fan 14
#define DHTTYPE DHT11

// إنشاء كائنات المستشعر والسيرفو
DHT dht(DHTPIN, DHTTYPE);
Servo myServo;

// تعيين حدود التهوية
#define TEMP_THRESHOLD 30  // درجة الحرارة القصوى
#define HUMIDITY_THRESHOLD 50  // نسبة الرطوبة القصوى

void readDHT(float &temperature, float &humidity);
void controlServo(float temperature, float humidity);

void setup() {
  Serial.begin(115200);
  pinMode(Fan, OUTPUT);
  dht.begin();
  myServo.attach(SERVO_PIN);
  Serial.println(F("System Initialized"));
}

void loop() {
  float temperature, humidity;
  
  readDHT(temperature, humidity);  // قراءة القيم من المستشعر
  controlServo(temperature, humidity);  // التحكم في السيرفو بناءً على القيم

  delay(1000);  // تأخير 2 ثانية قبل القراءة التالية
}

// دالة لقراءة مستشعر الحرارة والرطوبة
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

 
