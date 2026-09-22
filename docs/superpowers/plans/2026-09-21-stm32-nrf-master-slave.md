# STM32 NRF24L01 Master/Slave Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Produce separately buildable and flashable master and slave STM32F103C8T6 images, with slave telemetry shown above local measurements on the master OLED.

**Architecture:** Shared drivers remain common. NRF24L01 accepts a local and peer address; one telemetry packet carries sensor readings. Separate application entry points own the master OLED/alarm and the headless slave.

**Tech Stack:** C17, STM32CubeF1 HAL, CMake/Ninja, PowerShell, OpenOCD CMSIS-DAP.

**Spec:** `docs/superpowers/specs/2026-09-21-stm32-nrf-master-slave-design.md`

## Global Constraints

- STM32F103C8T6 and the existing 64 KiB linker script remain unchanged.
- NRF24L01 uses channel 76, 1 Mbps, master `NODE1`, slave `NODE2`.
- Master owns OLED/PB1 alarm; slave does not initialize those peripherals.
- Sensor and radio period is one second.
- Use the existing SWD-only CMSIS-DAP configuration.

## Review Focus

- With no slave packet for 3 seconds, master renders `S OFFLINE` while local readings and alarm continue.
- Invalid slave sensor values are signalled by status bits and never shown as valid.
- Missing peer has only the bounded radio timeout impact.
- Slave image contains no OLED/Buzzer initialization.
- Four display lines fit the 128x64 OLED without 16-pixel-font overlap.

---

### Task 1: Role-aware NRF24L01 telemetry API

**Files:**
- Modify: `Hardware/NRF24L01/NRF24L01.h`
- Modify: `Hardware/NRF24L01/NRF24L01.c`
- Create: `tests/verify-nrf-master-slave.ps1`

**Interfaces:**
- `void NRF24L01_Init(const uint8_t local_address[5], const uint8_t peer_address[5])`
- `int NRF24L01_SendTelemetry(int16_t temperature_tenths, uint16_t humidity_tenths, uint16_t lux, uint8_t status)`
- `int NRF24L01_ReadTelemetry(int16_t *temperature_tenths, uint16_t *humidity_tenths, uint16_t *lux, uint8_t *status)`

- [ ] Write a failing PowerShell source test requiring the three interfaces, caller-provided local/peer addresses, and `0x01U` packet validation.
- [ ] Run `powershell -NoProfile -ExecutionPolicy Bypass -File tests\verify-nrf-master-slave.ps1`; verify it fails because the receive interface is absent.
- [ ] Configure RX pipe 0 with local address, TX with peer address; encode/decode the documented eight-byte telemetry packet; retain bounded 20 ms transmit wait and restore RX mode.
- [ ] Re-run the test; verify pass.
- [ ] Commit this task.

### Task 2: Compact OLED rendering

**Files:**
- Modify: `Hardware/OLED/OLED.h`
- Modify: `Hardware/OLED/OLED.c`
- Create: `Hardware/OLED/OLED_Font8x8.h`
- Create: `tests/verify-oled-compact.ps1`

**Interfaces:**
- `void OLED_ShowString8(uint8_t row, uint8_t column, const char *string)`, rows 1 through 8.

- [ ] Write a failing source test requiring `OLED_ShowString8`, an 8-pixel font table, and page-oriented 8-pixel rendering.
- [ ] Run it and verify failure before adding production code.
- [ ] Add printable ASCII 8x8 glyphs and the compact renderer; preserve all existing 8x16 APIs.
- [ ] Re-run the test; verify pass.
- [ ] Commit this task.

### Task 3: Master and slave application entry points

**Files:**
- Create: `Core/Src/main_master.c`
- Create: `Core/Src/main_slave.c`
- Create: `tests/verify-master-slave-apps.ps1`

**Interfaces:**
- Consumes Task 1 radio API and Task 2 OLED API.
- Master receives packets and renders slave data above local data.
- Slave samples and sends one packet per second.

- [ ] Write a failing source test: master must call OLED/Buzzer initialization and `NRF24L01_ReadTelemetry`; slave must call `NRF24L01_SendTelemetry` and contain no OLED/Buzzer identifier.
- [ ] Run the test and verify failure because these source files do not yet exist.
- [ ] Implement master remote timestamp handling: before first packet and after 3000 ms it renders `S OFFLINE`; valid data renders the specified four compact lines. Its local BH1750 reading alone controls alarm.
- [ ] Implement slave sampling: always transmit every second, zero invalid measurements, and set status bit 0 for valid SHT30 and bit 1 for valid BH1750.
- [ ] Re-run the test; verify pass.
- [ ] Commit this task.

### Task 4: Dual build and flash targets

**Files:**
- Modify: `CMakeLists.txt`
- Modify: `scripts/build.ps1`
- Modify: `scripts/flash.ps1`
- Modify: `.vscode/tasks.json`
- Modify: `tests/verify-project.ps1`
- Modify: `tests/verify-flash-script.ps1`

**Outputs:**
- `build/stm32f103-master.elf` and `build/stm32f103-slave.elf`, each with HEX/BIN.
- `scripts/build.ps1 -Role Master|Slave` and matching flash command.

- [ ] Extend existing verification scripts with failing assertions for both CMake targets, role parameters, role-specific artifacts, and four VS Code tasks: Build/Flash Master and Build/Flash Slave.
- [ ] Run the verification scripts and confirm they fail against the one-target project.
- [ ] Factor shared CMake sources and create master/slave executables with their respective main files. Validate Role in both scripts and select role-specific ELF. Add four VS Code tasks.
- [ ] Run both verification scripts, then `scripts/build.ps1 -Role Master` and `scripts/build.ps1 -Role Slave`; both images must compile and fit 64 KiB.
- [ ] Flash master and slave separately; each must end `** Verified OK **`. Confirm master changes from `S OFFLINE` to received slave readings within three seconds.
- [ ] Commit this task.
