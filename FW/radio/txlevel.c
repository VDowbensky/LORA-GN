#include "txlevel.h"

float rflevel;

int8_t txpower;
uint8_t dutycycle;
uint8_t hpmax;
float attenuation;

void set_source_level(void);

void setrflevel(void)
{
	set_source_level();
	set_attenuation(SOURCE_LEVEL - FIXED_ATTENUATION - rflevel);
}

void set_source_level(void)
{
	if(radioConfig.RfBand== LF_BAND)
	{
		if(currfreq < 120000)
		{
			txpower = -6;
			dutycycle = 5;
			hpmax = 4;
		}
		else if(currfreq < 160000)
		{
			txpower = -7;
			dutycycle = 4;
			hpmax = 6;
		}
		else if(currfreq < 180000)
		{
			txpower = -8;
			dutycycle = 5;
			hpmax = 5;
		}
		else if(currfreq < 240000)
		{
			txpower = -9;
			dutycycle = 4;
			hpmax = 5;
		}
		else if(currfreq < 280000)
		{
			txpower = -9;
			dutycycle = 4;
			hpmax = 4;
		}
		else if(currfreq < 320000)
		{
			txpower = -8;
			dutycycle = 4;
			hpmax = 4;
		}
		
		else if(currfreq < 360000)
		{
			txpower = -8;
			dutycycle = 4;
			hpmax = 6;
		}
		
		else if(currfreq < 380000)
		{
			txpower = -7;
			dutycycle = 4;
			hpmax = 4;
		}
		else if(currfreq < 410000)
		{
			txpower = -7;
			dutycycle = 4;
			hpmax = 4;
		}
		else if(currfreq < 430000)
		{
			txpower = -7;
			dutycycle = 3;
			hpmax = 3;
		}
		else if(currfreq < 470000)
		{
			txpower = -8;
			dutycycle = 3;
			hpmax = 5;
		}
		else if(currfreq < 520000)
		{
			txpower = -8;
			dutycycle = 4;
			hpmax = 4;
		}
		else if(currfreq < 560000)
		{
			txpower = -9;
			dutycycle = 3;
			hpmax = 0;
		}
		else if(currfreq < 580000)
		{
			txpower = -7;
			dutycycle = 4;
			hpmax = 3;
		}
		else if(currfreq < 600000)
		{
			txpower = -5;
			dutycycle = 5;
			hpmax = 7;
		}
		else //not recommended
		{
			txpower = -1;
			dutycycle = 5;
			hpmax = 5;
		}
	}
	
	else
	{
		if(currfreq < 500000)
		{
			txpower = -9;
			dutycycle = 3;
			hpmax = 4;
		}
		else if(currfreq < 580000)
		{
			txpower = -9;
			dutycycle = 3;
			hpmax = 3;
		}
		else if(currfreq < 630000)
		{
			txpower = -9;
			dutycycle = 3;
			hpmax = 4;
		}
		else if(currfreq < 720000)
		{
			txpower = -9;
			dutycycle = 4;
			hpmax = 5;
		}
		else if(currfreq < 770000)
		{
			txpower = -9;
			dutycycle = 4;
			hpmax = 6;
		}
		else if(currfreq < 810000)
		{
			txpower = -9;
			dutycycle = 4;
			hpmax = 6;
		}
		else if(currfreq < 840000)
		{
			txpower = -9;
			dutycycle = 5;
			hpmax = 6;
		}
		else if(currfreq < 880000)
		{
			txpower = -9;
			dutycycle = 6;
			hpmax = 7;
		}
		else if(currfreq < 930000)
		{
			txpower = -8;
			dutycycle = 6;
			hpmax = 6;
		}
		else if(currfreq < 960000)
		{
			txpower = -7;
			dutycycle = 5;
			hpmax = 5;
		}
		else if(currfreq < 980000)
		{
			txpower = -7;
			dutycycle = 5;
			hpmax = 6;
		}
		else //not recommended
		{
			txpower = -7;
			dutycycle = 5;
			hpmax = 6;
		}
	}
	SX126X_SetPaConfig(dutycycle,hpmax,false);
	SX126X_SetTxParams(txpower,0);
}


