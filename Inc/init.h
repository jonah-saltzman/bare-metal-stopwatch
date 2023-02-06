#ifndef INIT_H_
#define INIT_H_

#include <stdint.h>

void abort();

extern struct ClockSpeeds clocks;

#define MILLION(x) (1000000 * (x))

#define HSI			   (0UL)
#define HSE			   (1UL)
#define PLL			   (2UL)

#define CLK_SOURCE_HSI HSI
#define CLK_SOURCE_HSE HSE
#define CLK_SOURCE_PLL PLL

#define PLL_SOURCE_HSI HSI
#define PLL_SOURCE_HSE HSE
#define PLL_SOURCE_OFF (2UL)

#define SYSTICK_SOURCE_HCLK_DIV8 (0UL)
#define SYSTICK_SOURCE_HCLK      (1UL)

#define TIMPRE_POS	     (24)

#define HSI_FREQ       (16000000)
#define HSE_FREQ        (8000000)

#define PLLP_2           (0b00UL)
#define PLLP_4           (0b01UL)
#define PLLP_6           (0b10UL)
#define PLLP_8           (0b11UL)

#define AHB_1          (0b0000UL)
#define AHB_2          (0b1000UL)
#define AHB_4          (0b1001UL)
#define AHB_8          (0b1010UL)
#define AHB_16         (0b1011UL)
#define AHB_64         (0b1100UL)
#define AHB_128        (0b1101UL)
#define AHB_256        (0b1110UL)
#define AHB_512        (0b1111UL)

#define APBx_1          (0b000UL)
#define APBx_2          (0b100UL)
#define APBx_4          (0b101UL)
#define APBx_8          (0b110UL)
#define APBx_16         (0b111UL)

#define PLL_Q_POS            (24)
#define PLL_P_POS            (16)
#define PLL_N_POS             (6)
#define PLL_M_POS             (0)

#define APB2_FACTOR_POS      (13)
#define APB1_FACTOR_POS      (10)
#define AHB_FACTOR_POS        (4)
#define SW_POS                (0)
#define SWS_POS               (2)

#define PLL_Q_MASK            (0xFUL   << PLL_Q_POS)
#define PLL_P_MASK            (0x3UL   << PLL_P_POS)
#define PLL_N_MASK            (0x1FFUL << PLL_N_POS)
#define PLL_M_MASK            (0x3FUL  << PLL_M_POS)

#define APB2_FACTOR_MASK      (0x7UL << APB2_FACTOR_POS)
#define APB1_FACTOR_MASK      (0x7UL << APB1_FACTOR_POS)
#define AHB_FACTOR_MASK       (0xFUL << AHB_FACTOR_POS)
#define SW_MASK 		      (0x3UL << SW_POS)
#define SWS_MASK 		      (0x3UL << SWS_POS)

uint32_t apbx_scale(uint32_t x);

uint32_t ahb_scale(uint32_t x);

uint32_t pll_P_scale(uint32_t x);

// input  = HSE || HSI
//          input / M       should be 2MHz
// f(vco) = input * (N / M) must be between 100MHz and 432MHz
// f(pll) = f(vco) / P      must be less than 180MHz
// f(usb) = f(vco) / Q      must be 48MHz
typedef struct ClockInit
{
    uint32_t pll_source; // default: HSI (0)
    uint32_t sys_source; // default: HSI (0)
    uint32_t systick_source;
    uint32_t timpre;

    uint32_t pll_q; // min: 2 default: 4 (0100)
    uint32_t pll_p; // default: 2 (00)
    uint32_t pll_n; // min: 50 max: 432 default: 192
    uint32_t pll_m; // default: 16 (SHOULD BE 4 with HSE bypass)

    uint32_t  ahb_pre;
    uint32_t apb2_pre;
    uint32_t apb1_pre;

    uint32_t ahb1_enr;
    uint32_t ahb2_enr;
    uint32_t ahb3_enr;
    uint32_t apb2_enr;
    uint32_t apb1_enr;

} ClockInit;

typedef struct ClockSpeeds
{
    uint32_t pll_input;
    uint32_t pll_output;
    uint32_t sysclk;
    uint32_t hclk;
    uint32_t pclk2;
    uint32_t pclk1;
    uint32_t tim2clk;
    uint32_t tim1clk;
    uint32_t systickclk;
} ClockSpeeds;

uint32_t enable_clocks(ClockInit* init, ClockSpeeds* speeds);

void enable_uart3(void);

void initialize_IO(void);

void initialize_systicks(void);

void enable_ext_intr(uint32_t irq_prio);

#endif
