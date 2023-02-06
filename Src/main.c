#include <stdio.h>
#include "sys/stm32f4xx.h"
#include "init.h"
#include "io.h"
#include "timers.h"
#include "display.h"

int main(void)
{

	// timer2: render timer
	// 1200Hz / 4 digits = 300Hz overall
	initialize_stopwatch(&tim2, 1200, 100);

	// timer5: 100Hz
	initialize_stopwatch(&tim5, 100, 101);

	// gpio pins for display & buttons
	initialize_IO();

	// button interrupt
	enable_ext_intr(50);

	// start rendering display
	start_timer(&tim2);

	while (1)
	{
		__WFI();
	}
}
