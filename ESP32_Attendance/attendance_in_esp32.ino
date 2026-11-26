#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

// -------- RFID Pins --------
#define SS_PIN 5
#define RST_PIN 22

// -------- Buzzer Pin ------
#define BUZZER_PIN 27

// -------- WiFi Details ---
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// 🔴 VERY IMPORTANT: Your PC IP (from Python output)
const char* serverIP = "YOUR_PC_IP";
const int serverPort = 5000;

// -------- Objects --------
MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiClient client;

// -------- Setup ----------
void setup() {
  Serial.begin(115200);

  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // LCD Init
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("RFID Attendance");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");

  // RFID Init
  SPI.begin();
  rfid.PCD_Init();

  // WiFi Connect
  lcd.clear();
  lcd.print("Connecting WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
  lcd.setCursor(0, 1);
  lcd.print("RFID Ready");

  Serial.println("RFID Ready");
}

// -------- Beep Function ----
void beepOnce() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
}

// -------- Main Loop --------
void loop() {

  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  String uidString = "";
  Serial.print("UID: ");

  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) Serial.print("0");
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
    uidString += String(rfid.uid.uidByte[i], HEX);
  }

  Serial.println();

  // Show on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Card Detected");
  lcd.setCursor(0, 1);
  lcd.print(uidString);

  // Beep
  beepOnce();

  // Send to PC
  sendToServer(uidString);

  // Stop reading same card repeatedly
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  delay(3000);  // 3 seconds gap
}

// -------- Send to Server ----
void sendToServer(String uid) {

  if (!client.connect(serverIP, serverPort)) {
    Serial.println("Server Connection Failed");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Server Error");
    lcd.setCursor(0, 1);
    lcd.print("Not Logged");
    return;
  }

  String url = "/mark?uid=" + uid;

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + serverIP + "\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("Sent to server: " + uid);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Attendance");
  lcd.setCursor(0, 1);
  lcd.print("Marked");

  delay(1000);
  client.stop();
}
