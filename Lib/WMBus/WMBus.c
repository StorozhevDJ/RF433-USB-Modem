
#include <stdint.h>

#include "cc430x513x.h"

#include "Utils\Utils.h"
#include "RFCounter433.h"
#include "Board\Board.h"

#include "WMBus.h"






// WMBUS Fields
//

void int2bcd(uint8_t bcdSize, uint32_t *intSource, uint8_t *bcdDest);



void InitWMBus(void)
{
uint8_t i;
struct wmbPacket *wP;
wP=(struct wmbPacket *)wmbTx;
//
for(i=0; i < wmbL; i++) wmbTx[i]=eeCustomer[i];
if(!wmbCheck(wmbTx,wmbL))
	{
	//for(i=0; i<wmbL; i++) wmbTx[i]=eeKvadrat[16+i];
	for(i=0; i<wmbL; i++) wmbTx[i]=eeSET[16+i];
	}
WaterCounter[0].cnt[0]=wP->block2[0].CNT1[0];
WaterCounter[0].cnt[1]=wP->block2[0].CNT1[1];
WaterCounter[0].cnt[2]=wP->block2[0].CNT1[2];
WaterCounter[0].cnt[3]=wP->block2[0].CNT1[3];

if (WaterCounter[0].bigCnt>99999999) WaterCounter[0].bigCnt=0;
}



//Создание пакетов для отправки
uint8_t wmbCreate(uint8_t *txBuf, uint8_t PacketTYPE)
{
struct wmbPacket *WP;
WP=(struct wmbPacket *)txBuf;

WP->block0.CF=PacketTYPE;
switch(PacketTYPE)
	{
	case SND_AD:
		InitWMBus();
		//        WP->MANIDF=devInfo->MANIDF;
		//        WP->ADDRF=(uint32_t *)UnixTime();
		//        WP->VERF=devInfo->VERF;//RTCSEC;  //AlarmState;
		//        WP->TYPEF=devInfo->TYPEF;
	break;
	case SND_ACK:
		WP->block0.LF= wmbLFields(L_BLOCK0);
		break;
		case SND_END:
		if(CounterFlags.Date)
			{
			WP->block0.LF= wmbLFields(L_PACKET40);
			WP->CIF=wmbNoHdr;
//			WP->block2[0].DIF1=7;
			WP->block2[0].VIF1=VIF_DATETIME;
			txBuf[15]=RTCSEC;
			txBuf[16]=RTCMIN;
			txBuf[17]=RTCHOUR;
			txBuf[18]=RTCDOW|WinterSummer;
			txBuf[19]=RTCDAY;
			txBuf[20]=RTCMON;
			txBuf[21]=RTCYEARL;
			txBuf[22]=RTCYEARH;
			}
		else WP->block0.LF= wmbLFields(L_BLOCK0);
	break;
	case SND_UD:
	case SND_NR:
		WP->block0.LF= wmbLFields(L_PACKET40);
		//        WaterCounter=PowerDog;
		//        PowerDog=0;
		//        WP->CNT1[0] = RTCSEC;
		//        WP->CNT1[1] = RTCMIN;
		//        WP->CNT1[2] = RTCHOUR;
		//        WP->CNT1[3] = RTCDOW|WinterSummer;
		__disable_interrupt();
		/*WP->CNT1[0] = WaterCounter.cnt[0];
		WP->CNT1[1] = WaterCounter.cnt[1];
		WP->CNT1[2] = WaterCounter.cnt[2];
		WP->CNT1[3] = WaterCounter.cnt[3];*/
		__enable_interrupt();
		//        AlarmState=eeCustomer[0];
		WP->block2[0].ALARM=AlarmState[0];
	break;
	//ReSPonse User Data (Data from counter)
	case RSP_UD:
	
	break;
	
	case SND_IR:
	
	break;
	case SND_IR_ACK:
	
	break;
	//User Data1 packet (Alarm packet)
	case REQ_UD1:
	
	break;
	//User Data2 packet  (Measure Data + Alarm field packet)
	case REQ_UD2:
		//WP->block0.LF= wmbLFields(L_PACKET40);
		WP->block0.LF=sizeof(struct wmbPacket)-1-2;//-Size -CRC1
		WP->block0.CF=RSP_UD;	//0x08
		WP->CIF=wmbNoHdr;
		for (char i=0; i<4; i++)
			{
			//char shift=sizeof(TSblock0)+sizeof(TSblock2)*i+1;
			WP->block2[i].DIF1=4;	//4 bytes
			//txBuf[shift+0]=4;
			WP->block2[i].VIF1=VIF_10l;	//10 Liter discrette
			//txBuf[shift+1]=VIF_10l;
			__disable_interrupt();
			if (WaterCounter[i].bigCnt>99999999) WaterCounter[i].bigCnt-=99999999;
			//int2bcd(8, &WaterCounter.bigCnt, WP->CNT1);
			WP->block2[i].CNT1[0] = WaterCounter[i].cnt[0];
			WP->block2[i].CNT1[1] = WaterCounter[i].cnt[1];
			WP->block2[i].CNT1[2] = WaterCounter[i].cnt[2];
			WP->block2[i].CNT1[3] = WaterCounter[i].cnt[3];
			WP->block2[i].DIF2=1;
			WP->block2[i].VIF2=VIF_Alarm;
			
			WP->block2[i].ALARM=AlarmState[i];
			__enable_interrupt();
			AlarmState[i]=0;
			}
	break;
		default:
		return 0;
	break;
	};

// Check CRC
if(WP->block0.LF > 9)
	{
	char part=(WP->block0.LF-L_BLOCK0+15)/16;//Количество частей для подсчета CRC
	char j=WP->block0.LF+1+2;
	for (char i=part; i>0; i--)
		{
		char start=j;
		for ( ; j>L_BLOCK0+16*(i-1); j--)
			{
			txBuf[j+2*(i-1)-1]=txBuf[j-1];
			}
		char stop=j;
		wmbCRC(txBuf+L_BLOCK0+(i-1)*18, start-stop, CRC_APPEND); // CRC16
		}
	}
wmbCRC(txBuf, L_BLOCK0-2 ,CRC_APPEND);
//return wmbPacketLength(WP->block0.LF);
return WP->block0.LF+1+2+2*(WP->block0.LF-L_BLOCK0+15)/16;//Return packet lenght
}



