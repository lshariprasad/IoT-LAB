

Here's a complete README.md for your GitHub repo:

```markdown
# ⚡ ESP32 Smart Switch - IoT Control Panel

A beautiful, real-time web-based switch monitoring system built on **ESP32**. Designed for **Dr. Anand**, Dean of the College — this project reads a physical switch connected to GPIO and displays its status on a stunning, animated web dashboard.

![ESP32](https://img.shields.io/badge/ESP32-Dev%20Module-blue?style=for-the-badge&logo=espressif&logoColor=white)
![Arduino](https://img.shields.io/badge/Arduino-IDE%201.8.19-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Working-brightgreen?style=for-the-badge)



---

## 📸 Preview

| Switch ON | Switch OFF | Loading State |
|:---------:|:----------:|:-------------:|
| 🟢 Green Glow | 🔴 Red Glow | ⚪ Gray Pulse |

> The dashboard features **glassmorphism UI**, **animated particles**, **rotating borders**, **real-time polling**, **activity logging**, and a **syntax-highlighted code viewer** — all served directly from the ESP32!

---

## ✨ Features

- 🔄 **Real-Time Monitoring** — Polls switch state every 1 second via AJAX
- 🎨 **Stunning UI** — Glassmorphism cards, gradient backgrounds, floating particles
- 💡 **Animated Switch Display** — Glowing ring with pulse animation (green = ON, red = OFF)
- 📊 **Live Dashboard** — Toggle counter, uptime tracker, last check timestamp
- 📝 **Activity Log** — Timestamped history of switch state changes (max 20 entries)
- 📋 **Built-in Code Viewer** — Syntax-highlighted Arduino code with copy-to-clipboard
- 📱 **Fully Responsive** — Works on mobile, tablet, and desktop
- ⚡ **No External Server** — Everything runs on the ESP32 itself
- 🔌 **Simple Wiring** — Just one switch between GPIO 5 and GND

---

## 🛠️ Hardware Required

| Component | Quantity | Notes |
|-----------|:--------:|-------|
| ESP32 Dev Module | 1 | Any ESP32 board works |
| Push Button / Toggle Switch | 1 | Normally open (NO) |
| Jumper Wires | 2 | Male-to-male or as needed |
| USB Cable | 1 | For programming & power |

> **No external resistors needed!** The code uses the ESP32's internal `INPUT_PULLUP` resistor.

---

## 🔌 Wiring Diagram

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
| **GPIO 5** | Switch Pin 1 |
| **GND** | Switch Pin 2 |

**How it works:**
- Switch **pressed** (GPIO reads `0`) → Website shows **🟢 ON**
- Switch **released** (GPIO reads `1`) → Website shows **🔴 OFF**

---

## 🚀 Getting Started

### Prerequisites

- [Arduino IDE](https://www.arduino.cc/en/software) (v1.8.19 or later)
- [ESP32 Board Package](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html) installed
- WiFi network (2.4GHz)

### Step 1: Clone the Repository

```bash
git clone https://github.com/YOUR_USERNAME/esp32-smart-switch.git
cd esp32-smart-switch
```

### Step 2: Configure WiFi Credentials

Open `esp32_smart_switch.ino` and change these two lines:

```cpp
const char* ssid = "YOUR_WIFI_NAME";        // ← Your WiFi SSID
const char* password = "YOUR_WIFI_PASSWORD"; // ← Your WiFi Password
```

### Step 3: Upload to ESP32

1. Open the `.ino` file in **Arduino IDE**
2. Go to **Tools → Board → ESP32 Dev Module**
3. Select the correct **COM Port**
4. Click **Upload** ⬆️

### Step 4: Find the IP Address

1. Open **Serial Monitor** (115200 baud)
2. Wait for WiFi connection
3. Note the IP address printed:

```
Connecting to WiFi.....
Connected!
IP Address: 192.168.1.100
Web server started!
```

### Step 5: Open the Dashboard

Open a browser on any device connected to the **same WiFi** and navigate to:

```
http://192.168.1.100
```

> Replace with your actual IP address from Step 4.

---

## 📁 Project Structure

```
esp32-smart-switch/
│
├── esp32_smart_switch.ino    # Main Arduino sketch (C++ & HTML combined)
├── README.md                 # This file
└── LICENSE                   # MIT License
```

### Inside the `.ino` File

```
┌─────────────────────────────────┐
│  C++ Code (Arduino)             │
│  ├── WiFi Setup                 │
│  ├── WebServer Routes           │
│  └── GPIO Read Logic            │
│                                 │
│  HTML Page (PROGMEM)            │
│  ├── CSS Styles                 │
│  │   ├── Glassmorphism          │
│  │   ├── Animations             │
│  │   └── Responsive Layout      │
│  ├── HTML Structure             │
│  │   ├── Header                 │
│  │   ├── Profile Section        │
│  │   ├── Switch Display         │
│  │   ├── Status Dashboard       │
│  │   ├── Activity Log           │
│  │   └── Code Viewer            │
│  └── JavaScript                 │
│      ├── AJAX Polling           │
│      ├── UI Updates             │
│      ├── Particle System        │
│      ├── Activity Logger        │
│      └── Clipboard Copy         │
└─────────────────────────────────┘
```

---

## 🔧 API Endpoints

The ESP32 serves two endpoints:

| Endpoint | Method | Response | Description |
|----------|:------:|----------|-------------|
| `/` | GET | HTML page | Full dashboard with CSS/JS |
| `/status` | GET | `0` or `1` | Raw switch state (plain text) |

### Example

```bash
# Get switch status
curl http://192.168.1.100/status

