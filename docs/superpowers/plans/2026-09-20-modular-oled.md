# Modular OLED Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Provide a reusable SSD1306 OLED module on I2C1 PB6/PB7 and show `Hello STM32`.

**Architecture:** Split the temporary OLED driver into a transport port module, a display module, and a font header. The application consumes only the public OLED API; CMake compiles all three components with the existing SPL project.

**Tech Stack:** C, STM32F1 SPL, I2C1, CMake/Ninja, OpenOCD/CMSIS-DAP.

**Spec:** `docs/superpowers/specs/2026-09-20-modular-oled-design.md`

## Global Constraints

- PB6/I2C1 SCL and PB7/I2C1 SDA; OLED address `0x3C`.
- Retain 3.3 V operation and bounded I2C waits.
- Retain PC13 blinking and DAPLink flashing.

## Review Focus

- The charge pump commands `0x8D, 0x14` are required for the OLED panel; tested in Task 2.
- OLED failures must not block the PC13 loop; tested by bounded waits in Task 1.
- The application must not directly access I2C; tested by source boundaries in Task 3.

---

### Task 1: OLED I2C transport module

**Files:**
- Create: `Hardware/OLED/OLED_Port.c`
- Create: `Hardware/OLED/OLED_Port.h`
- Create: `tests/verify-oled-port.ps1`

- [ ] Write and run a failing source test requiring I2C1, PB6/PB7, AF open-drain, `0x3C`, command/data APIs and `I2C_TIMEOUT`.
- [ ] Move bounded I2C initialization and command/data transfers from `Core/Src/ssd1306.c` to `OLED_Port.c`.
- [ ] Run `powershell -NoProfile -ExecutionPolicy Bypass -File tests/verify-oled-port.ps1; cmake --build build` and verify success.

### Task 2: OLED display and font modules

**Files:**
- Create: `Hardware/OLED/OLED.c`
- Create: `Hardware/OLED/OLED.h`
- Create: `Hardware/OLED/OLED_Font.h`
- Create: `tests/verify-oled-display.ps1`

- [ ] Write and run a failing test requiring `OLED_Init`, `OLED_Clear`, `OLED_ShowString`, `0x8D`, `0x14`, and `OLED_F8x16`.
- [ ] Implement legacy-compatible SSD1306 initialization and 8x16 text output using transport APIs and copied ASCII font data.
- [ ] Run `powershell -NoProfile -ExecutionPolicy Bypass -File tests/verify-oled-display.ps1; cmake --build build` and verify success.

### Task 3: Application migration and DAP verification

**Files:**
- Modify: `Core/Src/main.c`
- Modify: `CMakeLists.txt`
- Delete: `Core/Src/ssd1306.c`
- Delete: `Core/Inc/ssd1306.h`
- Create: `tests/verify-oled-app-boundary.ps1`

- [ ] Write and run a failing test requiring `#include "OLED.h"`, `OLED_Init();`, `OLED_ShowString(1, 1, "Hello STM32");`, and no `I2C1` reference in `main.c`.
- [ ] Wire the modules into CMake and migrate `main.c` to the public OLED API.
- [ ] Run all OLED/flash tests, build, and `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/flash.ps1`; require `Verified OK`.
