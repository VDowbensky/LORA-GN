#include "radio_proc.h"
#include "flash.h"
#include "sx126x_proc.h"

void processHardEvents(void);
void processTxEvents(void);
void processRxEvents(void);

uint32_t tx_ticks = 0;


uint8_t RADIO_txBuffer[RADIO_BUFFER_SIZE]; // = TX_PACKET_TEMPLATE;
uint8_t RADIO_rxBuffer[RADIO_BUFFER_SIZE];

SX126XX_radioconfig_t radioConfig;

//uint8_t iv[16]  = {0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff};

//const uint16_t deviceID = 0x7f0f;

volatile uint16_t txpacketnumber = 0;
volatile uint16_t rxpacketnumber = 0;
volatile uint16_t txpacketcount = 0;
volatile uint16_t rxpacketcount = 0;
bool master = false;
bool tx_needed = false;
uint16_t inter_packet_delay = 100;
bool answer_needed = false;
bool pre_rcv = false;
bool sync_rcv = false;
bool packet_received = false;
bool crc_error = false;
bool packet_sent = false;

rfpacket_t txpacket, rxpacket;
float RssiPkt;
float SnrPkt;
float SignalRssiPkt;

int16_t Channel;

uint16_t masterID;
uint16_t slaveID;

int16_t Rssi;

//uint16_t minchan;
//uint16_t maxchan;
volatile bool sweeptx = false;
volatile bool jumptx = false;
volatile bool sweeprx = false;
volatile bool sweepflag = false; //temp/

uint32_t startfreq;
uint32_t stopfreq;
uint32_t freq_save;
uint32_t currfreq;
uint32_t sweepstep;
uint32_t sweeptimeus;
uint8_t sweepstream;
float rssitr;

//struct AES_ctx aes_ctx;

void radio_init(void)
{
	readconfig();
	if(radioConfig.MagicNumber != MAGICNUMBER) SX126X_initconfigstructure(0);
	//config SX1262 (internal)
  if(SX126X_config() == 0) printf("SX1262 init OK.\r\n");
  else printf("SX1262 init error.\r\n");
	currfreq = radioConfig.RfFreq / 1000;
	rflevel = radioConfig.RfLevel;
	RADIO_SetAbsoluteFrequency(currfreq);
	setrflevel();
}

//radio events handler
void radio_proc(void)
{
  static uint8_t phase = 0;
  
  if(tx_needed)
  {
    tx_needed = false;
    prepareTxPacket();
    txled_on();
    SX126X_setopmode(SX126X_OPMODE_TX);
  }

  if(answer_needed)
  {
    answer_needed = false;
    prepareAnswerPacket();
    txled_on();
    SX126X_setopmode(SX126X_OPMODE_TX);
  }

  if(packet_received)
  {
    packet_received = false;
    rxled_on();
    //LoRa
    if(radioConfig.modem == SX126X_MODEM_LORA)
    {
      uint8_t pktstatus,rssipkt,signalrssipkt;
      int16_t snrpkt;
      
      SX126X_GetLoRaPacketStatus(&pktstatus,&rssipkt,&snrpkt,&signalrssipkt);
      RssiPkt = -((float)rssipkt/2);
      if(snrpkt < 128) SnrPkt = ((float)snrpkt)/4;
      else SnrPkt = (float)(snrpkt - 256)/4;
      //( status[1] < 128 ) ? ( pktStatus->Params.LoRa.SnrPkt = status[1] >> 2 ) : ( pktStatus->Params.LoRa.SnrPkt = ( ( status[1] - 256 ) >> 2 ) );
      SignalRssiPkt = -((float)signalrssipkt/2);
    }
    else
    {
      uint8_t status,rxstatus,rssisync,rssiavg;
      SX126X_GetFskPacketStatus(&status,&rxstatus,&rssisync,&rssiavg);
      RssiPkt = -((float)rssisync/2);
      SignalRssiPkt = -((float)rssiavg/2);
      SnrPkt = 0;
    }
    //retrieve packet from transceiver to RADIO_rxBuffer
    SX126X_readBuffer(0x80,RADIO_rxBuffer,32);
    if(crc_error)
    {
      crc_error = false;
      printcrcerror();
    }
    else
    {
      memcpy((void*)&rxpacket, (void*) RADIO_rxBuffer, 32);
      printRxPacket();
      rxpacketnumber = rxpacket.packetnumber;
      if(!master && (rxpacket.slaveid == radioConfig.deviceID)) answer_needed = true;
    }
    rxled_off();
    //execute commands if needed
    SX126X_setopmode(SX126X_OPMODE_RX);
  }

  if(packet_sent)
  {
    packet_sent = false;
    SX126X_setopmode(SX126X_OPMODE_RX);
    txled_off();
  }
  //sweep RF
  if(sweepflag)
  {
    sweepflag = false;
    if(sweeptx)
    {
			currfreq += sweepstep;
			if(currfreq > stopfreq) currfreq = startfreq;
			SX126X_setopmode(SX126X_OPMODE_STBYXOSC);
			RADIO_SetAbsoluteFrequency(currfreq);
			if(sweepstream == 1) SX126X_setopmode(SX126X_OPMODE_TXSTREAMCW);
			else SX126X_setopmode(SX126X_OPMODE_TXSTREAMPRE);
		}
		if(jumptx)
		{
			if(currfreq == startfreq) currfreq = stopfreq;
			else currfreq = startfreq;
			SX126X_setopmode(SX126X_OPMODE_STBYXOSC);
			RADIO_SetAbsoluteFrequency(currfreq);
			if(sweepstream == 1) SX126X_setopmode(SX126X_OPMODE_TXSTREAMCW);
			else SX126X_setopmode(SX126X_OPMODE_TXSTREAMPRE);
		}
		if(sweeprx)
		{
			float rssi_curr;
			rssi_curr = -((float)SX126X_GetRssiInst()/2);
			if(rssi_curr > rssitr) printf("@%d,%.1f\r\n",currfreq,rssi_curr);
			currfreq += sweepstep;
			if(currfreq > stopfreq) currfreq = stopfreq;
			RADIO_SetAbsoluteFrequency(currfreq);
		}
  }
}

