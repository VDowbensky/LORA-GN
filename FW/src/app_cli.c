#include <stdio.h>
#include "app_cli.h"
#include "bsp.h"
#include "radio_proc.h"
#include "adc.h"
#include "flash.h"
#include "atten.h"
#include "txlevel.h"
//#include "gui.h"
//#include "gpsparser.h"


CommandState_t state;
char ciBuffer[256];
//uint8_t printmode = 0;
uint8_t txmode = 0;

//General
void cli_reset(int argc, char **argv);
void cli_info(int argc, char **argv);
void cli_getver(int argc, char **argv);
void cli_getdevid(int argc, char **argv);
void cli_setdevid(int argc, char **argv);

//RF settings
void cli_getmodem(int argc, char **argv);
void cli_setmodem(int argc, char **argv);
void cli_getopmode(int argc, char **argv);
void cli_setopmode(int argc, char **argv);
void cli_getfreq(int argc, char **argv);
void cli_setfreq(int argc, char **argv);

void cli_getpower(int argc, char **argv);
void cli_setpower(int argc, char **argv);

void cli_getrflevel(int argc, char **argv);
void cli_setrflevel(int argc, char **argv);
void cli_getatt(int argc, char **argv);
void cli_setatt(int argc, char **argv);

void cli_getxotrim(int argc, char **argv);
void cli_setxotrim(int argc, char **argv);

void cli_getRSSI(int argc, char **argv);
void cli_getfreqoffset(int argc, char **argv);

void cli_getstatus(int argc, char **argv);

//LoRa
void cli_lr_getmodparams(int argc, char **argv);
void cli_lr_setmodparams(int argc, char **argv);
void cli_lr_getpacketparams(int argc, char **argv);
void cli_lr_setpacketparams(int argc, char **argv);
void cli_lr_getsync(int argc, char **argv);
void cli_lr_setsync(int argc, char **argv);

//FSK
//baudrate,pulse shape,RXBW,deviation
void cli_fs_getmodparams(int argc, char **argv);
void cli_fs_setmodparams(int argc, char **argv);
//prelen, predetlen,synclen,addr.filt,varlen,paylen,crctype,whitening
void cli_fs_getpacketparams(int argc, char **argv);
void cli_fs_setpacketparams(int argc, char **argv);
//node and broadcast addr.
void cli_fs_getaddr(int argc, char **argv);
void cli_fs_setaddr(int argc, char **argv);
//sync word(s)
void cli_fs_getsync(int argc, char **argv);
void cli_fs_setsync(int argc, char **argv);
//CRC init, CRC poly, whitening init
void cli_fs_getcrcwhite(int argc, char **argv);
void cli_fs_setcrcwhite(int argc, char **argv);


//debug functions
void cli_readreg(int argc, char **argv);
void cli_writereg(int argc, char **argv);
void cli_initconfig(int argc, char **argv);
void cli_storeconfig(int argc, char **argv);

void cli_dumpregs(int argc, char **argv);

void cli_sendburst(int argc, char **argv);
void cli_stopburst (int argc, char **argv);
void cli_txstream(int argc, char **argv);
void cli_startrxcount(int argc, char **argv);
void cli_stoprxcount(int argc, char **argv);
void cli_getrxcount(int argc, char **argv);

//device dependent
void cli_getvt(int argc, char **argv);

//special tests
void cli_sweeptx(int argc, char **argv);
void cli_sweeprx(int argc, char **argv);

void cli_setpaconfig(int argc, char **argv); //technological
void cli_getpaconfig(int argc, char **argv); //technological


