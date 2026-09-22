#include "Buzzer.h"
#include "stm32f1xx_hal.h"

#define BUZZER_ALARM_TOGGLE_MS 200U

static volatile uint8_t alarm_enabled;
static volatile uint16_t alarm_elapsed;
static volatile uint8_t initialized;

void Buzzer_Init(void)
{
    GPIO_InitTypeDef gpio = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();
    gpio.Pin = GPIO_PIN_1;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &gpio);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    initialized = 1U;
}

void Buzzer_Set(uint8_t enabled)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, enabled ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void Buzzer_Toggle(void)
{
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
}

void Buzzer_SetAlarm(uint8_t enabled)
{
    alarm_enabled = enabled;
    alarm_elapsed = 0U;
    if (enabled == 0U) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
}

void Buzzer_Tick1ms(void)
{
    if (initialized == 0U) return;
    if (alarm_enabled == 0U) return;
    if (++alarm_elapsed >= BUZZER_ALARM_TOGGLE_MS) {
        alarm_elapsed = 0U;
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
    }
}
