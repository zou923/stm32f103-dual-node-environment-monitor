#include "OLED.h"
#include "OLED_Font.h"
#include "OLED_Port.h"

static void OLED_WriteCommand(uint8_t command)
{
    (void)OLED_Port_WriteCommand(command);
}

static void OLED_WriteData(uint8_t data)
{
    (void)OLED_Port_WriteData(&data, 1U);
}

void OLED_SetCursor(uint8_t y, uint8_t x)
{
    OLED_WriteCommand((uint8_t)(0xB0U + y - 1U));
    OLED_WriteCommand((uint8_t)(((x - 1U) & 0xF0U) >> 4U | 0x10U));
    OLED_WriteCommand((uint8_t)((x - 1U) & 0x0FU));
}

void OLED_Clear(void)
{
    for (uint8_t y = 1U; y <= 8U; ++y) {
        OLED_SetCursor(y, 1U);
        for (uint8_t x = 0U; x < 128U; ++x) {
            OLED_WriteData(0x00U);
        }
    }
}

void OLED_ShowChar(uint8_t y, uint8_t x, char character)
{
    uint8_t index = (character >= ' ' && character <= '~') ? (uint8_t)(character - ' ') : 0U;
    OLED_SetCursor(y, x);
    for (uint8_t i = 0U; i < 8U; ++i) {
        OLED_WriteData(OLED_F8x16[(uint16_t)index * 16U + i]);
    }
    OLED_SetCursor((uint8_t)(y + 1U), x);
    for (uint8_t i = 0U; i < 8U; ++i) {
        OLED_WriteData(OLED_F8x16[(uint16_t)index * 16U + i + 8U]);
    }
}

void OLED_ShowString(uint8_t y, uint8_t x, const char *string)
{
    while (*string != '\0' && x <= 121U) {
        OLED_ShowChar(y, x, *string++);
        x = (uint8_t)(x + 8U);
    }
}

static void OLED_CompactGlyph(char character, uint8_t glyph[6])
{
    static const uint8_t blank[6] = {0,0,0,0,0,0};
    const uint8_t *shape = blank;
    static const uint8_t digits[10][6] = {
        {0x3E,0x51,0x49,0x45,0x3E,0}, {0,0x42,0x7F,0x40,0,0},
        {0x42,0x61,0x51,0x49,0x46,0}, {0x21,0x41,0x45,0x4B,0x31,0},
        {0x18,0x14,0x12,0x7F,0x10,0}, {0x27,0x45,0x45,0x45,0x39,0},
        {0x3C,0x4A,0x49,0x49,0x30,0}, {0x01,0x71,0x09,0x05,0x03,0},
        {0x36,0x49,0x49,0x49,0x36,0}, {0x06,0x49,0x49,0x29,0x1E,0}
    };
    static const uint8_t colon[6] = {0,0x36,0x36,0,0,0};
    static const uint8_t dot[6] = {0,0x60,0x60,0,0,0};
    static const uint8_t percent[6] = {0x62,0x64,0x08,0x13,0x23,0};
    static const uint8_t minus[6] = {0x08,0x08,0x08,0x08,0x08,0};
    static const uint8_t letters[][6] = {
        {0x7E,0x11,0x11,0x11,0x7E,0}, {0x7F,0x49,0x49,0x49,0x36,0},
        {0x3E,0x41,0x41,0x41,0x22,0}, {0x7F,0x41,0x41,0x22,0x1C,0},
        {0x7F,0x49,0x49,0x49,0x41,0}, {0x7F,0x09,0x09,0x09,0x01,0},
        {0x3E,0x41,0x49,0x49,0x7A,0}, {0x7F,0x08,0x08,0x08,0x7F,0},
        {0,0x41,0x7F,0x41,0,0}, {0x20,0x40,0x41,0x3F,0x01,0},
        {0x7F,0x08,0x14,0x22,0x41,0}, {0x7F,0x40,0x40,0x40,0x40,0},
        {0x7F,0x02,0x0C,0x02,0x7F,0}, {0x7F,0x04,0x08,0x10,0x7F,0},
        {0x3E,0x41,0x41,0x41,0x3E,0}, {0x7F,0x09,0x09,0x09,0x06,0},
        {0x3E,0x41,0x51,0x21,0x5E,0}, {0x7F,0x09,0x19,0x29,0x46,0},
        {0x46,0x49,0x49,0x49,0x31,0}, {0x01,0x01,0x7F,0x01,0x01,0},
        {0x3F,0x40,0x40,0x40,0x3F,0}, {0x1F,0x20,0x40,0x20,0x1F,0},
        {0x3F,0x40,0x38,0x40,0x3F,0}, {0x63,0x14,0x08,0x14,0x63,0},
        {0x07,0x08,0x70,0x08,0x07,0}, {0x61,0x51,0x49,0x45,0x43,0}
    };
    if (character >= '0' && character <= '9') shape = digits[(uint8_t)(character - '0')];
    else if (character >= 'A' && character <= 'Z') shape = letters[(uint8_t)(character - 'A')];
    else if (character == ':') shape = colon;
    else if (character == '.') shape = dot;
    else if (character == '%') shape = percent;
    else if (character == '-') shape = minus;
    for (uint8_t i = 0U; i < 6U; ++i) glyph[i] = shape[i];
}

void OLED_ShowString8(uint8_t row, uint8_t column, const char *string)
{
    uint8_t glyph[6];
    OLED_SetCursor(row, column);
    while (*string != '\0' && column <= 123U) {
        OLED_CompactGlyph(*string++, glyph);
        OLED_Port_WriteData(glyph, 6U);
        column = (uint8_t)(column + 6U);
    }
}

void OLED_Init(void)
{
    OLED_Port_Init();
    OLED_WriteCommand(0xAEU);
    OLED_WriteCommand(0xD5U); OLED_WriteCommand(0x80U);
    OLED_WriteCommand(0xA8U); OLED_WriteCommand(0x3FU);
    OLED_WriteCommand(0xD3U); OLED_WriteCommand(0x00U);
    OLED_WriteCommand(0x40U);
    OLED_WriteCommand(0xA1U);
    OLED_WriteCommand(0xC8U);
    OLED_WriteCommand(0xDAU); OLED_WriteCommand(0x12U);
    OLED_WriteCommand(0x81U); OLED_WriteCommand(0xCFU);
    OLED_WriteCommand(0xD9U); OLED_WriteCommand(0xF1U);
    OLED_WriteCommand(0xDBU); OLED_WriteCommand(0x30U);
    OLED_WriteCommand(0xA4U);
    OLED_WriteCommand(0xA6U);
    OLED_WriteCommand(0x8DU); OLED_WriteCommand(0x14U);
    OLED_WriteCommand(0xAFU);
}
