#define SoilMoisture 33
#define Pump 18

void ControlIrrigation();

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
Serial.println("System Initialized");
pinMode(SoilMoisture, INPUT);
pinMode(Pump, OUTPUT);
delay(100);
}

void loop() {
  // put your main code here, to run repeatedly:
  ControlIrrigation();
  delay(500);

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