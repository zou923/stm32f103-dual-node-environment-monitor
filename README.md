# STM32F103 Dual Node Environment Monitor

An STM32F103C8T6 dual-node environment monitoring and wireless alarm system.

## Features

- SHT30 temperature and humidity sensing
- BH1750 ambient-light sensing
- NRF24L01 2.4 GHz master/slave telemetry
- SSD1306 OLED display on the master node
- Alarm when either valid light reading exceeds 500 lx

## Firmware Images

- **Master**: board with OLED and buzzer; displays local and slave measurements.
- **Slave**: headless sensor board; transmits telemetry once per second.

Use the VS Code tasks `STM32F103: Flash Master (CMSIS-DAP)` and
`STM32F103: Flash Slave (CMSIS-DAP)` to compile and flash each image.

## Hardware

- MCU: STM32F103C8T6
- Sensors: SHT30 and BH1750
- Radio: NRF24L01
- Display: SSD1306 I2C OLED
- Debugger: CMSIS-DAP over SWD

## Dependency Setup

Clone the official STM32CubeF1 package into `third_party/STM32CubeF1` before
building:

```powershell
git clone https://github.com/STMicroelectronics/STM32CubeF1.git third_party/STM32CubeF1
```
