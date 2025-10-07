#include "bsp.h"
#include "radio_proc.h"
#include "app_cli.h"


int main(void)
{
	init_power_clk();
	init_peripherals();
	delay_ms(100);
	txled_on();
	rxled_on();
	printf("\r\nLoRa Golden Node\r\n");
	radio_init();
	txled_off();
	delay_ms(100);
	rxled_off();
	cli_init();
	
	while(1)
	{
		radio_proc();
		cli_proc();
	}
}
