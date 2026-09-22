#include "stm32f1xx_hal.h"
#include "SHT30.h"
#include "BH1750.h"
#include "NRF24L01.h"

#define SAMPLE_PERIOD_MS 1000U

static const uint8_t slave_address[5] = {'N','O','D','E','2'};
static const uint8_t master_address[5] = {'N','O','D','E','1'};

int main(void)
{
    GPIO_InitTypeDef gpio = {0};
    int16_t temperature;
    uint16_t humidity;
    uint16_t lux;
    HAL_Init();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    gpio.Pin = GPIO_PIN_13; gpio.Mode = GPIO_MODE_OUTPUT_PP; gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &gpio);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    SHT30_Init();
    BH1750_Init();
    NRF24L01_Init(slave_address, master_address);
    for (;;) {
        uint8_t status = 0U;
        uint32_t start = HAL_GetTick();
        if (SHT30_Read(&temperature, &humidity)) status |= 0x01U;
        else { temperature = 0; humidity = 0U; }
        if (BH1750_Read(&lux)) status |= 0x02U;
        else lux = 0U;
        (void)NRF24L01_SendTelemetry(temperature, humidity, lux, status);
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        while ((HAL_GetTick() - start) < SAMPLE_PERIOD_MS) { }
    }
}
