#include "../Inc/sys/stm32f4xx.h"
#include "../Inc/timers.h"

__STATIC_FORCEINLINE void set_pending_countdown()
{
    uint32_t result = 1;
    do
    {
        uint32_t pend = __LDREXW(&pending_countdown);
        if (pend)
            abort();
        else
            result = __STREXW(1UL, &pending_countdown);
    }
    while (result != 0);
}

__STATIC_FORCEINLINE uint32_t get_pending_countdown()
{
    uint32_t result = 1;
    uint32_t pend;
    do
    {
        pend = __LDREXW(&pending_countdown);
        result = __STREXW(pend, &pending_countdown);
    }
    while (result != 0);
    return pend;
}

__STATIC_FORCEINLINE void clear_pending_countdown()
{
    uint32_t result = 1;
    do
    {
        uint32_t pend = __LDREXW(&pending_countdown);
        if (pend == 0)
            abort();
        else
            result = __STREXW(0UL, &pending_countdown);
    }
    while (result != 0);
}