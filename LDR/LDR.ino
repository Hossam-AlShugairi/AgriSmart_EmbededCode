#define LDR 13  

void setup() {
  pinMode(LDR,INPUT);
  Serial.begin(115200);  
  delay(1000);
}

void loop() {
  int ldrValue = digitalRead(LDR);  
  Serial.print("LDR Value: ");
  Serial.println(ldrValue);  
  
  delay(500);  // Wait 500ms
}
