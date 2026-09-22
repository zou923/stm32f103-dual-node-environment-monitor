#ifndef SHT30_H
#define SHT30_H

#include <stdint.h>

void SHT30_Init(void);
int SHT30_Read(int16_t *temperature_tenths, uint16_t *humidity_tenths);
uint8_t SHT30_GetStatus(void);

#endif
