int Pump = 26; // Corrected variable name and declaration

void setup() {
  Serial.begin(115200);
  pinMode(Pump, OUTPUT);
}

void loop() {
  digitalWrite(Pump, HIGH);  // Turn OFF the pump (if active LOW)
  Serial.println("Pump Disabled");
 /* delay(5000);

  digitalWrite(Pump, LOW);   // Turn ON the pump (if active LOW)
  Serial.println("Pump Working");
  delay(5000);
  */
}
