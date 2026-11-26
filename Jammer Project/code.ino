#include <SPI.h>
#include <RF24.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

// ===== ST7735 pins =====
#define TFT_CS   32
#define TFT_DC   33
#define TFT_RST  16
Adafruit_ST7735 tft = Adafruit_ST7735(&SPI, TFT_CS, TFT_DC, TFT_RST);

// ===== nRF24 pins =====
#define CE_PIN   2
#define CSN_PIN  12
RF24 radio1(CE_PIN, CSN_PIN);

#define NUM_CHANNELS 126
#define SAMPLES_PER_CHANNEL 20
uint8_t channelActivity[NUM_CHANNELS];

int screenW, screenH;

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("=== ST7735 + nRF24 Spectrum Scanner ===");

  // Shared SPI: SCK=14, MISO=15, MOSI=4
  SPI.begin(14, 15, 4);

  // ---- ST7735 init ----
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  screenW = tft.width();
  screenH = tft.height();
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  tft.println("Init nRF24...");

  // ---- nRF24 init + hardware check ----
  if (!radio1.begin(&SPI)) {
    tft.fillScreen(ST77XX_RED);
    tft.setCursor(0, 0);
    tft.setTextColor(ST77XX_BLACK);
    tft.println("Radio begin FAILED");
    Serial.println("FATAL: radio1.begin() failed");
    for (;;);
  }

  if (!radio1.isChipConnected()) {
    tft.fillScreen(ST77XX_RED);
    tft.setCursor(0, 0);
    tft.setTextColor(ST77XX_BLACK);
    tft.println("Chip NOT detected");
    Serial.println("FATAL: nRF24 not detected");
    for (;;);
  }

  radio1.setPALevel(RF24_PA_LOW);
  radio1.setDataRate(RF24_250KBPS);
  radio1.disableCRC();
  radio1.stopListening();

  Serial.println("nRF24 verified OK.");
  radio1.printPrettyDetails();

  tft.fillScreen(ST77XX_GREEN);
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_BLACK);
  tft.println("nRF24 VERIFIED");
  delay(1000);
  tft.fillScreen(ST77XX_BLACK);
}

void loop() {
  memset(channelActivity, 0, sizeof(channelActivity));

  for (int ch = 0; ch < NUM_CHANNELS; ch++) {
    radio1.setChannel(ch);
    radio1.startListening();
    int hits = 0;
    for (int s = 0; s < SAMPLES_PER_CHANNEL; s++) {
      delayMicroseconds(140);
      if (radio1.testCarrier()) hits++;
    }
    radio1.stopListening();
    channelActivity[ch] = hits;
  }

  int maxCh = -1, maxHits = 0, activeCount = 0;
  for (int ch = 0; ch < NUM_CHANNELS; ch++) {
    if (channelActivity[ch] > 0) activeCount++;
    if (channelActivity[ch] > maxHits) { maxHits = channelActivity[ch]; maxCh = ch; }
  }

  // ---- Draw bar graph on ST7735 ----
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(0, 0);
  tft.print("Act:"); tft.print(activeCount);
  tft.print(" Pk:"); tft.println(maxCh >= 0 ? (2400 + maxCh) : 0);

  int barBaseY = screenH - 1;
  for (int x = 0; x < screenW; x++) {
    int ch = map(x, 0, screenW - 1, 0, NUM_CHANNELS - 1);
    int barHeight = map(channelActivity[ch], 0, SAMPLES_PER_CHANNEL, 0, screenH - 12);
    uint16_t color = barHeight > (screenH / 2) ? ST77XX_RED : ST77XX_GREEN;
    tft.drawFastVLine(x, barBaseY - barHeight, barHeight, color);
  }

  Serial.print("Active: "); Serial.print(activeCount);
  Serial.print(" | Peak: "); Serial.print(2400 + maxCh);
  Serial.println(" MHz");

  delay(300);
}
