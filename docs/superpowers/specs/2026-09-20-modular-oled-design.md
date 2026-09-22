# Modular OLED driver design

## Goal

Replace the temporary SSD1306 implementation with reusable OLED modules and display `Hello STM32` through I2C1 on PB6/PB7.

## Hardware contract

- STM32F103C8T6, 3.3 V OLED with common ground.
- PB6 = I2C1 SCL and PB7 = I2C1 SDA.
- SSD1306-compatible 128x64 OLED, 7-bit address `0x3C`.

## Module boundaries

- `Hardware/OLED/OLED_Port.c`: I2C1 pin/peripheral initialization and bounded SSD1306 command/data transfers.
- `Hardware/OLED/OLED.c` and `Hardware/OLED/OLED.h`: display initialization, cursor control, clear, character and string rendering.
- `Hardware/OLED/OLED_Font.h`: 8x16 ASCII font table copied from the supplied legacy driver.
- `Core/Src/main.c`: application behavior only: initialize OLED, render `Hello STM32`, then blink PC13.

## Display initialization

Use the legacy SSD1306 initialization sequence, including clock setup, multiplex ratio, COM pin configuration, contrast, precharge, VCOMH and charge pump (`0x8D`, `0x14`), but send it through I2C1 PB6/PB7 instead of the legacy PB8/PB9 bit-banged pins.

## Failure behavior

I2C transfers use finite timeouts. A missing OLED acknowledgement must not block PC13 blinking.

## Verification

- Source checks assert the module boundaries, PB6/PB7 I2C1 configuration, address `0x3C`, charge-pump commands and application display calls.
- ARM build succeeds.
- DAPLink flash reports `Verified OK`.
- User sees `Hello STM32` and observes PC13 blinking.
