#ifndef NRF24L01_H
#define NRF24L01_H

#include <stdint.h>

void NRF24L01_Init(const uint8_t local_address[5], const uint8_t peer_address[5]);
int NRF24L01_SendTelemetry(int16_t temperature_tenths, uint16_t humidity_tenths, uint16_t lux, uint8_t status);
int NRF24L01_ReadTelemetry(int16_t *temperature_tenths, uint16_t *humidity_tenths, uint16_t *lux, uint8_t *status);

#endif
