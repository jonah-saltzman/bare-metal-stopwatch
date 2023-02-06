#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

#include "sys/stm32f4xx.h"

void TIM2_IRQHandler(void);

void EXTI15_10_IRQHandler(void);

void HardFault_Handler(void);

void MemManage_Handler(void);

void BusFault_Handler(void);

void UsageFault_Handler(void);

void WWDG_IRQHandler(void);

#endif