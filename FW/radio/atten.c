#include "atten.h"

uint8_t atten[2] = {0,0};

void atten_init(void)
{
	//init appropriare GPIO
	gpio_init(ATTEN_DATA_PORT, ATTEN_DATA_PIN, GPIO_MODE_OUTPUT_PP_LOW);
	gpio_init(ATTEN_CLK_PORT, ATTEN_CLK_PIN, GPIO_MODE_OUTPUT_PP_LOW);
	gpio_init(ATTEN_LE1_PORT, ATTEN_LE1_PIN, GPIO_MODE_OUTPUT_PP_LOW);
	gpio_init(ATTEN_LE2_PORT, ATTEN_LE2_PIN, GPIO_MODE_OUTPUT_PP_LOW);
	atten_set(0,0);
	atten_set(1,0);
	//set initial values (maybe in RF configuration)
}

void atten_set(uint8_t atten,uint8_t value)
{
	uint8_t i;
	
	if(value > 63) value = 63;
	value = 63 - value;
	for(i = 0; i < 8; i++)
	{
		if(value & 0x80) gpio_write(ATTEN_DATA_PORT, ATTEN_DATA_PIN,GPIO_LEVEL_HIGH);
		else gpio_write(ATTEN_DATA_PORT, ATTEN_DATA_PIN,GPIO_LEVEL_LOW);
		gpio_write(ATTEN_CLK_PORT, ATTEN_CLK_PIN,GPIO_LEVEL_HIGH);
		gpio_write(ATTEN_CLK_PORT, ATTEN_CLK_PIN,GPIO_LEVEL_LOW);
    value <<= 1; 
	}
	//strobe
	if(atten == 0) 
	{
		gpio_write(ATTEN_LE1_PORT, ATTEN_LE1_PIN,GPIO_LEVEL_HIGH);
		gpio_write(ATTEN_LE1_PORT, ATTEN_LE1_PIN,GPIO_LEVEL_LOW);
	}
	else
	{
		gpio_write(ATTEN_LE2_PORT, ATTEN_LE2_PIN,GPIO_LEVEL_HIGH);
		gpio_write(ATTEN_LE2_PORT, ATTEN_LE2_PIN,GPIO_LEVEL_LOW);
	}
}

void set_attenuation(float val)
{
	if(val < 0.0) val = 0.0;
	//printf("attenuation=%.1f\r\n",val);
	uint8_t att = (uint8_t)(val * 2);
	//printf("att=%d\r\n",att);
	if(att <= 63)
	{
		atten[0] = att;
		atten[1] = 0;
	}
	else
	{
		atten[0] = 63;
		atten[1] = att - 63;
		if(atten[1] > 63) atten[1] = 63;
	}
	atten_set(0,atten[0]);
	atten_set(1,atten[1]);
	//printf("att0=%d,att1=%d\r\n",atten[0],atten[1]);
	
}




