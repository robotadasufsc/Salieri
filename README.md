# Sarieli

An electronic theremin.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## Hardware

- 1 × ESP-32D
- 2 × HC-SR04 ultrasonic distance sensor
- 1 × UDA1334 I²S stereo DAC

The project can be easily adapted to run on most ESP32 microcontrollers.

## Building

Open this project on the PlatformIO IDE and run the project task "Upload and Monitor".

## Connections

### UDA1334

- VIN: 
- 3V0: ~
- GND: ESP32 GND
- WSEL: ESP32 GPIO4
- DIN: ESP32 GPIO2
- BCLK: ESP32 GPIO3
- Lout: ~
- AGND: ~
- Rout: ~
- PLL: ~
- SF0: ~
- MUTE: ~
- SF1: ~
- SCLK: ~
