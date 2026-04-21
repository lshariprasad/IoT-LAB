# 📡 Signal Analysis System v2.0

A real-time signal monitoring system built with Arduino, featuring an animated OLED display, state machine architecture, and multi-level alert system.

![Arduino](https://img.shields.io/badge/Arduino-Uno%2FNano-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![Version](https://img.shields.io/badge/Version-2.0-blue?style=for-the-badge)

---

## 🖼️ Preview

```
╔══════════════════════════════╗
║  ● ○ ○ ○ ○ ○ ○        12s   ║
║ ══════════════════════════   ║
║  IDLE              90us      ║
║ ══════════════════════════   ║
║  ✓  GOOD         RNG: LOW    ║
║ ══════════════════════════   ║
║  ∿∿∿∿∿∿∿∿∿∿∿∿∿∿∿∿∿∿∿∿∿∿∿   ║
║  [████████░░░░░░░░░░░░░░░]   ║
╚══════════════════════════════╝
```

---

## ✨ Features

- **Clean State Machine** — 7 signal states cycle automatically with configurable durations
- **Animated OLED UI** — Live scrolling waveform, blinking alerts, progress bar, state dots
- **3 Alert Levels** — GOOD / WARNING / CRITICAL with distinct LED and buzzer behaviors
- **Smart Button** — Tap to toggle ON/OFF, hold 2 seconds to reset to State 0
- **Non-blocking Code** — 100% `millis()`-based timing, no `delay()` calls
- **Serial Debug Output** — Real-time state transitions logged at 115200 baud

---

## 🧰 Hardware Required

| Component | Quantity |
|-----------|----------|
| Arduino Uno / Nano | 1 |
| SSD1306 OLED (128×64, I2C) | 1 |
| Green LED | 1 |
| Red LED | 1 |
| Active Buzzer | 1 |
| Push Button | 1 |
| 220Ω Resistors | 2 |
| Breadboard + Jumper Wires | — |

---

## 🔌 Wiring

```
Arduino          Component
───────          ─────────
D2          →    Button (other leg → GND)
D6          →    Green LED → 220Ω → GND
D7          →    Red LED   → 220Ω → GND
D8          →    Buzzer (+) | Buzzer (−) → GND
A4 (SDA)    →    OLED SDA
A5 (SCL)    →    OLED SCL
5V          →    OLED VCC
GND         →    OLED GND
```

> ⚠️ **Note:** If using an Arduino Nano, SDA = A4 and SCL = A5 (same as Uno). For Mega, use SDA = 20 and SCL = 21.

---

## 📦 Libraries Required

Install via **Arduino IDE → Sketch → Include Library → Manage Libraries**:

| Library | Author | Version |
|---------|--------|---------|
| `Adafruit SSD1306` | Adafruit | ≥ 2.5.0 |
| `Adafruit GFX Library` | Adafruit | ≥ 1.11.0 |
| `Wire` | Arduino (built-in) | — |

---

## 🚀 Getting Started

1. **Clone this repository**
   ```bash
   git clone https://github.com/YOUR_USERNAME/signal-analysis-system.git
   ```

2. **Open in Arduino IDE**
   - File → Open → `signal_analysis_advanced.ino`

3. **Install libraries** (see table above)

4. **Wire up the hardware** (see wiring diagram above)

5. **Upload** to your Arduino

6. **Press the button** to start the system!

---

## 🔄 Signal States

| # | Label | Delay (µs) | Alert Level | Duration |
|---|-------|-----------|-------------|----------|
| 0 | IDLE | 90 | ✅ GOOD | 15s |
| 1 | SPIKE | 180 | 🔴 CRITICAL | 5s |
| 2 | NOISE | 130 | ⚠️ WARNING | 10s |
| 3 | BURST | 200 | 🔴 CRITICAL | 2s |
| 4 | STABLE | 85 | ✅ GOOD | 15s |
| 5 | SURGE | 170 | 🔴 CRITICAL | 6s |
| 6 | DRIFT | 140 | ⚠️ WARNING | 15s |

---

## 🚨 Alert Behavior

| Level | Green LED | Red LED | Buzzer |
|-------|-----------|---------|--------|
| GOOD | Solid ON | OFF | Silent |
| WARNING | Slow blink | OFF | Single beep every 2s |
| CRITICAL | OFF | Fast blink | Rapid double-beep pattern |

---

## 🏗️ Code Architecture

```
signal_analysis_advanced.ino
├── SignalState struct       — Data-driven state definitions
├── handleButton()           — Debounced tap + hold-to-reset
├── updateOutputs()          — LED & buzzer control per alert level
├── drawBootScreen()         — Startup splash screen
├── drawOffScreen()          — System OFF screen
├── drawMainScreen()         — Live monitoring UI
│   ├── State progress dots
│   ├── Large blinking label
│   ├── Animated sine waveform
│   └── Progress bar
└── loop()                   — Non-blocking state machine
```

---

## ⚙️ Customization

To change signal states, edit the `STATES[]` array in the `.ino` file:

```cpp
const SignalState STATES[] = {
  { "LABEL", delayMicros, ALERT_LEVEL, durationMs },
  //  ^name    ^µs value    ^GOOD/WARNING/CRITICAL  ^ms
};
```

To change the hold-reset duration:
```cpp
#define HOLD_RESET_MS 2000   // 2000 = 2 seconds
```

---

## 📁 Project Structure

```
signal-analysis-system/
├── signal_analysis_advanced.ino   # Main sketch
├── README.md                      # This file
├── LICENSE                        # MIT License
├── .gitignore                     # Arduino build artifacts
└── docs/
    └── wiring_diagram.md          # Detailed wiring reference
```

---

## 🤝 Contributing

Pull requests are welcome! For major changes, please open an issue first.

1. Fork the repo
2. Create your feature branch: `git checkout -b feature/my-feature`
3. Commit your changes: `git commit -m 'Add my feature'`
4. Push to branch: `git push origin feature/my-feature`
5. Open a Pull Request

---

## 📄 License

This project is licensed under the MIT License — see [LICENSE](LICENSE) for details.

---

## 🙏 Acknowledgements

- [Adafruit](https://github.com/adafruit) for the SSD1306 and GFX libraries
- Arduino community for the inspiration
