#include <stdio.h>
#include "sys/stm32f439xx.h"
#include "init.h"
#include "timers.h"

ClockSpeeds clocks __attribute__((section("DATA")));

void enable_uart3(void)
{
	GPIOD->MODER |= (GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1);
	GPIOD->AFR[1] |= ((0x7UL << GPIO_AFRH_AFSEL8_Pos) | (0x7UL << GPIO_AFRH_AFSEL9_Pos));

	USART3->CR1 |= (1UL << 13); // uart enable
	
	USART3->BRR = 0x15B; // 0x15 = 21; 0xB = 11; 11/16 ~= 0.701 + 21 = 21.701

	USART3->CR1 |= (1UL << 3); // tx enable
}

void abort(void)
{
	__disable_irq();
	while (1) {}
}

uint32_t calculate_clockspeeds(ClockInit* init, ClockSpeeds* speeds)
{
	uint32_t pll_base, pll_input, pll_vco, pll_main;
	uint32_t pll_usb __attribute__((unused));

	pll_base = init->pll_source == PLL_SOURCE_HSI ? HSI_FREQ : HSE_FREQ;
	pll_input = pll_base / init->pll_m;
	if (pll_input != MILLION(2))
		return 1;
	pll_vco = pll_input * init->pll_n;
	if (pll_vco < MILLION(100) || pll_vco > MILLION(432))
		return 1;
	pll_main = pll_vco / pll_P_scale(init->pll_p);
	if (pll_main > MILLION(180))
		return 1;
	pll_usb = pll_vco / init->pll_q;
	#ifdef USE_USB
	if (pll_usb != MILLION(48))
		return 1;
	#endif

	uint32_t sysclk, hclk, apb2pre, apb1pre, pclk2, pclk1, tim2clk, tim1clk, systickclk;

	switch (init->sys_source) {
		case CLK_SOURCE_HSI:
			sysclk = MILLION(16);
			break;
		case CLK_SOURCE_HSE:
			sysclk = MILLION(8);
			break;
		case CLK_SOURCE_PLL:
			sysclk = pll_main;
			break;
		default:
			abort();
	}

	hclk = sysclk / ahb_scale(init->ahb_pre);
	if (hclk > MILLION(180))
		return 1;

	apb2pre = apbx_scale(init->apb2_pre);
	apb1pre = apbx_scale(init->apb1_pre);
	
	pclk2 = hclk / apb2pre;
	pclk1 = hclk / apb1pre;

	if (pclk2 > MILLION(90) || pclk1 > MILLION(45))
		return 1;

	if (init->timpre == 1)
	{
		tim2clk = (apb2pre <= 4) ? hclk : pclk2 * 4;
		tim1clk = (apb1pre <= 4) ? hclk : pclk1 * 4;
	} else {
		tim2clk = (apb2pre == 1) ? pclk2 : 2 * pclk2;
		tim1clk = (apb1pre == 1) ? pclk1 : 2 * pclk1;
	}

	systickclk = init->systick_source == SYSTICK_SOURCE_HCLK ? hclk : hclk / 8;

	speeds->pll_input = pll_input;
	speeds->pll_output = pll_main;
	speeds->sysclk = sysclk;
	speeds->hclk = hclk;
	speeds->pclk2 = pclk2;
	speeds->pclk1 = pclk1;
	speeds->tim2clk = tim2clk;
	speeds->tim1clk = tim1clk;
	speeds->systickclk = systickclk;

	return 0;
}

uint32_t osc_config(ClockInit* init)
{
	if (init->sys_source == CLK_SOURCE_HSE || init->pll_source == PLL_SOURCE_HSE)
	{
		// enable HSE with bypass
		RCC->CR |= (RCC_CR_HSEBYP | RCC_CR_HSEON);
		while ((RCC->CR & RCC_CR_HSERDY) == 0UL)
		{
		}
	} else {
		RCC->CR &= ~RCC_CR_HSEON;
		while ((RCC->CR & RCC_CR_HSERDY) != 0UL)
		{
		}
	}

	if (init->sys_source == CLK_SOURCE_HSI || init->pll_source == PLL_SOURCE_HSI)
	{
		RCC->CR |= (RCC_CR_HSION);
		while ((RCC->CR & RCC_CR_HSIRDY) == 0)
		{
		}
	}

	if (init->pll_source != PLL_SOURCE_OFF)
	{
		// disable PLL
		RCC->CR &= ~RCC_CR_PLLON;
		while ((RCC->CR & RCC_CR_PLLRDY) != 0)
		{
		}

		// write reg
		uint32_t pll_reg = RCC->PLLCFGR;
		pll_reg &= ~(PLL_Q_MASK | PLL_P_MASK | PLL_N_MASK | PLL_M_MASK);
		pll_reg |= (init->pll_source << RCC_PLLCFGR_PLLSRC_Pos);
		pll_reg |= (init->pll_q << RCC_PLLCFGR_PLLQ_Pos);
		pll_reg |= (init->pll_p << RCC_PLLCFGR_PLLP_Pos);
		pll_reg |= (init->pll_n << RCC_PLLCFGR_PLLN_Pos);
		pll_reg |= (init->pll_m << RCC_PLLCFGR_PLLM_Pos);
		RCC->PLLCFGR = pll_reg;

		// enable PLL
		RCC->CR |= RCC_CR_PLLON;
		while ((RCC->CR & RCC_CR_PLLRDY) == 0)
		{
		}
	} else {
		RCC->CR &= ~RCC_CR_PLLON;
		while ((RCC->CR & RCC_CR_PLLRDY) != 0)
		{
		}
	}

	return 0;
}

