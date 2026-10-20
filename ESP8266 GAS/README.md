# GasGuard — ESP8266 IoT Gas Monitoring & Fan Controller

A Wi-Fi-enabled gas monitoring prototype built using an **ESP8266, digital gas sensor, motor driver, and 12V DC fan**. GasGuard provides a real-time web dashboard for viewing sensor status and manually controlling the fan from a phone or computer on the same local network.

## Features

- 📡 **Wi-Fi Connectivity** — Connects to a wireless network using the ESP8266.
- 📊 **Live Dashboard** — Displays gas sensor status, fan state, Wi-Fi status, and system uptime.
- 🚨 **Gas Detection Alerts** — Indicates when the digital sensor output crosses its configured threshold.
- 🌀 **Manual Fan Control** — Provides ON/OFF buttons through a browser-based interface.
- 🔄 **Automatic Dashboard Refresh** — Updates displayed status every two seconds.
- 📱 **Responsive Interface** — Works on mobile phones, tablets, and computers.
- 🔌 **Local Web Server** — Hosts the dashboard directly on the ESP8266 without requiring a cloud service.
- 🛡️ **Basic Control Interlock** — Rejects fan-start commands when a gas alert is active.

> **Safety notice:** This is an educational prototype, not a certified gas-leak detection or ventilation system. The interlock is software-only and is not a safety-rated protection mechanism. Do not use this project to protect people or property from hazardous gas leaks.

## Hardware Requirements

| Component | Purpose |
|---|---|
| ESP8266 NodeMCU | Main controller and Wi-Fi server |
| Digital gas sensor module | Detects a sensor threshold condition |
| L298N-compatible motor driver | Controls the DC fan |
| 12V DC fan | Demonstration output |
| 12V DC power supply | Supplies the fan and driver as required |
| Jumper wires | Electrical connections |
| USB cable | Programming and serial debugging |

**Note:** Confirm the exact motor-driver model and gas sensor module before powering the circuit.

## Circuit Connections

The following mapping assumes an ESP8266 NodeMCU and an L298N-type motor driver.

| ESP8266 pin | Connected component |
|---|---|
| D1 (GPIO5) | Gas sensor DO |
| D5 (GPIO14) | Motor driver IN1 |
| D6 (GPIO12) | Motor driver IN2 |
| D0 (GPIO16) | Motor driver ENA |

| Motor driver terminal | Connection |
|---|---|
| OUT1 | Fan terminal 1 |
| OUT2 | Fan terminal 2 |
| GND | Common ground with ESP8266 and power supply |
| Motor supply | Appropriate 12V supply, according to the driver's specifications |

### Wiring Notes

- Remove the ENA jumper if the ESP8266 is controlling ENA through D0.
- Connect IN2 to D6 if it is not already connected.
- Never apply 12V directly to an ESP8266 GPIO pin.
- ESP8266 GPIOs use 3.3V logic. Ensure the gas sensor's DO output cannot exceed the ESP8266 input voltage limit.
- Verify the motor driver's current rating against the fan's startup and operating current.
- Check the driver's documentation for its required power connections and jumper configuration.

## Software Requirements

- Arduino IDE
- ESP8266 board support package
- `ESP8266WiFi` library
- `ESP8266WebServer` library

The two libraries above are included with the ESP8266 Arduino core.

## Installation and Setup

### 1. Install Arduino IDE

Download Arduino IDE from the official website:

https://www.arduino.cc/en/software

### 2. Configure ESP8266 Support

In Arduino IDE, open **File → Preferences** and add this URL to *Additional Boards Manager URLs* if it is not already present:

`https://arduino.esp8266.com/stable/package_esp8266com_index.json`

Open **Tools → Board → Boards Manager**, search for `esp8266`, and install the ESP8266 platform.

Official project: https://github.com/esp8266/Arduino

### 3. Configure Wi-Fi

In the Arduino sketch, set your local network credentials:

```cpp
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
```

Use your actual network credentials locally. Do not commit private Wi-Fi passwords to a public repository.

### 4. Upload the Firmware

1. Open `GasGuard_ESP8266.ino`.
2. Select your ESP8266 board under **Tools → Board**.
3. Select the correct COM port.
4. Upload the sketch.
5. Open Serial Monitor at `115200 baud`.

### 5. Open the Dashboard

After the ESP8266 connects to Wi-Fi, Serial Monitor prints the dashboard address.