CommandEntry_t commands[] =
  {
    //general
    COMMAND_ENTRY("RESET", "", cli_reset, "Device reset"),
    COMMAND_ENTRY("GET_UID", "", cli_info, "Get CPU ID"),
    COMMAND_ENTRY("GET_VERSION", "", cli_getver, "Get HW/FW version"),
    COMMAND_ENTRY("GET_DEVID", "", cli_getdevid, "Get device ID"),
    COMMAND_ENTRY("SET_DEVID", "w", cli_setdevid, "Get device ID"),
    //RF settings
    COMMAND_ENTRY("GET_MODEM", "", cli_getmodem, "Get packet type"),
    COMMAND_ENTRY("SET_MODEM", "v", cli_setmodem, "Set packet type (LoRa or FSK)"),
    COMMAND_ENTRY("GET_OPMODE", "", cli_getopmode, "Get operation mode"),
    COMMAND_ENTRY("SET_OPMODE", "v", cli_setopmode, "Set operation mode"),
    COMMAND_ENTRY("GET_FREQ", "", cli_getfreq, "Get frequency settings"),
    COMMAND_ENTRY("SET_FREQ", "w", cli_setfreq, "Set frequency settings"),

    COMMAND_ENTRY("GET_POWER", "", cli_getpower, "Get TX power"),
    COMMAND_ENTRY("SET_POWER", "w", cli_setpower, "Set TX power"),
		
		COMMAND_ENTRY("GET_RFLEVEL", "", cli_getrflevel, "Get RF level in dBm"),
    COMMAND_ENTRY("SET_RFLEVEL", "f", cli_setrflevel, "Set RF level in dBm"),
		COMMAND_ENTRY("GET_ATT", "", cli_getatt, "Get attenuation value in dB"),
    COMMAND_ENTRY("SET_ATT", "wf", cli_setatt, "Get attenuation value in dB"),
		
    COMMAND_ENTRY("GET_XOTRIM", "", cli_getxotrim, "Get XO trim value"),
    COMMAND_ENTRY("SET_XOTRIM", "w", cli_setxotrim, "Set XO trim value, 0...94"),

    COMMAND_ENTRY("GET_RSSI", "", cli_getRSSI, "Get RSSI value"),
		COMMAND_ENTRY("GET_FREQOFFSET", "", cli_getfreqoffset, "Get frequency offset"),
    
    COMMAND_ENTRY("GET_STATUS", "", cli_getstatus, "Get transceiver status"),

    //LoRa
    COMMAND_ENTRY("LR_GET_MODPARAMS", "", cli_lr_getmodparams, "Get LoRa modulation parameters"),
    COMMAND_ENTRY("LR_SET_MODPARAMS", "wwww", cli_lr_setmodparams, "Set LoRa modulation parameters"),
    COMMAND_ENTRY("LR_GET_PACKETPARAMS", "", cli_lr_getpacketparams, "Get LoRa packet parameters"),
    COMMAND_ENTRY("LR_SET_PACKETPARAMS", "wwwww", cli_lr_setpacketparams, "Set LoRa packet parameters"),
    COMMAND_ENTRY("LR_GET_SYNC", "", cli_lr_getsync, "Get LoRa sync word"),
    COMMAND_ENTRY("LR_SET_SYNC", "w", cli_lr_setsync, "Set LoRa sync word"),
                        
    //FSK
    COMMAND_ENTRY("FS_GET_MODPARAMS", "", cli_fs_getmodparams, "Get FSK modulation parameters"),
    COMMAND_ENTRY("FS_SET_MODPARAMS", "wwww", cli_fs_setmodparams, "Set FSK modulation parameters"),
    COMMAND_ENTRY("FS_GET_PACKETPARAMS", "", cli_fs_getpacketparams, "Get FSK packet parameters"),
    COMMAND_ENTRY("FS_SET_PACKETPARAMS", "wwwwwwww", cli_fs_setpacketparams, "Set FSK packet parameters"),
    COMMAND_ENTRY("FS_GET_ADDR", "", cli_fs_getaddr, "Get FSK node and broadcast addresses"),
    COMMAND_ENTRY("FS_SET_ADDR", "ww", cli_fs_setaddr, "Set FSK node and broadcast addresses"),
    COMMAND_ENTRY("FS_GET_SYNC", "", cli_fs_getsync, "Get FSK sync word"),
    COMMAND_ENTRY("FS_SET_SYNC", "wwwwwwww", cli_fs_setsync, "Set FSK sync word"),
    COMMAND_ENTRY("FS_GET_CRCWH", "", cli_fs_getcrcwhite, "Get FSK CRC/WH parameters"),
    COMMAND_ENTRY("FS_SET_CRCWH", "www", cli_fs_setcrcwhite, "Set FSK CRC/WH parameters"),

    //RF tests
    COMMAND_ENTRY("START_TX", "www", cli_sendburst, "Start packet burst"),
    COMMAND_ENTRY("STOP_TX", "", cli_stopburst, "Stop packet burst"),

    COMMAND_ENTRY("TX_STREAM", "v", cli_txstream, "Start/stop TX stream"),
    COMMAND_ENTRY("START_RXCOUNT", "", cli_startrxcount, "Start RX packet count"),
    COMMAND_ENTRY("STOP_RXCOUNT", "", cli_stoprxcount, "Stop RX packet count"),
    COMMAND_ENTRY("GET_RXCOUNT", "", cli_getrxcount, "Get RX packet count"),
                        
    //System health - device dependent
    COMMAND_ENTRY("GET_VT", "", cli_getvt, "Get ADC data"),

    COMMAND_ENTRY("READ_REG", "w", cli_readreg, "Read register"),
    COMMAND_ENTRY("WRITE_REG", "ww", cli_writereg, "Write register"),
    COMMAND_ENTRY("DUMP_REGS", "ww", cli_dumpregs, "Dump registers (from...to"),
                        
    COMMAND_ENTRY("SWEEP_TX", "wwwww", cli_sweeptx, "TX sweep"),
    COMMAND_ENTRY("SWEEP_RX", "wwwww", cli_sweeprx, "RX scan"),

    COMMAND_ENTRY("INIT_CONFIG", "w", cli_initconfig, "Set default configuration"),
		COMMAND_ENTRY("STORE_CONFIG", "", cli_storeconfig, "Store configuration"),
    COMMAND_ENTRY("SET_PACONFIG", "ww", cli_setpaconfig, "Set PA configuration"), //technological
		COMMAND_ENTRY("GET_PACONFIG", "", cli_getpaconfig, "Set PA configuration"), //technological
		
		COMMAND_ENTRY(NULL, NULL, NULL, NULL)
  };


void cli_init(void)
{
  ciInitState(&state, ciBuffer, sizeof(ciBuffer), commands);
}

void cli_proc(void)
{
  char input = RETARGET_ReadChar();
  if(input != '\0' && input != 0xFF) ciProcessInput(&state, &input, 1);
}

void ciErrorCallback(char* command, CommandError_t error)
{
  if (error == CI_UNKNOWN_COMMAND) {
    printf("INVALID COMMAND\r\n");
  } else if (error == CI_MAX_ARGUMENTS) {
    printf("TOO MANY ARGUMENTS\r\n");
  } else if (error == CI_INVALID_ARGUMENTS) {
    printf("INVALID ARGUMENT\r\n");
  }
}


//cli functions implementation
//General
void cli_reset(int argc, char **argv)
{
  printf("RESET: OK\r\n");
	delay_ms(100);
	NVIC_SystemReset();
}

void cli_info(int argc, char **argv)
{
  uint64_t id;
	id = *(uint64_t*)0x4002002c;
	//id = EFC->SN_L | ((uint64_t)EFC->SN_H << 32);
	printf("GET_UID: 0x%llX\r\n", id);
}

void cli_getver(int argc, char **argv)
{
  printf("GET_VERSION: HW=%d,FW=%d.%d\r\n", HW_VERSION, FW_VERSION, FW_REVISION);
}

void cli_getdevid(int argc, char **argv)
{
  printf("GET_DEVID: 0x%04X\r\n", radioConfig.deviceID);
}

void cli_setdevid(int argc, char **argv)
{
  uint16_t id;
  id = ciGetUnsigned(argv[1]);
  radioConfig.deviceID = id;
  writeconfig();
  printf("SET_DEVID: 0x%04X\r\n", radioConfig.deviceID);
}


//RF settings
void cli_getmodem(int argc, char **argv)
{
  if(radioConfig.modem == SX126X_MODEM_LORA) printf("GET_MODEM: LORA\r\n");
  else printf("GET_MODEM: FSK\r\n");
}

