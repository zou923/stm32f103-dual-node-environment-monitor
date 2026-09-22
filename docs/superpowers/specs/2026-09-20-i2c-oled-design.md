# I2C OLED self-test design

## Goal

Display `Hello STM32` on the connected 0.96-inch I2C OLED while retaining the existing PC13 LED blink.

## Hardware contract

- MCU: STM32F103C8T6.
- I2C bus: I2C1, PB6=SCL and PB7=SDA.
- OLED supply: 3.3 V with a common ground.
- Controller: SSD1306-compatible 128x64 monochrome OLED.
- Initial slave address: 7-bit `0x3C`.

## Implementation

The application will use the STM32F1 Standard Peripheral Library already used by the project.

- Add a hardware-I2C1 setup at 100 kHz, with PB6/PB7 alternate-function open-drain.
- Add a focused SSD1306 module with command/data write helpers, reset/init sequence, a 128x64 page buffer, and a minimal 6x8 glyph set sufficient for `Hello STM32`.
- Initialize and flush the OLED once at boot, then keep the PC13 blink loop running.
- Keep the existing DAPLink/SWD build and flash workflow unchanged.

## Failure behavior

I2C writes use bounded waits.  If the OLED does not acknowledge, the firmware continues running so PC13 remains usable for hardware diagnosis.

## Verification

- A source-level test validates the I2C1 pin/address configuration and required `Hello STM32` drawing call.
- The ARM build must succeed.
- The resulting application is flashed and verified over DAPLink/SWD.
- User visually confirms text on the OLED and the PC13 LED blink.
