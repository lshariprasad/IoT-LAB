#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>



// ================= OLED =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);



// ================= DHT11 =================
#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);



// ================= MOTOR DRIVER =================

// Motor
#define IN1 26
#define IN2 27

// Exhaust Fan
#define IN3 32
#define IN4 33



// ================= SENSORS =================
#define FLAME_SENSOR 25
#define GAS_SENSOR   34



void setup() {

  Serial.begin(115200);



  // ===== Motor Driver Pins =====
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);



  // ===== Sensor Pins =====
  pinMode(FLAME_SENSOR, INPUT);
  pinMode(GAS_SENSOR, INPUT);



  // ===== DHT =====
  dht.begin();



  // ===== OLED =====
  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED Failed");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);



  // ===== Start Motor =====
  motorON();

  // ===== Fan OFF =====
  fanOFF();
}



void loop() {

  // ===== Read Sensors =====
  int flameValue = digitalRead(FLAME_SENSOR);
  int gasValue   = digitalRead(GAS_SENSOR);

  float humidity = dht.readHumidity();
  float temp     = dht.readTemperature();



  // =========================================
  // FLAME SENSOR LOGIC
  // =========================================

  if (flameValue == LOW) {

    // Flame Detected
    motorOFF();

  } else {

    // No Flame
    motorON();
  }



  // =========================================
  // GAS SENSOR LOGIC
  // =========================================
  // CHANGED LOGIC HERE
  // Your sensor gives LOW when gas detected
  // =========================================

  if (gasValue == LOW) {

    // Gas Detected
    fanON();

  } else {

    // No Gas
    fanOFF();
  }



  // =========================================
  // OLED DISPLAY
  // =========================================

  display.clearDisplay();

  display.setCursor(0, 0);
  display.println("SMART SAFETY SYSTEM");



  // ===== Flame Status =====
  if (flameValue == LOW) {

    display.println("FIRE DETECTED");
    display.println("MOTOR STOPPED");

  } else {

    display.println("Motor Running");
  }



  // ===== Gas Status =====
  if (gasValue == LOW) {

    display.println("GAS DETECTED");
    display.println("FAN ACTIVATED");

  } else {

    display.println("Fan OFF");
  }



  // ===== Temperature & Humidity =====
  display.print("Temp: ");
  display.print(temp);
  display.println(" C");

  display.print("Hum : ");
  display.print(humidity);
  display.println(" %");



  display.display();



  // ===== Serial Monitor =====
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print(" C  ");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");



  delay(1000);
}



// =========================================
// MOTOR FUNCTIONS
// =========================================

void motorON() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}

void motorOFF() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}



// =========================================
// FAN FUNCTIONS
// =========================================

void fanON() {

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void fanOFF() {

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