void cli_setmodem(int argc, char **argv)
{
  //0 - FSK, 1 - LoRa
  uint8_t m = ciGetUnsigned(argv[1]);
  if((m == SX126X_MODEM_FSK) || (m == SX126X_MODEM_LORA))
  {
    radioConfig.modem = m;
    SX126X_config();
    if(m == SX126X_MODEM_FSK) printf("SET_MODEM: FSK\r\n");
    else printf("SET_MODEM: LORA\r\n");
  }
  else printf("SET_MODEM: ERROR\r\n");
}

void cli_getopmode(int argc, char **argv)
{
  switch(opmode)
  {
    case SX126X_OPMODE_SLEEP:
    printf("GET_OPMODE: SLEEP\r\n");
    break;

    case SX126X_OPMODE_STBYRC:
    printf("GET_OPMODE: STANDBY RC\r\n");
    break;

    case SX126X_OPMODE_STBYXOSC:
    printf("GET_OPMODE: STANDBY XOSC\r\n");
    break;

    case SX126X_OPMODE_FS:
    printf("GET_OPMODE: SYNTH\r\n");
    break;

    case SX126X_OPMODE_TX:
    printf("GET_OPMODE: TX\r\n");
    break;

    case SX126X_OPMODE_RX:
    default:
    printf("GET_OPMODE: RX\r\n");
    break;

    case SX126X_OPMODE_TXSTREAMCW:
    printf("GET_OPMODE: TX CW\r\n");
    break;

    case SX126X_OPMODE_TXSTREAMPRE:
    printf("GET_OPMODE: TX PREAMBLE\r\n");
    break;
  }
}

void cli_setopmode(int argc, char **argv)
{
  uint8_t mode;
  mode = ciGetUnsigned(argv[1]);
  prevopmode = opmode;
  SX126X_setopmode(mode);
  switch(opmode)
  {
    case SX126X_OPMODE_SLEEP:
    SX126X_setopmode(SX126X_OPMODE_SLEEP);
    printf("SET_OPMODE: SLEEP\r\n");
    break;

    case SX126X_OPMODE_STBYRC:
    SX126X_setopmode(SX126X_OPMODE_STBYRC);
    printf("SET_OPMODE: STANDBY RC\r\n");
    break;

    case SX126X_OPMODE_STBYXOSC:
    SX126X_setopmode(SX126X_OPMODE_STBYXOSC);
    printf("SET_OPMODE: STANDBY XOSC\r\n");
    break;

    case SX126X_OPMODE_FS:
    SX126X_setopmode(SX126X_OPMODE_FS);
    printf("SET_OPMODE: SYNTH\r\n");
    break;

    case SX126X_OPMODE_TX:
    SX126X_setopmode(SX126X_OPMODE_TX);
    printf("SET_OPMODE: TX\r\n");
    break;

    case SX126X_OPMODE_RX:
    default:
    SX126X_setopmode(SX126X_OPMODE_RX);
    printf("SET_OPMODE: RX\r\n");
    break;

    case SX126X_OPMODE_TXSTREAMCW:
    SX126X_setopmode(SX126X_OPMODE_TXSTREAMCW);
    printf("SET_OPMODE: TX CW\r\n");
    break;

    case SX126X_OPMODE_TXSTREAMPRE:
    SX126X_setopmode(SX126X_OPMODE_TXSTREAMPRE);
    printf("SET_OPMODE: TX PREAMBLE\r\n");
    break;
  }
}

void cli_getfreq(int argc, char **argv)
{
  //printf("GET_FREQ: %u\r\n",radioConfig.RfFreq/1000);
  printf("GET_FREQ: %u,%u\r\n",radioConfig.RfFreq/1000,currfreq);
}

void cli_setfreq(int argc, char **argv)
{
  currfreq = ciGetUnsigned(argv[1]);
	radioConfig.RfFreq = currfreq * 1000;
  printf("SET_FREQ: %u\r\n",currfreq);
	RADIO_SetAbsoluteFrequency(currfreq);
	setrflevel();
}

void cli_getrflevel(int argc, char **argv)
{
	printf("GET_RFLEVEL: %.1fdBm\r\n",rflevel);
}

void cli_setrflevel(int argc, char **argv)
{
  rflevel = ciGetFloat(argv[1]);
	//if(rflevel < -132.0) rflevel = -132.0;
	//if(rflevel > -20.0) rflevel = -20.0;
	setrflevel();
	radioConfig.RfLevel = rflevel;
	printf("SET_RFLEVEL: %.1fdBm\r\n",rflevel);
}

void cli_getatt(int argc, char **argv)
{
	//uint8_t att = ciGetSigned(argv[1]);
	//if(att > 1) att = 1;
	printf("GET_ATT: %.1f,%.1f\r\n",((float)atten[0]) / 2,((float)atten[1]) / 2);
}

void cli_setatt(int argc, char **argv)
{
  uint8_t att = ciGetSigned(argv[1]);
	float val = ciGetFloat(argv[2]);
	if(att > 1) att = 1;
	if(val < 0) val = 0;
	if(val > 31.5) val = 31.5;
	atten[att] = (uint8_t)(val * 2);
	atten_set(att,atten[att]);
	printf("GET_ATT: %.1fdB\r\n",((float)atten[att]) / 2);
}

void cli_getxotrim(int argc, char **argv)
{
  uint8_t ctunea, ctuneb;

  ctunea = SX126X_readReg(SX126X_REG_XTATRIM);
  ctuneb = SX126X_readReg(SX126X_REG_XTBTRIM);
  printf("GET_XOTRIM: %d\r\n", ctunea + ctuneb);
}

void cli_setxotrim(int argc, char **argv)
{
  uint8_t tune;

  tune = ciGetUnsigned(argv[1]);
  if(tune > 94) tune = 94;
  RADIO_setctune(tune);
  printf("SET_XOTRIM: %d\r\n",tune);
}

void cli_getRSSI(int argc, char **argv)
{
  float rssi;
  rssi = -((float)SX126X_GetRssiInst()/2);
  printf("GET_RSSI: %.1f dBm\r\n",rssi);
}

void cli_getfreqoffset(int argc, char **argv)
{
	printf("GET_FREQOFFSET: %.3f\r\n",SX126X_GetFreqOffset());
}


void cli_getstatus(int argc, char **argv)
{
  uint8_t s = SX126X_GetStatus();
  printf("GET_STATUS: 0x%02X\r\n",s);
}

//LoRa