Example:

```text
Wi-Fi connected!
Dashboard: http://192.168.1.100
Web server started.
```

The address above is only an example. Use the IP address printed by your own ESP8266.

Open that address in a browser on a device connected to the same local network.

## Dashboard Overview

The web dashboard provides:

| Dashboard section | Function |
|---|---|
| Gas Sensor | Shows detected or clear status |
| Gas Alert | Displays the current sensor alert |
| Fan Controller | Sends manual ON/OFF commands |
| System Status | Shows Wi-Fi connectivity |
| Uptime | Displays time since the ESP8266 started |
| Last Update | Shows when the browser last refreshed its display |

**Sensor limitation:** The digital output indicates whether the module's configured threshold has been crossed. It does not provide a calibrated gas concentration in parts per million (ppm).

## API Endpoints

The ESP8266 hosts a simple local HTTP API.

| Method | Endpoint | Description |
|---|---|---|
| GET | `/` | Serves the web dashboard |
| GET | `/api/status` | Returns the latest sensor, fan, Wi-Fi, and uptime status |
| POST | `/api/fan?state=on` | Requests fan activation |
| POST | `/api/fan?state=off` | Requests fan shutdown |

Example status response:

```json
{
  "gasDetected": false,
  "fanOn": false,
  "wifi": true,
  "uptime": 120
}
```

The response values are illustrative.

## Control Logic

1. The ESP8266 reads the gas sensor's digital output.
2. The firmware converts the digital input into a detected/clear status.
3. The dashboard periodically requests the current status.
4. The user can request manual fan activation or shutdown.
5. An ON request is rejected if the sensor currently reports a gas alert.
6. If a gas alert appears while the fan is running, the firmware's shutdown logic disables the fan output.

The configured sensor polarity must match the actual module. The supplied sketch assumes the digital output goes LOW when gas is detected.

**Important:** Switching off an ordinary fan during a hazardous gas event may be unsafe, just as switching it on may be unsafe. This prototype must not determine real emergency ventilation behavior.

## Troubleshooting

### ESP8266 cannot connect to Wi-Fi

- Verify the SSID and password.
- Check that the network supports the ESP8266's 2.4 GHz Wi-Fi connection.
- Check whether the router uses client isolation or other settings that prevent devices from communicating.

### Dashboard does not load

- Check the IP address in Serial Monitor.
- Ensure the browser device is on the same local network.
- Confirm that the ESP8266 is powered and running the web server.

### Gas status is inverted

The sketch assumes an active-LOW sensor. If your module outputs HIGH when its threshold is triggered, change:

```cpp
constexpr bool GAS_ACTIVE_LOW = true;
```

to:

```cpp
constexpr bool GAS_ACTIVE_LOW = false;
```

Verify the sensor's actual behavior before relying on the displayed status.

### Fan does not start

- Verify the driver model and wiring.
- Check the IN1, IN2, and ENA connections.
- Confirm that the ENA jumper is configured correctly.
- Check the 12V supply and motor driver's current rating.
- Confirm that the sensor is not triggering the fan-start interlock.

### ESP8266 resets when the fan starts

Motor startup current and electrical noise may cause resets. Check the power supply, grounding, wiring, and decoupling. Use an appropriate motor supply and a properly designed power arrangement rather than powering the fan from the ESP8266.

## Security Considerations

- The sample dashboard uses plain HTTP.
- The example API has no authentication.
- Anyone who can reach the server on the local network may be able to issue control requests.
- Do not expose the ESP8266 web server directly to the public internet.
- Keep network credentials out of public source code.
- Add appropriate authentication and access controls before using this design beyond a controlled demonstration.

## Future Improvements

- Password-protected dashboard
- WebSocket-based live updates
- Historical sensor event logging
- Configurable threshold and sensor calibration
- Dashboard charts and event timestamps
- MQTT integration for a separate monitoring platform
- Secure remote access through a properly configured gateway

These are potential enhancements, not features already implemented in the current sketch.

## Project Status

**Prototype — Educational Development**

GasGuard demonstrates ESP8266 networking, digital sensor monitoring, embedded motor control, and browser-based IoT interfaces.

## License

Choose a license before publishing the repository. For example, the MIT License can be used if you want others to reuse and modify your code under its terms.

---

**Built with ESP8266, Arduino C++, HTML, CSS, and JavaScript.**