void radio_startburst(void)
{
  master = true;
  txpacketnumber = 1;
  //start TX timer here
	tx_ticks = 0; //to systick
  tx_needed = true;
}

void prepareTxPacket(void)
{
  txpacket.packetnumber = txpacketnumber;
  txpacket.params[0] = 'T';
  preparepacketcommon();
}

void prepareAnswerPacket(void)
{
  txpacket.packetnumber = rxpacketnumber;
  txpacket.params[0] = 'R';
  preparepacketcommon();
}

void preparepacketcommon(void)
{
  uint8_t i;

  txpacket.masterid = radioConfig.deviceID;
  txpacket.slaveid = slaveID;
//  txpacket.latitude = GPSdata.lat;
//  txpacket.longitude = GPSdata.lon;
//  txpacket.status = GPSdata.status;
//  txpacket.sat = GPSdata.sat;
//  txpacket.hour = GPSdata.hour;
//  txpacket.min = GPSdata.min;
//  txpacket.sec = GPSdata.sec;
//  txpacket.alt = GPSdata.alt;
//  calcVccTemp();
//  txpacket.temp = (int8_t)Temp;

  for(i = 1; i < 8; i++) txpacket.params[i] = 2;
  memcpy((void*)RADIO_txBuffer, (void*)&txpacket, 32);
  //fill SX126x TX buffer
  SX126X_writeBuffer(0,RADIO_txBuffer,32);
}

void printRxPacket(void)
{
  printf("%d,0x%04X,%c,%d:%d:%d,%d,%d,%.4f,%.4f,%.1f,%d,%.1f,%.1f,%.1f\r\n",
         rxpacket.packetnumber,rxpacket.masterid,rxpacket.params[0],rxpacket.hour,
         rxpacket.min,rxpacket.sec,rxpacket.status,rxpacket.sat,rxpacket.latitude,
         rxpacket.longitude,rxpacket.alt,rxpacket.temp,RssiPkt,SnrPkt,SignalRssiPkt);
}

void printcrcerror(void)
{
  printf("FERR\r\n");
}

void RADIO_setctune(uint8_t tune)
{
  uint8_t tuneA, tuneB;

  if(tune > 94) tune = 94;
  tuneA = tune / 2;
  tuneB = tune - tuneA;
	radioConfig.CtuneA = tuneA;
	radioConfig.CtuneB = tuneB;
  prevopmode = opmode;
  SX126X_setopmode(SX126X_OPMODE_STBYXOSC);
  SX126X_writeReg(SX126X_REG_XTATRIM,tuneA);
  SX126X_writeReg(SX126X_REG_XTBTRIM,tuneB);
  SX126X_setopmode(prevopmode);
}

void LORA_IRQHandler(void)
{
  uint16_t irqstatus;

  //read SX126x status
  irqstatus = SX126X_GetIrqStatus();
  SX126X_ClearIrqStatus(SX126X_ALL_IRQMSK);

  if(irqstatus & SX126X_TXDONE_IRQMSK) packet_sent = true;
  if(irqstatus & SX126X_RXDONE_IRQMSK) packet_received = true;
  if(irqstatus & SX126X_CRCERR_IRQMSK) crc_error = true;
}

void RADIO_SetAbsoluteFrequency(uint32_t freqkHz)
{
  double step = (double)FXO / 33554432;
  SX126X_SetRfFrequency((uint32_t)(freqkHz * 1000/ step));
}





