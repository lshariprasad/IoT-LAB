# 🏥 SmartHealth Monitor — ESP32 Patient Monitoring System

![Project Banner](images/banner.jpg)
> *Add your project photo above by placing it in an `images/` folder*

---

## 📌 About the Project

**SmartHealth Monitor** is a real-time patient health monitoring system built entirely on the **ESP32 microcontroller**. It measures heart rate (BPM), blood oxygen saturation (SpO₂), and detects whether a mask is being worn using a time-of-flight distance sensor.

All vitals are displayed on a local **OLED screen** and streamed live to a **hospital-grade web dashboard** accessible from any device on the same Wi-Fi network — no internet required.

A **buzzer alarm** triggers automatically when the mask is removed, making this ideal for clinic waiting rooms, isolation wards, or personal health tracking.

---

## ✨ Features

- 💓 **Live Heart Rate (BPM)** — Real-time detection via MAX30102
- 🩸 **SpO₂ Monitoring** — Blood oxygen saturation with ring gauge
- 📏 **Mask Detection** — VL53L0X distance sensor (threshold: 80mm)
- 🔔 **Buzzer Alert** — Beeps when mask is removed
- 📟 **OLED Display** — Animated heart icon + live ECG graph + vitals
- 🌐 **Web Dashboard** — Hospital-style UI with live BPM graph, SpO₂ gauge, distance bar, and alert banner
- 📡 **Wi-Fi Streaming** — Access from phone, tablet, or PC on local network

---

## 🖼️ Project Photos

| OLED Display | Web Dashboard |
|---|---|
| <img width="302.5" height="292.5" alt="image" src="https://github.com/user-attachments/assets/9fa64d1f-1901-4a49-b88e-ebe870305c46" /> 
| <img width="400" height="190.75" alt="image" src="https://github.com/user-attachments/assets/6fa901ca-c53e-43fd-8d54-064f0f090942" /> |

> *Replace with your own photos*

---

## 🔧 Hardware Required

| Component | Quantity |
|---|---|
| ESP32 Development Board | 1 |
| MAX30102 Heart Rate & SpO₂ Sensor | 1 |
| VL53L0X Time-of-Flight Distance Sensor | 1 |
| SSD1306 OLED Display (128×64, I²C) | 1 |
| Active Buzzer | 1 |
| Jumper Wires | As needed |
| Breadboard | 1 |

---
## Circuit Diagram :

<img width="896" height="642" alt="image" src="https://github.com/user-attachments/assets/15365e19-137a-4d58-8aa4-e901112512f3" />



---

## Real HardWare (ESP32) :

<img width="1353" height="962" alt="image" src="https://github.com/user-attachments/assets/d376905e-b496-4c5d-91c3-919cb21903df" />


---

##

---

## 📍 Pin Configuration

### I²C Bus (Shared by OLED, MAX30102, VL53L0X)

| ESP32 Pin | Connected To |
|---|---|
| GPIO 21 (SDA) | SDA of OLED + MAX30102 + VL53L0X |
| GPIO 22 (SCL) | SCL of OLED + MAX30102 + VL53L0X |
| 3.3V | VCC of all I²C modules |
| GND | GND of all I²C modules |

### Buzzer

| ESP32 Pin | Buzzer Pin |
|---|---|
| GPIO 25 | + (Positive) |
| GND | − (Negative) |

### I²C Addresses

| Module | I²C Address |
|---|---|
| SSD1306 OLED | `0x3C` |
| MAX30102 | `0x57` |
| VL53L0X | `0x29` |

---

## 🗂️ Project Structure
```
SmartHealth-Monitor/
├── smart_health_monitor.ino   # Main Arduino sketch
├── README.md                  # This file
└── images/
    ├── banner.jpg             # Add your banner photo
    ├── oled.jpg               # Add your OLED photo
    └── web.jpg                # Add your dashboard screenshot
```

---

## 🚀 Getting Started

### 1. Install Arduino Libraries

Open Arduino IDE → **Tools → Manage Libraries** and install:

- `Adafruit SSD1306`
- `Adafruit GFX Library`
- `Adafruit VL53L0X`
- `SparkFun MAX3010x Pulse and Proximity Sensor Library`

Also install **ESP32 board support** via Boards Manager:
```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

### 2. Configure Wi-Fi

Open `smart_health_monitor.ino` and update:
```cpp
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

