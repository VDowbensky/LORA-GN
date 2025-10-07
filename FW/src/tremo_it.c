#include "bsp.h"
#include "tremo_it.h"
#include "adc.h"
#include "radio_proc.h"


#define ADC_MS		10
uint32_t adc_ticks = 0;


/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{

    /* Go to infinite loop when Hard Fault exception occurs */
    //while(1){
    //}
	printf("Hard Fault occurs\r\n");
	delay_ms(100);
	NVIC_SystemReset();
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while(1){
    }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while(1){
    }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while(1){
    }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	adc_ticks++;
	if(adc_ticks == ADC_MS)
	{
		adc_ticks = 0;
		adc_start(true);
	}
	
	if(master)
	{
	 tx_ticks++;
   if((tx_ticks >= inter_packet_delay) && (master == true))
   {
     tx_ticks = 0;
     txpacketnumber++;
     //if((txpacketnumber <= txpacketcount) || (contTX)) tx_needed = true;
		 if(txpacketnumber <= txpacketcount) tx_needed = true;
     else
     {
       tx_needed = false;
       master = false;
       printf("SEND_PACKET: DONE\r\n");
     }
   }
	}
	//sweep procedures
//	   if(sweeprx || sweeptx) 
//   {
//     sweepcnt--;
//     if(sweepcnt == 0)
//     {
//       sweepcnt = sweepdelay;
//       sweepflag = true;
//     }
//   }
}

/**
  * @brief  This function handles PWR Handler.
  * @param  None
  * @retval None
  */
void PWR_IRQHandler()
{
}

/******************************************************************************/
/*                 Tremo Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_cm4.S).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

