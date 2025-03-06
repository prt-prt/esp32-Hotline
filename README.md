# Hello World for LOLIN S2 Mini

This is a simple Hello World project for the LOLIN S2 Mini (ESP32-S2) board.

## Hardware Requirements

- LOLIN S2 Mini (ESP32-S2 based) development board
- USB cable

## Software Setup

This project uses PlatformIO with the Arduino framework for development.

1. Install [PlatformIO](https://platformio.org/install)
2. Clone this repository
3. Open the project in PlatformIO
4. Build and upload to your LOLIN S2 Mini

## How It Works

This simple project:
1. Initializes the serial communication at 115200 baud
2. Sends a welcome message: "Hello World from LOLIN S2 Mini!"
3. Continuously prints "Hello World!" every second

## Serial Monitor Configuration

- Baud rate: 115200
- Data bits: 8
- Parity: None
- Stop bits: 1

## Troubleshooting

- If nothing appears in the serial monitor, check your connections and make sure the board is properly connected.
- If you see "Device not configured" errors, try pressing the reset button on your board.
- Try a different USB cable if you're experiencing connection issues. 