# iNose X6 Environmental Sensor Library

[中文](README_ZH.md)

The iNose X6 is a high-precision environmental sensor capable of detecting formaldehyde (HCHO), total volatile organic compounds (TVOC), carbon monoxide (CO), air quality index (IAQ), temperature, and humidity.
The sensor supports UART communication, LED control, sleep/wake functions, and is compatible with multiple platforms, including Arduino, ESP32, RP2040/RP2350, and Raspberry Pi.

- [Purchase Link](https://www.waveshare.com/environment-x6-sensor.htm)
- [Documentation](https://www.waveshare.com/wiki/Environment_X6_Sensor)

![Product Image](./assets/Product-1.webp)

## 🔧 Configuration

The library supports UART port and baud rate configuration.
Default settings:
1. Baud rate: 9600
2. Raspberry Pi 5 / CM5: /dev/ttyAMA0
3. Other Raspberry Pi models: /dev/ttyS0
4. Arduino / ESP32 / RP2040: Hardware UART

Supported features:
- Gas concentration detection: HCHO, TVOC, CO, IAQ
- Temperature and humidity measurement
- Reading sensor parameters (type, range, unit)
- LED control
- Sleep / wake functions
- Reading serial number and firmware version

Platform support table:
| Platform / MCU | Programming Language | Supported Features |
|----------------|----------------------|--------------------|
| ESP32 / RP2040 | Arduino (C++)        | Full functionality |
| ESP32          | ESP-IDF (C)          | Full functionality |
| ESP32 / RP2040 / Pi Pico | MicroPython | Full functionality |
| Raspberry Pi (all models) | Python | Full functionality |
| Raspberry Pi (all models) | C | Full functionality |

## 🛠️ Contributing
We welcome contributions! You can participate in the following ways:
1. Fork this repository.
2. Create a new branch for features or bug fixes.
3. Commit your changes with clear descriptions.
4. Submit a Pull Request for review.

## 🧩 Issues and Support
If you encounter any issues:
1. Check the Issues section.
2. Create an Issue with detailed information.
3. Refer to the documentation for troubleshooting tips.
4. Contact the Waveshare team and provide your order number for technical support.
