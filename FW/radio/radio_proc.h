#ifndef _RADIO_PROC_H_
#define _RADIO_PROC_H_

#include <stdio.h>
#include "bsp.h"
#include "sx126x.h"
#include "ustimer.h"
//#include "aes.h"
//#include "gpsparser.h"
//#include "timers.h"
#include "sx126x_proc.h"
#include "txlevel.h"

#define RADIO_BUFFER_SIZE 64
#define TX_PACKET_TEMPLATE {0, 0, 0, 0, -120, 0, 'T', 0, 0, 0, 0, 0, 0, 0, 0,'R', 'A', 'D', 'I', 'O',' ','T','E','S','T',' ',' '}

//HARD events
#define HARDEVENT_TXTIMER                       0x01

#define HIGH(x) ((x & 0xFF000000) >> 24)
#define HIGL(x) ((x & 0x00FF0000) >> 16)
#define LOWH(x) ((x & 0x0000FF00) >> 8)
#define LOWL(x)  (x & 0x000000FF)
	
#define MAGICNUMBER 0xc96c236b

typedef struct SX126XX_radioconfig
{
  uint32_t MagicNumber;

  uint8_t modem;
  //LoRa
  uint8_t LoRaSf; //spreading factor
  uint8_t LoRaBw; //bandwidth
  uint8_t LoRaCr; //coding rate
  bool LoRaLowRateOpt;
  uint16_t LoRaPreLen;
  bool LoRaImplHeader;
  uint16_t LoRaSyncWord;
  uint8_t LoRaPayLen;
  bool LoRaCrcOn;
  bool LoRaInvertIQ;

  //FSK parameters
  uint32_t FskBr; //baud rate
  uint8_t FskShaping;
  uint8_t FskBw;
  uint32_t FskFdev;
  uint16_t FskPreLen;
  uint8_t FskPreDetLen;
  uint8_t FskSyncLen;
  uint8_t FskSync[8];
  uint8_t FskAddrComp;
  uint8_t FskNodeAddr;
  uint8_t FskBrAddr;
  bool FskVarLen;
  uint8_t FskPayLen;
  uint8_t FskCrcType;
  uint16_t FskCrcInit;
  uint16_t FskCrcPoly;
  bool FskWhiteEn;
  uint16_t FskWhiteInit;
	
	uint8_t RfBand;
  uint32_t RfFreq;
  float RfLevel;
	
  uint16_t deviceID;
  uint16_t SlaveID;
	
  //Common calibration parameters
  uint8_t CtuneA;
  uint8_t CtuneB;
	
}SX126XX_radioconfig_t;

typedef struct rfpacket
{
    uint16_t masterid; //2
    uint16_t slaveid; //4
    uint16_t packetnumber; //6
    uint8_t hour;//7
    uint8_t min;//8
    uint8_t sec;//9
    uint8_t status;//10
    float latitude; //14
    float longitude; //18
    float alt; //22
    uint8_t sat;//23
    int8_t temp;
    uint8_t params[8];
}rfpacket_t;


void radio_init(void);
void radio_proc(void);
void printRxPacket(void);
void printcrcerror(void);
void prepareTxPacket(void);
void prepareAnswerPacket(void);
void preparepacketcommon(void);
void radio_startburst(void);
void radio_delayms(uint32_t ms);
void RADIO_DIO0handler(void);

void RADIO_setctune(uint8_t tune);

void RADIO_SetAbsoluteFrequency(uint32_t freqkHz);

extern uint8_t RADIO_rxBuffer[];
extern uint8_t RADIO_txBuffer[];

extern SX126XX_radioconfig_t radioConfig;

extern volatile uint16_t txpacketnumber;
extern volatile uint16_t rxpacketnumber;
extern volatile uint16_t txpacketcount;
extern volatile uint16_t rxpacketcount;

extern bool master;
extern bool tx_needed;
extern uint16_t inter_packet_delay;
extern bool answer_needed;
extern bool packet_received;
extern bool packet_sent;
extern uint32_t tx_ticks;

extern uint8_t txmode;
extern int16_t Channel;

extern uint16_t slaveID;
extern uint16_t masterID;
extern int16_t Rssi;

extern rfpacket_t rfpacket;

extern volatile bool sweepflag; //temp/
extern volatile bool sweeptx;
extern volatile bool jumptx; 
extern volatile bool sweeprx;
extern uint32_t startfreq;
extern uint32_t stopfreq;
extern uint32_t freq_save;
extern uint32_t currfreq;
extern uint32_t sweepstep;
extern uint32_t sweeptimeus;
extern uint8_t sweepstream;
extern float rssitr;

#endif