uint32_t enable_clocks(ClockInit* init, ClockSpeeds* speeds)
{

	if (calculate_clockspeeds(init, speeds) != 0)
		return 1;

	if (speeds->hclk > MILLION(168)) // overdrive not enabled
		return 1;

	// enable power clock
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;

	// set voltage scale
	uint32_t vos = speeds->hclk <= MILLION(120)
		? 0b01
		: speeds->hclk <= MILLION(144)
		? 0b10
		: speeds->hclk <= MILLION(168)
		? 0b11
		: 0;
	
	if (vos == 0)
		return 1;
	
	PWR->CR = ((PWR->CR & ~PWR_CR_VOS_Msk) | vos << PWR_CR_VOS_Pos);

	// configure oscillators
	if (osc_config(init) != 0)
		return 1;

	// calculate latency
	uint32_t flash_latency = speeds->hclk / MILLION(30);
	if (flash_latency > 5)
		return 1;
	
	// check latency
	if ((FLASH->ACR & FLASH_ACR_LATENCY) < flash_latency)
	{
		FLASH->ACR |= ((FLASH->ACR & ~FLASH_ACR_LATENCY) | flash_latency);
	}

	if ((FLASH->ACR & FLASH_ACR_LATENCY) < flash_latency)
	{
		return 1;
	}

	// first set PCLKs to min
	uint32_t rcc_cfg_reg = RCC->CFGR;
	rcc_cfg_reg &= ~(APB2_FACTOR_MASK | APB1_FACTOR_MASK);
	rcc_cfg_reg |= (APBx_16  << APB2_FACTOR_POS);
	rcc_cfg_reg |= (APBx_16  << APB1_FACTOR_POS);
	RCC->CFGR = rcc_cfg_reg;

	// then set HCLK
	rcc_cfg_reg = RCC->CFGR;
	rcc_cfg_reg &= ~(AHB_FACTOR_MASK);
	rcc_cfg_reg |= (init->ahb_pre << AHB_FACTOR_POS);
	RCC->CFGR = rcc_cfg_reg;

	// set clock source
	rcc_cfg_reg = RCC->CFGR;
	rcc_cfg_reg &= ~(SW_MASK);
	rcc_cfg_reg |= init->sys_source;
	RCC->CFGR = rcc_cfg_reg;

	// wait for clock source to switch
	while (((RCC->CFGR & SWS_MASK) >> SWS_POS) != init->sys_source)
	{
	}

	// then set PCLKs
	rcc_cfg_reg = RCC->CFGR;
	rcc_cfg_reg &= ~(APB2_FACTOR_MASK | APB1_FACTOR_MASK);
	rcc_cfg_reg |= (init->apb2_pre << APB2_FACTOR_POS);
	rcc_cfg_reg |= (init->apb1_pre << APB1_FACTOR_POS);

	RCC->CFGR = rcc_cfg_reg;

	// re-check latency
	if ((FLASH->ACR & FLASH_ACR_LATENCY) > flash_latency)
	{
		FLASH->ACR |= ((FLASH->ACR & ~FLASH_ACR_LATENCY) | flash_latency);
	}

	if ((FLASH->ACR & FLASH_ACR_LATENCY) > flash_latency)
	{
		return 1;
	}

	// enable peripherals
	RCC->AHB1ENR |= init->ahb1_enr;
	RCC->AHB2ENR |= init->ahb2_enr;
	RCC->AHB3ENR |= init->ahb3_enr;
	RCC->APB2ENR |= init->apb2_enr;
	RCC->APB1ENR |= init->apb1_enr;

	// set TIMPRE bit
	RCC->DCKCFGR |= ((RCC->DCKCFGR & ~(1UL << TIMPRE_POS)) | (init->timpre << TIMPRE_POS));

	// set SYSTICK source
	SysTick->CTRL |= ((SysTick->CTRL & ~SysTick_CTRL_CLKSOURCE_Msk) | (init->systick_source << SysTick_CTRL_CLKSOURCE_Pos));

	return 0;
}

