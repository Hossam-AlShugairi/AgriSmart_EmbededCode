#include <ESP32Servo.h>
#include "DHT.h"


#define MQ2_PIN 27  
#define SERVO_PIN 13
#define FAN_PIN 14
float co2_ppm;
void MQ2Reading();
void controlVentilation();
void controlirrigation();

Servo myServo;
void setup() {
  Serial.begin(115200);
  Serial.println("MQ-2 Gas Sensor Initialization...");
}

void loop() {
  MQ2Reading();
  controlVentilation();
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

  delay(2000);  // Wait 2 seconds
}

void controlVentilation() {
  if (co2_ppm>1500) {
    myServo.write(90);            // فتح النافذة
    digitalWrite(FAN_PIN, HIGH);  // تشغيل المروحة
    Serial.println("⚠️ Ventilation Opened!");

    // إرسال الحالة كنص إلى Blynk
    //Blynk.virtualWrite(VIRTUAL_FAN, "Opened");
    //Blynk.virtualWrite(VIRTUAL_SERVO, "Opened");

  } else {
    myServo.write(0);            // غلق النافذة
    digitalWrite(FAN_PIN, LOW);  // إيقاف المروحة
    Serial.println("✅ Ventilation Closed!");

    // إرسال الحالة كنص إلى Blynk
   // Blynk.virtualWrite(VIRTUAL_FAN, "Closed");
    //Blynk.virtualWrite(VIRTUAL_SERVO, "Closed");
  }
}

