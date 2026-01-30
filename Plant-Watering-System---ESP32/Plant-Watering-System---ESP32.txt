#define BLYNK_TEMPLATE_ID "TMPL3RNr6PY9R"
#define BLYNK_TEMPLATE_NAME "Plant watering system"
#define BLYNK_AUTH_TOKEN "-TYS0f4Ep0oBuzElqr7FUOyNsq1YzzmU"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// WiFi details
char ssid[] = "HARIDEVAN2006";
char pass[] = "HARIDEVAN@1410";

// Pin connections
#define SOIL_PIN 33
#define RELAY_PIN 2
#define TEMP_PIN 23

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Temperature sensor
OneWire oneWire(TEMP_PIN);
DallasTemperature sensors(&oneWire);

// Blynk Virtual Pins
#define VPIN_RELAY V1
#define VPIN_SOIL V2
#define VPIN_TEMP V3

// Relay control
BLYNK_WRITE(VPIN_RELAY) {
  int value = param.asInt();
  digitalWrite(RELAY_PIN, value);
}

void showFakeLCD() {
  // 1. Loading... (5 sec)
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Loading...");
  delay(5000);

  // 2. Temperature fake (10 sec)
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperature:");
  lcd.setCursor(0, 1);
  lcd.print("20 Degree C");
  delay(10000);

  // 3. Moisture fake (10 sec)
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Moisture At");
  lcd.setCursor(0, 1);
  lcd.print("Right Level");
  delay(10000);

  lcd.clear();
}

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  lcd.init();
  lcd.backlight();

  // Fake LCD animation here
  showFakeLCD();

  sensors.begin();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
  delay(1500);
  lcd.clear();
}

void loop() {
  Blynk.run();

  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);

  int soilValue = analogRead(SOIL_PIN);
  int moisturePercent = map(soilValue, 4095, 0, 0, 100);

  if (moisturePercent < 0) moisturePercent = 0;
  if (moisturePercent > 100) moisturePercent = 100;

  Blynk.virtualWrite(VPIN_TEMP, tempC);
  Blynk.virtualWrite(VPIN_SOIL, moisturePercent);

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(tempC);
  lcd.print((char)223);
  lcd.print("C   ");

  lcd.setCursor(0, 1);
  lcd.print("Soil: ");
  lcd.print(moisturePercent);
  lcd.print("%    ");

  delay(1000);
}