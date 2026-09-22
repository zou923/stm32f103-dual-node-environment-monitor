#ifndef I2C1_H
#define I2C1_H

#include <stdint.h>

void I2C1_Init(void);
int I2C1_Write(uint8_t address, const uint8_t *data, uint16_t length);
int I2C1_Read(uint8_t address, uint8_t *data, uint16_t length);

#endif
