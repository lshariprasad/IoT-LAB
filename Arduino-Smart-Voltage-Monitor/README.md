# Arduino Smart Voltage Monitoring and Automatic Fan Control System

An Arduino UNO-based electronics project that simulates voltage and current readings, displays system information on an OLED screen, automatically controls a 5V cooling fan, and generates buzzer alerts during low-voltage conditions.

## Project Overview

This project demonstrates a simple voltage monitoring and automatic cooling system using an Arduino UNO, an I2C OLED display, a voltage sensor, a current sensor, a buzzer, and a 5V DC fan.

The current version uses **simulated voltage and current values**, so the project can demonstrate its working logic without a battery or actual sensor measurements.

## Features

- OLED display for voltage, current, and fan status.
- Simulated voltage range from 1.5V to 5.0V.
- Automatic fan activation at 3.0V and above.
- Fan switched OFF below 3.0V.
- Three buzzer beeps during startup.
- Repeated buzzer alerts during low-voltage conditions.
- Simulated current readings for demonstration.
- Serial Monitor output for debugging.
- Battery-free demonstration using USB-powered Arduino.

## Components Required

| Component | Quantity |
|---|---:|
| Arduino UNO | 1 |
| I2C OLED display, SSD1306 compatible | 1 |
| Voltage sensor module | 1 |
| Current sensor module | 1 |
| Buzzer | 1 |
| 5V DC fan | 1 |
| NPN transistor or suitable logic-level MOSFET | 1 |
| 1kΩ resistor for transistor base | 1 |
| Flyback diode suitable for the fan | 1 |
| Jumper wires | As required |
| Breadboard | 1 |
| USB cable | 1 |

**Note:** A transistor or MOSFET driver is required to control the fan safely. Use an appropriately rated 5V supply for the fan if needed, with a common ground.

## Pin Connections

| Component | Pin | Arduino UNO |
|---|---|---|
| OLED | VCC | Suitable rated supply |
| OLED | GND | GND |
| OLED | SDA | A4 |
| OLED | SCL | A5 |
| Voltage sensor | OUT | A0 |
| Current sensor | OUT | A1 |
| Buzzer | Signal / + | D8 |
| Buzzer | Negative / − | GND |
| Fan driver | Control input | D9 |

Connect sensor grounds to Arduino GND. In the simulation version, the sensor outputs are not used to calculate readings; A0 and A1 are reserved for future real-sensor integration.

### Fan Driver Wiring

For an NPN transistor driver:

1. Connect Arduino D9 to a 1kΩ resistor and then to the transistor base.
2. Connect the transistor emitter to GND.
3. Connect the fan negative terminal to the transistor collector.
4. Connect the fan positive terminal to regulated +5V.
5. Connect the fan supply ground to Arduino GND.
6. Connect a suitable flyback diode across the fan, with its cathode at +5V and anode at the transistor collector/fan-negative node.

Verify the transistor pinout using its datasheet.

## Operating Logic

| Simulated voltage | Fan | Buzzer |
|---|---|---|
| 2.0V to below 3.0V | OFF | Warning beeps |
| 3.0V to 5.0V | ON | OFF |
| Startup | OFF initially | Three short beeps |

The simulated voltage rises and falls automatically, allowing both operating conditions to be demonstrated.

## Software Requirements

- Arduino IDE 1.8.x or later
- Adafruit SSD1306 library
- Adafruit GFX Library
- Arduino UNO board package

Install the libraries through **Arduino IDE → Sketch → Include Library → Manage Libraries**.

## How to Run

1. Connect the OLED, buzzer, and fan driver according to the pin table.
2. Open `Arduino-Smart-Voltage-Monitor.ino` in Arduino IDE.
3. Install the required display libraries.
4. Select **Tools → Board → Arduino UNO**.
5. Select the correct COM port.
6. Upload the sketch.
7. Open Serial Monitor at **9600 baud**.
8. Observe the simulated voltage, current, fan state, and buzzer alerts.

The OLED is configured for an SSD1306 128×64 display at I2C address `0x3C`. Some modules use `0x3D`.

## Simulation Details

The sketch generates fake voltage values between 1.5V and 5.0V. It generates illustrative current readings depending on the simulated fan state.

These values are **not actual sensor measurements**. The voltage and current sensor modules are included in the hardware plan but are not read by the current simulation code.

The minimum-voltage constant is 2.0V, and the fan switching threshold is 3.0V. The buzzer warns whenever simulated voltage is below the fan switching threshold.

## Future Improvements

- Read real voltage sensor measurements.
- Integrate calibrated current sensing.
- Add configurable voltage thresholds.
- Implement hysteresis to reduce rapid switching near the threshold.
- Add fan speed control using PWM.
- Store voltage and current history.
- Add battery monitoring and protection where appropriate.

## Applications

- Arduino learning and demonstrations
- Basic voltage monitoring experiments
- Automatic cooling control demonstrations
- Embedded systems mini-projects
- Electronics laboratory projects

## License

This project is available under the MIT License. Add a `LICENSE` file containing the standard MIT License text and your copyright year and name if you choose to distribute it under that license.

## Author

Created as an Arduino-based electronics learning project.

Contributions, suggestions, and improvements are welcome!
