# Advanced Motor Protection System v2.0

A complete protection system for single-phase motors using an Arduino Uno/Nano.  
Monitors temperature, humidity, motor current, and mechanical vibration to prevent damage.

## Features

- Over‑current, over‑temperature, over‑humidity protection
- Vibration‑based emergency stop (debounced)
- Hysteresis on all thresholds – eliminates relay chatter
- Fault log – last 5 faults with millisecond timestamps
- Multi‑screen LCD (16x2 I2C) – Status / Sensors / Fault history
- Running‑hours tracker – saved to EEPROM every 60 seconds
- Soft‑start delay after fault clearance
- Status LED with blink patterns per fault type
- Buzzer with different alarm patterns
- Serial command interface – change thresholds, reset logs, view status
- Sensor offline / NaN detection

## Required Components

| Component            | Quantity | Typical Model          |
|----------------------|----------|------------------------|
| Arduino Uno / Nano   | 1        | –                      |
| DHT11 or DHT22       | 1        | Temperature & humidity |
| ACS712               | 1        | 5A version             |
| Vibration sensor     | 1        | SW‑420 (digital)       |
| Relay module         | 1        | 5V, 1 or 2 channel     |
| Buzzer (passive)     | 1        | 5V                     |
| LED + 220Ω resistor  | 1        | Status indicator       |
| I2C LCD 16x2         | 1        | Address 0x27 or 0x3F   |
| 5V power supply      | 1        | For Arduino + relay    |

## Pin Mapping

| Arduino Pin | Component            | Remarks                    |
|-------------|----------------------|----------------------------|
| 2           | DHT11 data           |                            |
| 3           | SW‑420 vibration     | Interrupt capable          |
| 4           | Relay control        | Active HIGH (motor ON)     |
| 5           | Buzzer               |                            |
| 6           | Status LED           |                            |
| A0          | ACS712 output        | Current sensor             |
| A4 (SDA)    | LCD I2C SDA          |                            |
| A5 (SCL)    | LCD I2C SCL          |                            |

## Wiring Diagram (simplified)

Arduino 5V → DHT11 VCC, ACS712 VCC, Relay VCC, LCD VCC, Buzzer (+)
Arduino GND → all component grounds

ACS712 OUT → A0
DHT11 OUT → D2
SW‑420 OUT → D3
Relay IN → D4
Buzzer (+) → D5
LED anode → D6 (with 220Ω resistor)
LED cathode→ GND


## Installation

1. Install required libraries (Arduino Library Manager):
   - `LiquidCrystal I2C` by Frank de Brabander
   - `DHT sensor library` by Adafruit
   - `Wire` (built‑in)

2. Open `motor_protection_advanced.ino` in Arduino IDE.

3. **Set correct LCD I2C address** – run I2C scanner if unsure.  
   Default in code: `0x27`. If your LCD uses `0x3F`, change the line:
   ```cpp
   LiquidCrystal_I2C lcd(0x27, 16, 2);  // change to 0x3F if needed

   Adjust current sensor scaling (if using ACS712‑5A):

current = abs((voltage - 2.5) / 0.185);
For ACS712‑20A use 0.100, for ACS712‑30A use 0.066.

Upload the sketch to Arduino.

Configuration (Thresholds)
Edit these values in the Thresholds struct (near line 25):

Parameter	Default	Description
tempTrip	45.0 °C	Motor OFF above this temperature
tempHyst	2.0 °C	Restart when temp < (trip – hyst)
humTrip	80.0 %	Motor OFF above this humidity
humHyst	5.0 %	Restart when humidity < (trip – hyst)
currentTrip	4.0 A	Overcurrent trip level
currentHyst	0.3 A	Restart when current < (trip – hyst)
vibDebounce	200 ms	Ignore re‑triggers within this window
softStart	3000 ms	Delay before motor re‑starts after fault
Serial Commands
Open Serial Monitor (9600 baud). Commands are case‑insensitive:

Command	Action
STATUS	Print full system status
HELP	Show command list
RESET FAULTS	Clear fault log
RESET HOURS	Reset running‑hours counter to 0
SET TEMP 45	Set over‑temperature trip to 45 °C
SET HUM 75	Set over‑humidity trip to 75 %
SET CURR 3.5	Set over‑current trip to 3.5 A
Example usage:

SET TEMP 50
STATUS
Operation
Startup – LCD shows “MOTOR SHIELD v2.0”, then soft‑start for 3 seconds. Motor runs if no fault.

Normal running – LCD cycles every 3 seconds:

Screen 1: Motor state + running time

Screen 2: Temperature, humidity, current

Screen 3: Last fault code, timestamp and value

Fault condition – If any monitored value exceeds its trip point:

Relay opens (motor stops)

Buzzer sounds (pattern depends on fault type)

Status LED flashes (fast for overcurrent, slow for others)

Fault is logged

LCD shows “MTR:FAULT!”

Fault recovery – When values drop below (trip – hysteresis):

Soft‑start delay begins (3 seconds)

After delay, motor restarts automatically

(Vibration fault requires manual reset via serial command)

Vibration emergency stop – When SW‑420 detects shock, motor stops immediately and logs “Vibration” fault. To reset, use RESET FAULTS after vibration stops.

Running Hours & EEPROM
Running minutes are stored in EEPROM every 60 seconds.

Hours are displayed on LCD and via STATUS command.

To reset hours: RESET HOURS (and press reset on Arduino to restart timer).

Troubleshooting
Problem	Solution
LCD blank or no text	Check I2C address (run scanner). Adjust contrast potentiometer.
Motor never runs	Verify relay wiring (NO/COM). Test relay with simple blink sketch.
False vibration triggers	Increase vibDebounce (e.g., 300 ms).
Current reading inaccurate	Calibrate ACS712: measure voltage at A0 with no load, adjust formula.
Temperature/humidity = "ERR"	Check DHT11 wiring. Replace sensor if needed.
EEPROM not saving hours	Ensure saveRunningMinutes() is called (it is, every 60 sec).
Customising the LCD Address
If your LCD does not work with 0x27 or 0x3F, run this I2C scanner once:

```
#include <Wire.h>
void setup() {
  Wire.begin();
  Serial.begin(9600);
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print("Found at 0x");
      Serial.println(addr, HEX);
    }
  }
}
void loop() {}
Then replace the address in the main code.
```

License
This project is open‑source. Feel free to modify and distribute.

Enjoy your protected motor!
For questions, refer to the detailed comments inside the .ino file.