void cli_lr_getmodparams(int argc, char **argv)
{
  float bw_kHz;

  if(radioConfig.LoRaBw == SX126X_LORA_BW_7p8) bw_kHz = 7.8;
  else if(radioConfig.LoRaBw == SX126X_LORA_BW_10p4) bw_kHz = 10.4;
  else if(radioConfig.LoRaBw == SX126X_LORA_BW_15p6) bw_kHz = 15.6;
  else if(radioConfig.LoRaBw == SX126X_LORA_BW_20p8) bw_kHz = 20.8;
  else if(radioConfig.LoRaBw == SX126X_LORA_BW_31p3) bw_kHz = 31.3;
  else if(radioConfig.LoRaBw == SX126X_LORA_BW_41p7) bw_kHz = 41.7;
  else if(radioConfig.LoRaBw == SX126X_LORA_BW_62p5) bw_kHz = 62.5;
  else if(radioConfig.LoRaBw == SX126X_LORA_BW_125) bw_kHz = 125.0;
  else if(radioConfig.LoRaBw == SX126X_LORA_BW_250) bw_kHz = 250.0;
  else if(radioConfig.LoRaBw == SX126X_LORA_BW_500) bw_kHz = 500.0;
  else bw_kHz = -1.0;
  printf("LR_GET_MODPARAMS:\r\nBW=%.1f\r\nSF=%d\r\n",bw_kHz,radioConfig.LoRaSf);
  printf("CR:");
    switch(radioConfig.LoRaCr)
    {
      case 0:
      printf("OFF\r\n");
      break;
      case 1:
      printf("4_5\r\n");
      break;
      case 2:
      printf("4_6\r\n");
      break;
      case 3:
      printf("4_7\r\n");
      break;
      case 4:
      printf("4_8\r\n");
      break;
      default:
      printf("INVALID\r\n");
      break;
    }
  printf("LROPT:");
  if(radioConfig.LoRaLowRateOpt == true) printf("ON\r\n");
  else printf("OFF\r\n");
}

void cli_lr_setmodparams(int argc, char **argv)
{
  if(radioConfig.modem == SX126X_MODEM_LORA)
  {
    uint8_t bw_val,sf,cr,opt;
    uint16_t bw;
    bw = ciGetUnsigned(argv[1]);
    float bw_kHz;
    //BW in kHz
    if(bw <= 8) {bw_val = SX126X_LORA_BW_7p8; bw_kHz = 7.8;}
    else if(bw <= 11) {bw_val = SX126X_LORA_BW_10p4; bw_kHz = 10.4;}
    else if(bw <= 16) {bw_val = SX126X_LORA_BW_15p6; bw_kHz = 15.6;}
    else if(bw <= 21) {bw_val = SX126X_LORA_BW_20p8; bw_kHz = 20.8;}
    else if(bw <= 31) {bw_val = SX126X_LORA_BW_31p3; bw_kHz = 31.3;}
    else if(bw <= 42) {bw_val = SX126X_LORA_BW_41p7; bw_kHz = 41.7;}
    else if(bw <= 63) {bw_val = SX126X_LORA_BW_62p5; bw_kHz = 62.5;}
    else if(bw <= 125) {bw_val = SX126X_LORA_BW_125; bw_kHz = 125.0;}
    else if(bw <= 250) {bw_val = SX126X_LORA_BW_250; bw_kHz = 250.0;}
    else {bw_val = SX126X_LORA_BW_500; bw_kHz = 500.0;}

    sf = ciGetUnsigned(argv[2]);
    //check
    if(sf < 6) sf = 6;
    if(sf > 12) sf = 12;
    cr = ciGetUnsigned(argv[3]);
    //check
    if(cr > 4) cr = 4;
    opt = ciGetUnsigned(argv[4]);
    radioConfig.LoRaBw = bw_val;
    radioConfig.LoRaSf = sf;
    radioConfig.LoRaCr = cr;
    if(opt != 0) radioConfig.LoRaLowRateOpt = true;
    else radioConfig.LoRaLowRateOpt = false;
    SX126X_config();
    printf("LR_SET_MODPARAMS:\r\nBW=%.1f\r\nSF=%d\r\n",bw_kHz,radioConfig.LoRaSf);
    printf("CR:");
    switch(radioConfig.LoRaCr)
    {
      case 0:
      printf("OFF\r\n");
      break;
      case 1:
      printf("4_5\r\n");
      break;
      case 2:
      printf("4_6\r\n");
      break;
      case 3:
      printf("4_7\r\n");
      break;
      case 4:
      printf("4_8\r\n");
      break;
      default:
      printf("INVALID\r\n");
      break;
    }
    printf("LROPT:");
    if(radioConfig.LoRaLowRateOpt == true) printf("ON\r\n");
    else printf("OFF\r\n");
  }
  else printf("LR_SET_MODPARAMS: ERROR\r\n");
}

void cli_lr_getpacketparams(int argc, char **argv)
{
  printf("LR_GET_PACKETPARAMS:\r\nPRE_LEN=%d\r\nPAY_LEN=%d\r\nHEADER:",radioConfig.LoRaPreLen,radioConfig.LoRaPayLen);
  if(radioConfig.LoRaImplHeader == true) printf("IMPLICIT\r\n");
  else printf("EXPLICIT\r\n");
  printf("CRC:");
  if(radioConfig.LoRaCrcOn == true) printf("ON\r\n");
  else printf("OFF\r\n");
  printf("INVERTIQ:");
  if(radioConfig.LoRaInvertIQ == true) printf("ON\r\n");
  else printf("OFF\r\n");
}

