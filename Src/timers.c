#include <stdio.h>
#include "../Inc/sys/stm32f439xx.h"
#include "timers.h"
#include "init.h"

ClockSpeeds clocks __attribute__((section("DATA")));

volatile uint32_t timer2_ticks = 0;
volatile uint8_t timer2_counting = 0;

volatile uint32_t timer5_ticks = 0;
volatile uint8_t timer5_counting = 0;

volatile uint32_t timer10_ticks = 0;
volatile uint8_t timer10_counting = 0;

volatile uint32_t pending_countdown = 0;

volatile uint16_t centi_seconds = 0;

void reset_timer(TIM_TypeDef* timer)
{
    timer->EGR |= 1UL;
    switch((uint32_t)timer) {
        case TIM2_BASE:
            timer2_ticks = 0;
            break;
        case TIM5_BASE:
            timer5_ticks = 0;
            break;
        case TIM10_BASE:
            timer10_ticks = 0;
            break;
        default:
            return;
    }
}

void start_timer(TIM_TypeDef* timer)
{
    reset_timer(timer);
    timer->CR1 |= 1UL;
    switch((uint32_t)timer) {
        case TIM2_BASE:
            timer2_counting = 1;
            break;
        case TIM5_BASE:
            timer5_counting = 1;
            break;
        case TIM10_BASE:
            timer10_counting = 1;
            break;
        default:
            return;
    }
}

void stop_timer(TIM_TypeDef* timer)
{
    timer->CR1 &= ~1UL;
    switch((uint32_t)timer) {
        case TIM2_BASE:
            timer2_counting = 0;
            break;
        case TIM5_BASE:
            timer5_counting = 0;
            break;
        case TIM10_BASE:
            timer10_counting = 0;
            break;
        default:
            return;
    }
}

void error_led(uint32_t us)
{
    printf("error led\n");
    reset_timer(TIM10);
    GPIOB->ODR |= GPIO_ODR_OD14_Msk;
    timer10_ticks = us;
    timer10_counting = 1;
    TIM10->EGR |= 1UL;
    TIM10->CR1 |= 1UL;
}

void wait_microseconds(uint32_t us)
{
    reset_timer(TIM5);
    uint32_t tick_rate = clocks.tim1clk / MILLION(1);
    uint32_t ticks = tick_rate > (INT32_MAX / us) ? UINT32_MAX : us * tick_rate;
    timer5_counting = 1;
    TIM5->ARR = ticks;
    TIM5->CR1 |= (1UL << 3); // one-pulse mode
    TIM5->EGR |= 1UL;
    TIM5->CR1 |= 1UL;
    while (timer5_counting)
    {
        __WFI();
    }
}