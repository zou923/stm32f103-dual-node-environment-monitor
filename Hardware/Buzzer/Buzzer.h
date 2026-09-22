#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>

void Buzzer_Init(void);
void Buzzer_Set(uint8_t enabled);
void Buzzer_Toggle(void);
void Buzzer_SetAlarm(uint8_t enabled);
void Buzzer_Tick1ms(void);

#endif
