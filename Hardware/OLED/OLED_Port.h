#ifndef OLED_PORT_H
#define OLED_PORT_H

#include <stdint.h>

void OLED_Port_Init(void);
int OLED_Port_WriteCommand(uint8_t command);
int OLED_Port_WriteData(const uint8_t *data, uint16_t length);

#endif
