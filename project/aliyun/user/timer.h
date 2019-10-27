#ifndef __TIMER_H_
#define __TIMER_H_
#include "types.h"

void TIM2_ON(void);
void TIM2_OFF(void);
void TIM2_Config(uint32 cnt);
void TIM2_NVIC_Configuration(void);	

#endif
