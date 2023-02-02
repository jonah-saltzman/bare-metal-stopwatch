#include "../Inc/sys/stm32f4xx.h"

extern volatile uint32_t debug_status;

#define DEBUG_PRINT_MAIN = (1UL)
#define DEBUG_PRINT_INTR = (2UL)
#define DEBUG_PRINT_INTR_NUM_POS (2UL)
#define DEBUG_PRINT_INTR_NUM_MASK (0xFFUL << DEBUG_PRINT_INTR_NUM_POS)

#define IS_DEBUGGING (debug_status)
#define IS_MAIN_DEBUGGING (debug_status & 1UL)
#define IS_INTR_DEBUGGING (debug_status >> 1UL)

void set_main_debugging();

void clear_main_debugging();

uint32_t get_debugging_intr();

void set_debugging_intr(uint32_t IRQn);

void clear_debugging_intr();