//Разбор принятых пакетов
uint8_t wmbParse(uint8_t *rxBuf, uint8_t *txBuf)
{
struct wmbPacket *wP;
wP=(struct wmbPacket *)rxBuf;
// check adrr
//for(uint8_t i=2; i<10; i++) if(rxBuf[i]!=txBuf[i]) return 0;
//
switch(wP->block0.CF)
	{
	case SND_ACK:

	//0x40	END of LINK
	case SND_END:
		AlarmState[0]=0;
		if(wP->block0.LF  == wmbLFields(L_PACKET40) &&
		   wP->CIF == wmbNoHdr &&
		   wP->block2[0].VIF1== VIF_DATETIME)
			{
			SetupRTC((uint8_t *)wP->block2[0].CNT1);
			CounterFlags.Date=1;
			};
	break;
	//0x44	SEND/NO REPLY, meter initiative
	case SND_NR:
	
	break;
	//0x53	SEND USER DATA
	case SND_UD:
		//Открыть кран
		faucet=16*3;
		P1OUT&=~BIT1;
		P3OUT |= BIT5;//Красный
		for (char i=1; i<30; i++) {__delay_cycles(100000); RunWdt();}
		P1OUT|= BIT1;
		P3OUT&=~BIT5;//Красный
		return wmbCreate(txBuf, SND_ACK);
	break;
	//0x73	SEND USER DATA
	case SND_UD73:
		//Закрыть кран
		faucet=16*3;
		P1OUT&=~BIT2;
		P3OUT |= BIT5;//Красный
		for (char i=1; i<30; i++) {__delay_cycles(100000); RunWdt();}
		P1OUT|= BIT2;
		P3OUT&=~BIT5;//Красный
		return wmbCreate(txBuf, SND_ACK);
	break;
	//0x46	Send manually initiated installation data;
	case SND_IR:
	//        eeWrite(txBuf,eeCustomer,wmbCreate(txBuf,SND_IR_ACK));
	break;
	//0x06	ACK for INIT;
	case SND_IR_ACK:
	
	break;
	//0x48	Accsess demand
	case SND_AD:
	
	break;
	//0x5A	REQUEST USER DATA1
	case REQ_UD1:
	
	break;
	//0x5B	REQUEST USER DATA2 (Data request (Request User Data Class2))
	case REQ_UD2:
		/*__disable_interrupt();
		if (WaterCounter.bigCnt>99999999) WaterCounter.bigCnt-=99999999;
		int2bcd(8, &WaterCounter.bigCnt, wP->CNT1);
		__enable_interrupt();
		wP->ALARM=AlarmState;*/
	return wmbCreate(txBuf, REQ_UD2);
	//Undefined C-field
	default:
		return 0;
	break;
	}
return 0;
}



