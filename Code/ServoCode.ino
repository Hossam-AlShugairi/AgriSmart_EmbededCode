#include <ESP32Servo.h>

Servo myServo;  
int servoPin = 13; // GPIO16

void setup() {
  Serial.begin(115200); // Start Serial Monitor
  myServo.attach(servoPin); // Attach servo to GPIO16
  Serial.println("Servo SG90 with ESP32 started!");
}

void loop() {
  // Move from 0° to 180°
  for (int angle = 0; angle <= 180; angle += 50) { 
    myServo.write(angle);
    Serial.print("Moving to angle: ");
    Serial.println(angle);
    delay(500);
  }

  // Move back from 180° to 0°
  for (int angle = 180; angle >= 0; angle -= 50) { 
    myServo.write(angle);
    Serial.print("Moving to angle: ");
    Serial.println(angle);
    delay(500);
  }
}
