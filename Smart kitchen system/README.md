<div align="center">

# 🔥 Smart Kitchen Safety System
### ESP32-Based IoT Safety & Automation Project

[![Platform](https://img.shields.io/badge/Platform-ESP32-blue?style=for-the-badge&logo=espressif)](https://www.espressif.com/)
[![IDE](https://img.shields.io/badge/IDE-Arduino-teal?style=for-the-badge&logo=arduino)](https://www.arduino.cc/)
[![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)](LICENSE)
[![Language](https://img.shields.io/badge/Language-C++-orange?style=for-the-badge&logo=cplusplus)](https://isocpp.org/)
[![Status](https://img.shields.io/badge/Status-Active-brightgreen?style=for-the-badge)]()
[![GitHub](https://img.shields.io/badge/GitHub-lshariprasad-black?style=for-the-badge&logo=github)](https://github.com/lshariprasad)

<br>

> **Real-time fire and gas detection system with automatic motor cutoff, exhaust fan activation, and live OLED monitoring — all powered by ESP32.**

<br>

</div>

---

## 📌 Table of Contents

- [Overview](#-overview)
- [System Architecture](#-system-architecture)
- [Features](#-features)
- [Components Used](#-components-used)
- [Circuit Diagram](#-circuit-diagram)
- [Pin Configuration](#-pin-configuration)
- [Circuit Connections](#-circuit-connections)
- [Working Principle](#-working-principle)
- [Software & Libraries](#-software--libraries)
- [Code Structure](#-code-structure)
- [OLED Display Layout](#-oled-display-layout)
- [Serial Monitor Output](#-serial-monitor-output)
- [Sensor Logic Table](#-sensor-logic-table)
- [Future Enhancements](#-future-enhancements)
- [Applications](#-applications)
- [Author](#-author)

---

## 🧠 Overview

The **Smart Kitchen Safety System** is an embedded IoT project built on the **ESP32 microcontroller**. It continuously monitors kitchen safety conditions — detecting **fire**, **gas leaks**, **temperature**, and **humidity** — and responds autonomously to prevent accidents.

When a flame is detected, the **DC motor stops immediately**. When gas leakage is detected, the **exhaust fan activates automatically**. All sensor data and system states are displayed live on an **SSD1306 OLED display**.

---

## 🏗 System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                      ESP32 DevKit V1                        │
│                                                             │
│  INPUT SENSORS              OUTPUT ACTUATORS               │
│  ─────────────              ─────────────────              │
│  GPIO25 ← Flame Sensor DO   GPIO26/27 → L298N IN1/IN2      │
│  GPIO34 ← MQ-2 Gas DO       GPIO32/33 → L298N IN3/IN4      │
│  GPIO4  ← DHT11 DATA                                       │
│                             L298N OUT1/2 → 12V DC Motor    │
│  I2C BUS                    L298N OUT3/4 → 12V Exhaust Fan │
│  GPIO21 (SDA) → OLED                                       │
│  GPIO22 (SCL) → OLED        POWER                          │
│                             12V Adapter → L298N            │
│                             3.3V → OLED, DHT11, Flame      │
│                             5V   → MQ-2                    │
└─────────────────────────────────────────────────────────────┘
```

---

## ✨ Features

| Feature | Description |
|---|---|
| 🔥 **Flame Detection** | Detects fire via digital flame sensor; stops the DC motor instantly |
| 💨 **Gas Leak Detection** | Detects LPG/smoke via MQ-2; activates exhaust fan automatically |
| 🌡️ **Temp & Humidity Monitoring** | Reads real-time values using DHT11 and displays on OLED |
| 📟 **OLED Live Status** | SSD1306 128×64 display shows all sensor states and readings |
| ⚡ **Dual Motor Control** | L298N drives both DC motor and exhaust fan independently |
| 🔁 **Autonomous Operation** | No manual intervention needed; fully reactive system |
| 📡 **Serial Debugging** | Outputs live sensor data to Serial Monitor at 115200 baud |

---

## 🧰 Components Used

| # | Component | Quantity | Purpose |
|---|---|---|---|
| 1 | **ESP32 DevKit V1** | 1 | Main microcontroller |
| 2 | **SSD1306 OLED Display (128×64, I2C)** | 1 | Live status display |
| 3 | **DHT11 Temperature & Humidity Sensor** | 1 | Environmental monitoring |
| 4 | **Flame Sensor Module (Digital)** | 1 | Fire detection |
| 5 | **MQ-2 Gas Sensor Module** | 1 | LPG / smoke detection |
| 6 | **L298N Motor Driver Module** | 1 | Motor and fan control |
| 7 | **12V DC Motor** | 1 | Represents kitchen appliance motor |
| 8 | **12V Exhaust Fan** | 1 | Ventilation on gas detection |
| 9 | **12V DC Adapter** | 1 | Power supply for motor driver |
| 10 | **Jumper Wires** | Several | Connections |
| 11 | **Breadboard** | 1 | Prototyping |

---

## 🔌 Circuit Diagram

![Smart Kitchen System Circuit Diagram](circuit/circuit_diagram.svg)

> **Color Key:** 🔴 Red = 3.3V &nbsp;|&nbsp; 🟡 Yellow = I2C &nbsp;|&nbsp; 🟠 Orange = Sensor Signal &nbsp;|&nbsp; 🔵 Blue = Motor PWM &nbsp;|&nbsp; 🟢 Cyan = Fan PWM &nbsp;|&nbsp; ⚫ Black = GND

---

## 📍 Pin Configuration

### ESP32 — Complete Pin Map

```
╔══════════════════════════════════════════════════════════════╗
║            ESP32 PIN CONFIGURATION TABLE                     ║
╠════════════════╦═══════════╦══════════════════════════════╣
║ GPIO Pin       ║ Direction ║ Connected To                 ║
╠════════════════╬═══════════╬══════════════════════════════╣
║ GPIO 4         ║   INPUT   ║ DHT11 DATA                   ║
║ GPIO 21        ║  I2C SDA  ║ OLED SDA                     ║
║ GPIO 22        ║  I2C SCL  ║ OLED SCL                     ║
║ GPIO 25        ║   INPUT   ║ Flame Sensor DO (Active LOW) ║
║ GPIO 26        ║  OUTPUT   ║ L298N IN1 — Motor (+)        ║
║ GPIO 27        ║  OUTPUT   ║ L298N IN2 — Motor (−)        ║
║ GPIO 32        ║  OUTPUT   ║ L298N IN3 — Fan (+)          ║
║ GPIO 33        ║  OUTPUT   ║ L298N IN4 — Fan (−)          ║
║ GPIO 34        ║   INPUT   ║ MQ-2 Gas Sensor DO (Active LOW) ║
║ 3.3V           ║   POWER   ║ OLED VCC, DHT11 VCC, Flame VCC ║
║ 5V             ║   POWER   ║ MQ-2 VCC                     ║
║ GND            ║   POWER   ║ All GNDs + L298N GND         ║
╚════════════════╩═══════════╩══════════════════════════════╝
```

> ⚠️ **GPIO 34 is input-only** on ESP32 — no internal pull-up, perfect for sensor reading.

---

## 🔌 Circuit Connections

### 🟣 OLED Display (SSD1306 — I2C)

```
OLED Pin    →   ESP32 Pin        Notes
─────────────────────────────────────────
VCC         →   3.3V
GND         →   GND
SDA         →   GPIO 21          I2C Data
SCL         →   GPIO 22          I2C Clock
I2C Address →   0x3C             Default address
```

### 🟣 DHT11 Sensor

```
DHT11 Pin   →   ESP32 Pin        Notes
─────────────────────────────────────────
VCC         →   3.3V
GND         →   GND
DATA        →   GPIO 4
```

### 🔴 Flame Sensor (Digital Output)

```
Flame Pin   →   ESP32 Pin        Notes
─────────────────────────────────────────
VCC         →   3.3V
GND         →   GND
DO          →   GPIO 25          Active LOW
```

> **Logic:** `LOW` = 🔥 Fire Detected → Motor OFF &nbsp;|&nbsp; `HIGH` = ✅ No Fire → Motor ON

### 🟤 MQ-2 Gas Sensor

```
MQ-2 Pin    →   ESP32 Pin        Notes
─────────────────────────────────────────
VCC         →   5V               ⚠ Needs 5V for accurate readings
GND         →   GND
DO          →   GPIO 34          Active LOW (Input-only pin)
```

> **Logic:** `LOW` = 💨 Gas Detected → Fan ON &nbsp;|&nbsp; `HIGH` = ✅ No Gas → Fan OFF

### ⚙️ L298N Motor Driver

```
L298N Pin   →   ESP32 / Power         Notes
──────────────────────────────────────────────────────
IN1         →   GPIO 26               Motor control bit A
IN2         →   GPIO 27               Motor control bit B
IN3         →   GPIO 32               Fan control bit A
IN4         →   GPIO 33               Fan control bit B
12V         →   12V Adapter (+)       Main power input
GND         →   12V Adapter (−)       ← Also connect ESP32 GND here
5V (out)    →   Optional              Can power ESP32 (if jumper on)
OUT1/OUT2   →   12V DC Motor          Motor terminals
OUT3/OUT4   →   12V Exhaust Fan       Fan terminals
```

> ⚠️ **Common Ground is CRITICAL!** Always bridge ESP32 GND to L298N GND or the motor control signals won't work.

### ⚡ Motor / Fan Output States (L298N)

```
IN1  IN2  →  Motor State
─────────────────────────
HIGH LOW  →  Motor Runs Forward  (motorON)
LOW  LOW  →  Motor Stopped       (motorOFF)

IN3  IN4  →  Fan State
─────────────────────────
HIGH LOW  →  Fan ON              (fanON)
LOW  LOW  →  Fan OFF             (fanOFF)
```

---

## ⚙️ Working Principle

### 🟢 Normal Condition
```
No flame detected      → motorON()  — Motor runs continuously
No gas detected        → fanOFF()   — Exhaust fan stays OFF
DHT11 reading normally → Temp & Humidity displayed on OLED
```

### 🔴 Flame Detected
```
Flame Sensor DO → LOW signal received
         ↓
    motorOFF() triggered (IN1=LOW, IN2=LOW)
         ↓
OLED displays:
  ┌──────────────────┐
  │  FIRE DETECTED   │
  │  MOTOR STOPPED   │
  └──────────────────┘
```

### 🟡 Gas Leakage Detected
```
MQ-2 DO → LOW signal received
      ↓
  fanON() triggered (IN3=HIGH, IN4=LOW)
      ↓
OLED displays:
  ┌──────────────────┐
  │  GAS DETECTED    │
  │  FAN ACTIVATED   │
  └──────────────────┘
```

---

## 📦 Software & Libraries

Install the following via **Arduino IDE → Library Manager (Ctrl+Shift+I)**:

| Library | Purpose | Search Name |
|---|---|---|
| `Adafruit GFX Library` | Graphics core for OLED | `Adafruit GFX Library` |
| `Adafruit SSD1306` | OLED 128×64 driver | `Adafruit SSD1306` |
| `DHT sensor library` | DHT11 temp & humidity | `DHT sensor library by Adafruit` |
| `Wire` | I2C communication | Built into Arduino IDE |

### Board Setup in Arduino IDE

```
1. File → Preferences
   Add to "Additional Boards Manager URLs":
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

2. Tools → Board → Boards Manager
   Search "esp32" → Install "ESP32 by Espressif Systems"

3. Tools → Board → ESP32 Arduino → ESP32 Dev Module

4. Tools → Upload Speed → 115200

5. Tools → Port → Select your COM / ttyUSB port
```

---

## 🧩 Code Structure

```
smart_kitchen_system/
│
├── smart_kitchen_system.ino
│   │
│   ├── [Defines]
│   │   ├── SCREEN_WIDTH / SCREEN_HEIGHT (128 x 64)
│   │   ├── DHTPIN 4, DHTTYPE DHT11
│   │   ├── IN1=26, IN2=27 (Motor)
│   │   ├── IN3=32, IN4=33 (Fan)
│   │   ├── FLAME_SENSOR=25
│   │   └── GAS_SENSOR=34
│   │
│   ├── setup()
│   │   ├── Serial.begin(115200)
│   │   ├── pinMode — Motor, Fan, Sensor pins
│   │   ├── dht.begin()
│   │   ├── Wire.begin(21, 22) → OLED I2C
│   │   ├── display.begin(SSD1306_SWITCHCAPVCC, 0x3C)
│   │   ├── motorON()  ← Default: motor on
│   │   └── fanOFF()   ← Default: fan off
│   │
│   ├── loop()
│   │   ├── digitalRead(FLAME_SENSOR) → flameValue
│   │   ├── digitalRead(GAS_SENSOR)   → gasValue
│   │   ├── dht.readTemperature()     → temp
│   │   ├── dht.readHumidity()        → humidity
│   │   │
│   │   ├── if flameValue == LOW  → motorOFF()
│   │   │   else                  → motorON()
│   │   │
│   │   ├── if gasValue == LOW    → fanON()
│   │   │   else                  → fanOFF()
│   │   │
│   │   ├── OLED update (clearDisplay → print all states)
│   │   ├── Serial.print (temp + humidity)
│   │   └── delay(1000)
│   │
│   ├── motorON()  → digitalWrite(IN1,HIGH), digitalWrite(IN2,LOW)
│   ├── motorOFF() → digitalWrite(IN1,LOW),  digitalWrite(IN2,LOW)
│   ├── fanON()    → digitalWrite(IN3,HIGH), digitalWrite(IN4,LOW)
│   └── fanOFF()   → digitalWrite(IN3,LOW),  digitalWrite(IN4,LOW)
│
└── circuit/
    └── circuit_diagram.svg
```

---

## 📟 OLED Display Layout

```
╔════════════════════════╗
║  SMART SAFETY SYSTEM   ║  ← Always shown
║                        ║
║  FIRE DETECTED         ║  ← flameValue == LOW
║  MOTOR STOPPED         ║
║   — OR —               ║
║  Motor Running         ║  ← flameValue == HIGH
║                        ║
║  GAS DETECTED          ║  ← gasValue == LOW
║  FAN ACTIVATED         ║
║   — OR —               ║
║  Fan OFF               ║  ← gasValue == HIGH
║                        ║
║  Temp: 32.50 C         ║  ← DHT11 live reading
║  Hum : 65.20 %         ║
╚════════════════════════╝
```

---

## 🖥 Serial Monitor Output

**Baud Rate:** `115200`

```
Temperature: 32.50 C  Humidity: 65.20 %
Temperature: 32.60 C  Humidity: 65.10 %
Temperature: 32.50 C  Humidity: 65.30 %
```

---

## 📊 Sensor Logic Table

| Sensor | GPIO | Signal | Meaning | System Response |
|---|---|---|---|---|
| Flame Sensor | GPIO 25 | `LOW` | 🔥 Fire detected | `motorOFF()` → Motor stops |
| Flame Sensor | GPIO 25 | `HIGH` | ✅ No fire | `motorON()` → Motor runs |
| MQ-2 Gas Sensor | GPIO 34 | `LOW` | 💨 Gas detected | `fanON()` → Fan activates |
| MQ-2 Gas Sensor | GPIO 34 | `HIGH` | ✅ No gas | `fanOFF()` → Fan off |
| DHT11 | GPIO 4 | Continuous | 🌡️ Temp + Humidity | Displayed on OLED |

> ⚠️ Both sensors use **Active-LOW logic** (common for digital sensor breakout modules).

---

## 🚀 Future Enhancements

- [ ] 📶 **Wi-Fi Alerts** — Push notifications via Telegram Bot / Blynk
- [ ] 📱 **Mobile App Control** — Monitor and override via smartphone
- [ ] 🔔 **Buzzer Alarm** — Audio alert on fire/gas detection
- [ ] ☁️ **Cloud Monitoring** — Log sensor data to Firebase / ThingSpeak
- [ ] 📟 **GSM Emergency Alerts** — SMS via SIM800C module
- [ ] 🏠 **Smart Home Integration** — Amazon Alexa / Google Home support
- [ ] 📊 **Analog Sensitivity Tuning** — Use MQ-2 AO pin for threshold control
- [ ] 🔋 **Battery Backup** — UPS module for power cut safety

---

## 🏭 Applications

| Domain | Use Case |
|---|---|
| 🏠 Smart Homes | Kitchen automation and fire safety |
| 🍽️ Restaurants | Commercial kitchen gas leak monitoring |
| 🏨 Hotels | Centralized kitchen safety systems |
| 🏭 Industrial | Factory gas monitoring and ventilation |
| 🎓 Education | IoT and embedded systems demonstration |

---

## 📁 Repository Structure

```
📦 Smart-Kitchen-System/
 ┣ 📜 smart_kitchen_system.ino    ← Main Arduino sketch
 ┣ 📄 README.md                   ← This file
 ┗ 📁 circuit/
   └── 🖼️ circuit_diagram.svg     ← Full circuit diagram
```

---

## 🛠️ How to Flash

```bash
# 1. Clone the repo
git clone https://github.com/lshariprasad/Smart-Kitchen-System.git
cd Smart-Kitchen-System

# 2. Open smart_kitchen_system.ino in Arduino IDE

# 3. Install required libraries via Library Manager

# 4. Select:
#    Board  → ESP32 Dev Module
#    Port   → Your COM / ttyUSB port
#    Speed  → 115200

# 5. Click Upload (→)

# 6. Open Serial Monitor at 115200 baud to view live readings
```

---

## 👨‍💻 Author

**Hari Prasad L S**  
B.E. Electrical and Electronics Engineering  
SIMATS Saveetha School of Engineering, Chennai

[![GitHub](https://img.shields.io/badge/GitHub-lshariprasad-black?style=for-the-badge&logo=github)](https://github.com/lshariprasad)
[![LinkedIn](https://img.shields.io/badge/LinkedIn-ls__hariprasad-blue?style=for-the-badge&logo=linkedin)](https://linkedin.com/in/ls_hariprasad)
[![LeetCode](https://img.shields.io/badge/LeetCode-ls__hariprasad-orange?style=for-the-badge&logo=leetcode)](https://leetcode.com/ls_hariprasad)

---

## 📜 License

This project is licensed under the **MIT License** — free to use, modify, and distribute with attribution.

---

<div align="center">

⭐ **If this project helped you, drop a star!** ⭐

*Built with ESP32, sensors, and a drive to make embedded systems matter.*

</div>
