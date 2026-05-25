# 🔵 4x4x4 LED Cube — Arduino Nano

> A 4×4×4 LED cube with **31 animations** driven by an Arduino Nano, featuring custom PCB design made in KiCad.

---

## 📁 File Structure in Repo

```
IOT/
└── 4x4x4-LED-Cube/
    ├── README.md
    ├── 4x4x4_led_cube_project.ino
    └── images/
        ├── circuit_diagram.png
        ├── pcb_3d_top.png
        ├── pcb_layout.png
        ├── pcb_3d_kicad.png
        └── pcb_back.png
```

---

## 📸 Project Gallery

### 🔌 Circuit Diagram
![Circuit Diagram](images/circuit_diagram.png)

### 🟢 PCB 3D View (Top)
![PCB 3D Top](images/pcb_3d_top.png)

### 📐 PCB Layout (Top & Bottom Copper)
![PCB Layout](images/pcb_layout.png)

### 🖥️ KiCad 3D Viewer
![KiCad 3D Viewer](images/pcb_3d_kicad.png)

### 🔁 PCB Back Copper Layer
![PCB Back](images/pcb_back.png)

---

## 🧰 Components

| Component       | Value / Type  | Quantity |
|----------------|---------------|----------|
| Arduino Nano    | ATmega328P    | 1        |
| LEDs            | 5mm Blue/Any  | 64       |
| Resistors       | 330Ω          | 16       |
| Resistors       | 1KΩ           | 4        |
| Transistors     | BC547 (NPN)   | 4        |
| Custom PCB      | KiCad Design  | 1        |

---

## 📌 Pin Mapping

### Column Pins (16 LEDs per layer)
| Arduino Pin | Column |
|-------------|--------|
| D0 – D13    | C0–C13 |
| A0          | C14    |
| A1          | C15    |

### Layer Pins (via BC547 transistors)
| Arduino Pin | Layer  |
|-------------|--------|
| A2          | Layer 1 (Top)    |
| A3          | Layer 2          |
| A4          | Layer 3          |
| A5          | Layer 4 (Bottom) |

---

## 🎮 Animations (31 Total)

| # | Animation Name        |
|---|-----------------------|
| 1 | Spiral Rotation       |
| 2 | Vortex Effect         |
| 3 | Cross Rotation        |
| 4 | Layer Spin            |
| 5 | Diagonal 3D Rotation  |
| 6 | Wave Animation        |
| 7 | Rain Effect           |
| 8 | Expand Contract       |
| 9 | Diagonal Move         |
| 10 | Sparkle Effect       |
| 11 | Random Blink         |
| 12 | Layer Wipe           |
| 13 | Cube Rotation        |
| 14 | Border Glow          |
| 15 | Layer Snake          |
| 16 | Zigzag Wave          |
| 17 | Expanding Square     |
| 18 | Spiral Motion        |
| 19 | Checkerboard Blink   |
| 20 | Vertical Wave        |
| 21 | Corner Rotation      |
| 22 | Layer Rotation       |
| 23 | Edge Flow            |
| 24 | Diagonal Wipe        |
| 25 | Column Scan          |
| 26 | Tunnel Effect        |
| 27 | Flashing Borders     |
| 28 | Layer Cascade        |
| 29 | Circular Wave        |
| 30 | Center Spiral Rotation |
| 31 | Expanding Rings      |

---

## ⚙️ How It Works

- The cube uses **multiplexing** — only one layer is active at a time
- Each layer is controlled by a **BC547 NPN transistor** (1KΩ base resistor)
- Each column LED uses a **330Ω current-limiting resistor**
- Layer pins go **LOW** to activate (common cathode layer drive)
- Column pins go **HIGH** to light up an LED

---

## 🚀 Upload Instructions

1. Open `4x4x4_led_cube_project.ino` in **Arduino IDE**
2. Select Board: **Arduino Nano**
3. Select Processor: **ATmega328P**
4. Select the correct COM Port
5. Click **Upload**

---

## 🛠️ PCB Design

- Designed in **KiCad**
- All 16 column resistors (330Ω) on top row
- 4 transistor base resistors (1KΩ) on bottom row
- 4 BC547 transistors for layer switching
- Arduino Nano socket in center

---

## 👤 Author

**Hari Prasad L S**  
B.E. Electrical & Electronics Engineering  
SIMATS Saveetha School of Engineering, Chennai  

[![GitHub](https://img.shields.io/badge/GitHub-IOT_Repo-black?logo=github)](https://github.com/)

---

## 📄 License

MIT License — Free to use and modify.
