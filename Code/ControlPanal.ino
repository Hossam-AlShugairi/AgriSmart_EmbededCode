#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define BTN_UP       15
#define BTN_SELECT   4
#define BTN_INC      16
#define BTN_DEC      17

int tempThreshold = 30;
int humidityThreshold = 50;
int gasThreshold = 200;
int soilThreshold = 40;

int currentSensor = 0;
bool inEditMode = false;

String sensorNames[4] = {"Temp", "Humidity", "Gas", "Soil"};

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

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
  displayMenu();

  //  Moving around sensors
  if (digitalRead(BTN_UP) == LOW) {
    currentSensor = (currentSensor - 1 + 4) % 4;
    delay(300);
  }

  // Chose edit mode
  if (digitalRead(BTN_SELECT) == LOW) {
    inEditMode = !inEditMode;
    delay(300);
  }

  // change value
  if (inEditMode) {
    if (digitalRead(BTN_INC) == LOW) {
      changeValue(currentSensor, 1);
      delay(200);
    }
    if (digitalRead(BTN_DEC) == LOW) {
      changeValue(currentSensor, -1);
      delay(200);
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