# STM32 NRF24L01 Master/Slave Design

## Goal

Build two separately flashable STM32F103C8T6 firmware images for identical
PCBs. The slave board samples SHT30 and BH1750 once per second and transmits
the measurements. The master board displays the slave readings above its own
readings on its OLED and retains the local light alarm.

## Hardware Roles

### Master firmware

- Uses SHT30, BH1750, OLED, NRF24L01 and PB1 buzzer/LED alarm circuitry.
- Samples local temperature, humidity and lux once per second.
- Receives the slave telemetry packet.
- Controls the alarm from local lux only, using the existing threshold and
  timing implementation.

### Slave firmware

- Uses SHT30, BH1750 and NRF24L01.
- Does not initialize or access OLED or PB1 buzzer circuitry.
- Samples temperature, humidity and lux once per second and sends one packet
  to the master.

## NRF24L01 Protocol

- Channel: 76.
- Data rate: 1 Mbps.
- Master radio address: `NODE1`.
- Slave radio address: `NODE2`.
- Slave transmit destination: `NODE1`.
- Master receive address: `NODE1`.
- Telemetry payload length: 8 bytes:

| Byte | Meaning |
|---|---|
| 0 | Packet type `0x01` (telemetry) |
| 1-2 | Signed temperature in tenths of degrees C, big-endian |
| 3-4 | Humidity in tenths of percent RH, big-endian |
| 5-6 | Lux, unsigned big-endian |
| 7 | Sensor status bits: bit 0 SHT30 valid, bit 1 BH1750 valid |

The master marks remote values unavailable until a valid telemetry packet is
received, and after 3 seconds without a valid packet.

## OLED Layout

The 128x64 OLED uses a new compact 8-pixel-high ASCII renderer so both boards'
measurements fit at once. The master shows:

```text
S T:24.5 H:60%
S L:1234 lx

M T:24.8 H:61%
M L:1020 lx
```

`S` means slave and `M` means master. Remote unavailable values are rendered
as `S OFFLINE`. The OLED is not referenced by the slave firmware.

## Build and Flash Outputs

The project produces distinct master and slave ELF/HEX/BIN artifacts plus
separate VS Code build/flash task entries. The existing CMSIS-DAP, SWD-only
configuration is shared by both images.

## Error Handling

- A failed SHT30 or BH1750 read is represented in the status byte; the slave
  still sends a telemetry packet with zeroed invalid measurement fields.
- The master continues displaying and alarming from its local readings even
  if the slave packet is absent or invalid.
- NRF transmission failure does not block the periodic sampling loop beyond
  the module's bounded transmit timeout.
