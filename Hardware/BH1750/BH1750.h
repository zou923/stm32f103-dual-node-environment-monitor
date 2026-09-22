#ifndef BH1750_H
#define BH1750_H

#include <stdint.h>

void BH1750_Init(void);
int BH1750_Read(uint16_t *lux);

#endif
