# iot-smart-home
# ESP32-Based Smart Home Automation

A standalone smart-home controller with an ESP32-hosted responsive web dashboard. The ESP32 creates its own Wi-Fi hotspot, so a phone or laptop can connect and control the system without an internet connection.

## Features

- Live temperature and humidity monitoring with DHT11
- Ambient light monitoring with an LDR analog output
- Ultrasonic distance measurement and nearby-object indication with HC-SR04
- Automatic and manual operating modes
- Relay-based fan and light switching
- Responsive dashboard hosted directly on the ESP32
- Local Wi-Fi access without a router or internet connection

## Hardware

- ESP32 development board
- DHT11 temperature and humidity sensor or module
- HC-SR04 ultrasonic sensor
- LDR sensor module with analog output
- 2-channel relay module
- DC fan and light/load suitable for the relay and power supply
- Breadboard, jumper wires and suitable power supplies

## Software requirements

- Arduino IDE
- ESP32 board support package
- Adafruit DHT sensor library
- Adafruit Unified Sensor library

The `WiFi.h` and `WebServer.h` libraries are included with the ESP32 Arduino core.

## Pin configuration

| Device | ESP32 pin |
| DHT11 data | GPIO 4 |
| HC-SR04 TRIG | GPIO 5 |
| HC-SR04 ECHO | GPIO 18 through a voltage divider |
| LDR analog output | GPIO 34 |
| Fan relay input | GPIO 26 |
| Light relay input | GPIO 27 |

Connect sensor grounds to ESP32 ground. Power each module according to its specifications. The HC-SR04 ECHO output is typically 5 V and must not be connected directly to an ESP32 GPIO; use a resistor voltage divider to reduce it to 3.3 V or less. Confirm the relay module's input compatibility with 3.3 V logic.

The firmware assumes an active-low relay module. If the relay board is active-high, change `RELAY_ACTIVE_LOW` to `false` in `SmartHome.ino`.

## Configuration

The following settings are at the top of `SmartHome.ino`:

| Setting | Default |
| Wi-Fi hotspot name | `SmartHome-ESP32` |
| Wi-Fi password | `Home12345` |
| DHT type | `DHT11` |
| Fan turns on at | 30 °C |
| Fan turns off at | 28 °C |
| Dark threshold | 1800 (ADC scale 0–4095) |
| Object detection distance | 30 cm |

Adjust the GPIO assignments and thresholds to match the actual wiring and sensor behavior. Depending on the LDR module's voltage-divider arrangement, the ADC reading may rise or fall as the surroundings get darker. Calibrate `LDR_DARK_THRESHOLD` and, if necessary, reverse the comparison in the firmware.

## Upload and use

1. Install the ESP32 board package and the two Adafruit libraries listed above.
2. Open `src/SmartHome.ino` in Arduino IDE.
3. Select the correct ESP32 board and serial port.
4. Upload the sketch and open the Serial Monitor at 115200 baud.
5. Connect a phone or laptop to the `SmartHome-ESP32` Wi-Fi network using password `Home12345`.
6. Open `http://192.168.4.1` in a browser.
7. Choose Auto mode or Manual mode. Manual mode enables the fan and light controls.

## Automatic behavior

- The fan turns on at or above 30 °C and turns off at or below 28 °C.
- The light turns on when the LDR reading is below the configured dark threshold.
- Ultrasonic distance and object detection are displayed on the dashboard; they do not currently trigger the fan or light.

## Safety

Use an appropriately rated isolated power supply for the fan and other loads. Do not connect mains wiring to a breadboard. Mains-powered loads must be enclosed and wired by a qualified person using a properly rated relay, fuse and insulated terminals. Disconnect power before changing wiring. Never power a motor directly from an ESP32 GPIO pin.
