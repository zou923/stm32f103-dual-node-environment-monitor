#include "OLED_Port.h"
#include "I2C1.h"

#define OLED_ADDRESS 0x3CU

void OLED_Port_Init(void)
{
    I2C1_Init();
}

int OLED_Port_WriteCommand(uint8_t command)
{
    const uint8_t bytes[2] = {0x00U, command};
    return I2C1_Write(OLED_ADDRESS, bytes, 2U);
}

int OLED_Port_WriteData(const uint8_t *data, uint16_t length)
{
    while (length-- != 0U) {
        const uint8_t bytes[2] = {0x40U, *data++};
        if (!I2C1_Write(OLED_ADDRESS, bytes, 2U)) return 0;
    }
    return 1;
}
