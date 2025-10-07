#include "sx126x_proc.h"

const uint32_t default_freqs[CONFIG_COUNT] = {LF_FREQ_DEFAULT,HF_FREQ_DEFAULT};

float calculateLoraBw(uint8_t bw);

uint8_t opmode = 0;
uint8_t prevopmode = 0;
uint16_t irqflags = 0;
uint8_t rfstatus = 0;

/*
#define OPMODE_SLEEP            0
#define OPMODE_STBYRC           1
#define OPMODE_STBYXOSC         2
#define OPMODE_FS               3
#define OPMODE_TX               4
#define OPMODE_RX               5
#define OPMODE_TXSTREAMCW       6
#define OPMODE_TXSTREAMPRE      7
*/

int8_t SX126X_config(void)
{
	lorac_reset();
  SX126X_Wakeup();
  delay_ms(10);
  SX126X_setopmode(SX126X_OPMODE_STBYRC);
  SX126X_SetRegulatorMode(true);
  if(radioConfig.modem == SX126X_MODEM_LORA) SX126X_LoRaConfig();
  else if (radioConfig.modem == SX126X_MODEM_FSK) SX126X_FskConfig();
  else return -1; //wrong modem
  SX126X_SetRx(0xffffff); //0?
	currfreq = radioConfig.RfFreq;
	
  //SX126X_setopmode(OPMODE_STBYXOSC);
  SX126X_CalibrateIR(currfreq);
  SX126X_setopmode(SX126X_OPMODE_STBYXOSC);
  SX126X_writeReg(SX126X_REG_XTATRIM,radioConfig.CtuneA);
  SX126X_writeReg(SX126X_REG_XTBTRIM,radioConfig.CtuneB);
  
  SX126X_SetTxParams(0,0); //temporary
  SX126X_LNAboost(true); //???
  SX126X_SetDIO2AsRfSwitchCtrl(true); //???
  //SX126X_SetDioIrqParams(TXDONE_IRQMSK | RXDONE_IRQMSK | CRCERR_IRQMSK, TXDONE_IRQMSK | RXDONE_IRQMSK, 0, 0);
  SX126X_SetDioIrqParams(SX126X_TXDONE_IRQMSK | SX126X_RPEDET_IRQMSK | SX126X_SYNCDET_IRQMSK | SX126X_RXDONE_IRQMSK | SX126X_CRCERR_IRQMSK, SX126X_TXDONE_IRQMSK | SX126X_RPEDET_IRQMSK | SX126X_SYNCDET_IRQMSK | SX126X_RXDONE_IRQMSK, 0, 0);
  SX126X_setopmode(SX126X_OPMODE_RX);
  return 0;
}
//#define RFSW_CTRL_PORT	GPIOA
//#define RFSW_CTRL_PIN		GPIO_PIN_10 //GPIO10
//#define RFSW_TXRX_PORT	GPIOD
//#define RFSW_TXRX_PIN		GPIO_PIN_11 //GPIO59