# Response: 0 (switch pressed / ON)
# Response: 1 (switch released / OFF)
```

---

## ⚙️ Configuration

### Change the GPIO Pin

Edit this line in the code:

```cpp
#define SWITCH_PIN 5    // Change to any GPIO pin
```

### Change Polling Interval

In the JavaScript section, modify this line:

```javascript
setInterval(pollStatus, 1000);  // 1000ms = 1 second
```

### Customize the Profile

Edit the HTML section to change:
- Name: `Dr. Anand`
- Title: `Dean of the College`
- Avatar letter: `A`

---

## 🎨 UI Components

| Component | Description |
|-----------|-------------|
| **Floating Particles** | 20 animated circles floating upward |
| **Switch Circle** | 220px glowing circle with power icon |
| **Glow Ring** | Pulsing ring around switch (green/red) |
| **Rotating Border** | Dashed circle rotating every 20s |
| **Glass Cards** | Frosted glass panels with blur effect |
| **Status Bar** | Color-changing progress bar |
| **Live Dot** | Pulsing indicator dot |
| **Signal Dots** | Three blinking dots in header |
| **Activity Log** | Scrollable timestamped entries |
| **Code Block** | Syntax-highlighted with copy button |

---

## 🐛 Troubleshooting

### Dashboard shows "LOADING / Connecting..."

| Cause | Solution |
|-------|----------|
| WiFi not connected | Check Serial Monitor for IP address |
| Wrong WiFi band | ESP32 only supports **2.4GHz** WiFi |
| Browser on different network | Connect to the **same WiFi** as ESP32 |
| ESP32 WiFi sleep mode | Code already includes `WiFi.setSleep(false)` |

### Switch state not updating

| Cause | Solution |
|-------|----------|
| Wrong GPIO pin | Verify switch is on **GPIO 5** |
| Bad wiring | Check connections with multimeter |
| Browser caching | Code includes cache-busting headers |

### Serial Monitor shows nothing

| Cause | Solution |
|-------|----------|
| Wrong baud rate | Set to **115200** |
| Wrong COM port | Check **Tools → Port** |
| USB cable is charge-only | Use a **data-capable** USB cable |

### Compilation errors

| Error | Solution |
|-------|----------|
| `unterminated raw string` | Do NOT modify the `R"HTMLRAW(` delimiter |
| `WiFi.h multiple libraries` | The ESP32 library is auto-selected (ignore warning) |

---

## 🧠 Technical Details

### Why `R"HTMLRAW(` instead of `R"rawliteral(`?

The HTML page contains JavaScript that **displays Arduino code** including raw string delimiters. If the displayed text contains the same closing sequence as the outer delimiter, the C++ compiler terminates the string prematurely.

**Solution:** We use `HTMLRAW` as the delimiter and construct any displayed delimiter text using `String.fromCharCode()` in JavaScript, so the closing sequence `)HTMLRAW"` never appears inside the string.

### Why inline styles in JavaScript?

Tailwind CSS v4 Browser edition only processes classes found in the **initial HTML**. Classes added dynamically via `innerHTML` are **not detected**. Activity log entries and badge updates use inline `style.cssText` instead.

### Response Headers

The `/status` endpoint includes special headers:

```
Connection: close          → Prevents keep-alive blocking
Cache-Control: no-cache    → Forces fresh data every request
Access-Control-Allow-Origin: *  → Allows cross-origin access
```

---

## 📊 Performance

| Metric | Value |
|--------|-------|
| HTML Page Size | ~15 KB (PROGMEM) |
| `/status` Response Time | < 5ms |
| Polling Interval | 1 second |
| Max Log Entries | 20 (auto-pruned) |
| Particles | 20 (CSS animated) |
| ESP32 RAM Usage | ~45 KB |

---

## 🤝 Contributing

Contributions are welcome! Feel free to:

1. 🍴 Fork the repository
2. 🔧 Create a feature branch (`git checkout -b feature/amazing-feature`)
3. 💾 Commit changes (`git commit -m 'Add amazing feature'`)
4. 📤 Push to branch (`git push origin feature/amazing-feature`)
5. 📬 Open a Pull Request

---

## 📜 License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

- **Dr. Anand** — Dean of the College, for whom this project was designed
- **Espressif Systems** — For the amazing ESP32 platform
- **Tailwind CSS** — For the utility-first CSS framework
- **Arduino Community** — For the ecosystem and documentation

---

## 👨‍💻 Author

**Your Name**

- GitHub: [@YOUR_USERNAME](https://github.com/lshariprasad)

---

<div align="center">

**⭐ If you found this project useful, please give it a star! ⭐**

Built with ❤️ for IoT

</div>
```

---