### 3. Upload

- Board: **ESP32 Dev Module**
- Upload Speed: **115200**
- Flash Frequency: **80MHz**

### 4. Access Dashboard

After uploading, open Serial Monitor at **115200 baud**. The ESP32 will print its IP address. Open that IP in any browser on the same network.
```
Example: http://192.168.1.45
```

---

## 🧠 How It Works

### Mask Detection Logic
The VL53L0X sensor measures the distance to the nearest object in front of it (e.g., a face with a mask):

| Distance | Status |
|---|---|
| `< 80 mm` | ✅ **Mask ON** |
| `≥ 80 mm` | ❌ **Mask OFF** → Buzzer beeps |

### Heart Rate & SpO₂
The MAX30102 uses infrared and red LED reflectance to detect pulse. Each beat is timed to calculate BPM. SpO₂ is estimated from the IR/Red ratio.

### Web Dashboard
The ESP32 hosts a web server on port 80. The dashboard fetches `/data` (JSON) every second and updates:
- Live scrolling BPM graph (Chart.js)
- SpO₂ circular ring gauge
- Distance progress bar
- Mask status with flashing alert

### OLED Screen Layout
```
┌─────────────────────────────┐
│  SMART HEALTH MON.          │
│  ♥ BPM: 75                  │
│  ~~~~ECG Graph~~~~~         │
├─────────────────────────────┤
│  HR:75  O2:98%              │
│  D:62mm  Mask:ON            │
└─────────────────────────────┘
```

---

## 🌐 Web Dashboard Preview

The dashboard features:
- 🔴 **Heart Rate card** with animated beating heart
- 🟢 **SpO₂ card** with ring gauge (green/yellow/red by level)
- 🔵 **Distance card** with live progress bar
- ⚠️ **Mask Status card** with flashing red alert when removed
- 📈 **Live ECG-style BPM graph**
- 🕐 **Live clock** and connection status

---

## ⚙️ Configuration Reference

| Parameter | Location | Default |
|---|---|---|
| Wi-Fi SSID | Top of `.ino` | `YOUR_WIFI_SSID` |
| Wi-Fi Password | Top of `.ino` | `YOUR_WIFI_PASSWORD` |
| Buzzer Pin | `#define BUZZER_PIN` | GPIO 25 |
| SDA Pin | `#define SDA_PIN` | GPIO 21 |
| SCL Pin | `#define SCL_PIN` | GPIO 22 |
| Mask threshold | `maskStatus` logic | 80 mm |
| Buzzer interval | `lastBuzz` check | Every 2 seconds |

---

## 🔮 Future Improvements

- [ ] Temperature sensor (DS18B20 or MLX90614)
- [ ] Data logging to SD card or Google Sheets
- [ ] SMS/email alert when mask removed
- [ ] Battery + 3D printed enclosure
- [ ] MQTT integration for IoT dashboards (Node-RED, Home Assistant)
- [ ] Proper SpO₂ calculation using R-ratio algorithm

---

## 📦 Dependencies

| Library | Purpose |
|---|---|
| `Wire.h` | I²C communication |
| `WiFi.h` | ESP32 Wi-Fi |
| `WebServer.h` | HTTP server |
| `Adafruit_GFX` | Graphics core |
| `Adafruit_SSD1306` | OLED driver |
| `Adafruit_VL53L0X` | Distance sensor |
| `MAX30105` | Heart rate sensor |
| `heartRate.h` | Beat detection algorithm |
| Chart.js (CDN) | Web dashboard graph |

---

## 🤝 Contributing

Pull requests are welcome! For major changes, please open an issue first to discuss what you would like to change.

1. Fork the repo
2. Create your feature branch (`git checkout -b feature/NewFeature`)
3. Commit your changes (`git commit -m 'Add NewFeature'`)
4. Push to the branch (`git push origin feature/NewFeature`)
5. Open a Pull Request

---

## 👤 Author

**[HARI PRASAD L S]**

> Replace `[HARI PRASAD L S]` with your name before publishing

- GitHub: [@lshariprasad](https://github.com/lshariprasad)

---

## 📄 License
```
MIT License

Copyright (c) 2026 [Your Name Here]

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

<div align="center">
Made with ❤️ using ESP32 &nbsp;|&nbsp; SmartHealth Monitor v2.0
</div>
