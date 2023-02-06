#include "sys/stm32f4xx.h"

void uart3_writechar(int c)
{
	while ((USART3->SR & (1UL << 7)) == 0)
	{
	}
	USART3->DR = (c & 0xFF);
}

int __io_putchar(int ch)
{
	uart3_writechar(ch);
	return 1;
}
