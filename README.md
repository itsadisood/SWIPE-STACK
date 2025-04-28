# SWIPE-STACK

**SWIPE-STACK** is a modern re-implementation of the classic puzzle game *Tetris*, featuring a wireless glove controller and a dynamic LED-matrix-based main hub.

This project combines embedded systems design, wireless communication, real-time graphics processing, and custom hardware to create an innovative and portable gaming experience.

---

## Project Overview

The system consists of two main components:

### Glove Module
- **Input Device**: A wearable glove capturing hand movements and gestures.
- **Key Components**:
  - **IMU Sensor**: Collects motion and orientation data.
  - **STM32 Microcontroller**: Processes IMU data and converts it into game control inputs.
  - **HM19 Bluetooth Module**: Communicates inputs wirelessly to the hub via UART.
  - **LiPo Battery**: Powers the glove, with battery management provided by an off-the-shelf controller.

### Main Hub
- **Game Console**: Executes game logic, receives user input, and manages displays.
- **Key Components**:
  - **STM32 Microcontroller**: Runs the game engine and display drivers.
  - **HM19 Bluetooth Module**: Receives input data from the glove.
  - **64x32 LED Matrix Display**: Displays the active game screen, driven by a custom parallel interface using DMA and timers.
  - **OLED 16x2 Text Display**: Shows the current score and level via SPI.
  - **TI Buck Converter**: Provides regulated power for the system.


Each STM32 project (`Hub/` and `Glove/`) follows the typical CubeIDE structure, including source files (`Core/`), drivers (`Drivers/`), project configuration (`.ioc`), and build settings.

---

## Repository Structure

- **Hub/** – Firmware for the main hub (STM32 project using STM32Workbench)
- **Glove/** – Firmware for the glove controller (STM32 project using STM32Workbench)
- **Electrical/** – Hardware design files (schematics, PCB layouts)
- **Mechanical/** – Mechanical design files (CAD models for glove and hub enclosures)
- **README.md** – Project overview and documentation (this file)

---
## Features

- **Wireless Glove Controller**: Real-time gesture input using an IMU and STM32 MCU.
- **Bluetooth Communication**: Low-latency UART-based HM19 Bluetooth link.
- **Custom LED Matrix Driver**: High-speed, multiplexed driving of a 64x32 RGB LED matrix using DMA and timers.
- **Embedded Game Engine**: Fully on-board Tetris-like game running on STM32 microcontroller.
- **Text-Based Display**: OLED screen displaying real-time score and level information.
- **Portable and Modular Design**: Custom PCBs, enclosures, and battery-powered operation.

---

## Demo

Check out a live demo of SWIPE-STACK here:  
[![SWIPE-STACK Demo Video](https://img.youtube.com/vi/645_PaOeo9Y/0.jpg)](https://www.youtube.com/watch?v=645_PaOeo9Y&t=164s)

👉 [Watch the demo on YouTube](https://www.youtube.com/watch?v=645_PaOeo9Y&t=164s)

---

## Contributors

- Aditya Sood
- Alex Chitsazzadeh
- Vishnu Lagudu
- Justin Cambridge

---

## Notes

- This project is not intended for direct plug-and-play usage.
- Hardware assembly, flashing, and Bluetooth setup are required to build a fully functioning system.
- Schematics and mechanical models are provided for reference and reproduction.
- All firmware was developed using [STM32SystemWorkbench](https://www.st.com/en/development-tools/stm32cubeide.html).

---

## Acknowledgments

- STM32Cube HAL Drivers
- Public domain versions of the classic Tetris game logic
- Open-source hardware design inspirations for LED matrix driving techniques

---
