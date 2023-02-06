#include <stdio.h>
#include "sys/stm32f439xx.h"
#include "timers.h"
#include "init.h"

struct TimerStruct tim2 =  {TIM2,  0, 0};
struct TimerStruct tim5 =  {TIM5,  0, 0};
struct TimerStruct tim10 = {TIM10, 0, 0};

void initialize_stopwatch(
	TimerStruct* timer, 
	uint32_t resolution, 
	uint32_t irq_prio
)
{
	uint32_t arr = (get_timer_clockspeed(timer) / resolution);
	if (!is_32bit_timer(timer) && arr > UINT16_MAX)
		abort();
	timer->regs->CR1 |= (1UL << 2);
	timer->regs->ARR = arr;
	timer->regs->EGR |= 1UL;
	timer->regs->DIER |= 1UL;
	timer->regs->CR1 &= (~1UL);

	uint32_t irq_n = get_timer_irqn(timer);
	NVIC_SetPriority(irq_n, irq_prio);
	NVIC_EnableIRQ(irq_n);
}

void timer_interrupt_handler(TimerStruct* timer)
{
    timer->regs->SR &= (~1UL);
    if (timer->status & TIM_STATUS_SLEEPING)
    {
        timer->status &= ~TIM_STATUS_SLEEPING;
    }
    if (timer->status & TIM_STATUS_COUNTING_DOWN)
    {
        --(timer->ticks);
    }
    if (timer->status & TIM_STATUS_COUNTING_UP)
    {
        ++(timer->ticks);
    }
}

void reset_timer(TimerStruct* timer)
{
    timer->regs->EGR |= 1UL;
    timer->ticks = 0;
}

void start_timer(TimerStruct* timer)
{
    reset_timer(timer);
    timer->regs->CR1 |= 1UL;
    timer->status |= TIM_STATUS_COUNTING_UP;
}

void stop_timer(TimerStruct* timer)
{
    timer->regs->CR1 &= ~1UL;
    timer->status &= ~(TIM_STATUS_COUNTING_UP | TIM_STATUS_COUNTING_DOWN);
}

IRQn_Type get_timer_irqn(TimerStruct* timer)
{
    switch((uint32_t)timer->regs)
    {
        case TIM9_BASE:
            return TIM1_BRK_TIM9_IRQn;
        case TIM10_BASE:
            return TIM1_UP_TIM10_IRQn;
        case TIM11_BASE:
            return TIM1_TRG_COM_TIM11_IRQn;
        case TIM2_BASE:
            return TIM2_IRQn;
        case TIM3_BASE:
            return TIM3_IRQn;
        case TIM4_BASE:
            return TIM4_IRQn;
        case TIM5_BASE:
            return TIM5_IRQn;
        case TIM6_BASE:
            return TIM6_DAC_IRQn;
        case TIM7_BASE:
            return TIM7_IRQn;
        case TIM12_BASE:
            return TIM8_BRK_TIM12_IRQn;
        case TIM13_BASE:
            return TIM8_UP_TIM13_IRQn;
        case TIM14_BASE:
            return TIM8_TRG_COM_TIM14_IRQn;
        default:
            abort();
    }
}

uint32_t get_timer_clockspeed(TimerStruct* timer)
{
    switch((uint32_t)timer->regs)
    {
        case TIM1_BASE:
        case TIM8_BASE:
        case TIM9_BASE:
        case TIM10_BASE:
        case TIM11_BASE:
            return clocks.tim2clk;
        case TIM2_BASE:
        case TIM3_BASE:
        case TIM4_BASE:
        case TIM5_BASE:
        case TIM6_BASE:
        case TIM7_BASE:
        case TIM12_BASE:
        case TIM13_BASE:
        case TIM14_BASE:
            return clocks.tim1clk;
        default:
            abort();
    }
}

uint8_t is_32bit_timer(TimerStruct* timer)
{
    if (timer->regs == TIM2 || timer->regs == TIM5)
        return 1;
    else
        return 0;
}

void wait_microseconds(TimerStruct* timer, uint32_t us)
{
    reset_timer(timer);
    uint32_t tick_rate = get_timer_clockspeed(timer) / MILLION(1);
    uint32_t max_arr = is_32bit_timer(timer) ? INT32_MAX : INT16_MAX;
    if (tick_rate > (max_arr / us))
        abort();
    uint32_t ticks = us * tick_rate;
    timer->status |= TIM_STATUS_SLEEPING;
    timer->regs->ARR = ticks;
    timer->regs->CR1 |= (1UL << 3); // one-pulse mode
    timer->regs->EGR |= 1UL;
    timer->regs->CR1 |= 1UL;
    while (timer->status & TIM_STATUS_SLEEPING)
    {
        __WFI();
    }
}