int8_t SX126X_LoRaConfig(void)
{
//2. Define the protocol (LoRa� or FSK) with the command SetPacketType(...)
    SX126X_SetPacketType(SX126X_MODEM_LORA);
//3. Define the RF frequency with the command SetRfFrequency(...)
    SX126X_SetChannel();
//4. Define where the data will be stored inside the data buffer in Rx with the command SetBufferBaseAddress(...)
    SX126X_SetBufferBaseAddress(0, 0x80);
//5. Define the modulation parameter according to the chosen protocol with the command SetModulationParams(...)1
    SX126X_SetLoRaModParams(radioConfig.LoRaSf,radioConfig.LoRaBw,radioConfig.LoRaCr);
//6. Define the frame format to be used with the command SetPacketParams(...)
    SX126X_SetLoRaPacketParams(radioConfig.LoRaPreLen, radioConfig.LoRaImplHeader,radioConfig.LoRaPayLen,radioConfig.LoRaCrcOn,radioConfig.LoRaInvertIQ);
//7. Configure DIO and irq: use the command SetDioIrqParams(...) to select the IRQ RxDone and map this IRQ to a DIO (DIO1
//or DIO2 or DIO3), set IRQ Timeout as well.

    //SX126X_SetDioIrqParams(radioConfig.LoRaIRQmsk,radioConfig.LoRaDIO1msk,radioConfig.LoRaDIO2msk,radioConfig.LoRaDIO3msk);

//8. Define Sync Word value: use the command WriteReg(...) to write the value of the register via direct register access.
    SX126X_writeReg(SX126X_REG_LRSYNC_H,(radioConfig.LoRaSyncWord & 0xff00) >> 8);
    SX126X_writeReg(SX126X_REG_LRSYNC_L,radioConfig.LoRaSyncWord & 0xff);
//9. Set the circuit in reception mode: use the command SetRx(). Set the parameter to enable timeout or continuous mode
//10. Wait for IRQ RxDone2 or Timeout: the chip will stay in Rx and look for a new packet if the continuous mode is selected
//otherwise it will goes to STDBY_RC mode.
//11. In case of the IRQ RxDone, check the status to ensure CRC is correct: use the command GetIrqStatus()
    return 0;
}

int8_t SX126X_FskConfig(void)
{
  uint8_t sw[8];
  
  SX126X_SetPacketType(SX126X_MODEM_FSK);
//3. Define the RF frequency with the command SetRfFrequency(...)
  SX126X_SetChannel();
//4. Define where the data will be stored inside the data buffer in Rx with the command SetBufferBaseAddress(...)
  SX126X_SetBufferBaseAddress(0, 0x80);
  
  SX126X_SetFskModParams(radioConfig.FskBr,radioConfig.FskShaping,radioConfig.FskBw,radioConfig.FskFdev);
  //SX126X_SetFskPacketParams();
  SX126X_SetFskPacketParams(radioConfig.FskPreLen,radioConfig.FskPreDetLen,radioConfig.FskSyncLen,radioConfig.FskAddrComp,
                            radioConfig.FskVarLen,radioConfig.FskPayLen,radioConfig.FskCrcType,radioConfig.FskWhiteEn);
  //set syncword
  for(uint8_t i = 0; i < 8; i++) sw[i] = radioConfig.FskSync[i];
  SX126X_SetFskSyncWord(sw);
  SX126X_SetFskAddr(radioConfig.FskNodeAddr,radioConfig.FskBrAddr);
  //set CRC & whitening
  SX126X_SetFskCrcWhitening(radioConfig.FskCrcInit,radioConfig.FskCrcPoly,radioConfig.FskWhiteInit);
  //SX126X_writeReg(SX126X_REG_BITSYNC,0);
  return 0;
}


