#include "BH1750.h"
#include "I2C1.h"
#include "stm32f1xx_hal.h"

#define BH1750_ADDRESS 0x23U
#define BH1750_POWER_ON 0x01U
#define BH1750_RESET 0x07U
#define BH1750_CONTINUOUS_H_RES 0x10U

void BH1750_Init(void)
{
    GPIO_InitTypeDef gpio = {0};
    const uint8_t power_on = BH1750_POWER_ON;
    const uint8_t reset = BH1750_RESET;
    const uint8_t mode = BH1750_CONTINUOUS_H_RES;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    gpio.Pin = GPIO_PIN_0;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &gpio);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_Delay(1U);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    I2C1_Init();
    (void)I2C1_Write(BH1750_ADDRESS, &power_on, 1U);
    (void)I2C1_Write(BH1750_ADDRESS, &reset, 1U);
    (void)I2C1_Write(BH1750_ADDRESS, &mode, 1U);
}

int BH1750_Read(uint16_t *lux)
{
    uint8_t raw[2];
    HAL_Delay(180U);
    if (!I2C1_Read(BH1750_ADDRESS, raw, 2U)) return 0;
    *lux = (uint16_t)(((uint16_t)raw[0] << 8U | raw[1]) * 10U / 12U);
    return 1;
}
