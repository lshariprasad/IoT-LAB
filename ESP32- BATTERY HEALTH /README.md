# 🔋 EV Smart Battery Health Monitoring System

> **Advanced ESP32-based battery health monitor** with real-time EV health scoring, dual temperature sensing, predictive time-to-empty, live web dashboard, OLED display, and emergency alert system.

---

## 📸 Project Overview

This system monitors a Li-ion/Li-Po battery in real time using an ESP32 microcontroller. It reads voltage, current, and **room temperature** (via DHT11), derives the **battery temperature** using a thermal model (I²·R), computes an **AI health index**, and serves a sci-fi HUD-style **web dashboard** over WiFi — all while showing live data on an SSD1306 OLED display.

---

## 👨‍💻 Developed By

| Name | Roll Number |
|---|---|
| **YUVARAJ M** | 212224050062 |
| **SARAN R** | 212224050022 |

---

## ✨ Features

- **AI Health Score** — Multi-factor model: voltage sag + thermal stress + current stress + cycle wear
- **Dual Temperature** — DHT11 reads real room temp; battery temp derived via I²·R thermal model
- **Dynamic Alert Thresholds** — Overheat triggers at Room Temp +5°C (warn) / +8°C (critical) — self-adjusting
- **3-Tier Alert System** — WARNING → CRITICAL → EMERGENCY (with distinct buzzer patterns)
- **Predictive Time-to-Empty (TTE)** — Minutes remaining based on current draw and remaining mAh
- **Coulomb Counter** — Tracks mAh discharged per session
- **Cycle Counter** — Persisted in ESP32 NVS flash (survives power cycles)
- **Moving Average Filter** — 8-sample smoothing on voltage and current readings
- **OLED Display** — Title → Credits → WiFi IP → Live Data sequence
- **Emergency Button** — GPIO 17 triggers buzzer + fast LED blink + full OLED alert
- **Pro Web Dashboard** — Sci-fi HUD with live Chart.js waveforms, KPI cards, thermal panel, alert log
- **Offline Fallback** — Demo/fake data mode if sensors are disconnected
- **WiFi Web Server** — `/data` and `/history` JSON endpoints for external integration

---

## 🔌 Pin Configuration

> ⚠️ **DO NOT change these pins** — they are fixed in the firmware.

| Component | GPIO Pin | Type | Notes |
|---|---|---|---|
| **DHT11** (Temp Sensor) | GPIO **4** | Digital Input | Reads room temperature |
| **Voltage Divider** | GPIO **34** | ADC Input | Analog (12-bit, 11dB attenuation) |
| **ACS712 Current Sensor** | GPIO **35** | ADC Input | Analog, auto-calibrated at boot |
| **Buzzer** | GPIO **25** | Digital Output | Active buzzer |
| **Emergency Button** | GPIO **17** | Digital Input (PULLUP) | GND when pressed |
| **Blue LED** | GPIO **19** | Digital Output | Status / alert blink |
| **OLED SDA** | GPIO **21** | I²C | SSD1306 128×64 |
| **OLED SCL** | GPIO **22** | I²C | SSD1306 128×64 |

---

## 🧩 Hardware Components

| Component | Specification |
|---|---|
| Microcontroller | ESP32 (30-pin DevKit) |
| Display | SSD1306 OLED 128×64 (I²C, address 0x3C) |
| Temperature Sensor | DHT11 |
| Voltage Sensing | Resistor voltage divider (÷5 ratio) |
| Current Sensor | ACS712 5A module (sensitivity: 0.185 V/A) |
| Buzzer | 5V active buzzer |
| LED | Blue LED (3.3V with resistor) |
| Emergency Switch | Momentary push button (normally open, to GND) |
| Battery | Li-ion / Li-Po (3.3V min – 4.2V max) |

---

## 📚 Required Libraries

Install all of the following via **Arduino IDE → Library Manager** (`Sketch → Include Library → Manage Libraries`):

| Library | Author | Install Name |
|---|---|---|
| `Adafruit SSD1306` | Adafruit | `Adafruit SSD1306` |
| `Adafruit GFX Library` | Adafruit | `Adafruit GFX Library` |
| `DHT sensor library` | Adafruit | `DHT sensor library` |
| `ArduinoJson` | Benoit Blanchon | `ArduinoJson` |
| `Preferences` | *(Built-in)* | Included with ESP32 board package |
| `WiFi` | *(Built-in)* | Included with ESP32 board package |
| `WebServer` | *(Built-in)* | Included with ESP32 board package |
| `Wire` | *(Built-in)* | Included with ESP32 board package |

### ESP32 Board Package

If you haven't already, add ESP32 board support in Arduino IDE:

1. Go to `File → Preferences`
2. Add this URL to **Additional Boards Manager URLs**:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Go to `Tools → Board → Boards Manager`, search **esp32**, install by **Espressif Systems**

---

## ⚙️ Configuration (inside the `.ino` file)

```cpp
// WiFi credentials
const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Battery specs
const float CAPACITY_MAH   = 2000.0f;   // Your battery's rated capacity in mAh
const float VOLT_DIVIDER   = 5.0f;      // Adjust to match your resistor divider ratio
const float BATT_MIN_VOLT  = 3.3f;      // Minimum battery voltage (empty)
const float BATT_MAX_VOLT  = 4.2f;      // Maximum battery voltage (full)

// Alert thresholds
const float LOW_BATT_WARN  = 30.0f;     // % — warning level
const float LOW_BATT_CRIT  = 15.0f;     // % — critical level
const float OVER_CURR_WARN = 1.5f;      // A — current warning
const float OVER_CURR_CRIT = 2.0f;      // A — current critical
const float TEMP_DELTA_WARN = 5.0f;     // °C above room — thermal warning
const float TEMP_DELTA_CRIT = 8.0f;     // °C above room — thermal critical
```

