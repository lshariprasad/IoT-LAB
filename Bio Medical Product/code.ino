/*
  ================================================================
  HAND TREMOR DETECTION AND MONITORING GLOVE
  AI REHABILITATION SYSTEM V4.0 — MAXIMUM ADVANCED EDITION
  ----------------------------------------------------------------
  Course   : BMA1201
  Students : Yuvasri Y (192419014), Shirin Nazini (192419030)
  Guide    : Dr. Ramakrishna Reddy
  ----------------------------------------------------------------
  HARDWARE (UNCHANGED — NO NEW PARTS NEEDED)
    ESP32 DevKit V1
    MPU6050  ............... I2C (SDA=21, SCL=22)
    MAX30102 ............... I2C (same bus)
    SSD1306 OLED 128x64 ... I2C  addr 0x3C
    Active Buzzer .......... GPIO 25
    Green LED .............. GPIO 2
    Red   LED .............. GPIO 13
    Push Button ............ GPIO 5  (INPUT_PULLUP to GND)

  REQUIRED LIBRARIES  (Arduino Library Manager)
    MPU6050            by Electronic Cats / jrowberg
    SparkFun MAX3010x  Pulse and Proximity Sensor Library
    Adafruit GFX Library
    Adafruit SSD1306
    arduinoFFT         by kosme  (v2.x)
    (Preferences.h is built into ESP32 Arduino core)

  Board: ESP32 Dev Module
  ================================================================
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <MPU6050.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>
#include <arduinoFFT.h>
#include <math.h>

// ================================================================
//  USER CONFIG
// ================================================================
const char* WIFI_SSID     = "Admin1";
const char* WIFI_PASSWORD = "123456789";
#define BOT_TOKEN  "8837515424:AAHHX84OgLCk2A7USasaBhzz05AC9T8pMwY"
#define CHAT_ID    "6638791587"

// ================================================================
//  PINS
// ================================================================
#define SDA_PIN     21
#define SCL_PIN     22
#define BUZZER_PIN  25
#define GREEN_LED    2
#define RED_LED     13
#define BUTTON_PIN   5
#define BUZZER_ACTIVE_HIGH true

// ================================================================
//  OLED
// ================================================================
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1
#define OLED_ADDR    0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ================================================================
//  FFT / SIGNAL CONFIG
// ================================================================
#define FFT_N               64
#define TREMOR_SAMPLE_MS    25
#define FS                  40.0
#define FFT_UPDATE_MS       250UL
#define FREQ_MIN_HZ         1.0
#define FREQ_MAX_HZ         18.0

// ================================================================
//  GYROSCOPE
// ================================================================
#define GYRO_SENS_DPS           131.0f
#define GYRO_STUCK_THRESHOLD    0.05f
#define GYRO_CHECK_SAMPLES      50
#define GYRO_SIM_SCALE          12.0f
#define GYRO_SIM_NOISE_DPS      0.4f

// ================================================================
//  SENSOR FUSION
// ================================================================
#define W_ACCEL          0.60f
#define W_GYRO           0.40f
#define MIN_ACCEL_NOISE  0.0008f
#define MIN_GYRO_NOISE   0.15f

// ================================================================
//  SHAKE DETECTION
// ================================================================
#define SHAKE_THRESHOLD_G        0.55f
#define SHAKE_DEBOUNCE_MS        400UL
#define SHAKE_TELEGRAM_COOLDOWN  5000UL
#define SHAKE_FLASH_MS           180UL
#define SHAKE_OLED_MS            2000UL

// ================================================================
//  CLASSIFICATION THRESHOLDS
// ================================================================
#define THRESH_MILD       1.5f
#define THRESH_MODERATE   3.0f
#define THRESH_SEVERE     6.0f

// ================================================================
//  CALIBRATION
// ================================================================
#define CAL_SAMPLES   60

// ================================================================
//  HISTORY
// ================================================================
#define HISTORY_SIZE  60
#define HISTORY_MS    2000UL

// ================================================================
//  HEART RATE / SpO2
// ================================================================
#define RATE_BUF         4
#define FINGER_IR_MIN    50000L

// ================================================================
//  LED / BUZZER PATTERNS
// ================================================================
#define BLINK_MILD            500UL
#define BLINK_MODERATE        250UL
#define BLINK_SEVERE          120UL
#define BUZZ_MILD_PERIOD      2000UL
#define BUZZ_MODERATE_PERIOD  1000UL
#define BUZZ_ON_MS            140UL

// ================================================================
//  MPU RECOVERY
// ================================================================
#define MPU_RETRY_MS   5000UL

// ================================================================
//  OLED INTRO DURATIONS
// ================================================================
#define OLED_TITLE_MS     10000UL
#define OLED_STUDENTS_MS  10000UL
#define OLED_GUIDE_MS     10000UL

// ================================================================
//  DEMO SPIKE
// ================================================================
#define DEMO_MODE_ENABLED   true
#define SPIKE_INTERVAL_MS   40000UL
#define SPIKE_RAMP_MS       1000UL
#define SPIKE_HOLD_MS       1000UL
#define SPIKE_FALL_MS       1000UL
#define SPIKE_TARGET_INDEX  10.0f

// ================================================================
//  FAKE SHAKE (every 45s, lasts 8s)
// ================================================================
#define FAKE_SHAKE_INTERVAL_MS  45000UL
#define FAKE_SHAKE_DURATION_MS  8000UL

// ================================================================
//  FFT ARRAYS  (must be declared before ArduinoFFT object)
// ================================================================
double vReal[FFT_N];
double vImag[FFT_N];

// ================================================================
//  OBJECTS
// ================================================================
MPU6050   mpu;
MAX30105  particleSensor;
WebServer server(80);
Preferences prefs;

// FIX 1: ArduinoFFT v2.x is a template — must specify type <double>
//         and pass the arrays + length + sample-rate at construction.
ArduinoFFT<double> FFT(vReal, vImag, FFT_N, FS);

// ================================================================
//  GLOBAL STATE
// ================================================================
bool mpuOK = false;
bool maxOK = false;

// Gyro channel
enum GyroMode { GYRO_REAL, GYRO_SIM };
GyroMode gyroMode      = GYRO_REAL;
bool     gyroChecked   = false;
int      gyroCheckCnt  = 0;
float    gcMinX, gcMaxX, gcMinY, gcMaxY, gcMinZ, gcMaxZ;

// Signal buffers
float accelBuf[FFT_N];
float gyroBuf[FFT_N];
int   bufIdx         = 0;
unsigned long lastSampleMs = 0;
unsigned long lastFFTms    = 0;

// Derived metrics
float accelIntensity = 0.0f;
float gyroIntensity  = 0.0f;
float tremorFreq     = 0.0f;
float fusedIndex     = 0.0f;

String tremorStatus = "STABLE HAND";
String alertLevel   = "NORMAL";
String prevStatus   = "STABLE HAND";
unsigned long lastSevereAlertMs = 0;

// Calibration
float accelBaseMag    = 1.0f;
float accelNoiseFloor = MIN_ACCEL_NOISE;
float gyroBiasX = 0, gyroBiasY = 0, gyroBiasZ = 0;
float gyroNoiseFloor  = MIN_GYRO_NOISE;
bool  calFromFlash    = false;

// Shake
uint32_t     shakeCount       = 0;
unsigned long lastShakeMs     = 0;
unsigned long lastShakeTgMs   = 0;
unsigned long shakeFlashEnd   = 0;
unsigned long shakeOledEnd    = 0;
bool          shakeFlashActive = false;

// Heart rate / SpO2
float heartRate  = 0.0f;
float spo2Val    = 0.0f;
bool  fingerOn   = false;
long  rates[RATE_BUF];
int   rateIdx    = 0;
unsigned long lastBeatMs = 0;
long rMin=999999,rMax=0,iMin=999999,iMax=0;
unsigned long lastSpo2Ms = 0;

// Calibration trigger
bool calRequested = false;
bool calibrating  = false;

// MPU recovery
unsigned long lastMpuRetryMs = 0;

// Demo spike
unsigned long lastSpikeStart = 0;
bool          spikeActive    = false;

// Button
int           btnLast    = HIGH;
unsigned long btnDebounce = 0;

// LED/buzzer state
bool          greenState = false, redState = false;
unsigned long lastGreenBlink = 0, lastRedBlink = 0;

// History
float fusedHist[HISTORY_SIZE];
float freqHist[HISTORY_SIZE];
float hrHist[HISTORY_SIZE];
float spo2Hist[HISTORY_SIZE];
int   histIdx = 0, histCount = 0;
unsigned long lastHistMs = 0;

// OLED phases
enum OledPhase { PH_TITLE, PH_STUDENTS, PH_GUIDE, PH_LIVE };
OledPhase     oledPhase    = PH_TITLE;
unsigned long phaseStart   = 0;
bool titleDrawn=false, studentsDrawn=false, guideDrawn=false;
unsigned long lastOledMs   = 0;

// WiFi
bool          wifiWas        = false;
unsigned long lastWifiCheckMs = 0;

unsigned long bootMs = 0;

// ================================================================
//  FAKE SHAKE VARIABLES
// ================================================================
unsigned long lastFakeShakeMs = 0;
bool          fakeShakeActive = false;
unsigned long fakeShakeStartMs = 0;
bool          fakeShakeIncremented = false;

// ================================================================
//  PROTOTYPES
// ================================================================
void connectWiFi();
void checkWiFiStatus(unsigned long now);
void scanI2C();
bool initMPU();
void checkMpuRecovery(unsigned long now);
bool loadCal();
void saveCal();
void doCalibration(bool ui);
void checkGyroHealth(int16_t gxr, int16_t gyr, int16_t gzr);
float simulatedGyroMag(float accelDev);
float computeSpikeOffset(unsigned long now);
float runFFT();
void  checkShake(float accelDev, unsigned long now);
void  sampleTremor(unsigned long now);
void  samplePulseOx(unsigned long now);
void  checkButton(unsigned long now);
void  classifyTremor(unsigned long now);
void  driveOutputs(unsigned long now);
void  updateHistory(unsigned long now);
void  updateOLED(unsigned long now);
void  drawFrame();
void  printC(const String &s, int y, uint8_t sz = 1);
void  showBar(const String &lbl);
void  bootAnim();
void  selfTest();
void  hwStatus();
void  drawTitle();
void  drawStudents();
void  drawGuide();
void  drawLive();
String modeStr();
String arrayJson(float* arr);
String urlEnc(const String &s);
void  sendTg(const String &msg);
void  handleRoot();
void  handleData();
void  updateFakeMetrics(unsigned long now);

// ================================================================
//  BUZZER HELPERS
// ================================================================
inline void buzOn()  { digitalWrite(BUZZER_PIN, BUZZER_ACTIVE_HIGH ? HIGH : LOW); }
inline void buzOff() { digitalWrite(BUZZER_PIN, BUZZER_ACTIVE_HIGH ? LOW  : HIGH); }

// ================================================================
//  OLED HELPERS
// ================================================================
void printC(const String &s, int y, uint8_t sz) {
  int16_t x1,y1; uint16_t w,h;
  display.setTextSize(sz);
  display.getTextBounds(s, 0, y, &x1, &y1, &w, &h);
  int x = ((int)SCREEN_WIDTH - (int)w) / 2;
  if (x < 0) x = 0;
  display.setCursor(x, y);
  display.println(s);
}

void drawFrame() {
  display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
  display.drawRect(2, 2, SCREEN_WIDTH-4, SCREEN_HEIGHT-4, SSD1306_WHITE);
}

void showBar(const String &lbl) {
  for (int i = 0; i <= 100; i += 4) {
    display.clearDisplay(); drawFrame();
    printC("TREMOR GLOVE V4.0", 6);
    printC(lbl, 22);
    display.drawRect(10, 40, 108, 12, SSD1306_WHITE);
    display.fillRect(12, 42, map(i,0,100,0,104), 8, SSD1306_WHITE);
    printC(String(i)+"%", 56);
    display.display();
    delay(20);
  }
}

void bootAnim() { showBar("Initializing..."); }

void selfTest() {
  display.clearDisplay(); drawFrame();
  printC("SYSTEM SELF TEST", 6);
  printC("Buzzer + LEDs", 24);
  printC("Listen for 3 beeps", 40);
  display.display(); delay(800);
  for (int i = 0; i < 3; i++) {
    buzOn();
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED,   HIGH);
    delay(180);
    buzOff();
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED,   LOW);
    delay(180);
  }
  display.clearDisplay(); drawFrame();
  printC("SELF TEST DONE", 28);
  display.display(); delay(700);
}

void hwStatus() {
  display.clearDisplay(); drawFrame();
  printC("HARDWARE CHECK", 6);
  printC("------------------", 16);
  printC("MPU6050 (Tremor):", 26);
  printC(mpuOK ? "DETECTED - OK" : "NOT FOUND!", 36);
  printC("MAX30102 (HR/SpO2):", 46);
  printC(maxOK ? "DETECTED - OK" : "NOT FOUND!", 56);
  display.display(); delay(2000);
}

// ================================================================
//  I2C SCANNER
// ================================================================
void scanI2C() {
  Serial.println("=== I2C SCAN ===");
  int found = 0;
  for (byte a = 1; a < 127; a++) {
    Wire.beginTransmission(a);
    if (Wire.endTransmission() == 0) {
      Serial.printf("  0x%02X\n", a);
      found++;
    }
  }
  if (!found) Serial.println("  None found! Check wiring.");
  else Serial.println("  Expected: 0x3C 0x68/0x69 0x57");
}

// ================================================================
//  PERSISTENT CALIBRATION
// ================================================================
bool loadCal() {
  if (!prefs.begin("trcal", true)) return false;
  bool ok = prefs.getBool("done", false);
  if (ok) {
    accelBaseMag    = prefs.getFloat("aBase",  1.0f);
    accelNoiseFloor = prefs.getFloat("aNoise", MIN_ACCEL_NOISE);
    gyroBiasX       = prefs.getFloat("gbX",    0.0f);
    gyroBiasY       = prefs.getFloat("gbY",    0.0f);
    gyroBiasZ       = prefs.getFloat("gbZ",    0.0f);
    gyroNoiseFloor  = prefs.getFloat("gNoise", MIN_GYRO_NOISE);
  }
  prefs.end();
  return ok;
}

void saveCal() {
  prefs.begin("trcal", false);
  prefs.putFloat("aBase",  accelBaseMag);
  prefs.putFloat("aNoise", accelNoiseFloor);
  prefs.putFloat("gbX",    gyroBiasX);
  prefs.putFloat("gbY",    gyroBiasY);
  prefs.putFloat("gbZ",    gyroBiasZ);
  prefs.putFloat("gNoise", gyroNoiseFloor);
  prefs.putBool("done",    true);
  prefs.end();
}

// ================================================================
//  MPU6050 INIT + AUTO-RECOVERY
// ================================================================
bool initMPU() {
  mpu.initialize();
  if (mpu.testConnection()) return true;
  Wire.setClock(100000); delay(60);
  mpu.initialize();
  bool ok = mpu.testConnection();
  Wire.setClock(400000);
  return ok;
}

void checkMpuRecovery(unsigned long now) {
  if (mpuOK) return;
  if (now - lastMpuRetryMs < MPU_RETRY_MS) return;
  lastMpuRetryMs = now;
  if (!initMPU()) return;
  mpuOK = true;
  if (loadCal()) { calFromFlash = true; }
  else           { doCalibration(false); }
  for (int i=0;i<FFT_N;i++) { accelBuf[i]=0; gyroBuf[i]=0; }
  accelIntensity = gyroIntensity = fusedIndex = tremorFreq = 0;
  gyroChecked = false; gyroCheckCnt = 0;
  lastSpikeStart = millis(); spikeActive = false;
  String m = "MPU6050 Reconnected\nCal: ";
  m += (calFromFlash ? "Flash (saved)" : "Fresh");
  sendTg(m);
}

// ================================================================
//  GYRO HEALTH CHECK
// ================================================================
void checkGyroHealth(int16_t gxr, int16_t gyr, int16_t gzr) {
  if (gyroChecked) return;

  float gxd = gxr / GYRO_SENS_DPS;
  float gyd = gyr / GYRO_SENS_DPS;
  float gzd = gzr / GYRO_SENS_DPS;

  if (gyroCheckCnt == 0) {
    gcMinX = gcMaxX = gxd;
    gcMinY = gcMaxY = gyd;
    gcMinZ = gcMaxZ = gzd;
  } else {
    if (gxd < gcMinX) gcMinX=gxd; if (gxd > gcMaxX) gcMaxX=gxd;
    if (gyd < gcMinY) gcMinY=gyd; if (gyd > gcMaxY) gcMaxY=gyd;
    if (gzd < gcMinZ) gcMinZ=gzd; if (gzd > gcMaxZ) gcMaxZ=gzd;
  }
  gyroCheckCnt++;

  if (gyroCheckCnt >= GYRO_CHECK_SAMPLES) {
    gyroChecked = true;
    float rX = gcMaxX - gcMinX;
    float rY = gcMaxY - gcMinY;
    float rZ = gcMaxZ - gcMinZ;

    if (rX < GYRO_STUCK_THRESHOLD &&
        rY < GYRO_STUCK_THRESHOLD &&
        rZ < GYRO_STUCK_THRESHOLD) {
      gyroMode = GYRO_SIM;
      Serial.println("GYRO: Stuck! Switching to SIMULATED mode.");
      sendTg("Gyro sensor stuck/failed.\nSwitching to SIMULATED gyro.\nAll metrics remain operational.");
    } else {
      gyroMode = GYRO_REAL;
      Serial.println("GYRO: OK — REAL mode.");
    }
  }
}

// ================================================================
//  SIMULATED GYRO
// ================================================================
float simulatedGyroMag(float accelDev) {
  float base = fabsf(accelDev) * GYRO_SIM_SCALE;
  static uint32_t rng = 0xDEADBEEF;
  rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5;
  float noise = ((float)(int32_t)(rng & 0xFFFF) / 32768.0f) * GYRO_SIM_NOISE_DPS;
  float val = base + noise;
  if (val < 0) val = 0;
  return val;
}

// ================================================================
//  FULL CALIBRATION
// ================================================================
void doCalibration(bool ui) {
  if (!mpuOK) return;
  calibrating = true;

  if (ui) {
    display.clearDisplay(); drawFrame();
    printC("CALIBRATING", 8);
    printC("Keep hand still", 26);
    printC("~1.5 seconds...", 40);
    display.display(); delay(400);
  }

  float aSamples[CAL_SAMPLES];
  float gxS[CAL_SAMPLES], gyS[CAL_SAMPLES], gzS[CAL_SAMPLES];
  float aSum=0, gxSum=0, gySum=0, gzSum=0;

  for (int i = 0; i < CAL_SAMPLES; i++) {
    int16_t ax,ay,az,gx,gy,gz;
    mpu.getMotion6(&ax,&ay,&az,&gx,&gy,&gz);
    float axg=ax/16384.0f, ayg=ay/16384.0f, azg=az/16384.0f;
    float mag = sqrtf(axg*axg + ayg*ayg + azg*azg);
    aSamples[i] = mag; aSum += mag;
    gxS[i] = gx/GYRO_SENS_DPS; gxSum += gxS[i];
    gyS[i] = gy/GYRO_SENS_DPS; gySum += gyS[i];
    gzS[i] = gz/GYRO_SENS_DPS; gzSum += gzS[i];
    if (ui) {
      display.fillRect(14, 52, map(i,0,CAL_SAMPLES-1,0,100), 6, SSD1306_WHITE);
      display.display();
    }
    delay(15);
  }

  accelBaseMag = aSum / CAL_SAMPLES;
  gyroBiasX    = gxSum / CAL_SAMPLES;
  gyroBiasY    = gySum / CAL_SAMPLES;
  gyroBiasZ    = gzSum / CAL_SAMPLES;

  float aVar=0, gRMS=0;
  for (int i = 0; i < CAL_SAMPLES; i++) {
    float d = aSamples[i] - accelBaseMag;
    aVar += d*d;
    float dx=gxS[i]-gyroBiasX, dy=gyS[i]-gyroBiasY, dz=gzS[i]-gyroBiasZ;
    gRMS += dx*dx + dy*dy + dz*dz;
  }
  accelNoiseFloor = sqrtf(aVar  / CAL_SAMPLES);
  gyroNoiseFloor  = sqrtf(gRMS  / CAL_SAMPLES);
  if (accelNoiseFloor < MIN_ACCEL_NOISE) accelNoiseFloor = MIN_ACCEL_NOISE;
  if (gyroNoiseFloor  < MIN_GYRO_NOISE)  gyroNoiseFloor  = MIN_GYRO_NOISE;

  saveCal();
  calFromFlash = false;
  for (int i=0;i<FFT_N;i++) { accelBuf[i]=0; gyroBuf[i]=0; }
  accelIntensity=gyroIntensity=fusedIndex=tremorFreq=0;
  gyroChecked=false; gyroCheckCnt=0;
  lastSpikeStart=millis(); spikeActive=false;

  if (ui) {
    display.clearDisplay(); drawFrame();
    printC("CALIBRATION DONE", 10);
    printC("Base:"+String(accelBaseMag,3)+"g", 26);
    printC("GyroNF:"+String(gyroNoiseFloor,2)+"dps", 40);
    display.display(); delay(1800);
  }

  calibrating = false;

  String m = "Sensor Calibrated\n";
  m += "Accel base: "   + String(accelBaseMag,4)    + " g\n";
  m += "Accel NF: "     + String(accelNoiseFloor,4) + " g\n";
  m += "Gyro NF: "      + String(gyroNoiseFloor,2)  + " dps\n";
  m += "Gyro mode: "    + String(gyroMode==GYRO_REAL ? "REAL" : "SIM") + "\n";
  m += "Saved to flash.";
  sendTg(m);
}

// ================================================================
//  SETUP
// ================================================================
void setup() {
  Serial.begin(115200);
  Serial.println("\n=== TREMOR GLOVE V4.0 BOOT ===");

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(GREEN_LED,  OUTPUT);
  pinMode(RED_LED,    OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  buzOff();
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED,   LOW);

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR))
    Serial.println("OLED FAILED");
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  scanI2C();
  bootAnim();
  selfTest();

  mpuOK = initMPU();
  Serial.println(mpuOK ? "MPU6050 OK" : "MPU6050 FAIL");

  maxOK = particleSensor.begin(Wire, I2C_SPEED_FAST);
  if (maxOK) {
    particleSensor.setup(0x1F, 4, 2, 100, 411, 4096);
    particleSensor.setPulseAmplitudeRed(0x0A);
    particleSensor.setPulseAmplitudeGreen(0);
    Serial.println("MAX30102 OK");
  } else {
    Serial.println("MAX30102 FAIL");
  }

  hwStatus();

  for (int i=0;i<FFT_N;i++) { accelBuf[i]=0; gyroBuf[i]=0; vReal[i]=0; vImag[i]=0; }
  for (int i=0;i<RATE_BUF;i++) rates[i]=0;
  for (int i=0;i<HISTORY_SIZE;i++) {
    fusedHist[i]=freqHist[i]=hrHist[i]=spo2Hist[i]=0;
  }

  if (mpuOK) {
    if (loadCal()) {
      calFromFlash = true;
      Serial.println("Cal loaded from flash.");
    } else {
      Serial.println("First boot — calibrating.");
      doCalibration(false);
    }
  } else {
    // If MPU fails, generate fake calibration values
    accelBaseMag = 1.0f;
    accelNoiseFloor = MIN_ACCEL_NOISE;
    gyroNoiseFloor = MIN_GYRO_NOISE;
    gyroBiasX = gyroBiasY = gyroBiasZ = 0;
    calFromFlash = false;
  }

  connectWiFi();
  server.on("/",     handleRoot);
  server.on("/data", handleData);
  server.begin();

  bootMs         = millis();
  lastMpuRetryMs = millis();
  lastFFTms      = millis();
  lastSpikeStart = millis();
  spikeActive    = false;
  oledPhase      = PH_TITLE;
  titleDrawn     = false;

  // Fake shake init
  lastFakeShakeMs = millis();
  fakeShakeActive = false;

  String bm = "TREMOR GLOVE V4.0 Online\n";
  bm += "Cal: "   + String(calFromFlash?"Flash":"Fresh") + "\n";
  bm += "Gyro: "  + String(gyroMode==GYRO_REAL?"REAL":"SIM") + "\n";
  bm += "Demo: "  + String(DEMO_MODE_ENABLED?"ON (spikes ~40s tagged [DEMO])":"OFF");
  sendTg(bm);

  if (WiFi.status()==WL_CONNECTED) {
    wifiWas = true;
    sendTg("WiFi Connected\nIP: "+WiFi.localIP().toString());
  }
  if (!mpuOK || !maxOK) {
    String em = "Sensor Failure\n";
    if (!mpuOK) em += "MPU6050 not found\n";
    if (!maxOK) em += "MAX30102 not found\n";
    sendTg(em);
  }
}

// ================================================================
//  LOOP
// ================================================================
void loop() {
  server.handleClient();
  unsigned long now = millis();

  // Check MPU recovery
  checkMpuRecovery(now);

  // If MPU is not OK, generate fake tremor metrics
  if (!mpuOK) {
    updateFakeMetrics(now);
  } else {
    // Normal sampling
    sampleTremor(now);
  }

  // Heart rate / SpO2
  samplePulseOx(now);

  // Button check
  checkButton(now);
  if (calRequested) { calRequested=false; doCalibration(true); }

  // Classify tremor (handles fake values too)
  classifyTremor(now);

  // Drive outputs (LED, buzzer) - includes fake shake and finger detection
  driveOutputs(now);

  // Update history
  updateHistory(now);

  // OLED
  updateOLED(now);

  // WiFi check
  checkWiFiStatus(now);

  // Fake shake timer (every 45s, lasts 8s) - independent of MPU
  if (!fakeShakeActive && (now - lastFakeShakeMs >= FAKE_SHAKE_INTERVAL_MS)) {
    // Start fake shake
    fakeShakeActive = true;
    fakeShakeStartMs = now;
    fakeShakeIncremented = false; // will increment when we activate output
    lastFakeShakeMs = now; // reset interval
    // Set shake OLED end to now + duration
    shakeOledEnd = now + FAKE_SHAKE_DURATION_MS;
    // Increment shake count (will be done in driveOutputs when active)
    // But we want to increment only once per event, so we'll do it in driveOutputs when first activated
  }

  if (fakeShakeActive && (now - fakeShakeStartMs >= FAKE_SHAKE_DURATION_MS)) {
    fakeShakeActive = false;
    // Ensure we turn off shake indicators
    shakeOledEnd = 0;
    shakeFlashActive = false;
    // Turn off buzzer/LED if not already handled by driveOutputs
  }
}

// ================================================================
//  FAKE METRICS (when MPU is not working)
// ================================================================
void updateFakeMetrics(unsigned long now) {
  // Generate fake tremor values using sine waves
  float t = (float)now / 1000.0f;
  // Fused index between 1.0 and 6.0
  fusedIndex = 3.0f + 2.5f * sin(t * 0.3f) + 0.5f * sin(t * 0.7f + 1.2f);
  if (fusedIndex < 0.5f) fusedIndex = 0.5f;
  if (fusedIndex > 7.0f) fusedIndex = 7.0f;
  // Frequency between 2.0 and 12.0 Hz
  tremorFreq = 5.0f + 4.0f * sin(t * 0.2f + 0.8f) + 1.0f * sin(t * 0.5f + 2.3f);
  if (tremorFreq < 1.0f) tremorFreq = 1.0f;
  if (tremorFreq > 18.0f) tremorFreq = 18.0f;
  // Accel intensity
  accelIntensity = 0.002f + 0.005f * (1.0f + sin(t * 0.4f));
  // Gyro intensity
  gyroIntensity = 0.3f + 1.2f * (1.0f + cos(t * 0.25f));
  // For classification, we'll set tremorStatus and alertLevel in classifyTremor
}

// ================================================================
//  WIFI
// ================================================================
void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long t0=millis(); int dot=0;
  while (WiFi.status()!=WL_CONNECTED && millis()-t0<15000) {
    display.clearDisplay(); drawFrame();
    printC("CONNECTING WiFi",10);
    printC(WIFI_SSID,28);
    String d=""; for(int i=0;i<=(dot%4);i++) d+=".";
    printC(d,46); display.display(); dot++; delay(300);
  }
  display.clearDisplay(); drawFrame();
  if (WiFi.status()==WL_CONNECTED) {
    printC("WiFi CONNECTED!",18);
    printC(WiFi.localIP().toString(),36);
  } else {
    printC("WiFi OFFLINE",18);
    printC("Dashboard unavail.",36);
  }
  display.display(); delay(1200);
}

void checkWiFiStatus(unsigned long now) {
  if (now-lastWifiCheckMs < 3000) return;
  lastWifiCheckMs = now;
  bool c = (WiFi.status()==WL_CONNECTED);
  if (wifiWas && !c) { wifiWas=false; sendTg("WiFi Disconnected"); }
  if (!wifiWas && c) { wifiWas=true;  sendTg("WiFi Reconnected\nIP: "+WiFi.localIP().toString()); }
  if (!c) WiFi.reconnect();
}

// ================================================================
//  DEMO SPIKE
// ================================================================
float computeSpikeOffset(unsigned long now) {
  if (!DEMO_MODE_ENABLED || !mpuOK || calibrating) return 0.0f;
  if (!spikeActive && (now-lastSpikeStart >= SPIKE_INTERVAL_MS)) {
    spikeActive=true; lastSpikeStart=now;
  }
  if (!spikeActive) return 0.0f;
  unsigned long e = now - lastSpikeStart;
  float shape = 0.0f;
  if      (e < SPIKE_RAMP_MS)                                    shape = (float)e / SPIKE_RAMP_MS;
  else if (e < SPIKE_RAMP_MS+SPIKE_HOLD_MS)                      shape = 1.0f;
  else if (e < SPIKE_RAMP_MS+SPIKE_HOLD_MS+SPIKE_FALL_MS)        shape = 1.0f-(float)(e-SPIKE_RAMP_MS-SPIKE_HOLD_MS)/SPIKE_FALL_MS;
  else { spikeActive=false; return 0.0f; }
  float peakOffset = (SPIKE_TARGET_INDEX / W_ACCEL) * accelNoiseFloor;
  return peakOffset * shape;
}

// ================================================================
//  FFT  — FIX 1: use member functions, not static-style calls
// ================================================================
float runFFT() {
  // Copy accel buffer into vReal (already declared globally)
  for (int i=0;i<FFT_N;i++) { vReal[i]=(double)accelBuf[i]; vImag[i]=0.0; }

  FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);
  FFT.compute(FFTDirection::Forward);
  FFT.complexToMagnitude();

  double res   = FS / FFT_N;
  int minBin   = (int)fmax(1.0, FREQ_MIN_HZ/res + 0.5);
  int maxBin   = (int)fmin((double)(FFT_N/2-1), FREQ_MAX_HZ/res + 0.5);
  int    pk    = minBin;
  double pkM   = vReal[minBin];
  for (int i=minBin+1;i<=maxBin;i++) if(vReal[i]>pkM){pkM=vReal[i];pk=i;}
  return (float)(pk * res);
}

// ================================================================
//  SHAKE DETECTION (real shake from accelerometer)
// ================================================================
void checkShake(float accelDev, unsigned long now) {
  if (fabsf(accelDev) < SHAKE_THRESHOLD_G) return;
  if (now-lastShakeMs < SHAKE_DEBOUNCE_MS) return;
  lastShakeMs = now;
  shakeCount++;
  shakeFlashActive = true;
  shakeFlashEnd    = now + SHAKE_FLASH_MS;
  shakeOledEnd     = now + SHAKE_OLED_MS;
  buzOn();
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED,   HIGH);
  Serial.printf("SHAKE! count=%u dev=%.3fg\n", shakeCount, accelDev);
  if (now-lastShakeTgMs > SHAKE_TELEGRAM_COOLDOWN) {
    lastShakeTgMs = now;
    String m = "SHAKE DETECTED!\nCount: "+String(shakeCount)+"\n";
    m += "Deviation: "+String(accelDev,3)+" g\n";
    m += "Status: "+tremorStatus;
    sendTg(m);
  }
}

// ================================================================
//  TREMOR SAMPLING + SENSOR FUSION
// ================================================================
void sampleTremor(unsigned long now) {
  if (now-lastSampleMs < TREMOR_SAMPLE_MS) return;
  lastSampleMs = now;
  if (!mpuOK || calibrating) return;

  int16_t ax,ay,az,gx,gy,gz;
  mpu.getMotion6(&ax,&ay,&az,&gx,&gy,&gz);

  float axg=ax/16384.0f, ayg=ay/16384.0f, azg=az/16384.0f;
  float aMag = sqrtf(axg*axg+ayg*ayg+azg*azg);
  float aDev = aMag - accelBaseMag;

  checkShake(aDev, now);
  accelBuf[bufIdx] = aDev;

  checkGyroHealth(gx, gy, gz);

  float gMag = 0.0f;
  if (gyroMode == GYRO_REAL) {
    float gxd=gx/GYRO_SENS_DPS-gyroBiasX;
    float gyd=gy/GYRO_SENS_DPS-gyroBiasY;
    float gzd=gz/GYRO_SENS_DPS-gyroBiasZ;
    gMag = sqrtf(gxd*gxd+gyd*gyd+gzd*gzd);
  } else {
    gMag = simulatedGyroMag(aDev);
  }
  gyroBuf[bufIdx] = gMag;
  bufIdx = (bufIdx+1) % FFT_N;

  // Accel intensity (stddev)
  float mean=0;
  for(int i=0;i<FFT_N;i++) mean+=accelBuf[i];
  mean/=FFT_N;
  float varSum=0;
  for(int i=0;i<FFT_N;i++){float d=accelBuf[i]-mean;varSum+=d*d;}
  accelIntensity = sqrtf(varSum/FFT_N);

  // Gyro intensity (RMS)
  float gSq=0;
  for(int i=0;i<FFT_N;i++) gSq+=gyroBuf[i]*gyroBuf[i];
  gyroIntensity = sqrtf(gSq/FFT_N);

  // Demo spike
  float spike = computeSpikeOffset(now);
  accelIntensity += spike;
  if (accelIntensity < 0) accelIntensity = 0;

  // FFT frequency
  if (now-lastFFTms >= FFT_UPDATE_MS) {
    lastFFTms = now;
    tremorFreq = runFFT();
  }

  // Sensor fusion
  float aZ = accelIntensity / fmaxf(accelNoiseFloor, MIN_ACCEL_NOISE);
  float gZ = gyroIntensity  / fmaxf(gyroNoiseFloor,  MIN_GYRO_NOISE);
  fusedIndex = W_ACCEL*aZ + W_GYRO*gZ;
}

// ================================================================
//  HEART RATE / SpO2
// ================================================================
void samplePulseOx(unsigned long now) {
  if (!maxOK) return;
  long irV  = particleSensor.getIR();
  long redV = particleSensor.getRed();
  if (irV > FINGER_IR_MIN) {
    fingerOn = true;
    if (checkForBeat(irV)) {
      unsigned long d = now - lastBeatMs;
      lastBeatMs = now;
      float bpm = 60000.0f / (float)d;
      if (bpm>20 && bpm<255) {
        rates[rateIdx++]=(long)bpm; rateIdx%=RATE_BUF;
        long tot=0; for(int i=0;i<RATE_BUF;i++) tot+=rates[i];
        heartRate=tot/(float)RATE_BUF;
      }
    }
    if(redV<rMin)rMin=redV; if(redV>rMax)rMax=redV;
    if(irV <iMin)iMin=irV;  if(irV >iMax)iMax=irV;
    if (now-lastSpo2Ms > 1000) {
      float rAC=(float)(rMax-rMin),iAC=(float)(iMax-iMin);
      float rDC=(rMax+rMin)/2.0f,  iDC=(iMax+iMin)/2.0f;
      if(iAC>50&&rDC>0&&iDC>0){
        float R=(rAC/rDC)/(iAC/iDC);
        spo2Val=constrain(-45.06f*R*R+30.354f*R+94.845f,90.0f,100.0f);
      }
      rMin=iMin=999999; rMax=iMax=0;
      lastSpo2Ms=now;
    }
  } else {
    fingerOn=false; heartRate=0; spo2Val=0;
    for(int i=0;i<RATE_BUF;i++) rates[i]=0;
    rateIdx=0; lastBeatMs=0;
    rMin=iMin=999999; rMax=iMax=0;
  }
}

// ================================================================
//  BUTTON
// ================================================================
void checkButton(unsigned long now) {
  int s = digitalRead(BUTTON_PIN);
  if (s==LOW && btnLast==HIGH && (now-btnDebounce)>300) {
    btnDebounce=now; calRequested=true;
  }
  btnLast=s;
}

// ================================================================
//  TREMOR CLASSIFICATION
// ================================================================
void classifyTremor(unsigned long now) {
  // If MPU is not OK, set to "MONITORING" or "SIMULATED" based on fake values
  if (!mpuOK) {
    tremorStatus = "MONITORING";
    // Use fake fusedIndex to set alert level
    if (fusedIndex < THRESH_MILD)      alertLevel = "NORMAL";
    else if (fusedIndex < THRESH_MODERATE) alertLevel = "WARNING";
    else if (fusedIndex < THRESH_SEVERE)   alertLevel = "ELEVATED";
    else alertLevel = "CRITICAL";
    prevStatus = tremorStatus;
    return;
  }

  if (calibrating) {
    tremorStatus="STABLE HAND"; alertLevel="NORMAL";
    prevStatus=tremorStatus; return;
  }

  if      (fusedIndex < THRESH_MILD)     { tremorStatus="STABLE HAND";    alertLevel="NORMAL";   }
  else if (fusedIndex < THRESH_MODERATE) { tremorStatus="MILD TREMOR";    alertLevel="WARNING";  }
  else if (fusedIndex < THRESH_SEVERE)   { tremorStatus="MODERATE TREMOR";alertLevel="ELEVATED"; }
  else                                   { tremorStatus="SEVERE TREMOR";  alertLevel="CRITICAL"; }

  if (tremorStatus=="SEVERE TREMOR") {
    bool fresh = (prevStatus!="SEVERE TREMOR");
    bool timed = (now-lastSevereAlertMs>60000);
    if (fresh||timed) {
      String m="";
      if(DEMO_MODE_ENABLED&&spikeActive) m+="[DEMO] ";
      m+="SEVERE TREMOR ALERT\n";
      m+="Fused Idx: "+String(fusedIndex,2)+"x\n";
      m+="Accel: "+String(accelIntensity,4)+" g\n";
      m+="Gyro: "+String(gyroIntensity,2)+" dps (";
      m+=String(gyroMode==GYRO_REAL ? "REAL" : "SIM");
      m+=")\n";
      m+="Freq: "+String(tremorFreq,2)+" Hz\n";
      if(fingerOn&&heartRate>0){
        m+="HR: "+String((int)heartRate)+" BPM\n";
        m+="SpO2: "+String((int)spo2Val)+" %\n";
      } else { m+="HR/SpO2: no finger\n"; }
      m+="Shakes: "+String(shakeCount)+"\n";
      m+="Uptime: "+String((now-bootMs)/1000)+"s";
      sendTg(m);
      lastSevereAlertMs=now;
    }
  }
  prevStatus=tremorStatus;
}

// ================================================================
//  LED + BUZZER
// ================================================================
void driveOutputs(unsigned long now) {
  // 1. Fake shake overrides everything
  if (fakeShakeActive) {
    // If just started, increment shake count and send alert
    if (!fakeShakeIncremented) {
      fakeShakeIncremented = true;
      shakeCount++;
      // Trigger Telegram alert
      if (now - lastShakeTgMs > SHAKE_TELEGRAM_COOLDOWN) {
        lastShakeTgMs = now;
        String m = "FAKE SHAKE EVENT (45s interval)\nCount: "+String(shakeCount)+"\n";
        m += "Duration: 8 seconds\n";
        m += "Status: "+tremorStatus;
        sendTg(m);
      }
      // Flash and OLED end
      shakeFlashActive = true;
      shakeFlashEnd = now + SHAKE_FLASH_MS; // 180ms flash, but we keep buzzer/LED on for 8s
      shakeOledEnd = now + FAKE_SHAKE_DURATION_MS;
    }
    // Force buzzer and red LED on, green off
    buzOn();
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    // Keep shakeFlashActive true for the flash effect (if we want, but we can just keep on)
    // We'll just set red LED on solid.
    // Also, we can optionally blink quickly? But user said "buzzer and red light should be glow", so solid.
    return;
  }

  // 2. If no finger detected, red LED on, green off, buzzer off
  if (!fingerOn) {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    buzOff();
    return;
  }

  // 3. Handle shake flash from real shake (but fake shake already handled)
  if (shakeFlashActive) {
    if (now < shakeFlashEnd) {
      buzOn();
      digitalWrite(GREEN_LED,HIGH);
      digitalWrite(RED_LED,  HIGH);
      return;
    } else {
      shakeFlashActive=false;
      buzOff();
      digitalWrite(GREEN_LED,LOW);
      digitalWrite(RED_LED,  LOW);
    }
  }

  if (calibrating) {
    buzOff();
    digitalWrite(GREEN_LED,LOW);
    digitalWrite(RED_LED,  LOW);
    return;
  }

  // 4. Normal tremor-based LED/buzzer
  if (tremorStatus=="STABLE HAND") {
    digitalWrite(GREEN_LED,HIGH);
    digitalWrite(RED_LED,  LOW);
    buzOff();
  }
  else if (tremorStatus=="MILD TREMOR") {
    digitalWrite(RED_LED,LOW);
    if(now-lastGreenBlink>=BLINK_MILD){
      greenState=!greenState;
      digitalWrite(GREEN_LED,greenState);
      lastGreenBlink=now;
    }
    if((now%BUZZ_MILD_PERIOD)<BUZZ_ON_MS) buzOn(); else buzOff();
  }
  else if (tremorStatus=="MODERATE TREMOR") {
    digitalWrite(GREEN_LED,LOW);
    if(now-lastRedBlink>=BLINK_MODERATE){
      redState=!redState;
      digitalWrite(RED_LED,redState);
      lastRedBlink=now;
    }
    if((now%BUZZ_MODERATE_PERIOD)<BUZZ_ON_MS) buzOn(); else buzOff();
  }
  else if (tremorStatus=="SEVERE TREMOR") {
    digitalWrite(GREEN_LED,LOW);
    if(now-lastRedBlink>=BLINK_SEVERE){
      redState=!redState;
      digitalWrite(RED_LED,redState);
      lastRedBlink=now;
    }
    buzOn();
  }
  else {
    // For MONITORING (MPU fail) - red LED on? But we already handled no finger.
    // If tremorStatus is "MONITORING" and finger is on, we can show solid green or something.
    // Actually, we want to indicate that we are monitoring but no sensor? 
    // We'll set green on, red off, buzzer off.
    digitalWrite(GREEN_LED,HIGH);
    digitalWrite(RED_LED,  LOW);
    buzOff();
  }
}

// ================================================================
//  HISTORY
// ================================================================
void updateHistory(unsigned long now) {
  if(now-lastHistMs < HISTORY_MS) return;
  lastHistMs=now;
  fusedHist[histIdx]=fusedIndex;
  freqHist[histIdx] =tremorFreq;
  hrHist[histIdx]   =heartRate;
  spo2Hist[histIdx] =spo2Val;
  histIdx=(histIdx+1)%HISTORY_SIZE;
  if(histCount<HISTORY_SIZE) histCount++;
}

String arrayJson(float* arr) {
  String s="[";
  int st=(histCount<HISTORY_SIZE)?0:histIdx;
  for(int i=0;i<histCount;i++){
    int idx=(st+i)%HISTORY_SIZE;
    s+=String(arr[idx],3);
    if(i<histCount-1) s+=",";
  }
  s+="]"; return s;
}

// ================================================================
//  OLED
// ================================================================
String modeStr() {
  if(calibrating)                       return "CALIB";
  if(!fingerOn)                         return "NOFNGR";
  if(DEMO_MODE_ENABLED && spikeActive)  return "LIVE-D";
  if(!mpuOK)                            return "SIM";
  return "LIVE";
}

void drawTitle() {
  display.clearDisplay(); drawFrame();
  printC("------------------",  6);
  printC("HAND TREMOR",        16);
  printC("DETECTION &",        26);
  printC("MONITORING GLOVE",   36);
  printC("AI REHAB V4.0",      48);
  printC("------------------", 58);
  display.display();
}

void drawStudents() {
  display.clearDisplay(); drawFrame();
  printC("==== STUDENTS ====",  6);
  printC("Yuvasri Y",          20);
  printC("Reg: 192419014",     30);
  printC("------------------", 38);
  printC("Shirin Nazini",      46);
  printC("Reg: 192419030",     56);
  display.display();
}

void drawGuide() {
  display.clearDisplay(); drawFrame();
  printC("===== GUIDE =====",   8);
  printC("Dr. Ramakrishna",    24);
  printC("Reddy",              34);
  printC("------------------", 44);
  printC("Course: BMA1201",    54);
  display.display();
}

void drawLive() {
  unsigned long now=millis();
  display.clearDisplay(); drawFrame();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  printC("TREMOR MONITOR V4", 4);

  // Check if shake is active (real or fake)
  bool shakeActive = (now < shakeOledEnd) || fakeShakeActive;
  if (shakeActive) {
    printC("!! SHAKE !!", 14);
    printC("Count:"+String(shakeCount), 24);
  } else {
    if (tremorStatus=="SEVERE TREMOR") printC(">>> SEVERE <<<", 14);
    else printC(tremorStatus, 14);
    printC("Idx:"+String(fusedIndex,1)+"x "+String(tremorFreq,1)+"Hz", 24);
  }

  // Gyro mode
  printC(String(gyroMode==GYRO_REAL ? "GYRO:REAL" : "GYRO:SIM"), 34);

  String hrS  = fingerOn ? String((int)heartRate)+"BPM" : "--BPM";
  String sp2  = fingerOn ? String((int)spo2Val)+"%"     : "--%";
  printC("HR:"+hrS+" SpO2:"+sp2, 44);

  String wS=(WiFi.status()==WL_CONNECTED)?"W:OK":"W:NO";
  // Remove age from display
  printC("Shk:"+String(shakeCount)+" "+wS+" "+modeStr(), 54);
  display.display();
}

void updateOLED(unsigned long now) {
  if(calibrating) return;
  switch(oledPhase){
    case PH_TITLE:
      if(!titleDrawn){drawTitle();titleDrawn=true;phaseStart=now;lastOledMs=now;}
      if(now-phaseStart>=OLED_TITLE_MS){oledPhase=PH_STUDENTS;studentsDrawn=false;}
      break;
    case PH_STUDENTS:
      if(!studentsDrawn){drawStudents();studentsDrawn=true;phaseStart=now;lastOledMs=now;}
      if(now-phaseStart>=OLED_STUDENTS_MS){oledPhase=PH_GUIDE;guideDrawn=false;}
      break;
    case PH_GUIDE:
      if(!guideDrawn){drawGuide();guideDrawn=true;phaseStart=now;lastOledMs=now;}
      if(now-phaseStart>=OLED_GUIDE_MS) oledPhase=PH_LIVE;
      break;
    case PH_LIVE:
      if((millis()<shakeOledEnd) || (now-lastOledMs>=800)){
        lastOledMs=now; drawLive();
      }
      break;
  }
}

// ================================================================
//  TELEGRAM
// ================================================================
String urlEnc(const String &s) {
  String out="";
  for(size_t i=0;i<s.length();i++){
    uint8_t c=(uint8_t)s[i];
    if(isalnum(c)||c=='-'||c=='_'||c=='.'||c=='~') out+=(char)c;
    else{char buf[4];sprintf(buf,"%%%02X",c);out+=buf;}
  }
  return out;
}

void sendTg(const String &msg) {
  if(WiFi.status()!=WL_CONNECTED) return;
  WiFiClientSecure cli; cli.setInsecure();
  HTTPClient h;
  String url="https://api.telegram.org/bot";
  url+=BOT_TOKEN;
  url+="/sendMessage?chat_id=";
  url+=CHAT_ID;
  url+="&text=";
  url+=urlEnc(msg);
  if(h.begin(cli,url)){h.GET();h.end();}
}

// ================================================================
//  WEB SERVER — /data
// ================================================================
void handleData() {
  unsigned long now=millis();
  String j="{";
  j+="\"tremorStatus\":\""   + tremorStatus                              + "\",";
  j+="\"alertLevel\":\""     + alertLevel                                + "\",";
  j+="\"fusedIndex\":"       + String(fusedIndex,3)                      + ",";
  j+="\"accelIntensity\":"   + String(accelIntensity,5)                  + ",";
  j+="\"gyroIntensity\":"    + String(gyroIntensity,3)                   + ",";
  j+="\"gyroMode\":\""       + String(gyroMode==GYRO_REAL ? "REAL" : "SIM") + "\",";
  j+="\"tremorFreq\":"       + String(tremorFreq,2)                      + ",";
  j+="\"shakeCount\":"       + String(shakeCount)                        + ",";
  j+="\"heartRate\":"        + String((int)heartRate)                    + ",";
  j+="\"spo2\":"             + String((int)spo2Val)                      + ",";
  j+="\"fingerOn\":"         + String(fingerOn ? "true" : "false")       + ",";
  // Removed "age" field
  j+="\"mode\":\""           + modeStr()                                 + "\",";
  j+="\"wifiStatus\":\""     + String(WiFi.status()==WL_CONNECTED ? "Connected" : "Disconnected") + "\",";
  j+="\"ip\":\""             + WiFi.localIP().toString()                 + "\",";
  j+="\"rssi\":"             + String(WiFi.RSSI())                       + ",";
  j+="\"uptime\":"           + String((now-bootMs)/1000)                 + ",";
  j+="\"accelBaseMag\":"     + String(accelBaseMag,5)                    + ",";
  j+="\"accelNoiseFloor\":"  + String(accelNoiseFloor,5)                 + ",";
  j+="\"gyroNoiseFloor\":"   + String(gyroNoiseFloor,3)                  + ",";
  j+="\"calSource\":\""      + String(calFromFlash ? "Flash (saved)" : "Fresh (this boot)") + "\",";
  j+="\"fusedHist\":"        + arrayJson(fusedHist)                      + ",";
  j+="\"freqHist\":"         + arrayJson(freqHist)                       + ",";
  j+="\"hrHist\":"           + arrayJson(hrHist)                         + ",";
  j+="\"spo2Hist\":"         + arrayJson(spo2Hist);
  j+="}";
  server.send(200,"application/json",j);
}

// ================================================================
//  WEB SERVER — / (Dashboard)
// ================================================================
void handleRoot() {
  String page = R"RAW(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Tremor Glove V4.0</title>
<script src="https://cdn.jsdelivr.net/npm/chart.js@4.4.0/dist/chart.umd.min.js"></script>
<style>
:root{
  --bg:#04060e;--panel:rgba(8,18,40,0.72);--border:rgba(0,200,255,0.18);
  --neon:#00e5ff;--neon2:#7c4dff;--green:#00ff88;--yellow:#ffe234;
  --orange:#ffb340;--red:#ff3860;--dim:#7fb8d9;--text:#d9f1ff;
}
*{box-sizing:border-box;margin:0;padding:0;font-family:'Segoe UI',Arial,sans-serif}
body{background:radial-gradient(circle at top,#0b1530,var(--bg) 70%);
  color:var(--text);min-height:100vh;padding:14px}
.hdr{background:var(--panel);border:1px solid var(--border);border-radius:14px;
  padding:16px 22px;margin-bottom:14px;backdrop-filter:blur(12px);
  display:flex;flex-wrap:wrap;justify-content:space-between;align-items:center;gap:10px;
  box-shadow:0 0 30px rgba(0,229,255,0.07)}
.hdr h1{font-size:1.3rem;letter-spacing:2px;color:var(--neon);text-shadow:0 0 14px var(--neon)}
.hdr p{color:var(--neon2);font-size:0.72rem;letter-spacing:3px;margin-top:3px}
.meta{display:flex;gap:14px;font-size:0.76rem;flex-wrap:wrap}
.meta span{color:var(--dim)}.meta b{color:var(--neon)}
.emg{display:none;background:repeating-linear-gradient(45deg,#ff3860,#ff3860 20px,#a8001f 20px,#a8001f 40px);
  color:#fff;text-align:center;font-weight:bold;padding:14px;border-radius:10px;
  letter-spacing:3px;margin-bottom:14px;animation:fl 0.9s infinite}
.shk{display:none;background:rgba(255,226,52,0.1);border:1px solid var(--yellow);
  border-radius:10px;padding:10px 20px;margin-bottom:14px;text-align:center;
  color:var(--yellow);font-weight:bold;letter-spacing:2px;animation:fl 0.8s infinite}
@keyframes fl{50%{opacity:.3}}
.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(150px,1fr));gap:12px;margin-bottom:14px}
.card{background:var(--panel);border:1px solid var(--border);border-radius:12px;
  padding:14px;backdrop-filter:blur(10px)}
.lbl{font-size:0.63rem;letter-spacing:2px;color:var(--dim);text-transform:uppercase;margin-bottom:5px}
.val{font-size:1.5rem;font-weight:700;color:var(--neon)}
.sub{font-size:0.68rem;color:var(--dim);margin-top:3px}
.cg{color:var(--green)!important;text-shadow:0 0 8px var(--green)}
.cy{color:var(--yellow)!important;text-shadow:0 0 8px var(--yellow)}
.co{color:var(--orange)!important;text-shadow:0 0 8px var(--orange)}
.cr{color:var(--red)!important;text-shadow:0 0 8px var(--red)}
.panel{background:var(--panel);border:1px solid var(--border);border-radius:12px;
  padding:14px;margin-bottom:14px;backdrop-filter:blur(10px)}
.panel h3{font-size:0.75rem;letter-spacing:2px;color:var(--neon);margin-bottom:10px}
.badge{display:inline-block;padding:6px 18px;border-radius:20px;font-weight:bold;letter-spacing:2px;font-size:0.8rem}
.bn{background:rgba(0,255,136,.12);color:var(--green);border:1px solid var(--green)}
.bw{background:rgba(255,226,52,.12);color:var(--yellow);border:1px solid var(--yellow)}
.be{background:rgba(255,179,64,.12);color:var(--orange);border:1px solid var(--orange)}
.bc{background:rgba(255,56,96,.12);color:var(--red);border:1px solid var(--red)}
.gtag{display:inline-block;padding:2px 8px;border-radius:6px;font-size:0.62rem;
  font-weight:bold;letter-spacing:1px;margin-left:6px;vertical-align:middle}
.gtr{background:rgba(0,255,136,.15);color:var(--green);border:1px solid var(--green)}
.gts{background:rgba(255,226,52,.15);color:var(--yellow);border:1px solid var(--yellow)}
.charts{display:grid;grid-template-columns:repeat(auto-fit,minmax(290px,1fr));gap:12px;margin-bottom:14px}
.cp{background:var(--panel);border:1px solid var(--border);border-radius:12px;
  padding:14px;backdrop-filter:blur(10px)}
.cp h3{font-size:0.7rem;letter-spacing:2px;color:var(--neon);margin-bottom:10px}
canvas{max-height:195px}
.cal{display:flex;flex-wrap:wrap;gap:18px;font-size:0.8rem;margin-top:6px}
.cal span{color:var(--dim)}.cal b{color:var(--neon)}
.ig{display:grid;grid-template-columns:repeat(auto-fit,minmax(220px,1fr));gap:8px;font-size:0.8rem}
.ig b{color:var(--neon)}
</style>
</head>
<body>

<div class="emg" id="emg">&#9888; CRITICAL &mdash; SEVERE TREMOR DETECTED &#9888;</div>
<div class="shk" id="shk">&#127967; SHAKE DETECTED &mdash; Count: <span id="sc2">0</span></div>

<div class="hdr">
  <div>
    <h1>HAND TREMOR MONITORING GLOVE</h1>
    <p>AI REHABILITATION SYSTEM V4.0 &mdash; FFT + FUSION + SHAKE DETECTION</p>
  </div>
  <div class="meta">
    <span id="clk">--:--:--</span>
    <span>WiFi: <b id="wfi">--</b></span>
    <span>Mode: <b id="mod">--</b></span>
    <span>IP: <b id="ip">--</b></span>
    <span>RSSI: <b id="rs">--</b> dBm</span>
  </div>
</div>

<div class="grid">
  <div class="card"><div class="lbl">Tremor Status</div><div class="val" id="tSt">--</div></div>
  <div class="card"><div class="lbl">Fused Tremor Index</div><div class="val" id="fId">--</div><div class="sub">x noise floor</div></div>
  <div class="card"><div class="lbl">Frequency (FFT)</div><div class="val" id="frq">-- Hz</div></div>
  <div class="card"><div class="lbl">Accel Component</div><div class="val" id="aIn">--</div><div class="sub">g stddev</div></div>
  <div class="card">
    <div class="lbl">Gyro Component <span class="gtag gtr" id="gtg">REAL</span></div>
    <div class="val" id="gIn">--</div><div class="sub">deg/s RMS</div>
  </div>
  <div class="card"><div class="lbl">&#127967; Shake Events</div><div class="val" id="shk2">0</div></div>
  <div class="card"><div class="lbl">Heart Rate</div><div class="val" id="hr">-- BPM</div></div>
  <div class="card"><div class="lbl">SpO2</div><div class="val" id="sp">-- %</div></div>
  <div class="card"><div class="lbl">Finger Detected</div><div class="val" id="fg">--</div></div>
  <!-- Patient Age card removed -->
  <div class="card"><div class="lbl">Uptime</div><div class="val" id="upt">--:--:--</div></div>
  <div class="card"><div class="lbl">Alert Level</div><div class="val" id="sA">--</div></div>
</div>

<div class="panel">
  <h3>ALERT STATUS</h3>
  <span class="badge" id="bdg">--</span>
</div>

<div class="panel">
  <h3>CALIBRATION</h3>
  <div class="cal">
    <div><span>Source: </span><b id="cSr">--</b></div>
    <div><span>Accel Base: </span><b id="cAB">--</b></div>
    <div><span>Accel NF: </span><b id="cAN">--</b></div>
    <div><span>Gyro NF: </span><b id="cGN">--</b></div>
  </div>
  <p style="font-size:0.7rem;color:var(--dim);margin-top:8px">
    Hold glove still and press on-board button to recalibrate (~1.5s). Values saved to flash.
  </p>
</div>

<div class="charts">
  <div class="cp"><h3>FUSED TREMOR INDEX</h3><canvas id="c1"></canvas></div>
  <div class="cp"><h3>TREMOR FREQUENCY Hz (FFT)</h3><canvas id="c2"></canvas></div>
  <div class="cp"><h3>HEART RATE BPM</h3><canvas id="c3"></canvas></div>
  <div class="cp"><h3>SpO2 %</h3><canvas id="c4"></canvas></div>
</div>

<div class="panel">
  <h3>PROJECT INFORMATION</h3>
  <div class="ig">
    <div><b>Project:</b> Hand Tremor Detection and Monitoring Glove</div>
    <div><b>Students:</b> Yuvasri Y (192419014), Shirin Nazini (192419030)</div>
    <div><b>Guide:</b> Dr. Ramakrishna Reddy</div>
    <div><b>Course:</b> BMA1201</div>
    <div><b>V4.0:</b> FFT analysis, Sensor fusion, Flash calibration, Shake detection, Gyro SIM fallback</div>
  </div>
</div>

<script>
function mkC(id,lbl,col){
  return new Chart(document.getElementById(id),{
    type:'line',
    data:{labels:[],datasets:[{label:lbl,data:[],borderColor:col,
      backgroundColor:col+'22',tension:0.38,pointRadius:0,fill:true,borderWidth:2}]},
    options:{responsive:true,animation:false,
      scales:{x:{display:false},y:{ticks:{color:'#7fb8d9'},grid:{color:'rgba(255,255,255,0.05)'}}},
      plugins:{legend:{labels:{color:'#9fd9ff',font:{size:11}}}}}
  });
}
const ch1=mkC('c1','Fused Index','#00e5ff');
const ch2=mkC('c2','Freq Hz','#7c4dff');
const ch3=mkC('c3','HR BPM','#ff3860');
const ch4=mkC('c4','SpO2 %','#00ff88');

function upC(ch,arr){
  ch.data.labels=arr.map((_,i)=>i);
  ch.data.datasets[0].data=arr;
  ch.update();
}
function sc(el,s){
  el.classList.remove('cg','cy','co','cr');
  if(s==='STABLE HAND')          el.classList.add('cg');
  else if(s==='MILD TREMOR')     el.classList.add('cy');
  else if(s==='MODERATE TREMOR') el.classList.add('co');
  else                           el.classList.add('cr');
}
function ft(s){
  let h=Math.floor(s/3600),m=Math.floor((s%3600)/60),ss=s%60;
  return String(h).padStart(2,'0')+':'+String(m).padStart(2,'0')+':'+String(ss).padStart(2,'0');
}
let lastShk=0;
function refresh(){
  fetch('/data').then(r=>r.json()).then(d=>{
    let tE=document.getElementById('tSt');
    tE.textContent=d.tremorStatus; sc(tE,d.tremorStatus);
    document.getElementById('fId').textContent =d.fusedIndex.toFixed(2)+'x';
    document.getElementById('frq').textContent =d.tremorFreq.toFixed(2)+' Hz';
    document.getElementById('aIn').textContent =d.accelIntensity.toFixed(4);
    document.getElementById('gIn').textContent =d.gyroIntensity.toFixed(2);
    let gt=document.getElementById('gtg');
    if(d.gyroMode==='REAL'){gt.textContent='REAL';gt.className='gtag gtr';}
    else{gt.textContent='SIM';gt.className='gtag gts';}
    document.getElementById('shk2').textContent=d.shakeCount;
    document.getElementById('sc2').textContent =d.shakeCount;
    let sb=document.getElementById('shk');
    if(d.shakeCount>lastShk){
      sb.style.display='block';
      if(sb._t)clearTimeout(sb._t);
      sb._t=setTimeout(()=>sb.style.display='none',4000);
    }
    lastShk=d.shakeCount;
    document.getElementById('hr').textContent  =d.heartRate+' BPM';
    document.getElementById('sp').textContent  =d.spo2+' %';
    document.getElementById('fg').textContent  =d.fingerOn?'YES':'NO';
    document.getElementById('wfi').textContent =d.wifiStatus;
    document.getElementById('mod').textContent =d.mode;
    document.getElementById('ip').textContent  =d.ip;
    document.getElementById('rs').textContent  =d.rssi;
    document.getElementById('upt').textContent =ft(d.uptime);
    let sA=document.getElementById('sA');
    sA.textContent=d.alertLevel; sc(sA,d.tremorStatus);
    let b=document.getElementById('bdg');
    b.textContent=d.alertLevel; b.className='badge';
    if(d.alertLevel==='NORMAL')    b.classList.add('bn');
    else if(d.alertLevel==='WARNING')   b.classList.add('bw');
    else if(d.alertLevel==='ELEVATED')  b.classList.add('be');
    else b.classList.add('bc');
    document.getElementById('emg').style.display=(d.alertLevel==='CRITICAL')?'block':'none';
    document.getElementById('cSr').textContent=d.calSource;
    document.getElementById('cAB').textContent=d.accelBaseMag.toFixed(5)+' g';
    document.getElementById('cAN').textContent=d.accelNoiseFloor.toFixed(5)+' g';
    document.getElementById('cGN').textContent=d.gyroNoiseFloor.toFixed(3)+' deg/s';
    upC(ch1,d.fusedHist);
    upC(ch2,d.freqHist);
    upC(ch3,d.hrHist);
    upC(ch4,d.spo2Hist);
  }).catch(e=>console.warn(e));
}
setInterval(refresh,2000);
setInterval(()=>{document.getElementById('clk').textContent=new Date().toLocaleTimeString();},1000);
refresh();
</script>
</body>
</html>)RAW";
  server.send(200, "text/html", page);
}
