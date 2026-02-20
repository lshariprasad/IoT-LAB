# ⚡ ESP32 Smart Switch – IoT Control Panel

A beautiful, real-time web-based switch monitoring system built on **ESP32**.  
Designed for **Dr. Anand**, Dean of the College — this project reads a physical switch connected to GPIO and displays its status on a stunning animated web dashboard.

![ESP32](https://img.shields.io/badge/ESP32-Dev%20Module-blue?style=for-the-badge&logo=espressif&logoColor=white)
![Arduino](https://img.shields.io/badge/Arduino-IDE%201.8.19-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Working-brightgreen?style=for-the-badge)

---

## 📸 Preview

<img width="360" height="800" alt="image" src="https://github.com/user-attachments/assets/0372723e-3fa8-490a-ae6f-f0b5950920d3" />
<img width="360" height="800" alt="image" src="https://github.com/user-attachments/assets/1a812f35-e527-4a2e-9645-fae4fe4a2d09" />
<img width="360" height="800" alt="image" src="https://github.com/user-attachments/assets/accfaaf6-8fd4-4789-9c97-190bea700d39" />
<img width="360" height="800" alt="image" src="https://github.com/user-attachments/assets/7f842ada-9cd5-4faa-a958-d2ef294cdade" />
<img width="1047" height="432" alt="image" src="https://github.com/user-attachments/assets/783c5ea2-8f98-4fe8-9412-52d6d54dfb1a" />

| Switch ON | Switch OFF | Loading State |
|:---------:|:----------:|:-------------:|
| 🟢 Green Glow | 🔴 Red Glow | ⚪ Gray Pulse |

> The dashboard features glassmorphism UI, animated particles, rotating borders, real-time polling, activity logging, and a syntax-highlighted code viewer — all served directly from the ESP32.

---

## ✨ Features

- 🔄 **Real-Time Monitoring** — Polls switch state every second via AJAX  
- 🎨 **Modern UI** — Glassmorphism cards, gradient backgrounds, floating particles  
- 💡 **Animated Switch Display** — Glowing ring with pulse animation  
- 📊 **Live Dashboard** — Toggle counter, uptime tracker, last check timestamp  
- 📝 **Activity Log** — Timestamped history of switch state changes (max 20 entries)  
- 📋 **Built-in Code Viewer** — Syntax-highlighted Arduino code with copy-to-clipboard  
- 📱 **Fully Responsive** — Works on mobile, tablet, and desktop  
- ⚡ **No External Server Required** — Everything runs directly on ESP32  
- 🔌 **Simple Wiring** — Only one switch between GPIO 5 and GND  

---

## 🛠 Hardware Required

| Component | Quantity | Notes |
|-----------|:--------:|-------|
| ESP32 Dev Module | 1 | Any ESP32 board |
| Push Button / Toggle Switch | 1 | Normally Open (NO) |
| Jumper Wires | 2 | As required |
| USB Cable | 1 | For programming & power |

> No external resistors required — the code uses `INPUT_PULLUP`.

---

## 🔌 Wiring

```
ESP32                    Switch
┌──────────┐            ┌──────┐
│          │            │      │
│    GPIO 5├────────────┤ Pin 1│
│          │            │      │
│      GND ├────────────┤ Pin 2│
│          │            │      │
└──────────┘            └──────┘
```

| ESP32 Pin | Connects To |
|:---------:|:-----------:|
| GPIO 5 | Switch Pin 1 |
| GND | Switch Pin 2 |

**Working Logic:**
- Switch pressed → GPIO reads `0` → 🟢 ON  
- Switch released → GPIO reads `1` → 🔴 OFF  

---

## 🚀 Getting Started

### Prerequisites

- Arduino IDE (v1.8.19 or later)
- ESP32 Board Package installed
- 2.4GHz WiFi Network

---

### 1️⃣ Clone the Repository

```bash
git clone https://github.com/YOUR_USERNAME/esp32-smart-switch.git
cd esp32-smart-switch
```

---

### 2️⃣ Configure WiFi Credentials

Open `esp32_smart_switch.ino` and update:

```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
```

---

### 3️⃣ Upload to ESP32

1. Open `.ino` file in Arduino IDE  
2. Select **ESP32 Dev Module**  
3. Select correct COM Port  
4. Click Upload  

---

### 4️⃣ Find the IP Address

Open Serial Monitor (115200 baud).

Example output:

```
Connecting...
Connected!
IP Address: 192.168.1.100
```

---

### 5️⃣ Open Dashboard

In browser:

```
http://192.168.1.100
```

(Use your actual IP address)

---

## 📁 Project Structure

```
esp32-smart-switch/
│
├── esp32_smart_switch.ino
├── README.md
└── LICENSE
```

---

## 🔧 API Endpoints

| Endpoint | Method | Response | Description |
|----------|:------:|----------|-------------|
| `/` | GET | HTML Page | Full dashboard |
| `/status` | GET | `0` or `1` | Switch state |

Example:

```bash
curl http://192.168.1.100/status
```

---

## ⚙ Configuration

### Change GPIO Pin

```cpp
#define SWITCH_PIN 5
```

### Change Polling Interval

```javascript
setInterval(pollStatus, 1000);
```

---

## 🐛 Troubleshooting

### Dashboard Stuck on Loading
- Ensure ESP32 is connected to WiFi
- Use 2.4GHz network
- Check IP address in Serial Monitor
- Ensure browser is on same network

### Switch Not Updating
- Verify wiring
- Confirm correct GPIO pin
- Refresh browser

### Serial Monitor Blank
- Set baud rate to 115200
- Check COM port
- Use data USB cable

### Compilation Error: Unterminated Raw String
Do not modify the raw string delimiter in the HTML section.

---

## 📊 Performance

| Metric | Value |
|--------|-------|
| HTML Size | ~15 KB |
| Status Response | < 5ms |
| Polling | 1 sec |
| Max Log Entries | 20 |
| RAM Usage | ~45 KB |

---

## 🤝 Contributing

1. Fork repository  
2. Create feature branch  
3. Commit changes  
4. Push branch  
5. Open Pull Request  

---

## 📜 License

MIT License — see LICENSE file.

---

## 👨‍💻 Author

**Your Name**  
GitHub: https://github.com/lshariprasad  

---

<div align="center">

⭐ If you found this project useful, please give it a star! ⭐  

Built with ❤️ for IoT  

</div>