// *****************************************************************************
// @fn          wmbCheckAddr
// @brief       Check addresat
// @param       uint8_t *rxBuf	Array WM-Bus data
// @return      0 - Not correct address (packet not for my)
// @return      1 - My packet
// *****************************************************************************
uint8_t wmbCheckAddr(uint8_t *rxBuf)
{
// check adrr
for(char i=2; i<10; i++)	if(rxBuf[i]!=eeSET[16+i]) return 0;
return 1;
}



// *****************************************************************************
// @fn          wmbCheck
// @brief       Check correct WM-Bus packet (CRC and Lenght for block 1 and 2)
// @param       uint8_t *rxBuf	Array WM-Bus data
// @param       uint8_t cnt		Number of received bytes
// @return      0 - Find errors in WM-Bus packet
// @return      not null - lenght WM-Bus packet
// *****************************************************************************
uint8_t wmbCheck(uint8_t *rxBuf, uint8_t cnt)
{
uint8_t packetL,total,p;
if(cnt < 12) return  0;   // RADIO
if(cnt > 64) return  0;   // RADIO
// Block 0
if(wmbCRC(rxBuf, L_BLOCK0-2, CRC_CHECK)) return  0;   // RADIO
packetL=wmbPacketLength(rxBuf[0]);
if(packetL > cnt) return  0;   // RADIO
if(packetL < 9)   return  0;   // BUG
if(packetL > 55)  return  0;   // BUG

total = L_BLOCK0;
//  Block 1
while(total < packetL)
	{
	p = packetL - total;
	if(p > L_BLOCK1) p=L_BLOCK1;
	if(wmbCRC(rxBuf+total,p-2,CRC_CHECK))  return 0; // RADIO
	else total += p;
	};
return total;
}



/*******************************************************************************
// @fn		wmbCRC
// @brief		Calculate and control CRC
			CRC: Cyclic redundancy check
			The CRC polynomial is: x16 + x13 + x12 + x11 + x10 + x8 + x6 + x5 + x2 + 1
			The initial value is: 0
// @param		uint8_t *rxBuf	Array WM-Bus data
// @param		uint8_t cnt		Counter bytes
// @param		uint8_t mode		0 normal return CRC16
							1 return CRC16 and place to end of buffer
							2 return CRC16 ^ CRC16fromBUFFER (0 if TRUE)
// @return	uint16_t			CRC is complemented
*******************************************************************************/
uint16_t wmbCRC(uint8_t *buf, uint8_t cnt, uint8_t mode)
{
#define wmbPoly 0x3D65
uint8_t i;
union
	{
	uint16_t crc16;
	uint8_t  crc8[2];
	} CRC= {0};
//    CRC.crc16=0;
while(cnt--)
	{
	CRC.crc8[1] ^= *buf++;
	for (i = 0; i < 8; i++)
	CRC.crc16 = CRC.crc16 & 0x8000 ? (CRC.crc16 << 1) ^ wmbPoly : CRC.crc16 << 1;
	};
//  Complement output
CRC.crc16 ^= 0xFFFF;
if (mode==CRC_CALC){ }
else if(mode==CRC_APPEND)
	{
	*buf++=CRC.crc8[1];
	*buf++=CRC.crc8[0];
	}
else if(mode==CRC_CHECK)
	{
	CRC.crc8[1]  ^= *buf++;
	CRC.crc8[0]  ^= *buf++;
	};
return CRC.crc16;
}




void int2bcd(uint8_t bcdSize, uint32_t *intSource, uint8_t *bcdDest)
{
uint64_t tmp = *intSource;
while(bcdSize != 0)
	{
	*bcdDest = (tmp % 10);
	tmp -= (tmp % 10);
	tmp = tmp / 10;
	bcdSize--;
	if (bcdSize == 0) break;
	*bcdDest += ((tmp % 10) << 4);
	tmp -= (tmp % 10);
	tmp = tmp / 10;
	bcdDest++;
	bcdSize--;
	}
}