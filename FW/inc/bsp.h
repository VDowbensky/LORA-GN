#ifndef _BSP_H_
#define _BSP_H_

#include <stdio.h>
#include <string.h>
#include "tremo_system.h"
#include "tremo_rcc.h"
#include "tremo_flash.h"
#include "tremo_gpio.h"
#include "tremo_uart.h"
#include "tremo_adc.h"
#include "tremo_delay.h"
#include "retargetserial.h"

#define	LF_BAND				0
#define	HF_BAND				1

#define CONFIG_COUNT	2

//#define RF_BAND				LF_BAND
//#define RF_BAND				HF_BAND
#define LF_FREQ_DEFAULT	380000000UL
#define HF_FREQ_DEFAULT	760000000UL
#define XOTRIM_DEFAULT	16

#define HW_VERSION    1
#define FW_VERSION		0
#define FW_REVISION		1

#define TXD0_PORT						GPIOB
#define TXD0_PIN						GPIO_PIN_0
#define RXD0_PORT						GPIOB
#define RXD0_PIN						GPIO_PIN_1

#define LPRXD_PORT					GPIOD //probably for GPS
#define LPRXD_PIN						GPIO_PIN_12


#define TXLED_PORT						GPIOA
#define TXLED_PIN							GPIO_PIN_8
#define RXLED_PORT						GPIOA
#define RXLED_PIN							GPIO_PIN_11

#define RFSW_CTRL_PORT			GPIOA
#define RFSW_CTRL_PIN				GPIO_PIN_10 //GPIO10
#define RFSW_TXRX_PORT			GPIOD
#define RFSW_TXRX_PIN				GPIO_PIN_11 //GPIO59

#define ATTEN_DATA_PORT			GPIOA
#define ATTEN_DATA_PIN			GPIO_PIN_15
#define ATTEN_CLK_PORT			GPIOA
#define ATTEN_CLK_PIN				GPIO_PIN_14
#define ATTEN_LE1_PORT			GPIOA
#define ATTEN_LE1_PIN				GPIO_PIN_4
#define ATTEN_LE2_PORT			GPIOA
#define ATTEN_LE2_PIN				GPIO_PIN_5



#define UART0_BR						115200
#define UART1_BR						115200


void init_power_clk(void);
void init_peripherals(void);
void txled_on(void);
void txled_off(void);
void rxled_on(void);
void rxled_off(void);

#endif