void cli_lr_setpacketparams(int argc, char **argv)
{
  if(radioConfig.modem == SX126X_MODEM_LORA)
  {
    uint16_t pre;
    uint8_t pay;
    uint8_t head;
    uint8_t crc;
    uint8_t inviq;
    pre = ciGetUnsigned(argv[1]) & 0xffff;
    pay = ciGetUnsigned(argv[2]) & 0xff;
    head = ciGetUnsigned(argv[3]) & 0x1;
    crc = ciGetUnsigned(argv[4]) & 0x1;
    inviq = ciGetUnsigned(argv[5]) & 0x1;
    radioConfig.LoRaPreLen = pre;
    radioConfig.LoRaPayLen = pay;
    if(head != 0) radioConfig.LoRaImplHeader = true;
    else radioConfig.LoRaImplHeader = false;
    if(crc != 0) radioConfig.LoRaCrcOn = true;
    else radioConfig.LoRaCrcOn = false;
    if(inviq != 0) radioConfig.LoRaInvertIQ = true;
    else radioConfig.LoRaInvertIQ = false;
    SX126X_config();
    printf("LR_SET_PACKETPARAMS:\r\nPRE_LEN=%d\r\nPAY_LEN=%d\r\nHEADER:",radioConfig.LoRaPreLen,radioConfig.LoRaPayLen);
    if(radioConfig.LoRaImplHeader == true) printf("IMPLICIT\r\n");
    else printf("EXPLICIT\r\n");
    printf("CRC:");
    if(radioConfig.LoRaCrcOn == true) printf("ON\r\n");
    else printf("OFF\r\n");
    printf("INVERTIQ:");
    if(radioConfig.LoRaInvertIQ == true) printf("ON\r\n");
    else printf("OFF\r\n");
  }
  else printf("LR_SET_PACKETPARAMS: ERROR\r\n");
}

void cli_lr_getsync(int argc, char **argv)
{
  printf("LR_GET_SYNC: 0x%04X\r\n",radioConfig.LoRaSyncWord);
}

void cli_lr_setsync(int argc, char **argv)
{
  if(radioConfig.modem == SX126X_MODEM_LORA)
  {
    uint16_t sw;
    sw = ciGetUnsigned(argv[1]) & 0xffff;
    radioConfig.LoRaSyncWord = sw;
    printf("LR_SET_SYNC: 0x%04X\r\n",radioConfig.LoRaSyncWord);
  }
  else printf("LR_SET_SYNC: ERROR\r\n");
}

void cli_lr_getsymboltimeout(int argc, char **argv)
{
  printf("LR_GET_SYMTO: IN DEVELOPMENT\r\n");
}

void cli_lr_setsymboltimeout(int argc, char **argv)
{
  if(radioConfig.modem == SX126X_MODEM_LORA)
  {
    printf("LR_SET_SYMTO: IN DEVELOPMENT\r\n");
  }
  else printf("LR_SET_SYMTO: ERROR\r\n");
}

//FSK
//baudrate,pulse shape,RXBW,deviation
void cli_fs_getmodparams(int argc, char **argv)
{
  float bw_kHz;

  if(radioConfig.FskBw == SX126X_FSK_BW_4p8) bw_kHz = 4.8;
  else if(radioConfig.FskBw == SX126X_FSK_BW_5p8) bw_kHz = 5.8;
  else if(radioConfig.FskBw == SX126X_FSK_BW_7p3) bw_kHz = 7.3;
  else if(radioConfig.FskBw == SX126X_FSK_BW_11p7) bw_kHz = 11.7;
  else if(radioConfig.FskBw == SX126X_FSK_BW_19p5) bw_kHz = 19.5;
  else if(radioConfig.FskBw == SX126X_FSK_BW_23p4) bw_kHz = 23.4;
  else if(radioConfig.FskBw == SX126X_FSK_BW_29p3) bw_kHz = 29.3;
  else if(radioConfig.FskBw == SX126X_FSK_BW_39) bw_kHz = 39.0;
  else if(radioConfig.FskBw == SX126X_FSK_BW_46p9) bw_kHz = 46.9;
  else if(radioConfig.FskBw == SX126X_FSK_BW_58p6) bw_kHz = 58.6;
  else if(radioConfig.FskBw == SX126X_FSK_BW_78p2) bw_kHz = 78.2;
  else if(radioConfig.FskBw == SX126X_FSK_BW_93p8) bw_kHz = 93.8;
  else if(radioConfig.FskBw == SX126X_FSK_BW_117p3) bw_kHz = 117.3;
  else if(radioConfig.FskBw == SX126X_FSK_BW_156p2) bw_kHz = 156.2;
  else if(radioConfig.FskBw == SX126X_FSK_BW_187p2) bw_kHz = 187.2;
  else if(radioConfig.FskBw == SX126X_FSK_BW_234p3) bw_kHz = 234.3;
  else if(radioConfig.FskBw == SX126X_FSK_BW_312) bw_kHz = 312.0;
  else if(radioConfig.FskBw == SX126X_FSK_BW_373p6) bw_kHz = 373.6;
  else if(radioConfig.FskBw == SX126X_FSK_BW_467) bw_kHz = 467.0;
  else bw_kHz = -1.0;
  printf("FS_GETMODPARAMS:\r\nBR=%d\r\nSH=%d\r\nBW=%.1f\r\nDEV=%d\r\n",radioConfig.FskBr,radioConfig.FskShaping,bw_kHz,radioConfig.FskFdev);
}

