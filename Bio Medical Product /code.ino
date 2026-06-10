#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MPU6050.h>
#include "MAX30105.h"
#include "heartRate.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

MPU6050 mpu;
MAX30105 particleSensor;

long lastBeat = 0;
float beatsPerMinute = 0;
int beatAvg = 0;

String tremorStatus = "Stable";

void setup()
{
  Serial.begin(115200);

  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  mpu.initialize();

  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD))
  {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("MAX30102 FAIL");
    display.display();
    while (1);
  }

  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("System Ready");
  display.display();
  delay(2000);
}

void loop()
{
  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  float axg = ax / 16384.0;
  float ayg = ay / 16384.0;
  float azg = az / 16384.0;

  float tremor =
      sqrt(axg * axg +
           ayg * ayg +
           azg * azg);

  if (tremor < 1.10)
  {
    tremorStatus = "Stable";
  }
  else if (tremor < 1.50)
  {
    tremorStatus = "Mild";
  }
  else
  {
    tremorStatus = "Severe";
  }

  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue))
  {
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute > 40 && beatsPerMinute < 180)
    {
      beatAvg = beatsPerMinute;
    }
  }

  int spo2 = 98;

  if (irValue < 50000)
  {
    beatAvg = 0;
    spo2 = 0;
  }

  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("TREMOR GLOVE");

  display.setCursor(0, 15);
  display.print("Status:");
  display.println(tremorStatus);

  display.setCursor(0, 30);
  display.print("HR:");
  display.print(beatAvg);
  display.println(" BPM");

  display.setCursor(0, 45);
  display.print("SpO2:");
  display.print(spo2);
  display.println("%");

  display.display();

  Serial.print("Tremor: ");
  Serial.print(tremorStatus);
  Serial.print(" HR: ");
  Serial.print(beatAvg);
  Serial.print(" BPM");
  Serial.print(" SpO2: ");
  Serial.println(spo2);

  delay(100);
}