int8_t SX126X_initconfigstructure(uint8_t config)
{
  uint8_t i;

  //Common calibration parameters
  radioConfig.MagicNumber = MAGICNUMBER;
  radioConfig.deviceID = 0xffff;
  radioConfig.CtuneA = XOTRIM_DEFAULT;
  radioConfig.CtuneB = XOTRIM_DEFAULT;
  radioConfig.modem = SX126X_MODEM_LORA;
	radioConfig.RfBand = config;
  radioConfig.RfFreq = default_freqs[config];
  radioConfig.RfLevel = RF_LEVEL_DEFAULT;
  //LoRa config
  radioConfig.LoRaSf = 8; //spreading factor
  radioConfig.LoRaBw = SX126X_LORA_BW_41p7;//LORA_BW_  20p8; //LORA_BW_7p8; //LORA_BW_125; //bandwidth
  radioConfig.LoRaCr = SX126X_LORA_CR_4_5; //coding rate
  radioConfig.LoRaLowRateOpt = true;
  radioConfig.LoRaPreLen = 64;
  radioConfig.LoRaImplHeader = false;
  radioConfig.LoRaPayLen = 32;
  radioConfig.LoRaCrcOn = true;
  radioConfig.LoRaInvertIQ = false;
  radioConfig.LoRaSyncWord = 0x1424;
  //FSK parameters
  radioConfig.FskBr = 50000; //baud rate
  radioConfig.FskShaping = SX126X_FSK_BT0p5;
  radioConfig.FskBw = SX126X_FSK_BW_93p8;
  radioConfig.FskFdev = 25000;
  radioConfig.FskPreLen = 32;
  radioConfig.FskPreDetLen = 4; //8 bits
  radioConfig.FskSyncLen = 16;
  radioConfig.FskSync[0] = 0xc9;
  radioConfig.FskSync[1] = 0x6c;
  radioConfig.FskSync[2] = 0x23;
  radioConfig.FskSync[3] = 0x6b;
  radioConfig.FskSync[4] = 0xc9;
  radioConfig.FskSync[5] = 0x6c;
  radioConfig.FskSync[6] = 0x23;
  radioConfig.FskSync[7] = 0x6b;
  radioConfig.FskAddrComp = 0; //addr. filtering disable
  radioConfig.FskNodeAddr = 0;
  radioConfig.FskBrAddr = 0; //broadcast addr
  radioConfig.FskVarLen = false;
  radioConfig.FskPayLen = 32;
  radioConfig.FskCrcType = SX126X_FSK_CRC_1B;
  radioConfig.FskCrcInit = 0x1d0f;
  radioConfig.FskCrcPoly = 0x1021;
  radioConfig.FskWhiteEn = true;
  radioConfig.FskWhiteInit = 0x0100;
  
  radioConfig.SlaveID = 0xffff;
	writeconfig();
  printf("Config init OK.\r\n");
  return 0;
}

void SX126X_SetChannel(void) //not used
{
  //here maybe save and restore working mode
  double step = (double)FXO / 33554432;
  SX126X_SetRfFrequency((uint32_t)(radioConfig.RfFreq / step));
}

void SX126X_setopmode(uint8_t mode)
{
  switch(mode)
  {
    case SX126X_OPMODE_SLEEP:
    opmode = SX126X_OPMODE_SLEEP;
    SX126X_SetSleep(0,false);
    break;

    case SX126X_OPMODE_STBYRC:
    opmode = SX126X_OPMODE_STBYRC;
    SX126X_SetStandby(0);
    break;

    case SX126X_OPMODE_STBYXOSC:
    opmode = SX126X_OPMODE_STBYXOSC;
    SX126X_SetStandby(1);
    break;

    case SX126X_OPMODE_FS:
    opmode = SX126X_OPMODE_FS;
    SX126X_SetFs();
    break;

    case SX126X_OPMODE_TX:
    opmode = SX126X_OPMODE_TX;
    SX126X_SetTx(0xffffff); //0?
    break;

    case SX126X_OPMODE_RX:
    default:
    opmode = SX126X_OPMODE_RX;
    //SX126X_SetRx(radioConfig.LoRaRxTimeout); //temp.
    SX126X_SetRx(0); //temp.
    break;

    case SX126X_OPMODE_TXSTREAMCW:
    opmode = SX126X_OPMODE_TXSTREAMCW;
    SX126X_SetCW();
    break;

    case SX126X_OPMODE_TXSTREAMPRE:
    opmode = SX126X_OPMODE_TXSTREAMPRE;
    SX126X_SetTxInfinitePreamble();
    break;
  }
}

//boost LNA
void SX126X_LNAboost(bool boost)
{
  if(boost) SX126X_writeReg(0x08ac,0x96);
  else SX126X_writeReg(0x08ac,0x94);
}

