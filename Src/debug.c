#include "../Inc/sys/stm32f4xx.h"
#include "../Inc/debug.h"

volatile uint32_t debug_status = 0;

__INLINE void set_main_debugging()
{
    debug_status |= 1UL;
}

__INLINE void clear_main_debugging()
{
    debug_status &= ~(1UL);
}

__INLINE uint32_t get_debugging_intr()
{
    return (debug_status & DEBUG_PRINT_INTR_NUM_MASK) >> DEBUG_PRINT_INTR_NUM_POS;
}

__INLINE void set_debugging_intr(uint32_t IRQn)
{
    debug_status |= (2UL | (debug_status & ~DEBUG_PRINT_INTR_NUM_MASK) | ((IRQn & 0xFF) << DEBUG_PRINT_INTR_NUM_POS));
}

__INLINE void clear_debugging_intr()
{
	uint32_t temp = debug_status;
	temp &= ~(DEBUG_PRINT_INTR_NUM_MASK | 2UL);
    debug_status = temp;
}
