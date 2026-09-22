#include "stm32f1xx_hal.h"
#include "usb_dfu.h"

#define APPLICATION_ADDRESS 0x08003000U
#define DFU_WAIT_MS 3000U
typedef void (*entry_function_t)(void);

static void delay_ms(uint32_t milliseconds)
{
    while (milliseconds-- != 0U) {
        for (volatile uint32_t cycles = 0U; cycles < 12000U; ++cycles) {
            __NOP();
        }
    }
}

static int application_is_valid(void)
{
    const uint32_t stack = *(const uint32_t *)APPLICATION_ADDRESS;
    const uint32_t reset = *(const uint32_t *)(APPLICATION_ADDRESS + 4U);
    return ((stack & 0x2FFE0000U) == 0x20000000U) &&
           ((reset & 0xFF000001U) == 0x08000001U);
}

static void start_application(void)
{
    const uint32_t reset = *(const uint32_t *)(APPLICATION_ADDRESS + 4U);
    __disable_irq();
    SCB->VTOR = APPLICATION_ADDRESS;
    __set_MSP(*(const uint32_t *)APPLICATION_ADDRESS);
    ((entry_function_t)reset)();
}

int main(void)
{
    if (application_is_valid()) {
        start_application();
    }

    for (;;) {
        /* No valid application is installed. */
    }
}
