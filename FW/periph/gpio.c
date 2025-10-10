#include "gpio.h"

void mygpio_init(void)
{	
	rcc_enable_peripheral_clk(RCC_PERIPHERAL_GPIOA, true);
	rcc_enable_peripheral_clk(RCC_PERIPHERAL_GPIOB, true);
	rcc_enable_peripheral_clk(RCC_PERIPHERAL_GPIOC, true);
	rcc_enable_peripheral_clk(RCC_PERIPHERAL_GPIOD, true);
	
	//LED
	gpio_init(TXLED_PORT, TXLED_PIN, GPIO_MODE_OUTPUT_OD_HIZ);
	gpio_config_drive_capability(TXLED_PORT, TXLED_PIN,GPIO_DRIVE_CAPABILITY_8MA);
	gpio_init(RXLED_PORT, RXLED_PIN, GPIO_MODE_OUTPUT_OD_HIZ);
	gpio_config_drive_capability(RXLED_PORT, RXLED_PIN,GPIO_DRIVE_CAPABILITY_8MA);
}