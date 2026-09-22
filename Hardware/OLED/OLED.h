#ifndef OLED_H
#define OLED_H

#include <stdint.h>

void OLED_Init(void);
void OLED_SetCursor(uint8_t y, uint8_t x);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t y, uint8_t x, char character);
void OLED_ShowString(uint8_t y, uint8_t x, const char *string);
void OLED_ShowString8(uint8_t row, uint8_t column, const char *string);

#endif
