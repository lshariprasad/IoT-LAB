<div align="center">

<h1>🤖 Human Following Robot</h1>

<p>
  <img src="https://img.shields.io/badge/Platform-Arduino%20UNO-00979D?style=for-the-badge&logo=arduino&logoColor=white"/>
  <img src="https://img.shields.io/badge/Language-C%2B%2B-00599C?style=for-the-badge&logo=cplusplus&logoColor=white"/>
  <img src="https://img.shields.io/badge/Domain-Robotics%20%26%20Embedded-blueviolet?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/License-MIT-green?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/Status-Active-brightgreen?style=for-the-badge"/>
</p>

<p>
  An autonomous human-following robot built on Arduino UNO, combining IR-based lateral tracking, ultrasonic distance sensing, and a servo-mounted scanning head. The robot detects and follows a human target in real time using a simple, reliable sensor fusion approach — no Bluetooth, no camera, no app required.
</p>

---

</div>

## 📑 Table of Contents

- [Overview](#-overview)
- [System Architecture](#-system-architecture)
- [Hardware Components](#-hardware-components)
- [Circuit & Pin Mapping](#-circuit--pin-mapping)
- [Power Architecture](#-power-architecture)
- [Robot Decision Logic](#-robot-decision-logic)
- [Working Principle](#-working-principle)
- [Code Structure](#-code-structure)
- [Setup & Upload](#-setup--upload)
- [Calibration Guide](#-calibration-guide)
- [Troubleshooting](#-troubleshooting)
- [Known Limitations](#-known-limitations)
- [Roadmap](#-roadmap)
- [Contributing](#-contributing)
- [License](#-license)

---

## 🔍 Overview

This project implements a differential-drive robot that autonomously tracks and follows a human or object using:

- **IR Sensors** — detect the presence of a target on the left/right flanks
- **HC-SR04 Ultrasonic Sensor** — measures frontal distance to maintain a safe following gap
- **SG90 Servo Motor** — pans the ultrasonic sensor head to indicate tracking direction
- **L298N Motor Driver** — controls two pairs of DC gear motors for forward, left, right, and stop motion

The robot operates fully standalone — no wireless connectivity, no external computer, no app needed. It is ideal as a robotics lab demonstration, college mini-project, or foundation for more advanced autonomous systems.

---

## 🏗 System Architecture

```
┌─────────────────────────────────────────────────────────┐
│                     Arduino UNO                          │
│                                                          │
│  D2 ◄── Left IR        D9 ──► Ultrasonic TRIG           │
│  D3 ◄── Right IR       D10 ◄── Ultrasonic ECHO          │
│  D6 ──► Servo Motor                                     │
│  D4/D5 ──► L298N (Left Motor Pair)                      │
│  D7/D8 ──► L298N (Right Motor Pair)                     │
└──────────────────────────┬──────────────────────────────┘
                           │
              ┌────────────▼────────────┐
              │      Sensor Fusion       │
              │  IR Left | IR Right      │
              │  + Ultrasonic Distance   │
              └────────────┬────────────┘
                           │
              ┌────────────▼────────────┐
              │    Decision Engine       │
              │  Forward / Left / Right  │
              │         / Stop           │
              └────────────┬────────────┘
                           │
       ┌───────────────────┼───────────────────┐
       ▼                   ▼                   ▼
  L298N Driver         Servo Head         Serial Monitor
  (4 DC Motors)       (Direction Pan)      (Debug 9600)
```
---

## 🛠 Hardware Components

| # | Component | Specification | Quantity |
|---|-----------|--------------|----------|
| 1 | Arduino UNO | ATmega328P, 16 MHz | 1 |
| 2 | L298N Motor Driver | Dual H-Bridge, 2A per channel | 1 |
| 3 | DC Gear Motor | Yellow TT Motor, 3–6V | 4 |
| 4 | IR Obstacle Sensor | Digital output, adjustable sensitivity | 2 |
| 5 | HC-SR04 Ultrasonic Sensor | Range: 2cm–400cm, ±3mm accuracy | 1 |
| 6 | SG90 Servo Motor | 180° rotation, 4.8–6V | 1 |
| 7 | Robot Chassis | 4WD acrylic/metal chassis | 1 |
| 8 | Lithium-ion Battery Pack | 7.4V or 11.1V for motors | 1 |
| 9 | 9V Battery / Power Bank | Arduino UNO power | 1 |
| 10 | Jumper Wires | Male-to-Male, Male-to-Female | As required |
| 11 | Breadboard | 400/830 tie-points | 1 |

---

## 🔌 Circuit & Pin Mapping

========================================
🤖 HUMAN FOLLOWING ROBOT PIN DIAGRAM
========================================


                +-------------------+
                |   Arduino UNO     |
                |                   |
                | D2  <-- Left IR   |
                | D3  <-- Right IR  |
                | D4  --> L298N IN1 |
                | D5  --> L298N IN2 |
                | D6  --> Servo     |
                | D7  --> L298N IN3 |
                | D8  --> L298N IN4 |
                | D9  --> TRIG      |
                | D10 <-- ECHO      |
                | 5V  --> Sensors   |
                | GND --> Common GND|
                +-------------------+



========================================
📡 ULTRASONIC SENSOR (HC-SR04)
========================================

        HC-SR04              Arduino UNO
      -----------------------------------
        VCC      --------->     5V
        GND      --------->     GND
        TRIG     --------->     D9
        ECHO     --------->     D10



========================================
📍 IR SENSOR CONNECTIONS
========================================

        LEFT IR SENSOR       Arduino UNO
      -----------------------------------
        VCC      --------->     5V
        GND      --------->     GND
        OUT      --------->     D2


        RIGHT IR SENSOR      Arduino UNO
      -----------------------------------
        VCC      --------->     5V
        GND      --------->     GND
        OUT      --------->     D3



========================================
⚙️ SERVO MOTOR CONNECTION
========================================

        SERVO MOTOR          Arduino UNO
      -----------------------------------
        Red       --------->     5V
        Brown     --------->     GND
        Orange    --------->     D6



========================================
🚗 L298N MOTOR DRIVER CONNECTION
========================================

        L298N                Arduino UNO
      -----------------------------------
        IN1      --------->     D4
        IN2      --------->     D5
        IN3      --------->     D7
        IN4      --------->     D8
        GND      --------->     GND



========================================
🔋 BATTERY CONNECTION
========================================

        BATTERY              L298N
      -----------------------------------
        Positive  --------->   12V
        Negative  --------->   GND



========================================
🛞 MOTOR CONNECTIONS
========================================

        LEFT MOTORS          L298N
      -----------------------------------
        Motor Pair --------> OUT1 & OUT2


        RIGHT MOTORS         L298N
      -----------------------------------
        Motor Pair --------> OUT3 & OUT4



========================================
⚠️ IMPORTANT NOTES
========================================

✅ Connect ALL GNDs together
✅ Disconnect sensors during upload if needed
✅ Do NOT use Arduino pins 0 and 1
✅ Servo may require separate power
✅ Use lithium batteries for motors


### Arduino UNO ↔ All Components

| Pin | Mode | Connected To | Notes |
|-----|------|-------------|-------|
| D2 | INPUT | Left IR Sensor OUT | LOW when object detected |
| D3 | INPUT | Right IR Sensor OUT | LOW when object detected |
| D4 | OUTPUT | L298N IN1 | Left motor pair control |
| D5 | OUTPUT | L298N IN2 | Left motor pair control |
| D6 | OUTPUT | SG90 Servo Signal | PWM for pan control |
| D7 | OUTPUT | L298N IN3 | Right motor pair control |
| D8 | OUTPUT | L298N IN4 | Right motor pair control |
| D9 | OUTPUT | HC-SR04 TRIG | Ultrasonic trigger pulse |
| D10 | INPUT | HC-SR04 ECHO | Ultrasonic echo return |
| 5V | POWER | IR Sensors VCC, HC-SR04 VCC, Servo VCC | Regulated 5V rail |
| GND | GROUND | All component GNDs | Common ground — critical! |

> ⚠️ **Important:** All GND pins (Arduino, L298N, batteries) must share a common ground rail. Failing to do so will cause erratic sensor behavior.

---

## ⚡ Power Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    Power Distribution                   │
│                                                         │
│  Lithium Pack (7.4V–12V)                                │
│       │                                                 │
│       ├──► L298N 12V IN ──► DC Motors (via H-Bridge)    │
│       │         │                                       │
│       │    L298N 5V OUT ──► (Optional: Arduino Vin)     │
│       │                                                 │
│  9V Battery / USB Power Bank                            │
│       │                                                 │
│       └──► Arduino UNO 5V/Vin                           │
│                   │                                     │
│               Arduino 5V Rail                           │
│                   │                                     │
│       ├──► IR Sensor VCC (×2)                           │
│       ├──► HC-SR04 VCC                                  │
│       └──► SG90 Servo VCC                               │
│                                                         │
│  ⚠ Common GND must tie ALL components together          │
└─────────────────────────────────────────────────────────┘
```

**Recommendations:**
- Use a dedicated power bank or 9V battery for the Arduino to avoid voltage dips from motor load
- If servo stutters, add a 100µF electrolytic capacitor across its VCC and GND pins
- For high-torque motors, use a 3S LiPo (11.1V) with the L298N's onboard 5V regulator feeding the Arduino

---

## 🧠 Robot Decision Logic

The robot evaluates two sensor inputs every loop cycle and chooses one of four actions:

```
┌─────────────────┬──────────────────┬───────────────────────────────────────┐
│  Left IR Value  │  Right IR Value  │  Action                               │
├─────────────────┼──────────────────┼───────────────────────────────────────┤
│  LOW (0)        │  HIGH (1)        │  Servo → 150° → Turn Left             │
│  HIGH (1)       │  LOW (0)         │  Servo → 30°  → Turn Right            │
│  LOW (0)        │  LOW (0)         │  Servo → 90°  → Move Forward          │
│  HIGH (1)       │  HIGH (1)        │  Servo → 90°  → Stop                  │
├─────────────────┴──────────────────┼───────────────────────────────────────┤
│  Ultrasonic Distance ≥ 50cm OR 0   │  Servo → 90°  → Stop (out of range)   │
└────────────────────────────────────┴───────────────────────────────────────┘
```

**Distance Gating:** The ultrasonic sensor acts as a range gate. If no object is within 0–50 cm frontally, the robot stops regardless of IR state — preventing the robot from chasing phantom reflections.

---

## ⚙️ Working Principle

### 1. Ultrasonic Distance Measurement
The HC-SR04 sends a 10µs pulse on the TRIG pin. The sensor emits 8 ultrasonic bursts at 40 kHz and listens for the echo. The ECHO pin stays HIGH for a duration proportional to the round-trip travel time of the sound wave.

```
Distance (cm) = (Echo Pulse Duration in µs × 0.034) / 2
```

The division by 2 accounts for the two-way travel (emitter → object → receiver).

### 2. IR Sensor Lateral Detection
IR sensors emit infrared light and measure the reflected intensity. When an object is within range, the reflected IR is sufficient to pull the digital output pin LOW. Sensitivity is tunable via the onboard potentiometer.

### 3. Servo Pan Indication
The servo motor rotates the sensor head to visually indicate the tracked direction:
- **90°** → Forward / Stop (center)
- **150°** → Left detected
- **30°** → Right detected

This also physically orients the ultrasonic sensor toward the target for better frontal accuracy in future iterations.

### 4. Differential Drive
The L298N controls two motor pairs:
- **Forward:** Both pairs spin forward
- **Turn Left:** Left pair reverses, right pair forward
- **Turn Right:** Right pair reverses, left pair forward
- **Stop:** All motor outputs LOW

---

## 📁 Code Structure

```
Arduino-Human-Following-Robot/
│
├── human_following_robot.ino     # Main Arduino sketch
│
├── README.md                     # Project documentation
│
├── circuit/
│   └── wiring_diagram.png        # Circuit diagram (add yours here)
│
├── images/
│   └── robot_photo.jpg           # Physical robot photo
│
└── docs/
    └── component_datasheet/      # Optional: HC-SR04, SG90, L298N datasheets
```

### Key Functions

| Function | Description |
|----------|-------------|
| `setup()` | Initializes all pins, attaches servo, centers at 90° |
| `loop()` | Main cycle: reads sensors → fuses logic → drives motors |
| `moveForward()` | Sets both motor pairs to forward direction |
| `turnLeft()` | Reverses left pair, forwards right pair |
| `turnRight()` | Forwards left pair, reverses right pair |
| `stopRobot()` | Cuts all motor outputs to LOW |

---

## 🚀 Setup & Upload

### Prerequisites
- [Arduino IDE 2.x](https://www.arduino.cc/en/software) or Arduino IDE 1.8.x
- `Servo.h` library — included by default in Arduino IDE

### Steps

```bash
# 1. Clone this repository
git clone https://github.com/YOUR_USERNAME/Arduino-Human-Following-Robot.git

# 2. Open the sketch
#    File → Open → human_following_robot.ino

# 3. Select your board
#    Tools → Board → Arduino UNO

# 4. Select the correct port
#    Tools → Port → COMx (Windows) or /dev/ttyUSB0 (Linux/Mac)

# 5. Upload
#    Click the Upload button (→) or press Ctrl+U
```

> ⚠️ **Note:** Do not connect anything to Arduino pins **D0 (RX)** and **D1 (TX)** during upload — they are used by the serial bootloader.

---

## 🔧 Calibration Guide

### IR Sensor Sensitivity
Each IR sensor has a small blue potentiometer on the PCB:
- **Clockwise** → increases detection range
- **Counter-clockwise** → decreases detection range

Calibration steps:
1. Power the robot
2. Open the Serial Monitor at 9600 baud
3. Place your hand at the desired follow distance (≈20–30 cm side)
4. Adjust the potentiometer until the LED on the sensor turns ON

### Ultrasonic Distance Threshold
The default follow distance threshold is **50 cm**. To change it, modify this line in `loop()`:

```cpp
if (distance > 0 && distance < 50) {  // Change 50 to your preferred cm value
```

### Servo Angle Tuning
If your servo's center or extreme angles feel off, adjust these values:

```cpp
myservo.write(90);   // Center — increase/decrease for straight alignment
myservo.write(150);  // Left pan — reduce if over-rotating
myservo.write(30);   // Right pan — increase if under-rotating
```

---

## 🐛 Troubleshooting

| Symptom | Likely Cause | Fix |
|---------|-------------|-----|
| Robot doesn't move at all | Motor power not connected | Check L298N battery connection and GND |
| Robot spins in circles | Motor wires reversed on one side | Swap the two wires on the affected motor |
| IR sensor always triggered | Too close to wall / sensitivity too high | Reduce sensitivity via potentiometer |
| Ultrasonic reads 0 constantly | TRIG/ECHO wires swapped | Verify D9→TRIG, D10→ECHO |
| Servo jitters continuously | Insufficient current on 5V rail | Add 100µF cap across Servo VCC-GND |
| Robot follows too close / too far | Distance threshold off | Tune the `distance < 50` value in code |
| Robot unresponsive after upload | Serial port conflict | Disconnect D0/D1 wires before upload |
| Motors turn backward | IN1/IN2 or IN3/IN4 logic flipped | Swap the corresponding motor wire pair |

---

## ⚠️ Known Limitations

- IR sensors can be triggered by reflective surfaces, bright sunlight, or high-contrast clothing
- The HC-SR04 has a narrow beam angle (~15°), so the robot may lose tracking if the human moves sharply to the side
- No obstacle avoidance — the robot will collide with walls or furniture between itself and the target
- A 200ms loop delay limits responsiveness at high speeds
- No speed control (PWM on ENA/ENB) — motors run at full voltage

---

## 🗺 Roadmap

| Phase | Feature | Status |
|-------|---------|--------|
| v1.0 | IR + Ultrasonic following with servo pan | ✅ Complete |
| v1.1 | PWM speed control (ENA/ENB) | 🔄 Planned |
| v1.2 | Obstacle avoidance (side ultrasonic) | 🔄 Planned |
| v2.0 | OpenCV / Pi Camera vision tracking | 🔜 Future |
| v2.1 | Bluetooth/WiFi override control | 🔜 Future |
| v2.2 | Mobile app (MIT App Inventor) | 🔜 Future |
| v3.0 | ROS integration + SLAM navigation | 💡 Concept |

---

## 🤝 Contributing

Contributions are welcome! Here's how to get started:

1. **Fork** this repository
2. **Create** a feature branch: `git checkout -b feature/obstacle-avoidance`
3. **Commit** your changes: `git commit -m "Add obstacle avoidance logic"`
4. **Push** to the branch: `git push origin feature/obstacle-avoidance`
5. **Open a Pull Request**

Please ensure your code is commented, tested on hardware, and documented in the PR description.

---

## 👨‍💻 Author

**Hari Prasad L S**  
B.E. Electrical and Electronics Engineering  
SIMATS Saveetha School of Engineering, Chennai  

[![LinkedIn](https://img.shields.io/badge/LinkedIn-Connect-blue?style=flat-square&logo=linkedin)](https://linkedin.com/in/YOUR_PROFILE)
[![GitHub](https://img.shields.io/badge/GitHub-Follow-black?style=flat-square&logo=github)](https://github.com/YOUR_USERNAME)
[![LeetCode](https://img.shields.io/badge/LeetCode-Profile-orange?style=flat-square&logo=leetcode)](https://leetcode.com/YOUR_PROFILE)

---

## 📄 License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.

```
MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files, to deal in the Software
without restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software.
```

---

<div align="center">

**⭐ If this project helped you, please star the repository!**

*Built with ❤️ using Arduino*

</div>
