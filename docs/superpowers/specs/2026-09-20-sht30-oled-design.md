# SHT30 temperature and humidity display design

## Goal

Read temperature and relative humidity from the on-board SHT30 once per
second, and show the values on the existing SSD1306 OLED while PC13
continues to blink every 500 ms.

## Hardware contract

- STM32F103C8T6 I2C1: PB6 = SCL and PB7 = SDA.
- OLED uses 7-bit address `0x3C`.
- SHT30 ADDR is grounded and therefore uses 7-bit address `0x44`.
- Both devices share the existing 3.3 V I2C bus and its pull-up resistors.

## Module boundaries

- `Hardware/I2C/I2C1.c/.h`: initialize PB6/PB7 and provide bounded generic
  I2C1 read/write transfers to a supplied 7-bit device address.
- `Hardware/OLED/OLED_Port.c/.h`: OLED-specific command/data wrappers over
  the shared I2C1 transport.
- `Hardware/SHT30/SHT30.c/.h`: issue the single-shot high-repeatability
  command `0x2C06`, wait for conversion, read six bytes, verify both CRC-8
  checksums, and convert raw samples to fixed-point tenths.
- `Core/Src/main.c`: initialize modules, read and render SHT30 values once
  per second, and keep the existing PC13 500 ms toggle independent.

## Display and timing behavior

After each successful measurement, the OLED is refreshed with:

```text
Temp: 25.3 C
Humi: 48.6 %
```

Numbers are rendered using integer fixed-point formatting: no floating-point
library is required. A failed transfer or CRC check renders `SHT30 ERROR`.
The SHT30 command has a finite conversion wait, and every I2C operation has a
finite timeout; neither may permanently stop the PC13 loop.

## Verification

- Source tests check the addresses, PB6/PB7 shared transport, command,
  CRC-8, conversion formula, one-second refresh, OLED output and PC13 loop.
- ARM build succeeds.
- DAPLink flashing returns `Verified OK`.
- Hardware validation: OLED updates the two value lines about once per second.
