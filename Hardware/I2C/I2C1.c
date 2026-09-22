#include "I2C1.h"
#include "stm32f1xx_hal.h"

static I2C_HandleTypeDef hi2c1;

void I2C1_Init(void)
{
    GPIO_InitTypeDef gpio = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();
    gpio.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    gpio.Mode = GPIO_MODE_AF_OD;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &gpio);
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000U;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0U;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0U;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    (void)HAL_I2C_Init(&hi2c1);
}

int I2C1_Write(uint8_t address, const uint8_t *data, uint16_t length)
{
    return HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)(address << 1U), (uint8_t *)data, length, 100U) == HAL_OK;
}

int I2C1_Read(uint8_t address, uint8_t *data, uint16_t length)
{
    return HAL_I2C_Master_Receive(&hi2c1, (uint16_t)(address << 1U), data, length, 100U) == HAL_OK;
}