---

## 🌡️ Temperature Model Explained

The **DHT11 reads actual room/ambient temperature**. Since DHT11 is not mounted on the battery cell, the **battery temperature is derived** using a simple I²R thermal model:

```
Battery Temp = Room Temp + (Current² × 3.0)
```

This means:
- At 0 A draw → Battery Temp ≈ Room Temp (idle, no heating)
- At 1 A draw → Battery Temp ≈ Room Temp + 3°C
- At 2 A draw → Battery Temp ≈ Room Temp + 12°C

**Alert thresholds are dynamic** and always relative to the current room temperature — so the system works correctly whether it's 20°C in winter or 38°C in summer.

---

## 🧠 AI Health Score Formula

```
Health = (Voltage Sag × 60%)
       + (Thermal Safety × 20%)
       + (Current Safety × 15%)
       - (Cycle Wear × up to 20%)
       + 5 (base offset)
```

| Factor | Description |
|---|---|
| **Voltage Sag** | How close voltage is to minimum (low voltage = degraded cell) |
| **Thermal Safety** | How far battery temp is from critical threshold |
| **Current Safety** | How far current is from overcurrent limit |
| **Cycle Wear** | Linear degradation — ~1000 cycles → ~20% health penalty |

---

## 📡 Web Dashboard Endpoints

Once connected to WiFi, open the IP address shown on OLED in any browser.

| Endpoint | Method | Description |
|---|---|---|
| `/` | GET | Full sci-fi web dashboard (HTML) |
| `/data` | GET | JSON with all live sensor data |
| `/history` | GET | JSON with last 8 alert events |

### `/data` JSON Response

```json
{
  "v":    3.82,       // Battery voltage (V)
  "p":    58.0,       // State of charge (%)
  "c":    0.42,       // Discharge current (A)
  "rt":   27.4,       // Room temperature °C (DHT11)
  "bt":   28.1,       // Battery temperature °C (derived)
  "h":    83.0,       // AI health index (%)
  "tte":  94.0,       // Time to empty (minutes)
  "cc":   12.4,       // Coulombs discharged this session (mAh)
  "cycles": 12,       // Total cycle count (NVS persisted)
  "low":  false,      // Low battery warning
  "lowCrit": false,   // Low battery critical
  "ot":   false,      // Overheat warning
  "otCrit": false,    // Overheat critical
  "oc":   false,      // Overcurrent warning
  "ocCrit": false,    // Overcurrent critical
  "cold": false,      // Battery too cold (room < 10°C)
  "emergency": false, // Emergency button state
  "fakeData": false   // True if running on demo data
}
```

---

## 🚨 Alert System

### Buzzer Patterns

| Condition | Pattern |
|---|---|
| Normal | Silent |
| Warning (any) | 1 short beep |
| Critical (any) | 3 long beeps |
| Emergency button | Continuous ON |

### LED Patterns

| Condition | LED |
|---|---|
| Normal | Solid ON |
| Warning | Slow blink (600ms) |
| Critical | Fast blink (150ms) |
| Emergency | Very fast blink (150ms) |

### OLED Startup Sequence

```
[0–8s]   Title screen — "AI BATTERY HEALTH MONITORING SYSTEM v2.0"
[8–18s]  Credits screen — Team names and roll numbers
[18–38s] WiFi screen — Connected IP address (if WiFi success)
[38s+]   Live data — Voltage, SOC, Current, Room Temp, Battery Temp, Health, TTE
```

---

## 🔧 How to Upload

1. Clone or download this repository
2. Open `battery_monitor_advanced.ino` in Arduino IDE
3. Install all required libraries (see above)
4. Select your board: `Tools → Board → ESP32 Dev Module`
5. Select your port: `Tools → Port → COMx` (Windows) or `/dev/ttyUSB0` (Linux/Mac)
6. Set upload speed: `Tools → Upload Speed → 115200`
7. Click **Upload** ✅
8. Open Serial Monitor at **115200 baud** to see boot logs and IP address
9. Open the IP address in your browser 🌐

---

## 📁 File Structure

```
📦 AI-Battery-Health-Monitor/
 ┣ 📄 battery_monitor_advanced.ino   ← Main firmware
 ┣ 📄 README.md                      ← This file
 ┗ 📄 LICENSE                        ← MIT License
```

---

## 🖥️ Dashboard Preview

The web dashboard features:
- **Sci-fi HUD aesthetic** with Orbitron font, animated grid background, scanline overlay
- **6 live KPI cards** — Voltage, SOC, Current, AI Health, TTE, System Status
- **Color-coded alerts** — green (normal) → yellow (warn) → red (critical) with glow effects
- **Thermal dual panel** — Room °C and Battery °C side by side with live Δ delta
- **Two live Chart.js charts** — Waveform (V, A, Health) + Thermal profile (Room vs Battery)
- **Alert log** — Last 8 events with timestamps ("Xs ago")
- **Emergency banner** — Full-width flashing red stripe when GPIO 17 is pressed
- **DEMO DATA badge** — Appears when sensor fallback is active

---

## 📄 License

```
MIT License

Copyright (c) 2025 YUVARAJ M & SARAN R

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## 🙏 Acknowledgements

- [Adafruit](https://www.adafruit.com/) — SSD1306 and DHT libraries
- [Benoit Blanchon](https://arduinojson.org/) — ArduinoJson library
- [Espressif](https://www.espressif.com/) — ESP32 Arduino core
- [Chart.js](https://www.chartjs.org/) — Live web charts
- [Google Fonts](https://fonts.google.com/) — Orbitron, Share Tech Mono, Rajdhani

---

> Made with ❤️ for our embedded systems project · 2026
