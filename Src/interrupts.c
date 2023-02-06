#include <stdio.h>
#include "sys/stm32f4xx.h"
#include "timers.h"
#include "display.h"

void TIM1_UP_TIM10_IRQHandler(void)
{
	
}


// button press
void EXTI15_10_IRQHandler(void)
{
	EXTI->PR |= EXTI_PR_PR13;
	if (tim5.status & TIM_STATUS_COUNTING_UP)
	{
		stop_timer(&tim5);
	}
	else
	{
		start_timer(&tim5);
	}
}


void HardFault_Handler(void)
{
	if (SCB->HFSR & (1UL << 30))
	{
		printf("FORCED hard fault\n");
		printf("MMFAR: %p\n", (void*)SCB->MMFAR);
		printf("BFAR: %p\n", (void*)SCB->BFAR);
		printf("MMFAR valid: %s\n", (SCB->CFSR & SCB_CFSR_MMARVALID_Msk) ? "true" : "false");
		printf("BFAR valid: %s\n", (SCB->CFSR & SCB_CFSR_BFARVALID_Msk) ? "true" : "false");
		uint32_t cfsr = SCB->CFSR;
		cfsr++;
	}
	if (SCB->HFSR & (1UL << 1))
	{
		printf("Vector table fault\n");
	}
	while (1)
	{
	}
}

void MemManage_Handler(void)
{
	while (1)
	{
	}
}

void BusFault_Handler(void)
{
	while (1)
	{
	}
}

void UsageFault_Handler(void)
{
	while (1)
	{
	}
}

void WWDG_IRQHandler(void)
{
	while (1)
	{
	}
}