void cli_fs_setmodparams(int argc, char **argv)
{
  if(radioConfig.modem == SX126X_MODEM_FSK)
  {
    uint32_t br;
    uint8_t sh;
    uint32_t bw;
    uint32_t dev;
    float bw_kHz;
    uint8_t bw_val;
    
    
    br = ciGetUnsigned(argv[1]); //1,2,3 - baudrate
    //check
    if(br < 600) br = 600;
    if(br > 300000) br = 300000;
    sh = ciGetUnsigned(argv[2]); //4 - shaping
    //check
    bw = ciGetUnsigned(argv[3]); //5 - RX BW
    if(bw <= 5) {bw_val = SX126X_FSK_BW_4p8; bw_kHz = 4.8;}
    else if(bw <= 6) {bw_val = SX126X_FSK_BW_5p8; bw_kHz = 5.8;}
    else if(bw <= 8) {bw_val = SX126X_FSK_BW_7p3; bw_kHz = 7.3;}
    else if(bw <= 10) {bw_val = SX126X_FSK_BW_9p7; bw_kHz = 9.7;}
    else if(bw <= 12) {bw_val = SX126X_FSK_BW_11p7; bw_kHz = 11.7;}
    else if(bw <= 15) {bw_val = SX126X_FSK_BW_14p6; bw_kHz = 14.6;}
    else if(bw <= 20) {bw_val = SX126X_FSK_BW_19p5; bw_kHz = 19.5;}
    else if(bw <= 24) {bw_val = SX126X_FSK_BW_23p4; bw_kHz = 23.4;}
    else if(bw <= 30) {bw_val = SX126X_FSK_BW_29p3; bw_kHz = 29.3;}
    else if(bw <= 39) {bw_val = SX126X_FSK_BW_39; bw_kHz = 39.0;}
    else if(bw <= 47) {bw_val = SX126X_FSK_BW_46p9; bw_kHz = 46.9;}
    else if(bw <= 59) {bw_val = SX126X_FSK_BW_58p6; bw_kHz = 58.6;}
    else if(bw <= 79) {bw_val = SX126X_FSK_BW_78p2; bw_kHz = 78.2;}
    else if(bw <= 94) {bw_val = SX126X_FSK_BW_93p8; bw_kHz = 93.8;}
    else if(bw <= 118) {bw_val = SX126X_FSK_BW_117p3; bw_kHz = 117.3;}
    else if(bw <= 157) {bw_val = SX126X_FSK_BW_156p2; bw_kHz = 156.2;}
    else if(bw <= 188) {bw_val = SX126X_FSK_BW_187p2; bw_kHz = 187.2;}
    else if(bw <= 235) {bw_val = SX126X_FSK_BW_234p3; bw_kHz = 234.3;}
    else if(bw <= 312) {bw_val = SX126X_FSK_BW_312; bw_kHz = 312.0;}
    else if(bw <= 374) {bw_val = SX126X_FSK_BW_373p6; bw_kHz = 373.6;}
    else  {bw_val = SX126X_FSK_BW_467; bw_kHz = 467.0;}

    dev = ciGetUnsigned(argv[4]); //6,7,8 - Fdev
    //check
    if(dev < 600) dev = 600;
    if(dev > 200000) dev = 200000;
    radioConfig.FskBr = br;
    radioConfig.FskShaping = sh;
    radioConfig.FskBw = bw_val;
    radioConfig.FskFdev = dev;
    SX126X_config();
    printf("FS_SETMODPARAMS:\r\nBR=%d\r\nSH=%d\r\nBW=%.1f\r\nDEV=%d\r\n",radioConfig.FskBr,radioConfig.FskShaping,bw_kHz,radioConfig.FskFdev);
  }
  else printf("FS_SET_MODPARAMS: ERROR\r\n");
	//printf("FS_SET_MODPARAMS: TODO\r\n");
}

//prelen, predetlen,synclen,addr.filt,varlen,paylen,crctype,whitening
void cli_fs_getpacketparams(int argc, char **argv) //bool FskVarLen, bool FskWhiteEn,uint8_t FskCrcType;
{
  printf("FS_GET_PACKETPARAMS:\r\nPRE_LEN=%d\r\nPRE_DET=%d\r\nSYNC_LEN=%d\r\n",radioConfig.FskPreLen,radioConfig.FskPreDetLen,radioConfig.FskSyncLen);
  if(radioConfig.FskAddrComp == 0) printf("ADDR_COMP:OFF\r\n");
  else if(radioConfig.FskAddrComp == 1) printf("ADDR_COMP:NODE\r\n");
  else printf("ADDR_COMP:NODE_BR\r\n");
  if(radioConfig.FskVarLen == true) printf("LEN:VAR\r\n"); 
  else printf("LEN:FIXED\r\n");
  printf("PAY_LEN=%d\r\n",radioConfig.FskPayLen);
  switch(radioConfig.FskCrcType)
  {
    case 0:
      printf("CRC:1BYTE\r\n");
      break;
    case 1:
      printf("CRC:OFF\r\n");
      break;
    case 2:
      printf("CRC:2BYTES\r\n");
      break;
    case 4:
      printf("CRC:1BYTE_INV\r\n");
      break;
    case 6:
      printf("CRC:2BYTES_INV\r\n");
      break;
    default:
      printf("CRC:INVALID\r\n");
      break;
  }
  if(radioConfig.FskWhiteEn == true) printf("WHITE:ON\r\n"); 
  else printf("WHITE:OFF\r\n"); 
	printf("FS_GET_PACKETPARAMS: TODO\r\n");
}

void cli_fs_setpacketparams(int argc, char **argv)
{
  if(radioConfig.modem == SX126X_MODEM_FSK)
  {
    uint16_t prelen;
    uint8_t predetlen;
    uint8_t synclen;
    uint8_t addrfilt;
    bool varlen;
    uint8_t paylen;
    uint8_t crctype;
    bool whiteen;
    
    prelen = ciGetUnsigned(argv[1]);
    //check
    predetlen = ciGetUnsigned(argv[2]);
    //check
    synclen = ciGetUnsigned(argv[3]);
    //check
    addrfilt = ciGetUnsigned(argv[4]);
    //check
    varlen = (bool)ciGetUnsigned(argv[5]);
    //check
    paylen = ciGetUnsigned(argv[6]);
    //check
    crctype = ciGetUnsigned(argv[7]);
    //check
    whiteen = (bool)ciGetUnsigned(argv[8]);
    //check
    //FLASH_Unlock(FLASH_MemType_Data);
    radioConfig.FskPreLen = prelen;
    radioConfig.FskPreDetLen = predetlen;
    radioConfig.FskSyncLen = synclen;
    radioConfig.FskAddrComp = addrfilt;
    radioConfig.FskVarLen = varlen;
    radioConfig.FskPayLen = paylen;
    radioConfig.FskCrcType = crctype;
    radioConfig.FskWhiteEn = whiteen;
    //FLASH_Lock(FLASH_MemType_Data);
    SX126X_config();
    printf("FS_SET_PACKETPARAMS:\r\nPRE_LEN=%d\r\nPRE_DET=%d\r\nSYNC_LEN=%d\r\n",radioConfig.FskPreLen,radioConfig.FskPreDetLen,radioConfig.FskSyncLen);
    if(radioConfig.FskAddrComp == 0) printf("ADDR_COMP:OFF\r\n");
    else if(radioConfig.FskAddrComp == 1) printf("ADDR_COMP:NODE\r\n");
    else printf("ADDR_COMP:NODE_BR\r\n");
    if(radioConfig.FskVarLen == true) printf("LEN:VAR\r\n"); 
    else printf("LEN:FIXED\r\n");
    printf("PAY_LEN=%d\r\n",radioConfig.FskPayLen);
    switch(radioConfig.FskCrcType)
    {
      case 0:
      printf("CRC:1BYTE\r\n");
      break;
      case 1:
      printf("CRC:OFF\r\n");
      break;
      case 2:
      printf("CRC:2BYTES\r\n");
      break;
      case 4:
      printf("CRC:1BYTE_INV\r\n");
      break;
      case 6:
      printf("CRC:2BYTES_INV\r\n");
      break;
      default:
      printf("CRC:INVALID\r\n");
      break;
    }
    if(radioConfig.FskWhiteEn == true) printf("WHITE:ON\r\n"); 
    else printf("WHITE:OFF\r\n"); 
  }
  else printf("FS_SET_PACKETPARAMS: ERROR\r\n");
	printf("FS_SET_PACKETPARAMS: TODO\r\n");
}

