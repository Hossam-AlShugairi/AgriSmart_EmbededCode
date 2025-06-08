#define BLYNK_TEMPLATE_ID "TMPL2cmff-cKM"
#define BLYNK_TEMPLATE_NAME "Blynk ESP32 DHT11"

#include <ESP32Servo.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include "DHT.h"

#define BLYNK_PRINT Serial


char auth[] = "8CNr1BAUGdYCW2QA814y1KHqLGVcXSVO";  // Authonticaton code
char ssid[] = "Hossam";                            // wifi ssid
char pass[] = "12345678";                          // wifi pass

// Sensors pins
#define DHTPIN 13
#define MQ2_PIN 14
#define SoilMoisture 26

//Acuatours pins
#define SERVO_PIN 33
#define FAN_PIN 4
#define Pump 2


#define DHTTYPE DHT11

//Functions
void readDHT();
void MQ2Reading();
void controlVentilation();
void ControlIrrigation();

//variables
float TEMP_THRESHOLD = 30 ;
float HUMIDITY_THRESHOLD = 50 ;
float co2_ppm;
float temperature, humidity;

DHT dht(DHTPIN, DHTTYPE);
Servo myServo;




// virtual pins to send data over internet 
#define VIRTUAL_TEMP V0
#define VIRTUAL_HUMIDITY V1
#define VIRTUAL_FAN V3
#define VIRTUAL_SERVO V2
#define Virtual_MQ2 V4

void setup() {
  Serial.begin(115200);
  Serial.println("MQ-2 Gas Sensor Initialization...");


  /*
WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi");
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {  // محاولة الاتصال لمدة 10 ثواني تقريباً
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n WiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n Failed to connect to WiFi!");
  }

  Blynk.begin(auth, ssid, pass);  // بدء الاتصال بـ Blynk


*/
  dht.begin();
  myServo.attach(SERVO_PIN);
  pinMode(FAN_PIN, OUTPUT);
  Serial.println("System Initialized");
}

void loop() {
  //checkWiFiStatus();
  // Blynk.run();
  readDHT();
  controlVentilation();
  MQ2Reading();
  ControlIrrigation();
  delay(5000);
}

void checkWiFiStatus() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi is connected.");
  } else {
    Serial.println("WiFi is not connected!");
  }
}


void readDHT() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("%  | Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");
  /*
  Blynk.virtualWrite(VIRTUAL_TEMP, temperature);
  Blynk.virtualWrite(VIRTUAL_HUMIDITY, humidity);
  */
}

void MQ2Reading() {
  int sensorValue = analogRead(MQ2_PIN);       
  float voltage = sensorValue * (3.3 / 4095.0); 

  // Serial.print("Raw Sensor Value: ");
  // Serial.print(sensorValue);
  // Serial.print(" | Voltage: ");
  //Serial.print(voltage);
  //Serial.println("V");

  co2_ppm = map(sensorValue, 0, 4095, 400, 5000);  

  Serial.print("Estimated CO2 Level: ");
  Serial.print(co2_ppm);
  Serial.println(" ppm");

  //  Blynk.virtualWrite(Virtual_MQ2, co2_ppm);
  delay(2000);  
}

void controlVentilation() {
  if (temperature > TEMP_THRESHOLD || humidity > HUMIDITY_THRESHOLD || co2_ppm > 1500) {
    myServo.write(90);           
    digitalWrite(FAN_PIN, HIGH); 
    Serial.println("Ventilation Opened!");


    /*
    Blynk.virtualWrite(VIRTUAL_FAN, "Opened");
    Blynk.virtualWrite(VIRTUAL_SERVO, "Opened");
    */

  } else {
    myServo.write(0);           
    digitalWrite(FAN_PIN, LOW);
    Serial.println("Ventilation Closed");

    /*
    Blynk.virtualWrite(VIRTUAL_FAN, "Closed");
    Blynk.virtualWrite(VIRTUAL_SERVO, "Closed");
    */
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
