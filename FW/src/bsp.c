#include "bsp.h"

#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "lorac.h"
#include "ustimer.h"
#include "atten.h"
#include "retargetserial.h"


void init_power_clk(void)
{
	rcc_enable_oscillator(RCC_OSC_XO32M, true);
	rcc_set_sys_clk_source(RCC_SYS_CLK_SOURCE_XO32M);
	rcc_set_hclk_div(RCC_HCLK_DIV_1);
	rcc_set_pclk_div(RCC_PCLK0_DIV_1,RCC_PCLK1_DIV_1);
	rcc_set_systick_source(RCC_SYSTICK_SOURCE_HCLK);
	rcc_enable_peripheral_clk(RCC_PERIPHERAL_SYSCFG,true);
	//rcc_enable_peripheral_clk(RCC_PERIPHERAL_PWR, true);
	
	delay_init();
}

void init_peripherals(void)
{
	mygpio_init();
	//myadc_init();
	RETARGET_SerialInit();
	lorac_init();
	ustimer_init();
	atten_init();
}

void txled_on(void)
{
	gpio_write(TXLED_PORT, TXLED_PIN,GPIO_LEVEL_LOW);
}

void txled_off(void)
{
	gpio_write(TXLED_PORT, TXLED_PIN,GPIO_LEVEL_HIGH);
}

void rxled_on(void)
{
	gpio_write(RXLED_PORT, RXLED_PIN,GPIO_LEVEL_LOW);
}

void rxled_off(void)
{
	gpio_write(RXLED_PORT, RXLED_PIN,GPIO_LEVEL_HIGH);
}