//node and broadcast addr.
void cli_fs_getaddr(int argc, char **argv)
{
  printf("FS_GET_ADDR: 0x%02X,0x%02X\r\n",radioConfig.FskNodeAddr,radioConfig.FskBrAddr);
}

void cli_fs_setaddr(int argc, char **argv)
{
  if(radioConfig.modem == SX126X_MODEM_FSK)
  {
    uint8_t nodeaddr,braddr;
    
    nodeaddr = ciGetUnsigned(argv[1]);
    braddr = ciGetUnsigned(argv[2]);
    radioConfig.FskNodeAddr = nodeaddr;
    radioConfig.FskBrAddr = braddr;
    SX126X_config();
    printf("FS_SET_ADDR: 0x%02X,0x%02X\r\n",radioConfig.FskNodeAddr,radioConfig.FskBrAddr);
  }
  else printf("FS_SET_ADDR: ERROR\r\n");
}

//sync word(s)
void cli_fs_getsync(int argc, char **argv)
{
  uint8_t i;
  
  printf("FS_GET_SYNC: ");
  for(i = 0; i < 7; i++) printf("0x%02X,",radioConfig.FskSync[i]);
  printf("0x%02X\r\n",radioConfig.FskSync[7]);
}

void cli_fs_setsync(int argc, char **argv)
{
  if(radioConfig.modem == SX126X_MODEM_FSK)
  {
    uint8_t sync[8];
    uint8_t i;
    
    for(i = 0; i < 8; i++) sync[i] = ciGetUnsigned(argv[i+1]);
    for(i = 0; i < 8; i++) radioConfig.FskSync[i] = sync[i];
    SX126X_config();
    printf("FS_SET_SYNC: ");
    for(i = 0; i < 7; i++) printf("0x%02X,",radioConfig.FskSync[i]);
    printf("0x%02X\r\n",radioConfig.FskSync[7]);
  }
  else printf("FS_SET_SYNC: ERROR\r\n");
}
//CRC init, CRC poly, whitening init
void cli_fs_getcrcwhite(int argc, char **argv)
{
  printf("FS_GET_CRCWH:\r\nCRC_INIT=0x%04X\r\nCRC_POLY=0x%04X\r\nWH_INIT=0x%04X\r\n",radioConfig.FskCrcInit,radioConfig.FskCrcPoly,radioConfig.FskWhiteInit);
}

void cli_fs_setcrcwhite(int argc, char **argv)
{
  if(radioConfig.modem == SX126X_MODEM_FSK)
  {
    uint16_t c_init, c_poly, w_init;
    
    c_init = ciGetUnsigned(argv[1]);
    c_poly = ciGetUnsigned(argv[2]);
    w_init = ciGetUnsigned(argv[3]);
    
    radioConfig.FskCrcInit = c_init;
    radioConfig.FskCrcPoly = c_poly;
    radioConfig.FskWhiteInit = w_init;
    SX126X_config();
    printf("FS_SET_CRCWH:\r\nCRC_INIT=0x%04X\r\nCRC_POLY=0x%04X\r\nWH_INIT=0x%04X\r\n",radioConfig.FskCrcInit,radioConfig.FskCrcPoly,radioConfig.FskWhiteInit);
  }
  else printf("FS_SET_CRCWH: ERROR\r\n");
}

//System tests
void cli_sendburst(int argc, char **argv)
{
  txpacketcount = ciGetUnsigned(argv[1]);
  if(txpacketcount == 0) txpacketcount = 1;
  inter_packet_delay = ciGetUnsigned(argv[2]);
  slaveID = ciGetUnsigned(argv[3]);
  if(inter_packet_delay < 100) inter_packet_delay = 100;
  radio_startburst();
  printf("SEND_PACKET: %d,%d\r\n",txpacketcount,inter_packet_delay);
	//updatescreen();
}

void cli_stopburst (int argc, char **argv)
{
  if(master)
  {
    master = false;
    tx_needed = false;
    printf("STOP_TX: OK\r\n");
  }
  else printf("STOP_TX: ERROR\r\n");
	//updatescreen();
}


void cli_txstream(int argc, char **argv)
{
  uint8_t s;
  s = ciGetUnsigned(argv[1]);
  if(s > 2) s = 2;
  switch(s)
  {
    case 0:
    default:
      if(radioConfig.modem == SX126X_MODEM_FSK) SX126X_FskConfig();
      
      SX126X_setopmode(SX126X_OPMODE_RX); //SX126X_SetRx(radioConfig.LoRaRxTimeout);
      printf("TX_STREAM: OFF\r\n");
      txmode = 0;
      break;

    case 1:
      prevopmode = opmode;
      SX126X_setopmode(SX126X_OPMODE_TXSTREAMCW);//SX126X_SetCW();
      printf("TX_STREAM: CW\r\n");
      txmode = 1;
      break;

    case 2:
      prevopmode = opmode;
      SX126X_setopmode(SX126X_OPMODE_TXSTREAMPRE);//SX126X_SetTxInfinitePreamble();
      printf("TX_STREAM: PREAMBLE\r\n");
      txmode = 2;
      break;
  }
	//updatescreen();
}