//calibrate IR according to RF ftequency
//The calibration frequencies are computed as follows:
//Calibration freq = CalFreq * 4 MHz where CalFreq1 < CalFreq2
//Example: 0x6B = 428 MHz.
//When CalFreq1 = CalFreq2, the image calibration is done at a single frequency.
//For frequencies between CalFreq1 and CalFreq2, the calibration coefficient is linearly
//interpolated from the values obtained during the image calibration at CalFreq1 and CalFreq2.
//For frequencies < CalFreq1, the coefficient obtained during the image calibration at CalFreq1 is used.
//For frequencies > CalFreq2, the coefficient obtained during the image calibration at CalFreq2 is used.

void SX126X_CalibrateIR(uint32_t frequency)
{
  uint8_t f1,f2;
  uint32_t f;
  f = frequency / 1000000UL;
  if(f > 1020) f = 1020;
  f1 = (uint8_t)(f / 4);
  f = (f * 3) / 2; //probably 1.5 times
  if(f > 1020) f = 1020;
  f2 = (uint8_t)(f / 4);
  SX126X_CalibrateImage(f1,f2);
  //restore Ctune needed
}

float SX126X_GetFreqOffset(void)
{
//from radiolib
//  uint8_t efeRaw[3] = {0};
//  int16_t state = readRegister(SX126X_REG_FREQ_ERROR_RX_CRC, &efeRaw[0], 1);
//  ASSERT(state);
//  state = readRegister(SX126X_REG_FREQ_ERROR_RX_CRC + 1, &efeRaw[1], 1);
//  ASSERT(state);
//  state = readRegister(SX126X_REG_FREQ_ERROR_RX_CRC + 2, &efeRaw[2], 1);
//  ASSERT(state);
//  uint32_t efe = ((uint32_t) efeRaw[0] << 16) | ((uint32_t) efeRaw[1] << 8) | efeRaw[2];
//  efe &= 0x0FFFFF;
//  // check the first bit
//  if (efe & 0x80000) 
//	{
//    // frequency error is negative
//    efe |= (uint32_t) 0xFFF00000;
//    efe = ~efe + 1;
//    error = 1.55f * (float) efe / (1600.0f / (float) this->bandwidthKhz) * -1.0f;
//  } 
//	else error = 1.55f * (float) efe / (1600.0f / (float) this->bandwidthKhz);
//  return error;

	uint8_t buf[3] = {0};
	uint32_t efe;
	float bw_kHz;
	SX126X_readRegs(SX126X_REG_FREQ_ERROR_RX_CRC,buf,3);
	efe = ((uint32_t)buf[0] << 16) | ((uint32_t)buf[1] << 8) | buf[2];
	efe &= 0x0fffff;
	//uint8_t LoRaBw
	bw_kHz = calculateLoraBw(radioConfig.LoRaBw);
  float error = 0;
  if (efe & 0x80000) 
	{
    // frequency error is negative
    efe |= (uint32_t) 0xfff00000;
    efe = ~efe + 1;
    //error = 1.55f * (float)efe / (1600.0f / bw_kHz) * -1.0f;
		error = -1.55f * (float)efe / (1600.0f / bw_kHz);
  } 
	else error = 1.55f * (float)efe / (1600.0f / bw_kHz);
  return error;
}

float calculateLoraBw(uint8_t bw)
{
	switch(bw)
	{
		case SX126X_LORA_BW_7p8:             //0
		default:
		return 7.8125;
		
		case SX126X_LORA_BW_10p4:            //0x08
		return 10.4167;
		
		case SX126X_LORA_BW_15p6:            //0x01
		return 15.625;
		
		case SX126X_LORA_BW_20p8:            //0x09
		return 20.8333;
		
		case SX126X_LORA_BW_31p3:            //0x02
		return 31.25;
		
		case SX126X_LORA_BW_41p7:            //0x0a
		return 41.6667;
		
		case SX126X_LORA_BW_62p5:            //0x03
		return 62.5;
		
		case SX126X_LORA_BW_125:             //0x04
		return 125.0;
		
		case SX126X_LORA_BW_250:             //0x05
		return 250.0;
		
		case SX126X_LORA_BW_500:             //0x06
		return 500.0;
	}
}
