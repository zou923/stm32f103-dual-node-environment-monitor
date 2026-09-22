#include "stm32f1xx_hal.h"
#include "Buzzer.h"

void SysTick_Handler(void)
{
    HAL_IncTick();
    Buzzer_Tick1ms();
}