uint32_t apbx_scale(uint32_t x)
{
    switch(x) {
        case APBx_1:
            return 1;
        case APBx_2:
            return 2;
        case APBx_4:
            return 4;
        case APBx_8:
            return 8;
        case APBx_16:
            return 16;
        default:
            abort();
    }
}

uint32_t ahb_scale(uint32_t x)
{
    switch(x) {
        case AHB_1:
            return 1;
        case AHB_2:
            return 2;
        case AHB_4:
            return 4;
        case AHB_8:
            return 8;
        case AHB_16:
            return 16;
        case AHB_64:
            return 64;
        case AHB_128:
            return 128;
        case AHB_256:
            return 256;
        case AHB_512:
            return 512;
        default:
            abort();
    }
}

uint32_t pll_P_scale(uint32_t x)
{
    switch(x) {
        case PLLP_2:
            return 2;
        case PLLP_4:
            return 4;
        case PLLP_6:
            return 6;
        case PLLP_8:
            return 8;
        default:
            abort();
    }
}

void initialize_IO(void)
{
	GPIOB->MODER &= (~(GPIO_MODER_MODER7_0 | GPIO_MODER_MODER0_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER5_0));
	GPIOB->MODER |= (GPIO_MODER_MODER7_0 | GPIO_MODER_MODER0_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER5_0);

	GPIOA->MODER &= ~(GPIO_MODER_MODER5_Msk | GPIO_MODER_MODER6_Msk | GPIO_MODER_MODER7_Msk | GPIO_MODER_MODER3_Msk);
	GPIOA->MODER |= (GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0 | GPIO_MODER_MODER3_0);

	GPIOE->MODER |= (GPIO_MODER_MODER0_0  | 
					 GPIO_MODER_MODER2_0  | 
					 GPIO_MODER_MODER7_0  | 
					 GPIO_MODER_MODER8_0  | 
					 GPIO_MODER_MODER9_0  | 
					 GPIO_MODER_MODER11_0 | 
					 GPIO_MODER_MODER13_0 | 
					 GPIO_MODER_MODER15_0 );
}

void enable_ext_intr(uint32_t irq_prio)
{
	// select port C for EXTI 13
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;

	// rising and falling trigger
	EXTI->RTSR |= EXTI_RTSR_TR13;
	// EXTI->FTSR |= EXTI_FTSR_TR13;
	
	// enable IRQ by writing 1 to the corresponding bit in the interrupt mask register
	// EXTI_IMR

	EXTI->IMR |= EXTI_IMR_MR13;

	NVIC_SetPriority(EXTI15_10_IRQn, irq_prio);
	NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void initialize_systicks(void)
{
	SysTick->CTRL |= 0b11;
	SysTick->LOAD = 0xFFFFF;
}

void SystemInit(void)
{
	ClockInit clock_settings = {0};

	clock_settings.pll_source = PLL_SOURCE_HSE;
	clock_settings.sys_source = CLK_SOURCE_PLL;
	clock_settings.systick_source = SYSTICK_SOURCE_HCLK_DIV8;
	clock_settings.timpre = 0UL;

	clock_settings.pll_q = 4UL;
	clock_settings.pll_p = PLLP_2;
	clock_settings.pll_n = 160UL;
	clock_settings.pll_m = 4UL;

	clock_settings.ahb_pre = AHB_1;
	clock_settings.apb2_pre = APBx_2;
	clock_settings.apb1_pre = APBx_4;

	clock_settings.ahb1_enr = (
		RCC_AHB1ENR_GPIOAEN | 
		RCC_AHB1ENR_GPIOBEN | 
		RCC_AHB1ENR_GPIOCEN |
		RCC_AHB1ENR_GPIODEN |
		RCC_AHB1ENR_GPIOEEN |
		RCC_AHB1ENR_DMA1EN  |
		RCC_AHB1ENR_DMA2EN
	);

	clock_settings.apb1_enr = (
		RCC_APB1ENR_PWREN    |
		RCC_APB1ENR_USART3EN |
		RCC_APB1ENR_TIM2EN   |
		RCC_APB1ENR_TIM5EN
	);

	clock_settings.apb2_enr = (
		RCC_APB2ENR_TIM10EN |
		RCC_APB2ENR_SYSCFGEN
	);
	if (enable_clocks(&clock_settings, &clocks) != 0)
		abort();
	enable_uart3();
}
