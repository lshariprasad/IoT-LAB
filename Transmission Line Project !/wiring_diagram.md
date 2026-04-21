# 🔌 Wiring Diagram

## Full Connection Table

| Arduino Pin | Wire Color (suggested) | Component | Component Pin |
|-------------|----------------------|-----------|---------------|
| D2 | Yellow | Push Button | Terminal A |
| GND | Black | Push Button | Terminal B |
| D6 | Green | Green LED | Anode (+) |
| GND | Black | 220Ω Resistor → GND | (series with green LED) |
| D7 | Red | Red LED | Anode (+) |
| GND | Black | 220Ω Resistor → GND | (series with red LED) |
| D8 | Orange | Active Buzzer | (+) positive |
| GND | Black | Active Buzzer | (−) negative |
| A4 (SDA) | Blue | OLED SSD1306 | SDA |
| A5 (SCL) | Purple | OLED SSD1306 | SCL |
| 5V | Red | OLED SSD1306 | VCC |
| GND | Black | OLED SSD1306 | GND |

---

## ASCII Breadboard Layout

```
                    Arduino Uno
                 ┌─────────────┐
           RESET─┤             ├─5V  ──── OLED VCC
              3V─┤             ├─GND ──── OLED GND / LED GND / BTN GND
              5V─┤             ├─A5  ──── OLED SCL
             GND─┤             ├─A4  ──── OLED SDA
              D2─┤  [USB]      ├─
              D6─┤             ├─
              D7─┤             ├─
              D8─┤             ├─
                 └─────────────┘
                       │
           ┌───────────┼──────────────────────────┐
           │           │                          │
       D2──┤BTN├──GND  D6──[220Ω]──┤G-LED├──GND  │
                       D7──[220Ω]──┤R-LED├──GND  │
                       D8──┤BUZ(+)├──BUZ(-)──GND │
                                                  │
```

---

## OLED I2C Address

The default I2C address is `0x3C`. Some modules use `0x3D`.

To scan for your OLED address, upload the I2C scanner sketch:

```cpp
#include <Wire.h>
void setup() {
  Wire.begin();
  Serial.begin(9600);
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print("Found device at 0x");
      Serial.println(addr, HEX);
    }
  }
}
void loop() {}
```

If your address is `0x3D`, change this line in the sketch:
```cpp
#define OLED_ADDRESS  0x3D
```

---

## Notes

- Use **active buzzer** (not passive) for the built-in `tone()` calls to work correctly.
- LEDs **must** have series resistors (220Ω recommended) to prevent burning out.
- The button uses `INPUT_PULLUP` — no external pull-up resistor needed.
- OLED runs on 3.3V logic but is 5V tolerant on most breakout boards.
