

#include <stdint.h>

#include "cc430x513x.h"

#include "Utils\Utils.h"
#include "Board\Board.h"
#include "Radio.h"
#include "WMBus\WMBus.h"
#include "Work.h"
#include "UART.h"

#include "RFCounter433.h"

#include "Utils\Debug\Debug.h"





//
uint16_t  RadioTimer;
uint8_t G_LedMask;
uint8_t R_LedMask;

uint8_t WinterSummer;
//

uint8_t wmbTxCnt,rfRxCnt,rfRSSI;
uint8_t rfBufRx[64];
//



uint16_t RadioDog_rx, RadioDog_tx;
uint16_t PowerDog;







// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1ISR(void)
{
if(P1IFG & BIT3)
	{
	P1IFG &= ~BIT3;
	};
if(P1IFG & BIT4)
	{
	P1IFG &= ~BIT4;
	};
}





// RTC Interrupt Service Routine
uint16_t timeout;
unsigned char  t;
#pragma vector=RTC_VECTOR
__interrupt void RTC_ISR(void)
{
USART_Timer10ms();
//P3OUT^=BIT4;

if (t==0)
	{
	t=1;
	if (timeout) timeout--;
	if(RadioDog_rx) RadioDog_rx--;
	if(RadioDog_tx) RadioDog_tx--;
	
//	G_LedOff;
	//CheckGerkon();
	//LedBlinking();
	//CheckButton();
//	LowerPower_3_EXIT;
	}
else t--;

// ??
uint16_t tI = RTCIV;
//   main ticks 16 herz
//for(tI=3; tI; tI--) __no_operation();

}



void InitCounter433(void)
{
//StopWdt();
InitPMM ();
//
//InitClock();
InitRTC();
InitLed();
//InitButton();
//InitGerkon();
//    InitAdc();
//    InitUart();
//    InitEEPROM();
//    InitTimer0();
//    RealTimer = 0;

//    InitRadio();
//    InitSleep();
}




// Watchdog Timer interrupt service routine
#pragma vector=WDT_VECTOR
__interrupt void WDT_ISR(void)
{
Debug("!!! WatchDog Timer Alarm !!!\r\n");
__no_operation();
WDTCTL=0x00;	//Reset
}



/*#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
//   main ticks 16 herz
//    TA0CCR0 += (FLF/FGERKON);
//    if(radioTimer) --radioTimer;
//    sbit(Flags,flTimer);
//    if(!(++Tick & 0x0F)) ++RealTimer;
//    LedBlinking();
//    CheckButton();
//    LowerPower_0_EXIT;
}*/
// ADC10 interrupt service routine
#pragma vector=ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
//    __no_operation();
//   Y_LedFlp;
//    uint16_t d;
//    d = ADC10MEM0;
//    ADC10CTL0 &= ~ADC10ENC;
//    ADC10CTL0 |= ADC10ENC | ADC10SC; // Sampling and conversion start
}



// USART interrupt service routine
/*#pragma vector=USCI_A0_VECTOR
__interrupt void USART_ISR(void)
{

}*/