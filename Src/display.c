#include "sys/stm32f439xx.h"
#include "display.h"
#include "init.h"
#include "timers.h"

static uint8_t current_digit = 0;

static volatile uint16_t s_next_value = 0;

static uint16_t temp_number = 0;

static uint16_t digits[4] = {
    FIRST_DIGIT,
    SECOND_DIGIT,
    THIRD_DIGIT,
    FOURTH_DIGIT
};

static uint16_t digit_symbols[10] = {
    DIGIT_ZERO,
    DIGIT_ONE,
    DIGIT_TWO,
    DIGIT_THREE,
    DIGIT_FOUR,
    DIGIT_FIVE,
    DIGIT_SIX,
    DIGIT_SEVEN,
    DIGIT_EIGHT,
    DIGIT_NINE
};

static void display_digit(uint16_t val, uint16_t decimal_mask)
{
    GPIOE->ODR = digit_symbols[val] | (DECIMAL_POINT & decimal_mask);
}

void set_next_value(uint16_t val)
{
    s_next_value = val;
}

void render_display()
{
    uint16_t digit = temp_number % 10;
    temp_number /= 10;
    GPIOA->ODR = digits[3 - current_digit]; // select digit to display
    display_digit(digit, current_digit == 2 ? 0xFFFFU : 0x0U); // place decimal on 2nd digit
    current_digit = (current_digit + 1) & 0x3U;
}

void TIM5_IRQHandler(void)
{
	timer_interrupt_handler(&tim5);
    ++s_next_value;
}

void TIM2_IRQHandler(void)
{
	timer_interrupt_handler(&tim2);
    if (current_digit == 0)
        temp_number = s_next_value;
    render_display();
}
