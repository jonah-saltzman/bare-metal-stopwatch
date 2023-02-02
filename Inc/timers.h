#include "./sys/stm32f4xx.h"
#include "init.h"

extern ClockSpeeds clocks;

extern volatile uint32_t timer2_ticks;
extern volatile uint32_t timer5_ticks;
extern volatile uint32_t timer10_ticks;
extern volatile uint8_t timer2_counting;
extern volatile uint8_t timer5_counting;
extern volatile uint8_t timer10_counting;
extern volatile uint32_t pending_countdown;
extern volatile uint16_t centi_seconds;

#define RESOLUTION_MICROSECOND 1000000

#define TIM2_RESOLUTION RESOLUTION_MICROSECOND
#define TIM5_RESOLUTION RESOLUTION_MICROSECOND
#define TIM10_RESOLUTION RESOLUTION_MICROSECOND

#define TIM2_IRQ_PRIO 100
#define TIM5_IRQ_PRIO 101
#define TIM10_IRQ_PRIO 40

void reset_timer(TIM_TypeDef* timer);

void start_timer(TIM_TypeDef* timer);

void stop_timer(TIM_TypeDef* timer);

void wait_microseconds(uint32_t us);

void error_led(uint32_t us);
