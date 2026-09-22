#include "stm32f1xx_hal.h"
#include "OLED.h"
#include "SHT30.h"
#include "BH1750.h"
#include "NRF24L01.h"
#include "Buzzer.h"

#define LED_TOGGLE_MS 200U
#define SHT30_REFRESH_MS 1000U
#define LIGHT_ALARM_LX 300U

static uint8_t light_alarm_active;

static void format_tenths(char *text, const char *label, int16_t value, char unit)
{
    uint16_t magnitude = (value < 0) ? (uint16_t)(-value) : (uint16_t)value;
    uint16_t whole = magnitude / 10U;
    uint8_t index = 0U;
    while (*label != '\0') text[index++] = *label++;
    if (value < 0) text[index++] = '-';
    if (whole >= 100U) text[index++] = (char)('0' + whole / 100U);
    if (whole >= 10U) text[index++] = (char)('0' + (whole / 10U) % 10U);
    text[index++] = (char)('0' + whole % 10U);
    text[index++] = '.';
    text[index++] = (char)('0' + magnitude % 10U);
    text[index++] = ' ';
    text[index++] = unit;
    text[index] = '\0';
}

static void format_lux(char *text, uint16_t lux)
{
    uint8_t index = 0U;
    uint16_t divisor = 10000U;
    uint8_t started = 0U;
    const char *label = "Light: ";
    while (*label != '\0') text[index++] = *label++;
    while (divisor > 0U) {
        uint8_t digit = (uint8_t)(lux / divisor);
        if (digit != 0U || started != 0U || divisor == 1U) {
            text[index++] = (char)('0' + digit);
            started = 1U;
        }
        lux = (uint16_t)(lux % divisor);
        divisor = (uint16_t)(divisor / 10U);
    }
    text[index++] = ' ';
    text[index++] = 'l';
    text[index++] = 'x';
    text[index] = '\0';
}

static void update_display(void)
{
    int16_t temperature;
    uint16_t humidity;
    char temperature_text[16];
    char humidity_text[16];
    char light_text[16];
    uint16_t lux = 0U;
    uint8_t command;
    uint16_t argument;
    OLED_Clear();
    if (!SHT30_Read(&temperature, &humidity)) {
        switch (SHT30_GetStatus()) {
        case 1U: OLED_ShowString(1U, 1U, "SHT ACK ERROR"); break;
        case 2U: OLED_ShowString(1U, 1U, "SHT READ ERROR"); break;
        case 3U: OLED_ShowString(1U, 1U, "SHT CRC ERROR"); break;
        default: OLED_ShowString(1U, 1U, "SHT30 ERROR"); break;
        }
        return;
    }
    format_tenths(temperature_text, "Temp: ", temperature, 'C');
    format_tenths(humidity_text, "Humi: ", (int16_t)humidity, '%');
    OLED_ShowString(1U, 1U, temperature_text);
    OLED_ShowString(3U, 1U, humidity_text);
    if (BH1750_Read(&lux)) {
        light_alarm_active = (lux > LIGHT_ALARM_LX) ? 1U : 0U;
        Buzzer_SetAlarm(light_alarm_active);
        format_lux(light_text, lux);
        OLED_ShowString(5U, 1U, light_text);
    } else {
        light_alarm_active = 0U;
        Buzzer_SetAlarm(0U);
        OLED_ShowString(5U, 1U, "BH1750 ERROR");
    }
    (void)NRF24L01_SendTelemetry(temperature, humidity, lux, 0U);
    (void)NRF24L01_ReadCommand(&command, &argument);
}

int main(void)
{
    GPIO_InitTypeDef gpio = {0};
    uint16_t elapsed_ms = 0U;

    HAL_Init();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    gpio.Pin = GPIO_PIN_13;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &gpio);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

    OLED_Init();
    SHT30_Init();
    BH1750_Init();
    NRF24L01_Init();
    Buzzer_Init();
    update_display();

    for (;;) {
        HAL_Delay(LED_TOGGLE_MS);
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        elapsed_ms = (uint16_t)(elapsed_ms + LED_TOGGLE_MS);
        if (elapsed_ms >= SHT30_REFRESH_MS) {
            elapsed_ms = 0U;
            update_display();
        }
    }
}
