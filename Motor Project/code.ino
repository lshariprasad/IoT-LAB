/*
 * ============================================================
 *   ADVANCED MOTOR PROTECTION SYSTEM v2.0
 *   Target: Arduino Uno/Nano
 * ------------------------------------------------------------
 *   Sensors  : DHT11/22, ACS712-5A, Vibration SW-420
 *   Outputs  : Relay, Buzzer, Status LED, I2C LCD 16x2
 * ------------------------------------------------------------
 *   Features :
 *     - Overcurrent, over-temp, over-humidity protection
 *     - Vibration-based emergency stop with debounce
 *     - Hysteresis on all thresholds (no relay chatter)
 *     - Fault log (last 5 faults, millis-based timestamps)
 *     - Multi-screen LCD cycling (Status / Sensors / Faults)
 *     - Running-hours tracker (auto-saved to EEPROM)
 *     - Serial command interface (HELP / RESET / SET / STATUS)
 *     - Soft-start delay after fault clear
 *     - Status LED patterns per fault type
 *     - NaN / sensor-offline detection
 * ============================================================
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <EEPROM.h>

// ─── PIN MAP ───────────────────────────────────────────────
#define DHTPIN          2
#define DHTTYPE         DHT11
#define VIBRATION_PIN   3      // SW-420 (interrupt-capable)
#define RELAY_PIN       4
#define BUZZER_PIN      5
#define STATUS_LED_PIN  6      // Dedicated status LED
#define CURRENT_SENSOR  A0

// ─── TUNABLE THRESHOLDS ────────────────────────────────────
struct Thresholds {
  float tempTrip    = 40.0;   // °C  — motor OFF above this
  float tempHyst    =  2.0;   // °C  — motor ON again when temp < (trip - hyst)
  float humTrip     = 80.0;   // %
  float humHyst     =  5.0;   // %
  float currentTrip =  4.0;   // A   — overcurrent trip
  float currentHyst =  0.3;   // A
  unsigned long vibDebounce = 200; // ms — ignore re-triggers within this window
  unsigned long softStart   = 3000; // ms — delay before relay re-closes after fault
};
Thresholds thr;

// ─── EEPROM LAYOUT ─────────────────────────────────────────
#define EEPROM_HOURS_ADDR  0   // 4 bytes — uint32_t running minutes

// ─── FAULT CODES ───────────────────────────────────────────
enum FaultCode : uint8_t {
  FAULT_NONE      = 0,
  FAULT_OVERTEMP  = 1,
  FAULT_OVERHUM   = 2,
  FAULT_OVERCURR  = 3,
  FAULT_VIBRATION = 4,
  FAULT_SENSOR    = 5     // DHT offline / NaN
};

const char* faultName(FaultCode c) {
  switch (c) {
    case FAULT_OVERTEMP:  return "OverTemp";
    case FAULT_OVERHUM:   return "OverHum";
    case FAULT_OVERCURR:  return "OverCurr";
    case FAULT_VIBRATION: return "Vibration";
    case FAULT_SENSOR:    return "SensorFail";
    default:              return "None";
  }
}

// ─── FAULT LOG ─────────────────────────────────────────────
#define FAULT_LOG_SIZE 5
struct FaultEntry {
  FaultCode code;
  unsigned long timestamp; // millis()
  float value;             // e.g. temperature at trip point
};
FaultEntry faultLog[FAULT_LOG_SIZE];
uint8_t faultLogHead = 0;
uint8_t faultLogCount = 0;

void logFault(FaultCode code, float value) {
  // Avoid duplicating the same active fault
  if (faultLogCount > 0) {
    uint8_t last = (faultLogHead == 0) ? FAULT_LOG_SIZE - 1 : faultLogHead - 1;
    if (faultLog[last].code == code) return;
  }
  faultLog[faultLogHead] = {code, millis(), value};
  faultLogHead = (faultLogHead + 1) % FAULT_LOG_SIZE;
  if (faultLogCount < FAULT_LOG_SIZE) faultLogCount++;
}

// ─── SYSTEM STATE ──────────────────────────────────────────
enum MotorState { STATE_RUNNING, STATE_FAULT, STATE_SOFTSTART };
MotorState motorState    = STATE_RUNNING;
FaultCode  activeFault   = FAULT_NONE;
unsigned long faultClearTime = 0;  // when fault condition last cleared

// ─── SENSOR DATA ───────────────────────────────────────────
float  temperature   = 0;
float  humidity      = 0;
float  current       = 0;
bool   sensorOk      = false;

// ─── VIBRATION (interrupt-driven) ──────────────────────────
volatile bool vibFlag = false;
unsigned long lastVibTime = 0;

// Fixed: removed IRAM_ATTR for AVR compatibility
void onVibration() {
  vibFlag = true;
}

// ─── RUNNING HOURS ─────────────────────────────────────────
uint32_t runningMinutes = 0;
unsigned long lastMinuteMark = 0;
unsigned long lastEepromSave = 0;
#define EEPROM_SAVE_INTERVAL 60000UL  // save every 60 s

void loadRunningMinutes() {
  EEPROM.get(EEPROM_HOURS_ADDR, runningMinutes);
  if (runningMinutes == 0xFFFFFFFF) runningMinutes = 0; // blank EEPROM
}
void saveRunningMinutes() {
  EEPROM.put(EEPROM_HOURS_ADDR, runningMinutes);
}

// ─── LCD ───────────────────────────────────────────────────
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

enum DisplayScreen { SCREEN_STATUS, SCREEN_SENSORS, SCREEN_FAULT };
DisplayScreen currentScreen = SCREEN_STATUS;
unsigned long lastScreenSwitch = 0;
#define SCREEN_INTERVAL 3000UL

// Custom LCD chars
byte degreeChar[8] = {0b00110,0b01001,0b01001,0b00110,0b00000,0b00000,0b00000,0b00000};
byte heartChar[8]  = {0b00000,0b01010,0b11111,0b11111,0b01110,0b00100,0b00000,0b00000};
byte boltChar[8]   = {0b00100,0b01100,0b11111,0b00110,0b11110,0b00110,0b00100,0b00000};

// ─── BUZZER PATTERN ────────────────────────────────────────
unsigned long buzzerTimer  = 0;
bool          buzzerState  = false;
int           buzzerPattern = 0;   // 0=off 1=slow 2=fast 3=SOS

void updateBuzzer() {
  if (buzzerPattern == 0) {
    digitalWrite(BUZZER_PIN, LOW);
    return;
  }
  unsigned long interval = (buzzerPattern == 1) ? 800 : (buzzerPattern == 2) ? 200 : 150;
  if (millis() - buzzerTimer >= interval) {
    buzzerState = !buzzerState;
    digitalWrite(BUZZER_PIN, buzzerState ? HIGH : LOW);
    buzzerTimer = millis();
  }
}

// ─── STATUS LED BLINK ──────────────────────────────────────
unsigned long ledTimer = 0;
bool          ledState = false;
int           ledBlinkRate = 1000; // ms per cycle

void updateLED() {
  if (millis() - ledTimer >= (unsigned long)ledBlinkRate) {
    ledState = !ledState;
    digitalWrite(STATUS_LED_PIN, ledState ? HIGH : LOW);
    ledTimer = millis();
  }
}

// ─── SERIAL COMMAND INTERFACE ──────────────────────────────
String serialBuf = "";

void printStatus() {
  Serial.println(F("===== MOTOR PROTECTION SYSTEM v2.0 ====="));
  Serial.print(F("Motor State  : "));
  if (motorState == STATE_RUNNING)  Serial.println(F("RUNNING"));
  else if (motorState == STATE_FAULT) Serial.println(F("FAULT"));
  else Serial.println(F("SOFT-START"));

  Serial.print(F("Temperature  : ")); Serial.print(temperature, 1); Serial.println(F(" C"));
  Serial.print(F("Humidity     : ")); Serial.print(humidity, 0);    Serial.println(F(" %"));
  Serial.print(F("Current      : ")); Serial.print(current, 3);     Serial.println(F(" A"));
  Serial.print(F("Active Fault : ")); Serial.println(faultName(activeFault));
  Serial.print(F("Running Time : ")); Serial.print(runningMinutes / 60); 
  Serial.print(F("h ")); Serial.print(runningMinutes % 60); Serial.println(F("m"));
  Serial.println(F("Fault Log:"));
  for (uint8_t i = 0; i < faultLogCount; i++) {
    uint8_t idx = (faultLogHead - faultLogCount + i + FAULT_LOG_SIZE) % FAULT_LOG_SIZE;
    Serial.print(F("  [")); Serial.print(i + 1); Serial.print(F("] "));
    Serial.print(faultName(faultLog[idx].code));
    Serial.print(F(" @ ")); Serial.print(faultLog[idx].timestamp / 1000);
    Serial.print(F("s  val=")); Serial.println(faultLog[idx].value, 2);
  }
  Serial.println(F("========================================="));
}

void printHelp() {
  Serial.println(F("Commands:"));
  Serial.println(F("  STATUS           — print system status"));
  Serial.println(F("  RESET FAULTS     — clear fault log"));
  Serial.println(F("  RESET HOURS      — zero running-time counter"));
  Serial.println(F("  SET TEMP  <val>  — set over-temp trip (°C)"));
  Serial.println(F("  SET HUM   <val>  — set over-hum  trip (%)"));
  Serial.println(F("  SET CURR  <val>  — set overcurrent trip (A)"));
  Serial.println(F("  HELP             — this menu"));
}

void handleSerial() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      serialBuf.trim();
      serialBuf.toUpperCase();
      if (serialBuf == "STATUS") {
        printStatus();
      } else if (serialBuf == "HELP") {
        printHelp();
      } else if (serialBuf == "RESET FAULTS") {
        faultLogCount = 0; faultLogHead = 0;
        Serial.println(F("Fault log cleared."));
      } else if (serialBuf == "RESET HOURS") {
        runningMinutes = 0; saveRunningMinutes();
        Serial.println(F("Running hours reset."));
      } else if (serialBuf.startsWith("SET TEMP ")) {
        thr.tempTrip = serialBuf.substring(9).toFloat();
        Serial.print(F("Temp trip set to ")); Serial.println(thr.tempTrip);
      } else if (serialBuf.startsWith("SET HUM ")) {
        thr.humTrip = serialBuf.substring(8).toFloat();
        Serial.print(F("Hum trip set to ")); Serial.println(thr.humTrip);
      } else if (serialBuf.startsWith("SET CURR ")) {
        thr.currentTrip = serialBuf.substring(9).toFloat();
        Serial.print(F("Current trip set to ")); Serial.println(thr.currentTrip);
      } else if (serialBuf.length() > 0) {
        Serial.println(F("Unknown command. Type HELP."));
      }
      serialBuf = "";
    } else {
      serialBuf += c;
    }
  }
}

// ─── LCD DISPLAY SCREENS ───────────────────────────────────
void lcdStatusScreen() {
  lcd.setCursor(0, 0);
  if (motorState == STATE_RUNNING) {
    lcd.print("MTR:ON  ");
    lcd.write(1); // heart
  } else if (motorState == STATE_FAULT) {
    lcd.print("MTR:FAULT! ");
    lcd.write(2); // bolt
  } else {
    lcd.print("MTR:STARTING");
  }

  // Row 1: running time
  lcd.setCursor(0, 1);
  lcd.print("Run:");
  lcd.print(runningMinutes / 60);
  lcd.print("h");
  lcd.print(runningMinutes % 60);
  lcd.print("m       ");
}

void lcdSensorsScreen() {
  // Row 0: Temp + Humidity
  lcd.setCursor(0, 0);
  lcd.print("T:");
  if (sensorOk) {
    lcd.print(temperature, 1);
    lcd.write(0); // degree
    lcd.print(" H:");
    lcd.print(humidity, 0);
    lcd.print("%  ");
  } else {
    lcd.print("ERR  H:ERR  ");
  }

  // Row 1: Current
  lcd.setCursor(0, 1);
  lcd.print("I:");
  lcd.print(current, 2);
  lcd.print("A ");
  if (current > thr.currentTrip * 0.8) {
    lcd.print("!HIGH! ");
  } else {
    lcd.print("       ");
  }
}

void lcdFaultScreen() {
  lcd.setCursor(0, 0);
  lcd.print("FAULT:");
  lcd.print(faultName(activeFault));
  lcd.print("        ");

  lcd.setCursor(0, 1);
  if (faultLogCount > 0) {
    uint8_t last = (faultLogHead == 0) ? FAULT_LOG_SIZE - 1 : faultLogHead - 1;
    lcd.print("@");
    lcd.print(faultLog[last].timestamp / 1000);
    lcd.print("s v=");
    lcd.print(faultLog[last].value, 1);
    lcd.print("     ");
  } else {
    lcd.print("No fault history");
  }
}

// ─── SETUP ─────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);
  Serial.println(F("Motor Protection System v2.0 — Booting..."));

  pinMode(VIBRATION_PIN, INPUT);
  pinMode(RELAY_PIN,     OUTPUT);
  pinMode(BUZZER_PIN,    OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);

  // Motor OFF during init
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(STATUS_LED_PIN, LOW);

  lcd.init();
  lcd.backlight();
  lcd.createChar(0, degreeChar);
  lcd.createChar(1, heartChar);
  lcd.createChar(2, boltChar);

  lcd.setCursor(0, 0); lcd.print("  MOTOR SHIELD  ");
  lcd.setCursor(0, 1); lcd.print("   v2.0  INIT   ");
  delay(1500);
  lcd.clear();

  dht.begin();
  loadRunningMinutes();

  // Attach vibration interrupt (RISING = SW-420 triggers)
  attachInterrupt(digitalPinToInterrupt(VIBRATION_PIN), onVibration, RISING);

  // Soft-start: wait a moment, then bring motor up
  motorState = STATE_SOFTSTART;
  faultClearTime = millis();

  Serial.println(F("Boot OK. Type HELP for commands."));
}

// ─── MAIN LOOP ─────────────────────────────────────────────
void loop() {
  unsigned long now = millis();

  // ── 1. Read Sensors ──────────────────────────────────────
  float rawTemp = dht.readTemperature();
  float rawHum  = dht.readHumidity();
  sensorOk = (!isnan(rawTemp) && !isnan(rawHum));
  if (sensorOk) { temperature = rawTemp; humidity = rawHum; }

  int rawADC = analogRead(CURRENT_SENSOR);
  float voltage = rawADC * (5.0 / 1023.0);
  current = abs((voltage - 2.5) / 0.185);  // ACS712-5A

  // ── 2. Vibration Debounce ─────────────────────────────────
  bool vibTriggered = false;
  if (vibFlag) {
    vibFlag = false;
    if (now - lastVibTime > thr.vibDebounce) {
      lastVibTime  = now;
      vibTriggered = true;
    }
  }

  // ── 3. Fault Detection (with hysteresis) ─────────────────
  FaultCode newFault = FAULT_NONE;

  if (!sensorOk) {
    newFault = FAULT_SENSOR;
  } else if (vibTriggered) {
    newFault = FAULT_VIBRATION;
  } else if (motorState == STATE_RUNNING || motorState == STATE_SOFTSTART) {
    // Trip conditions
    if (temperature > thr.tempTrip)    newFault = FAULT_OVERTEMP;
    else if (humidity > thr.humTrip)   newFault = FAULT_OVERHUM;
    else if (current  > thr.currentTrip) newFault = FAULT_OVERCURR;
  } else if (motorState == STATE_FAULT) {
    // Stay in fault until ALL values drop below (trip - hysteresis)
    bool tempOk = sensorOk && (temperature < thr.tempTrip  - thr.tempHyst);
    bool humOk  = sensorOk && (humidity    < thr.humTrip   - thr.humHyst);
    bool currOk = (current  < thr.currentTrip - thr.currentHyst);
    bool allClear = tempOk && humOk && currOk;

    if (allClear && activeFault != FAULT_VIBRATION) {
      // Transition to soft-start
      motorState = STATE_SOFTSTART;
      faultClearTime = now;
      activeFault = FAULT_NONE;
      Serial.println(F("Fault cleared — soft-start initiated."));
    }
    // Vibration fault requires manual RESET via serial
  }

  // ── 4. Apply Fault ───────────────────────────────────────
  if (newFault != FAULT_NONE && motorState != STATE_FAULT) {
    motorState  = STATE_FAULT;
    activeFault = newFault;
    float faultValue = (newFault == FAULT_OVERTEMP)  ? temperature :
                       (newFault == FAULT_OVERHUM)   ? humidity    :
                       (newFault == FAULT_OVERCURR)  ? current     : 0.0;
    logFault(newFault, faultValue);
    Serial.print(F("FAULT TRIGGERED: ")); Serial.println(faultName(newFault));
    digitalWrite(RELAY_PIN, LOW);  // Motor HARD OFF
  }

  // ── 5. Soft-Start Completion ─────────────────────────────
  if (motorState == STATE_SOFTSTART && (now - faultClearTime >= thr.softStart)) {
    motorState = STATE_RUNNING;
    Serial.println(F("Motor running."));
  }

  // ── 6. Relay Control ─────────────────────────────────────
  if (motorState == STATE_RUNNING) {
    digitalWrite(RELAY_PIN, HIGH);
  } else {
    digitalWrite(RELAY_PIN, LOW);
  }

  // ── 7. Buzzer & LED Patterns ─────────────────────────────
  if (motorState == STATE_FAULT) {
    switch (activeFault) {
      case FAULT_VIBRATION: buzzerPattern = 3; break;  // SOS-like rapid
      case FAULT_OVERCURR:  buzzerPattern = 2; break;  // fast
      default:              buzzerPattern = 1; break;  // slow
    }
    ledBlinkRate = (activeFault == FAULT_OVERCURR) ? 100 : 300;
  } else if (motorState == STATE_SOFTSTART) {
    buzzerPattern = 0;
    ledBlinkRate  = 500;
  } else {
    buzzerPattern = 0;
    ledBlinkRate  = 2000; // slow heartbeat = healthy
  }
  updateBuzzer();
  updateLED();

  // ── 8. Running Time ──────────────────────────────────────
  if (motorState == STATE_RUNNING) {
    if (now - lastMinuteMark >= 60000UL) {
      runningMinutes++;
      lastMinuteMark = now;
    }
    if (now - lastEepromSave >= EEPROM_SAVE_INTERVAL) {
      saveRunningMinutes();
      lastEepromSave = now;
    }
  }

  // ── 9. LCD Multi-Screen ──────────────────────────────────
  if (now - lastScreenSwitch >= SCREEN_INTERVAL) {
    currentScreen = (DisplayScreen)((currentScreen + 1) % 3);
    lastScreenSwitch = now;
    lcd.clear();
  }
  switch (currentScreen) {
    case SCREEN_STATUS:  lcdStatusScreen();  break;
    case SCREEN_SENSORS: lcdSensorsScreen(); break;
    case SCREEN_FAULT:   lcdFaultScreen();   break;
  }

  // ── 10. Serial Commands ──────────────────────────────────
  handleSerial();

  // ── 11. Serial Telemetry (every 2s) ──────────────────────
  static unsigned long lastTelem = 0;
  if (now - lastTelem >= 2000UL) {
    lastTelem = now;
    Serial.print(F("T:")); Serial.print(temperature, 1);
    Serial.print(F(" H:")); Serial.print(humidity, 0);
    Serial.print(F(" I:")); Serial.print(current, 3);
    Serial.print(F(" State:"));
    if (motorState == STATE_RUNNING)    Serial.print(F("RUN"));
    else if (motorState == STATE_FAULT) { Serial.print(F("FAULT[")); Serial.print(faultName(activeFault)); Serial.print(F("]")); }
    else Serial.print(F("SOFTSTART"));
    Serial.print(F(" Run:")); Serial.print(runningMinutes); Serial.println(F("min"));
  }

  delay(100); // 10 Hz loop — fast enough for all sensors
}
