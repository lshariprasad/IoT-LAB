# ESP32 Smart Switch Dashboard

A Wi-Fi-connected ESP32 project that reads a physical switch and serves a browser dashboard showing its current state.

## Features verified in the firmware

- Reads a normally-open switch using the ESP32 internal `INPUT_PULLUP`.
- Serves the dashboard from the ESP32 using the Arduino `WebServer` library.
- Exposes `/` for the dashboard and `/status` for the switch state.
- The browser polls the status endpoint every 500 ms.
- Uses GPIO 5 for the switch input.

## Hardware

| Component | Quantity | Notes |
|---|---:|---|
| ESP32 development board | 1 | Select the matching ESP32 board in Arduino IDE |
| Normally-open push button or switch | 1 | Connect between GPIO 5 and GND |
| USB data cable | 1 | Programming and power |

### Wiring

| ESP32 | Switch |
|---|---|
| GPIO 5 | Terminal 1 |
| GND | Terminal 2 |

The firmware configures GPIO 5 as `INPUT_PULLUP`; no external pull-up resistor is required for this simple button connection.

## Repository location

From the repository root, the project is in:

```text
ESP32 - Staff Project/
└── ESP32_-_Switching_On_Or_Off/
    ├── ESP32_-_Switching_On_Or_Off.ino
    └── README.md
```

## Setup

1. Open `ESP32_-_Switching_On_Or_Off.ino` in Arduino IDE.
2. In the source file, set `YOUR_WIFI_SSID` and `YOUR_WIFI_PASSWORD` to your local 2.4 GHz Wi-Fi credentials.
3. Do not commit real Wi-Fi credentials. Keep your local configured copy private; before sharing code, restore the placeholders.
4. Install/select the ESP32 board support package and choose the correct ESP32 board and serial port.
5. Upload the sketch.
6. Open Serial Monitor at 115200 baud and note the IP address printed after Wi-Fi connects.
7. Open that IP address in a browser on the same local network.

## HTTP endpoints

| Path | Method | Purpose |
|---|---|---|
| `/` | GET | Serves the dashboard page |
| `/status` | GET | Returns the current switch state |

## Security notes

- The firmware is intended for a trusted local network demonstration.
- Do not expose the ESP32 web server directly to the public internet.
- The dashboard endpoint has no authentication in the current firmware; anyone on the reachable network may view the switch status.
- If Wi-Fi credentials were ever committed to a public repository, changing them is necessary. Replacing the values in the current file does not remove them from previous Git commits.

## Troubleshooting

- **ESP32 does not connect:** confirm the SSID/password locally, use a 2.4 GHz network, and check Serial Monitor output.
- **Dashboard does not load:** confirm the phone/computer and ESP32 are on the same network and use the current IP address.
- **Switch state does not change:** check the GPIO 5-to-GND wiring and confirm the sketch was uploaded to the intended board.
- **No serial output:** select the correct port and set Serial Monitor to 115200 baud.

## Author

Hari Prasad L S — [GitHub profile](https://github.com/lshariprasad)
