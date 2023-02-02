#include <stdio.h>
#include "../Inc/sys/stm32f4xx.h"
#include "../Inc/init.h"
#include "../Inc/io.h"
#include "../Inc/timers.h"
#include "../Inc/display.h"

int main(void)
{

	// timer2: render timer
	// 1200Hz / 4 digits = 300Hz overall
	initialize_TIM2_5_stopwatch(TIM2, 1200, TIM2_IRQ_PRIO);

	// timer5: 100Hz
	initialize_TIM2_5_stopwatch(TIM5, 100, TIM5_IRQ_PRIO);

	// gpio pins for display & buttons
	initialize_IO();

	// button interrupt
	enable_ext_intr(50);

	// start rendering display
	start_timer(TIM2);

	while (1)
	{
		__WFI();
	}
}
