#ifndef TIMERS_H_
#define TIMERS_H_
#include "sys/stm32f4xx.h"
#include "init.h"

extern struct TimerStruct tim2;
extern struct TimerStruct tim5;
extern struct TimerStruct tim10;

typedef struct TimerStruct
{
    TIM_TypeDef* regs;
    volatile uint32_t ticks;
    volatile uint32_t status;
} TimerStruct;

#define TIM_STATUS_COUNTING_UP   (1UL)
#define TIM_STATUS_COUNTING_DOWN (2UL)
#define TIM_STATUS_SLEEPING      (4UL)

#define RESOLUTION_MICROSECOND (1000000)

#define TIM2_IRQ_PRIO (100)
#define TIM5_IRQ_PRIO (101)
#define TIM10_IRQ_PRIO (40)

void initialize_stopwatch(
	TimerStruct* timer, 
	uint32_t resolution, 
	uint32_t irq_prio
);

void timer_interrupt_handler(TimerStruct* timer);
void reset_timer(TimerStruct* timer);
void start_timer(TimerStruct* timer);
void stop_timer(TimerStruct* timer);

IRQn_Type get_timer_irqn(TimerStruct* timer);
uint32_t get_timer_clockspeed(TimerStruct* timer);
uint8_t is_32bit_timer(TimerStruct* timer);

void wait_microseconds(TimerStruct* timer, uint32_t us);
#endif