# ESP32 Rotary Phone Interface

A project that interfaces a vintage rotary phone with an ESP32-S2 microcontroller, converting rotary dial pulses into MQTT messages for modern IoT applications.

## Hardware Requirements

- LOLIN S2 Mini (ESP32-S2) development board
- Rotary phone with three connections:
  - Pulse pin (Green wire)
  - Dial pin (White wire)
  - Hook switch pin
- USB cable for programming

## Features

- Converts rotary dial pulses to digital numbers
- Detects phone on/off hook state
- Publishes events via MQTT:
  - `phone/dial`: Dialed numbers
  - `phone/hook`: Phone state (picked up/hung up)
  - `phone/status`: Connection status

## Setup

1. Install [PlatformIO](https://platformio.org/install)
2. Clone this repository
3. Configure WiFi and MQTT settings in `src/main.cpp`
4. Connect the rotary phone wires to the ESP32:
   - Pulse pin → GPIO 21
   - Dial pin → GPIO 17
   - Hook switch → GPIO 34
5. Build and upload using PlatformIO

## Serial Monitor

- Baud rate: 115200
- Data bits: 8
- Parity: None
- Stop bits: 1

## Dependencies

- PubSubClient (MQTT client)
- ESP32Ping (Network utilities) 