<div align="center">

<!-- Animated Banner -->
<img src="https://capsule-render.vercel.app/api?type=waving&color=0:00d4ff,100:0066ff&height=200&section=header&text=4×4×4%20LED%20Cube&fontSize=60&fontColor=ffffff&fontAlignY=35&desc=Arduino%20Nano%20%7C%2031%20Animations%20%7C%20Custom%20PCB&descAlignY=55&descSize=20&animation=fadeIn" width="100%"/>

<br/>

<!-- Badges Row 1 -->
![Arduino](https://img.shields.io/badge/Arduino-Nano-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-ATmega328P-blue?style=for-the-badge&logo=cplusplus&logoColor=white)
![PCB](https://img.shields.io/badge/PCB-KiCad-314CB0?style=for-the-badge&logo=kicad&logoColor=white)
![LEDs](https://img.shields.io/badge/LEDs-64%20Total-FFD700?style=for-the-badge&logo=lightbulb&logoColor=black)

<!-- Badges Row 2 -->
![Animations](https://img.shields.io/badge/Animations-31-blueviolet?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![Repo](https://img.shields.io/badge/Repo-IOT-orange?style=for-the-badge&logo=github)
![Status](https://img.shields.io/badge/Status-Complete-brightgreen?style=for-the-badge)

<br/>

> ### ✨ *A fully hand-soldered 4×4×4 LED cube with 64 LEDs, 31 unique animations, multiplexed layer control via BC547 transistors, and a professionally designed custom PCB — all driven by a single Arduino Nano.*

<br/>

</div>

---

## 📸 Project Gallery

<div align="center">

### 🔌 Circuit Diagram
> Complete wiring — Arduino Nano → Resistors → BC547 Transistors → LED Layers

<img src="images/circuit_diagram.png" width="80%" alt="Circuit Diagram"/>

<br/><br/>

### 🟢 PCB 3D Render (Top View)
> Designed in KiCad — 16 column resistors (330Ω) + 4 transistor base resistors (1KΩ) + 4× BC547

<img src="images/pcb_3d_top.png" width="60%" alt="PCB 3D Top View"/>

<br/><br/>

### 📐 PCB Copper Layout (Front & Back)
> F.Cu and B.Cu layers — traces routed for all 16 column lines and 4 layer control lines

<img src="images/pcb_layout.png" width="60%" alt="PCB Copper Layout"/>

<br/><br/>

### 🖥️ KiCad 3D Viewer — Full Board
> Full component placement preview with Arduino Nano socket, resistors, and transistors

<img src="images/pcb_3d_kicad.png" width="80%" alt="KiCad 3D Full Board"/>

<br/><br/>

### 🔁 PCB Back Copper Layer
> Bottom copper traces for layer transistor connections and GND routing

<img src="images/pcb_back.png" width="60%" alt="PCB Back Layer"/>

</div>

---

## 📁 Repository Structure

```
IOT/
└── 4x4x4-LED-Cube/
    ├── 📄 README.md
    ├── 💾 4x4x4_led_cube_project.ino
    └── 🖼️ images/
        ├── circuit_diagram.png
        ├── pcb_3d_top.png
        ├── pcb_layout.png
        ├── pcb_3d_kicad.png
        └── pcb_back.png
```

---

## ⚡ Features at a Glance

<div align="center">

| Feature | Detail |
|:-------:|:------:|
| 🧠 **Microcontroller** | Arduino Nano (ATmega328P) |
| 💡 **LED Count** | 64 (4 layers × 16 columns) |
| 🎬 **Animations** | 31 unique effects |
| 🔁 **Multiplexing** | Layer-by-layer (persistence of vision) |
| 🔌 **Column Driver** | Direct GPIO (330Ω series resistors) |
| 🔋 **Layer Driver** | BC547 NPN transistor (1KΩ base resistor) |
| 🛠️ **PCB Tool** | KiCad 7 |
| 📦 **Power** | USB via Arduino Nano |

</div>

---

## 🧰 Bill of Materials (BOM)

<div align="center">

| # | Component | Value | Qty | Purpose |
|---|-----------|-------|-----|---------|
| 1 | Arduino Nano | ATmega328P | 1 | Main controller |
| 2 | LEDs | 5mm (any color) | 64 | Cube matrix |
| 3 | Resistor | 330Ω | 16 | LED current limiting |
| 4 | Resistor | 1KΩ | 4 | Transistor base limiting |
| 5 | Transistor | BC547 (NPN) | 4 | Layer switching |
| 6 | Custom PCB | KiCad design | 1 | Neat wiring base |

</div>

---

## 📌 Complete Pin Mapping

### 🔵 Column Pins — 16 LED Columns (Direct GPIO)

<div align="center">

| Arduino Pin | Column Index | Arduino Pin | Column Index |
|:-----------:|:------------:|:-----------:|:------------:|
| D0 | C0 | D7 | C7 |
| D1 | C1 | D8 | C8 |
| D2 | C2 | D9 | C9 |
| D3 | C3 | D10 | C10 |
| D4 | C4 | D11 | C11 |
| D5 | C5 | D12 | C12 |
| D6 | C6 | D13 | C13 |
| A0 | C14 | A1 | C15 |

</div>

### 🟠 Layer Pins — 4 Layers (via BC547 Transistors)

<div align="center">

| Arduino Pin | Layer | Position |
|:-----------:|:-----:|:--------:|
| A2 | Layer 1 | Top |
| A3 | Layer 2 | Upper Middle |
| A4 | Layer 3 | Lower Middle |
| A5 | Layer 4 | Bottom |

</div>

> 💡 **Note:** Layer pins go `LOW` to activate the transistor base, pulling the layer cathode to GND.

---

## 🎮 All 31 Animations

<div align="center">

| # | Animation | # | Animation | # | Animation |
|---|-----------|---|-----------|---|-----------|
| 01 | 🌀 Spiral Rotation | 12 | 🧹 Layer Wipe | 23 | 〰️ Edge Flow |
| 02 | 🌪️ Vortex Effect | 13 | 🔄 Cube Rotation | 24 | ↗️ Diagonal Wipe |
| 03 | ✝️ Cross Rotation | 14 | 🌟 Border Glow | 25 | 📊 Column Scan |
| 04 | 🔃 Layer Spin | 15 | 🐍 Layer Snake | 26 | 🕳️ Tunnel Effect |
| 05 | ↙️ Diagonal 3D Rotation | 16 | ⚡ Zigzag Wave | 27 | 💥 Flashing Borders |
| 06 | 🌊 Wave Animation | 17 | 🔲 Expanding Square | 28 | 🌊 Layer Cascade |
| 07 | 🌧️ Rain Effect | 18 | 🌀 Spiral Motion | 29 | ⭕ Circular Wave |
| 08 | ⬛ Expand Contract | 19 | ♟️ Checkerboard Blink | 30 | 🔘 Center Spiral Rotation |
| 09 | ↘️ Diagonal Move | 20 | 📶 Vertical Wave | 31 | 💫 Expanding Rings |
| 10 | ✨ Sparkle Effect | 21 | 🔛 Corner Rotation | | |
| 11 | 💡 Random Blink | 22 | 🔁 Layer Rotation | | |

</div>

---

## ⚙️ How It Works

```
┌─────────────────────────────────────────────────────────┐
│                   MULTIPLEXING EXPLAINED                │
│                                                         │
│  Only 1 layer active at a time (HIGH speed cycling)     │
│                                                         │
│  Arduino Nano                                           │
│   │                                                     │
│   ├── D0–D13, A0, A1 ──► 330Ω ──► LED Columns (1–16)    │
│   │                                                     │
│   ├── A2 ──► 1KΩ ──► BC547 Base ──► Layer 1 Cathode     │
│   ├── A3 ──► 1KΩ ──► BC547 Base ──► Layer 2 Cathode     │
│   ├── A4 ──► 1KΩ ──► BC547 Base ──► Layer 3 Cathode     │
│   └── A5 ──► 1KΩ ──► BC547 Base ──► Layer 4 Cathode     │
│                                                         │
│  Loop: Activate Layer → Light Columns → Deactivate      │
│   (happens so fast the eye sees all layers at once)     │
└─────────────────────────────────────────────────────────┘
```

---

## 🚀 Getting Started

### Step 1 — Clone the Repo
```bash
git clone https://github.com/YourUsername/IOT.git
cd IOT/4x4x4-LED-Cube
```

### Step 2 — Open in Arduino IDE
```
File → Open → 4x4x4_led_cube_project.ino
```

### Step 3 — Configure Board Settings

| Setting | Value |
|---------|-------|
| Board | Arduino Nano |
| Processor | ATmega328P |
| Upload Speed | 115200 |
| Port | Your COM Port |

### Step 4 — Upload
```
Click the → Upload button  (or Ctrl + U)
```

---

## 🛠️ PCB Design Details

<div align="center">

| Layer | Purpose |
|-------|---------|
| **F.Cu (Front Copper)** | Column resistor connections + Arduino socket |
| **B.Cu (Back Copper)** | Transistor base + GND routing |
| **F.Silkscreen** | Component labels (R1–R20, Q1–Q4) |
| **Edge.Cuts** | Board outline |

</div>

- Designed entirely in **KiCad 7**
- All 16 column resistors on the top row
- 4 BC547 transistors at the bottom with their base resistors
- Arduino Nano centered with full pin socket

---

## 🐛 Troubleshooting

| Problem | Likely Cause | Fix |
|---------|-------------|-----|
| `stray '\302'` compile error | Non-breaking spaces in code | Use the `.ino` file from this repo directly |
| LEDs not lighting | Layer transistor not triggering | Check BC547 orientation (CBE) |
| Flickering | Delay too low | Increase `delay()` values in animations |
| Only 1 layer works | Wrong layer pin | Verify A2–A5 mapping |
| Upload fails | Wrong board selected | Set board to **Arduino Nano + ATmega328P** |

---

## 👤 Author

<div align="center">

**Hari Prasad L S**
B.E. Electrical & Electronics Engineering
SIMATS Saveetha School of Engineering, Chennai 🇮🇳

[![GitHub](https://img.shields.io/badge/GitHub-Follow-black?style=for-the-badge&logo=github)](https://github.com/YourUsername)
[![LinkedIn](https://img.shields.io/badge/LinkedIn-Connect-0077B5?style=for-the-badge&logo=linkedin)](https://linkedin.com/in/YourProfile)
[![LeetCode](https://img.shields.io/badge/LeetCode-120%2B_Problems-FFA116?style=for-the-badge&logo=leetcode&logoColor=black)](https://leetcode.com/YourProfile)

</div>

---

## 📄 License

```
MIT License — Free to use, modify, and distribute.
Give credit if you build something cool with it! 🙌
```

---

<div align="center">

<img src="https://capsule-render.vercel.app/api?type=waving&color=0:0066ff,100:00d4ff&height=100&section=footer" width="100%"/>

⭐ **Star this repo if you found it useful!**
Made with ❤️ and a soldering iron 🔧

</div>
