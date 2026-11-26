# ESP32 RFID Attendance System

An ESP32-based RFID attendance demonstration. The ESP32 reads an RC522 card UID and sends it over the local Wi-Fi network to a Python Flask server, which maps known UIDs to student records and appends attendance entries to an Excel workbook.

## Architecture

```text
RFID card
   |
RC522 reader --SPI--> ESP32
                       |-- I2C --> 16x2 LCD
                       |-- GPIO 27 --> buzzer
                       |
                     Wi-Fi
                       |
                 Flask server (PC)
                       |
                 attendance.xlsx
```

## Hardware

- ESP32 development board
- MFRC522 RFID reader and compatible cards/tags
- 16x2 I2C LCD (configured at address `0x27`)
- Buzzer
- Jumper wires and common ground

## Wiring verified from the firmware

### RC522 to ESP32

| RC522 | ESP32 GPIO |
|---|---:|
| SDA / SS | 5 |
| SCK | 18 (default VSPI clock) |
| MOSI | 23 (default VSPI MOSI) |
| MISO | 19 (default VSPI MISO) |
| RST | 22 |
| 3.3V | 3.3V |
| GND | GND |

Power the RC522 from 3.3 V.

### LCD and buzzer

| Module | ESP32 |
|---|---|
| LCD SDA | GPIO 21 |
| LCD SCL | GPIO 22 |
| LCD VCC | 5V/VIN as supported by the specific LCD module |
| LCD GND | GND |
| Buzzer control | GPIO 27 |
| Buzzer GND | GND |

**Pin conflict to resolve before wiring:** the firmware currently assigns GPIO 22 to both the RC522 reset pin and the LCD SCL line. These cannot share the same signal. Change one assignment in the sketch and update the wiring table before building the circuit. This documentation intentionally flags the conflict rather than guessing which pin you used physically.

## Software

- Arduino IDE with ESP32 board support
- Arduino libraries: MFRC522 and LiquidCrystal_I2C
- Python 3
- Python packages: Flask and openpyxl

Install the Python packages:

```bash
python -m pip install flask openpyxl
```

## Configuration

### 1. Configure the ESP32 sketch

Open `attendance_in_esp32.ino` and set these local values:

```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* serverIP = "YOUR_PC_IP";
const int serverPort = 5000;
```

Use your 2.4 GHz Wi-Fi network and the PC's LAN IPv4 address. Keep real Wi-Fi credentials out of Git commits.

### 2. Configure student UID mapping

In `attendance_server.py`, update the `students` dictionary with the card UIDs and the correct student records for your own demonstration. Do not publish real student identifiers or personal data without permission.

### 3. Start the Flask server

From the `ESP32_Attendance` folder, run:

```bash
python attendance_server.py
```

The server listens on port 5000 on all network interfaces. Keep it on a trusted private network.

### 4. Upload the ESP32 sketch

- Select the appropriate ESP32 board and serial port in Arduino IDE.
- Upload `attendance_in_esp32.ino`.
- Open Serial Monitor at 115200 baud.
- Scan a registered card and check the server output and generated `attendance.xlsx` file.

## Firmware/server behavior

1. The ESP32 reads the UID from the RC522.
2. It converts the UID bytes to a hexadecimal string.
3. It sends an HTTP GET request to `/mark?uid=<UID>` on the Flask server.
4. The server looks up the UID in the local mapping.
5. A recognised UID is appended to `attendance.xlsx` with the server's current date and time.

Unknown UIDs return `UNKNOWN CARD`. The current ESP32 firmware does not parse the HTTP response body; its LCD displays the marked message after sending the request, so that display message alone does not prove the server saved a record.

## Important limitations and security

- The current Flask endpoint uses an unauthenticated HTTP GET request. Anyone who can reach the server may attempt to submit UIDs.
- The server binds to `0.0.0.0`; use a trusted LAN and do not expose port 5000 to the public internet.
- The current implementation has no duplicate-scan prevention on the server side.
- The server stores student names, departments, UIDs and attendance timestamps. Protect the workbook and do not commit real attendance records.
- If credentials were previously committed publicly, rotate them. Replacing them in the current file does not remove old values from Git history.

## Repository files

```text
ESP32_Attendance/
├── attendance_in_esp32.ino
├── attendance_server.py
└── README.md
```

The `attendance.xlsx` workbook is generated when the server runs; do not commit real attendance data.

## Author

Hari Prasad L S — [GitHub](https://github.com/lshariprasad)
