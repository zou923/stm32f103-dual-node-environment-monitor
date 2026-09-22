#include "stm32f1xx_hal.h"
#include "OLED.h"
#include "SHT30.h"
#include "BH1750.h"
#include "NRF24L01.h"
#include "Buzzer.h"

#define SAMPLE_PERIOD_MS 1000U
#define REMOTE_TIMEOUT_MS 3000U
#define LIGHT_ALARM_LX 500U

static const uint8_t master_address[5] = {'N','O','D','E','1'};
static const uint8_t slave_address[5] = {'N','O','D','E','2'};

static void append_u16(char *text, uint8_t *index, uint16_t value)
{
    uint16_t divisor = 10000U;
    uint8_t started = 0U;
    while (divisor != 0U) {
        uint8_t digit = (uint8_t)(value / divisor);
        if (digit != 0U || started != 0U || divisor == 1U) {
            text[(*index)++] = (char)('0' + digit);
            started = 1U;
        }
        value = (uint16_t)(value % divisor);
        divisor = (uint16_t)(divisor / 10U);
    }
}

static void measurement_lines(char prefix, int16_t temperature, uint16_t humidity, uint16_t lux, char *line1, char *line2)
{
    uint8_t index = 0U;
    uint16_t magnitude = (temperature < 0) ? (uint16_t)(-temperature) : (uint16_t)temperature;
    line1[index++] = prefix; line1[index++] = ' '; line1[index++] = 'T'; line1[index++] = ':';
    if (temperature < 0) line1[index++] = '-';
    append_u16(line1, &index, (uint16_t)(magnitude / 10U));
    line1[index++] = '.'; line1[index++] = (char)('0' + magnitude % 10U);
    line1[index++] = ' '; line1[index++] = 'H'; line1[index++] = ':';
    append_u16(line1, &index, (uint16_t)(humidity / 10U));
    line1[index++] = '%'; line1[index] = '\0';
    index = 0U;
    line2[index++] = prefix; line2[index++] = ' '; line2[index++] = 'L'; line2[index++] = ':';
    append_u16(line2, &index, lux);
    line2[index++] = ' '; line2[index++] = 'L'; line2[index++] = 'X'; line2[index] = '\0';
}

static void draw_display(int16_t remote_t, uint16_t remote_h, uint16_t remote_l, uint8_t remote_valid, int16_t local_t, uint16_t local_h, uint16_t local_l)
{
    char line1[17], line2[17], line3[17], line4[17];
    OLED_Clear();
    if (remote_valid != 0U) {
        measurement_lines('S', remote_t, remote_h, remote_l, line1, line2);
        OLED_ShowString8(1U, 1U, line1);
        OLED_ShowString8(2U, 1U, line2);
    } else {
        OLED_ShowString8(1U, 1U, "S OFFLINE");
    }
    measurement_lines('M', local_t, local_h, local_l, line3, line4);
    OLED_ShowString8(5U, 1U, line3);
    OLED_ShowString8(6U, 1U, line4);
}

static void poll_remote(int16_t *remote_t, uint16_t *remote_h, uint16_t *remote_l, uint8_t *remote_valid, uint32_t *last_remote_ms)
{
    uint8_t remote_status;
    while (NRF24L01_ReadTelemetry(remote_t, remote_h, remote_l, &remote_status)) {
        *remote_valid = ((remote_status & 0x03U) == 0x03U) ? 1U : 0U;
        *last_remote_ms = HAL_GetTick();
    }
}

int main(void)
{
    GPIO_InitTypeDef gpio = {0};
    int16_t remote_t = 0, local_t = 0;
    uint16_t remote_h = 0, remote_l = 0, local_h = 0, local_l = 0;
    uint8_t remote_valid = 0;
    uint32_t last_remote_ms = 0U;

    HAL_Init();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    gpio.Pin = GPIO_PIN_13; gpio.Mode = GPIO_MODE_OUTPUT_PP; gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &gpio);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

    OLED_Init();
    SHT30_Init();
    BH1750_Init();
    NRF24L01_Init(master_address, slave_address);
    Buzzer_Init();

    for (;;) {
        uint32_t start = HAL_GetTick();
        uint8_t local_light_valid = 0U;
        poll_remote(&remote_t, &remote_h, &remote_l, &remote_valid, &last_remote_ms);
        if (SHT30_Read(&local_t, &local_h) && BH1750_Read(&local_l)) {
            local_light_valid = 1U;
        }
        if ((HAL_GetTick() - last_remote_ms) > REMOTE_TIMEOUT_MS) remote_valid = 0U;
        Buzzer_SetAlarm(((local_light_valid != 0U && local_l > LIGHT_ALARM_LX) ||
                         (remote_valid != 0U && remote_l > LIGHT_ALARM_LX)) ? 1U : 0U);
        draw_display(remote_t, remote_h, remote_l, remote_valid, local_t, local_h, local_l);
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        while ((HAL_GetTick() - start) < SAMPLE_PERIOD_MS) poll_remote(&remote_t, &remote_h, &remote_l, &remote_valid, &last_remote_ms);
    }
}
