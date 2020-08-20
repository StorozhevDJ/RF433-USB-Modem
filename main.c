
#include <stdint.h>


#include <string.h>

#include "main.h"
#include "Utils\Utils.h"

#include "Radio.h"
#include "Radio\hal_pmm.h"
#include "RFCounter433.h"
#include "Board\Board.h"
#include "Source\UART.h"
#include "Source\Cmd.h"

#include "Utils\Debug\Debug.h"

#include "Print_scan\Parser.h"



uint8_t rssi_dec;
bool dtr_last;


unsigned long unixtime;


char urx_buf[256];
unsigned char urx_len;

Emodem_mode modem_mode;


void delay_ms(long msec)
{
for(long i = 0; i < msec; i++) __delay_cycles(F_OSC/1000);
}



//Прием данных от UART (строка или массив)
void USART_RX_Complete(char *buf, char len)
{
//memcpy(urx_buf, buf, len+1);	//Copy data from receive buffer to work buffer
memcpy(urx_buf, buf, len);	//Copy data from receive buffer to work buffer
urx_len=len;				//Ignoge in lenght end string (null) symbol
}



#pragma vector = TIMER0_A0_VECTOR
__interrupt void CCR0_ISR(void)
{
unixtime++;

// Сброс флага прерывания таймера происходит автоматически, ручное обнуление не нужно
//if (counter1_timeout<0xffff-1) counter1_timeout++;


} //End: CCR0_ISR


void JTAG_Lock(void)
{
// segment erase BSL signature

StopWdt();
__disable_interrupt();              // no interrupts during erase

SYSBSLC = 0;                        // enable and unlock BSL segment 3
FCTL4 = FWKEY;                      // unlock INFO
FCTL3 = FWKEY;                      // unlock FLASH and INFO A
FCTL1 = FWKEY | WRT;              // select segment erase mode
//    FCTL1 = FWKEY | ERASE;              // select segment erase mode
//    ( *(uint32_t*)0x17fc ) = 0;          // dummy write starts erase
//( *(uint32_t*)0x17fc ) = 4;          // dummy write starts erase
( *(uint32_t*)0x17fc ) = 0x55555555;
   // dummy write starts erase
while( ( FCTL3 & BUSY ) == BUSY );  // wait for erase complete
	{
	_DINT();
	}
FCTL1 = FWKEY;
FCTL3 = FWKEY | LOCK;
}

extern unsigned char RFRxBufferLength;
extern unsigned char RFRxBuffer[256];
extern char ansver_ok[];		//Send ansver Ok
extern char ansver_err[];

void main( void )
{
//Start watchdog with ACLK (32768) / 32768 = 1sec + Interrupt
RunWdt();

UnusedPORT_Init();

R_LedInit; 
R_LedOn;

__delay_cycles(F_OSC/2);

__enable_interrupt();

RadioInit();

USART_Init(USART_RX_Complete);
USART_SetMode(ASCII);
__delay_cycles(F_OSC/8);

R_LedOff;
Debug("--- RF Modem start ---\r\n");
__delay_cycles(F_OSC/2);
PrintInfo();

InitCounter433();

R_LedOff;
G_LedOff;

if (UART_IS_DSR)
	{
	modem_mode=transparent;
	USART_SetMode(BIN);
	UART_SET_DTR;
	}
else
	{
	modem_mode=cmd;
	USART_SetMode(ASCII);
	UART_RESET_DTR;
	}
dtr_last=UART_IS_DSR?true:false;

while (1)
{
RunWdt();
rf_receive();
rf_transmit();
//В режиме команд
if (modem_mode==cmd)
	{
	//Приняли по UART
	if (urx_len)
		{
		urx_len=0;
		if (echo) USART_SendString(urx_buf);
		//если получили команду, выполним её
		if (IsCmd(urx_buf)==true)
			{
			//Debug("Command received: ");
			char cmd_name[16];
			if (Parser_GetParam(&urx_buf[3], RX_END_STR_CHAR, 0, cmd_name)) Cmd_execute(cmd_name);
			else Debug("Wrong command");
			}
		else USART_SendString(ansver_err);
		}
	}
//В прозрачном режиме (данных)
else if (modem_mode==transparent)
	{
	//Приняли по UART
	if (urx_len)
		{
		//Если приняли команду перехода в режиме команд
		if ((urx_len==3)&&(urx_buf[0]=='+'&&urx_buf[1]=='+'&&urx_buf[2]=='+'))
			{
			R_LedOff;
			G_LedOff;
			modem_mode=cmd;
			USART_SetMode(ASCII);
			UART_RESET_DTR;
			USART_SendString(&ansver_ok);
			urx_len=0;
			}
		//если данные, то передаем по радио все что приняли
		else
			{
			if (RF_transmitting == 0)
			{
				if (TransmitCCA((uint8_t *)urx_buf, urx_len))	// Пытаемся, пока не получится
				{
					while(RF_transmitting) rf_transmit();	//_new
					urx_len=0;
				}
			}
			}
		}
	//Приняли по радио, перешлем по UART
	if (RFRxBufferLength)
		{
		USART_SendBuf((char *)RFRxBuffer, RFRxBufferLength);
		RFRxBufferLength=0;
//		ReceiveOn();
		//Отошлем данные о качестве связи
		if (lqi)
			{
			if(rssi_dec >= 128) USART_SendChar((int16_t)((int16_t)( rssi_dec - 256) / 2) - 72); 
			else USART_SendChar((rssi_dec / 2) - 72);
			//USART_SendChar(ReadSingleReg(LQI));
			}
		}
	//Если нужно отслеживать уровень принимаемого сигнала
	else if (lqi)
		{
		rssi_dec = ReadSingleReg(RSSI);
		__delay_cycles(F_OSC/1000);
		}
/*	if (ReadSingleReg(MARCSTATE)==0x11)
		{
		Strobe(RF_SIDLE);
		Strobe(RF_SFRX);
		ReceiveOn();
		}*/
	}
if (dtr_last!=UART_IS_DSR) if (UART_IS_DSR)
	{
	modem_mode=transparent;
	USART_SetMode(BIN);
	UART_SET_DTR;
	//R_LedOn;
	R_LedOff;
	G_LedOff;
	dtr_last=true;
	}
else
	{
	modem_mode=cmd;
	USART_SetMode(ASCII);
	UART_RESET_DTR;
	dtr_last=false;
	}
}
}
