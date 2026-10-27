#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"

// Install ArduinoJson library using Library Manager.
static const unsigned long POLL_MS = 2000;
static const unsigned long REPORT_MS = 5000;
static const unsigned long WIFI_RETRY_MS = 10000;
static const unsigned long MIN_INTERVAL_MS = 10000;
static const unsigned long MAX_INTERVAL_MS = 86400000;
static const unsigned long MIN_DURATION_MS = 1000;
static const unsigned long MAX_DURATION_MS = 60000;

unsigned long intervalMs = 600000;
unsigned long durationMs = 5000;
unsigned long lastCycleStart = 0;
unsigned long sprayStarted = 0;
unsigned long lastPoll = 0;
unsigned long lastReport = 0;
unsigned long lastWiFiAttempt = 0;
bool spraying = false;
bool timerEnabled = true;
bool cycleRecorded = false;
String lastCommandId = "";

String apiUrl(const char* path) {
  return String(API_BASE_URL) + path;
}

void setPump(bool on) {
  digitalWrite(RELAY_PIN, on ? RELAY_ON : RELAY_OFF);
  spraying = on;
  if (on) sprayStarted = millis();
}

void stopPump() {
  digitalWrite(RELAY_PIN, RELAY_OFF);
  spraying = false;
}

bool postJson(const String& url, const String& body, String* response = nullptr) {
  if (WiFi.status() != WL_CONNECTED) return false;
  HTTPClient http;
  http.setConnectTimeout(4000);
  http.setTimeout(4000);
  if (!http.begin(url)) return false;
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-API-Token", API_TOKEN);
  int code = http.POST(body);
  if (response && code > 0) *response = http.getString();
  http.end();
  return code >= 200 && code < 300;
}

void reportState() {
  unsigned long now = millis();
  unsigned long nextSec = 0;
  if (timerEnabled) {
    unsigned long base = cycleRecorded ? lastCycleStart : now;
    unsigned long passed = now - base;
    nextSec = passed >= intervalMs ? 0 : (intervalMs - passed + 999) / 1000;
  }
  unsigned long elapsed = spraying ? (now - sprayStarted) / 1000 : 0;

  String body = "{";
  body += "\"spraying\":" + String(spraying ? "true" : "false") + ",";
  body += "\"timer\":" + String(timerEnabled ? "true" : "false") + ",";
  body += "\"interval\":" + String(intervalMs / 1000) + ",";
  body += "\"duration\":" + String(durationMs / 1000) + ",";
  body += "\"next_seconds\":" + String(nextSec) + ",";
  body += "\"elapsed_seconds\":" + String(elapsed);
  body += "}";
  postJson(apiUrl("report.php"), body);
}

void pollCommands() {
  if (WiFi.status() != WL_CONNECTED) return;
  HTTPClient http;
  http.setConnectTimeout(4000);
  http.setTimeout(4000);
  String url = apiUrl("command.php");
  // The command endpoint is POST-only; ESP32 obtains queued commands from
  // command_poll.php to keep browser actions and device polling separate.
  http.begin(apiUrl("command_poll.php"));
  http.addHeader("X-API-Token", API_TOKEN);
  int code = http.GET();
  if (code == 200) {
    String payload = http.getString();
    DynamicJsonDocument doc(1024);
    if (deserializeJson(doc, payload) == DeserializationError::Ok) {
      String id = doc["id"] | "";
      String action = doc["action"] | "";
      if (id.length() && id != "none" && id != lastCommandId) {
        lastCommandId = id;
        if (action == "start") {
          setPump(true);
        } else if (action == "stop") {
          stopPump();
        } else if (action == "settings") {
          long i = doc["interval"] | 600;
          long d = doc["duration"] | 5;
          intervalMs = constrain(i, 10L, 86400L) * 1000UL;
          durationMs = constrain(d, 1L, 60L) * 1000UL;
          timerEnabled = doc["timer"] | true;
          lastCycleStart = millis();
          cycleRecorded = true;
        }
      }
    }
  }
  http.end();
}

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  unsigned long now = millis();
  if (now - lastWiFiAttempt < WIFI_RETRY_MS) return;
  lastWiFiAttempt = now;
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void setup() {
  Serial.begin(115200);
  digitalWrite(RELAY_PIN, RELAY_OFF);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, RELAY_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  lastWiFiAttempt = millis();
  lastCycleStart = millis();
  cycleRecorded = true;
  Serial.println("ESP32 cloud mist controller starting");
}

void loop() {
  unsigned long now = millis();
  connectWiFi();

  // Pump safety timeout is enforced locally even if Wi-Fi/server goes down.
  if (spraying && now - sprayStarted >= durationMs) {
    stopPump();
  }

  if (timerEnabled && !spraying && now - lastCycleStart >= intervalMs) {
    lastCycleStart = now;
    cycleRecorded = true;
    setPump(true);
  }

  if (now - lastPoll >= POLL_MS) {
    lastPoll = now;
    pollCommands();
  }
  if (now - lastReport >= REPORT_MS) {
    lastReport = now;
    reportState();
  }
  delay(5);
}
