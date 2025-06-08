#define soil_moisture_pin 15

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println(analogRead(soil_moisture_pin));
  delay(1000);
}