void cli_sweeptx(int argc, char **argv)
{
	startfreq = ciGetUnsigned(argv[1]);
	stopfreq = ciGetUnsigned(argv[2]);
	sweepstep = ciGetUnsigned(argv[3]);
	sweeptimeus = ciGetUnsigned(argv[4]);
	sweepstream = ciGetUnsigned(argv[5]);
	
  if(sweepstream > 2) sweepstream = 2;
  if(sweepstream == 0)
  {
    if(sweeptx || jumptx)
    {
      sweeptx = false;
			jumptx = false;	
			ustimer_stop();
      SX126X_setopmode(prevopmode);
			currfreq = freq_save;
      RADIO_SetAbsoluteFrequency(currfreq);
      txmode = 0;
    }
    printf("SWEEP_TX: STOP\r\n");
  }
  else
  {
    if(txmode != 0)
    {
      printf("SWEEP_TX: ERROR\r\n");
    }
    else
    {
      printf("SWEEP_TX: START\r\n");
      prevopmode = opmode;
			currfreq = startfreq;
      RADIO_SetAbsoluteFrequency(currfreq);
      //sweepcnt = sweepdelay;
			ustimer_setinterval(sweeptimeus);
      if(sweepstream == 1) 
      {
        SX126X_setopmode(SX126X_OPMODE_TXSTREAMCW);
				txmode = 1;
        printf("TX_STREAM: CW\r\n");
      }
      else 
      {
        SX126X_setopmode(SX126X_OPMODE_TXSTREAMPRE);
        txmode = 2;
        printf("TX_STREAM: PREAMBLE\r\n");
      }
			if(sweepstep != 0) 
			{
				sweeptx = true;
				jumptx = false;
			}
			else
			{
				sweeptx = false;
				jumptx = true;
			}
			ustimer_start();
    }
  }
}

void cli_sweeprx(int argc, char **argv)
{
	startfreq = ciGetUnsigned(argv[1]);
	stopfreq = ciGetUnsigned(argv[2]);
	sweepstep = ciGetUnsigned(argv[3]);
	sweeptimeus = ciGetUnsigned(argv[4]) * 1000;
	rssitr = (float)ciGetSigned(argv[5]);
	
  if((sweeptimeus == 0) || (rssitr == 0))
  {
    if(sweeprx)
		{
			sweeprx = false;
			currfreq = freq_save;
			RADIO_SetAbsoluteFrequency(currfreq);
			ustimer_stop();
		}
    printf("SWEEP_RX: STOP\r\n");
  }
  else
  {
    printf("SWEEP_RX: START\r\n");
    RADIO_SetAbsoluteFrequency(startfreq);
		ustimer_setinterval(sweeptimeus);
		ustimer_start();
    sweeprx = true;
  }
}

void cli_startrxcount(int argc, char **argv)
{
  printf("START_RXCOUNT: TODO\r\n");
}

void cli_stoprxcount(int argc, char **argv)
{
  printf("STOP_RXCOUNT: TODO\r\n");
}

void cli_getrxcount(int argc, char **argv)
{
  printf("GET_RXCOUNT: TODO\r\n");
}

void cli_getrssioffset(int argc, char **argv)
{
  printf("GET_RSSIOFFSET: TODO\r\n");
}

void cli_setrssioffset(int argc, char **argv)
{
  printf("SET_RSSIOFFSET: TODO\r\n");
}

//Device dependent commands
void cli_getvt(int argc, char **argv)
{
  printf("GET_VT: T=%.3f,V=%.3f\r\n",T,Vcc);
}

void cli_readreg(int argc, char **argv)
{
  uint16_t r;
  r = ciGetUnsigned(argv[1]);
  printf("READ_REG: 0x%02X\r\n",SX126X_readReg(r));
}

void cli_writereg(int argc, char **argv)
{
  uint16_t reg;
  uint8_t val;
  reg = ciGetUnsigned(argv[1]) & 0xfff;
  val = ciGetUnsigned(argv[2]) & 0xff;
  SX126X_writeReg(reg,val);
  printf("WRITE_REG: 0x%04X,0x%02X\r\n",reg,val);
}

void cli_dumpregs(int argc, char **argv)
{
  uint16_t reg_l,reg_h;
  uint16_t i;
  reg_l = ciGetUnsigned(argv[1]);
  reg_h = ciGetUnsigned(argv[2]);
  printf("DUMP_REGS:\r\n"); 
  for(i = reg_l; i <= reg_h; i++)
  {
    printf("0x%04X,0x%02X\r\n",i,SX126X_readReg(i));
  }
}

void cli_initconfig(int argc, char **argv)
{
  int8_t retval;
	uint8_t config = ciGetUnsigned(argv[1]);
	if(config >= CONFIG_COUNT) printf("INIT_CONFIG: INVALID CONFIG\r\n");
	else
	{
		retval = SX126X_initconfigstructure(config);
		if(retval == 0) printf("INIT_CONFIG: %d,OK\r\n",config);
		else printf("INIT_CONFIG: ERROR %d\r\n", retval);
	}
}

void cli_storeconfig(int argc, char **argv)
{
	writeconfig();
	printf("STORE_CONFIG: OK\r\n");
}

void cli_getpower(int argc, char **argv)
{
  printf("GET_POWER: %d dBm\r\n",txpower);
}

void cli_setpower(int argc, char **argv)
{
  int8_t pwr;

  pwr = ciGetSigned(argv[1]);
  if(pwr > 22) pwr = 22;
  if(pwr < -9) pwr = -9;
  txpower = pwr;
  SX126X_SetTxParams(txpower,0);
  printf("SET_POWER: %d dBm\r\n",txpower);
}


void cli_setpaconfig(int argc, char **argv)
{
	uint8_t dutycycle;
	uint8_t hpmax;
	
	dutycycle = ciGetUnsigned(argv[1]);
	hpmax = ciGetUnsigned(argv[2]);
	if(dutycycle > 7) dutycycle = 7;
	if(hpmax > 7) hpmax = 7;
	SX126X_SetPaConfig(dutycycle,hpmax,false);
	printf("SET_PACONFIG: %d,%d\r\n",dutycycle,hpmax);
}

void cli_getpaconfig(int argc, char **argv)
{
	printf("GET_PACONFIG: %d,%d\r\n",dutycycle,hpmax);
}


      


