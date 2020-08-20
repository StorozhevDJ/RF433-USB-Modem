
//#include <stdio.h>
//#include <string.h>
//
//#include <legacymsp430.h>
//#include <msp430.h>
//#include <signal.h>
#include <cc430x513x.h>

#include "Utils\Debug\Debug.h"
#include "Utils\Utils.h"

#include "RFCounter433.h"

//#include "Board.h"


#pragma location="INFOC"
const TSmodem_settings ee_modem_settings = {
	.rf_freq = {0x04, 0xB0, 0x10},
	.modem_serial = 0x60600026,
	.rf_length = 64,
	.var_length = 0};
/*
const uint8_t ee_modem_settings[128] =
{// ___________________________________
// |  rf_freq[3]  | Modem Serial number|
// |--------------|--------------------|
    0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,

    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF
};
*/

// Utils
void DelayCPUx10(uint16_t t)
{
    do
    {
        --t;
        __no_operation();
    }
    while(t);
}



void DelayXT1(uint16_t ticks)
{
    uint16_t tStart=RTCPS;
    while((RTCPS-tStart) < ticks);
}



void InitUart(void)
{
    PMAPPWD = 0x02D52;                        // Get write-access to port mapping regs
    P1MAP5 = PM_UCA0RXD;                      // Map UCA0RXD output to P1.5
    P1MAP6 = PM_UCA0TXD;                      // Map UCA0TXD output to P1.6
    PMAPPWD = 0;                              // Lock port mapping registers
    P1DIR |= BIT6;                            // Set P1.6 as TX output
    cbit(P1REN,5);
    cbit(P1REN,6);
    P1SEL |= BIT5 + BIT6;                     // Select P1.5 & P1.6 to UART function
//
    UCA0CTL0 = 0; // UART 8bit
    UCA0CTL1 |= UCSWRST; // **Put state machine in reset**
    UCA0CTL1 |= UCSSEL_2; // SMCLK
    UCA0BR0   = FSYS/BAUD;  // 1.00MHz 38400 (see User's Guide)
    UCA0BR1   = 0;          // 1.00MHz 38400
    UCA0MCTL |= UCBRS_1 + UCBRF_0; // Modulation UCBRSx=1, UCBRFx=0
    UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**
//    UCA0TXBUF = 0x0A;
//    UCA0IFG   = 0;
    //UCA0IE   |= UCRXIE;     //Enable USCI_A1 RX interrupt//
//USCI control 1             UCA0CTL1   00h
//USCI control 0             UCA0CTL0   01h
//USCI baud rate 0           UCA0BR0    06h
//USCI baud rate 1           UCA0BR1    07h
//USCI modulation control    UCA0MCTL   08h
//USCI status                UCA0STAT   0Ah
//USCI receive buffer        UCA0RXBUF  0Ch
//USCI transmit buffer       UCA0TXBUF  0Eh
//USCI LIN control           UCA0ABCTL  10h
//USCI IrDA transmit control UCA0IRTCTL 12h
//USCI IrDA receive control  UCA0IRRCTL 13h
//USCI interrupt enable      UCA0IE     1Ch
//USCI interrupt flags       UCA0IFG    1Dh
//USCI interrupt vector word UCA0IV     1Eh
}



void LedBlinking(void)
{
#define rTimerLedHerz ( RT1PS & 0x08)
    //R_LedOff;
//    G_LedOff;
    if(rTimerLedHerz) return;
//    if(!(CounterState==cInstall ||  CounterState==cProbe)) return;
/*    if(tbit(R_LedMask,7))
    {
        R_LedOn;
        R_LedMask <<= 1;
        sbit(R_LedMask,0);
    }
    else
    {
        R_LedMask <<= 1;
    };
    if(tbit(G_LedMask,7))
    {
        G_LedOn;
        G_LedMask <<= 1;
        sbit(G_LedMask,0);
    }
    else
    {
        G_LedMask <<= 1;
    };*/
}



/*void CheckButton(void)
{
    uint8_t t = (mButton.cnt & 0x7F);
// button freq 8 herz
//    if(RT1PS & 0x08) return;
    if(t < 127) ++mButton.cnt;
    if(U_ButTest)   // Not Pressed
    {
        if(mButton.cnt & 0x80) 	// Always 	released
        {
            if(t == 2*FGERKON)
            {
//                printf("But=%02X\n",mButton.state);
                ParseButCode(mButton.state);
                mButton.state = 1;
            };
        }
        else    // Rising edge
        {
            mButton.cnt = 0x80;
            if(t > (FGERKON+(FGERKON>>2)))		// Press error
            {
                mButton.state = 0;
            }
            else if(t > (FGERKON>>1)) // long
            {
                mButton.state  |= 1;
            }
            else					// short
            {
                mButton.state  |= 0;
            };
        };
    }
    else        // Pressed
    {
        U_ButRst;
        if(mButton.cnt & 0x80)	// Now pressed
        {
            mButton.state <<= 1;
            mButton.cnt = 0;
        }
        else						// Always pressed
        {
            if(t == (FGERKON<<2))
            {
#define SpecialCode 0x06
                ParseButCode(SpecialCode);
//                CounterState ^= 1;
//                if(CounterState) G_LedMask = MaskError;
//                G_LedMask = 0xFF;
                mButton.state = 0;
//                sbit(Flags,flRadioCal);
            };
        };
    };
//    if(Bcode) ParseButCode(Bcode);
}*/



void InitClock(void)
{
//    PMAPPWD = 0x02D52;  // Enable Write-access to modify port mapping registers
//    P4MAP7 = PM_MCLK;
//    PMAPPWD = 0;        // Disable Write-Access to modify port mapping registers
//
//    P4SEL |= BIT7;
//    P4DIR |= BIT7; // MCLK set out to pins
    P5SEL |= BIT0|BIT1;
// setup clock to FSYS
    __bis_SR_register(SCG0);    // Disable the FLL control loop
    UCSCTL0 = 0x0000;           // Set lowest possible DCOx, MODx
    UCSCTL1 = DCORSEL_2;        // Set RSELx for DCO = FSYS
    UCSCTL2 =0x0000 + (FSYS/FREQXT1) - 1;   // 15
//   UCSCTL3 = SELREF__REFOCLK;               //  REFOCLK div 1
    UCSCTL3  = SELREF__XT1CLK;
//    UCSCTL4 = SELA__REFOCLK|SELS__DCOCLKDIV|SELM__DCOCLKDIV;         // DCOCLKDIV DCOCLKDIV
    UCSCTL4  = SELA__XT1CLK|SELS__DCOCLK|SELM__DCOCLK;     // f(ACLK)/1  f(SMCLK)/1  f(MCLK)/1
//    UCSCTL4  = SELA__XT1CLK|SELS__DCOCLKDIV|SELM__DCOCLKDIV;
    UCSCTL5 = 0;
    UCSCTL6 = XT2OFF|XT1OFF; //  XT2 off XT1 off
    UCSCTL6 |= XCAP_3|XT1DRIVE_3;    // Select XT1
    UCSCTL6 &= ~XT1OFF;  // Start XT1
    UCSCTL8 = SMCLKREQEN|MCLKREQEN|ACLKREQEN ; // MCLKREQEN
//     Selects XT1 bypass input swing level. Must be set for reduced swing operation.
//      0b = Input range from 0 to DVCC
//      1b = Input range from 0 to DVIO
//    UCSCTL9 = 0x0003;  //
    __bic_SR_register(SCG0);
// Loop until XT1,XT2 & DCO stabilizes - In this case loop until XT1 and DCo settle
    do
    {
        UCSCTL7 &= ~(XT2OFFG | XT1LFOFFG | DCOFFG);    // Clear XT2,XT1,DCO fault flags
        SFRIFG1 &= ~OFIFG;                      // Clear fault flags
    }
    while (SFRIFG1&OFIFG);                    // Test oscillator fault flag
    UCSCTL6 &= ~XT1DRIVE_3;
}



void InitPMM (void)
{
    SFRRPCR |= SYSRSTUP| SYSRSTRE;  // pull-up on RESET
//    SYSNMIIES  0b = NMI on rising edge
//    SYSNMI 0b = Reset function

//SFRIE1 = 0; SFRIFG1
// SYSRSTIV 16h = WDT time out (PUC)

// Configure PMM DVCC >  Fsys max <
// PMMSWPOR RW 0h Software power-on reset. Setting this bit to 1 triggers a POR.
//fSYS Max      DVCC    SVSHRVL[1:0]  SVSMHRRL[2:0]  PMMCOREV[1:0]
//  8 MHz       >1.8 V          00          000         00
//  12          >2.0            01          001         01
//  20          >2.2            10          010         10
//  25          >2.4            11          011         11
#define Vcore 0x00
// Turn off SVSH, SVSM
    PMMCTL0_H = 0xA5;
    SVSMHCTL = 0;
    SVSMLCTL = 0;
    PMMCTL0_H = 0x00;
}



void InitLed(void)
{
    R_LedInit;
    R_LedOff;
    R_LedOn;
    G_LedInit;
    G_LedOff;
    G_LedOn;
}



/*******************************************************************************
* @fn JumperPORT_Init
* @bref Initialization of Pins to connect Jumpers
*******************************************************************************/
void UnusedPORT_Init (void)
{
P3OUT&=~(BIT3|BIT6);
P3DIR|= (BIT3|BIT6);
P2OUT&=~(BIT3|BIT4|BIT5);
P2DIR|= (BIT3|BIT4|BIT5);
PJOUT&=~(BIT3|BIT2|BIT1|BIT0);
PJDIR|= (BIT3|BIT2|BIT1|BIT0);
}



void InitTimer0(void)
{
//    PMAPPWD = 0x02D52;
//    P1MAP3 = PM_TA0CCR1A;
//    PMAPPWD = 0;
//    P1DIR |= BIT3;
//    P1SEL |= BIT3;
//    TA0CTL = ID__1|TASSEL_1 | MC_2 | TACLR; // div1  SMCLK, upmode, clear TAR
//    TA0CCR0 = (FLF/FGERKON)-1;
//    TA0CCTL0 = CCIE; // CCR0 interrupt enabled
}



/*void InitAdc(void)
{
//    ADC_state=0;
//    P6SEL |= BIT0|BIT1|BIT2|BIT3;
//    P5SEL |= BIT0;
//// By default, REFMSTR=1 => REFCTL is used to configure the internal reference
//    while(REFCTL0 & REFGENBUSY);  // If ref generator busy, WAIT
//#ifdef TEMPERATURE
//    REFCTL0 |= REFVSEL_2|REFON;   // Select internal ref = 2.5V
//#else
//    REFCTL0 |= REFVSEL_0|REFON;   // Select internal ref = 1.5V
//#endif
//// Internal Reference ON
//    ADC10CTL0 &= ~ADC10ENC;
//    ADC10CTL0 = ADC10SHT_2 | ADC10ON;
//    ADC10CTL1 |= ADC10SHP;                    // ADCCLK = MODOSC; sampling timer
//    ADC10CTL2 |= ADC10RES;                    // 10-bit conversion results
////  ADC10IE |= ADC10IE0;                 // Enable ADC conv complete interrupt
//    ADC10MCTL0 |= ADC10INCH_0 | ADC10SREF_1;  // A0 ADC input select; Vref=internal

}*/



void InitSleep(void)
{
//    WDTCTL = WDT_MDLY_32;                     // WDT 32ms, SMCLK, interval timer
//    WDTCTL = WDT_ADLY_250;  /* 250ms   " */
//    SFRIE1 |= WDTIE;                          // Enable WDT interrupt
//    __bis_SR_register(LPM0_bits | GIE);       // Enter LPM0, enable interrupts
//   __no_operation();

//   SFRIE1 |= WDTIE;
}



void InitRTC(void)
{
    uint8_t iT[8];
// Setup RTC Timer
    RTCCTL01 = RTCMODE|RTCSSEL_3|RTCBCD; // RTC Mode, RTC1PS, 8-bit ovf, in BCD format
// overflow interrupt enable
    RTCPS0CTL = RT0PSDIV_6 ; // RT0IP_0 // ACLK, /256, start timer
    RTCPS1CTL = RT1SSEL_3 | RT1PSDIV_6 | RT1IP_1 | RT1PSIE;  // out from RT0PS, /128, start timer

    /*WinterSummer = iT[6]&0xC0;
    iT[4]  = iT[3]&0x1F;   // DAY
    iT[3]  = iT[3]/32    ; // DOW (0-6)
    iT[3] |= WinterSummer;
    iT[5]  = iT[6]&(~0xC0);   // MON
    iT[6]  = iT[7];*/
    iT[0]=0x00;	//sec
    iT[1]=0x00;	//min
    iT[2]=0x00;	//Hour
    iT[3]=0x04;	//Dow
    iT[4]=0x01;	//Day
    iT[5]=0x01;	//Mon
    iT[6]=0x15;	//Year
//    TimeUnix(iT);
    SetupRTC(iT);  // uart RX Error;
}
//



void SetupRTC(uint8_t *TimeBuf)
{
    RTCSEC  = TimeBuf[0];
    RTCMIN  = TimeBuf[1];
    RTCHOUR = TimeBuf[2];
    RTCDOW  = TimeBuf[3]&(~0xC0);
    //WinterSummer = TimeBuf[3]&0xC0;
    RTCDAY  = TimeBuf[4];
    RTCMON  = TimeBuf[5];
    RTCYEAR = TimeBuf[6]+0x20<<8;
}



////ATD=01 27 10 05 14 06 13
//void PrintRTCDateTime(void)
//{
//    //    RealTimeClock.tm_sec = RTCSEC;
////    RealTimeClock.tm_min = RTCMIN;
////    RealTimeClock.tm_hour= RTCAHOUR;
//////    RealTimeClock.tm_wday.day_of_week   =4;
//////    RealTimeClock.tm_wday.day.SWF = 1;
////    RealTimeClock.tm_wday.day.wday = RTCDOW;
////    RealTimeClock.tm_mday = RTCDAY;
////    RealTimeClock.tm_mon = RTCMON;
////    RealTimeClock.tm_year=RTCYEAR;
//////Пт. июня 14 11:16:55 OMST 2013
//////    *RealTimeClock.tm_loc++='O';
//////    *RealTimeClock.tm_loc++='m';
//////    *RealTimeClock.tm_loc++='s';
//////    *RealTimeClock.tm_loc++='k';
//////    *RealTimeClock.tm_loc++=0;
////    RealTimeClock.tm_loc = +6;
////    printf("OK DATE ");
//// day of week
//    printf("\n%s,",DayOfWeek[RTCDOW]);
//// time
//    printf(" %02u:%02u:%02u",
//           RTCHOUR,
//           RTCMIN,
//           RTCSEC
//          );
//// date
//    printf(" %02u-%02u-%04u",
//           RTCDAY,
//           RTCMON,
//           RTCYEAR);
//// GMT
//    printf(" %s",DayOfWeek[7]);
//    printf("\n");
//}
////
//void PrintDateTimeHex(uint8_t flWinterSummer)
//{
//    printf("ATD=");
//// time
//    printf("%02u%02u%02u",RTCSEC,RTCMIN,RTCHOUR);
//    flWinterSummer|= RTCDOW+1;
//// date
//    printf("%02X%02u%02u%02u",flWinterSummer,RTCDAY,RTCMON,RTCYEAR-2000);
//    //// day of week
////    printf(" %s",DayOfWeek[RTCDOW]);
////// GMT
////    printf(" %s",DayOfWeek[7]);
//    printf("\r\n");
//}
//
//



void StopWdt(void)
{
WDTCTL = WDTPW | WDTHOLD;   // Stop WDT
//; Periodically clear an active watchdog
//MOV #WDTPW+WDTIS2+WDTIS1+WDTCNTCL,&WDTCTL
//;
//; Change watchdog timer interval
//MOV #WDTPW+WDTCNTCL+SSEL,&WDTCTL
//;
//; Stop the watchdog
//MOV #WDTPW+WDTHOLD,&WDTCTL
//;
//; Change WDT to interval timer mode, clock/8192 interval
//MOV #WDTPW+WDTCNTCL+WDTTMSEL+WDTIS2+WDTIS0,&WDTCTL
//
//
//
}



void RunWdt(void)
{
WDTCTL  = WDTPW | WDTSSEL__ACLK | WDTIS__512K | WDTCNTCL | WDTTMSEL;
SFRIE1 |= WDTIE;                          // Enable WDT interrupt
//StopWdt();
}



void StartXT1(void)
{
    P5SEL |= BIT0|BIT1;
    UCSCTL6 |= XCAP_3|XT1DRIVE_3;                     // Select XT1
    UCSCTL6 &= ~XT1OFF;  // Start XT1
//    __bic_SR_register(SCG0);
    UCSCTL3  = SELREF__XT1CLK;
    UCSCTL4  = SELA__XT1CLK|SELS__DCOCLKDIV|SELM__DCOCLKDIV;
//    __bic_SR_register(SCG0);
    // Loop until XT1,XT2 & DCO stabilizes - In this case loop until XT1 and DCo settle
    do
    {
        UCSCTL7 &= ~(XT2OFFG | XT1LFOFFG | DCOFFG);    // Clear XT2,XT1,DCO fault flags
        SFRIFG1 &= ~OFIFG;                      // Clear fault flags
    }
    while (SFRIFG1&OFIFG);                    // Test oscillator fault flag
    UCSCTL6 &= ~XT1DRIVE_3;
}



void StopXT1(void)
{
    UCSCTL3  = SELREF__REFOCLK;               //  REFOCLK div 1
    UCSCTL4  = SELA__REFOCLK|SELS__DCOCLKDIV|SELM__DCOCLKDIV;
    UCSCTL6 |= XT1OFF;  // Stop XT1
}



void StartXT2(void)
{
    UCSCTL6 &= ~XT2OFF;  // Start XT2
// Loop until XT1,XT2 & DCO stabilizes - In this case loop until XT1 and DCo settle
    //    do
//    {
//        UCSCTL7 &= ~(XT2OFFG | XT1LFOFFG | DCOFFG);    // Clear XT2,XT1,DCO fault flags
//        SFRIFG1 &= ~OFIFG;                      // Clear fault flags
//    }
//    while (SFRIFG1&OFIFG);                    // Test oscillator fault flag
}



void WaitXT2(void)
{
// Loop until XT1,XT2 & DCO stabilizes - In this case loop until XT1 and DCo settle
    do
    {
        UCSCTL7 &= ~(XT2OFFG | XT1LFOFFG | DCOFFG);    // Clear XT2,XT1,DCO fault flags
        SFRIFG1 &= ~OFIFG;                      // Clear fault flags
    }
    while (SFRIFG1&OFIFG);                    // Test oscillator fault flag
}



void StopXT2(void)
{
    UCSCTL6 |= XT2OFF;  // Stop XT2
}



void InitEEPROM(void)
{
//    uint8_t j;
//    for(j=0; j< eeSize; j++) eT[j] = eeStore[j];
}



uint8_t  eeRead(uint8_t *Flash_ptr)
{
    return *Flash_ptr;
}



void eeErase(uint8_t *eAdr)
{
//   WDTCTL = WDTPW | WDTHOLD;   // Stop WDT
    _DINT();
    FCTL4 = FWKEY;
    FCTL3 = FWKEY;
    FCTL1 = FWKEY|ERASE;    // Set Bank Erase bit
    while(FCTL3 & BUSY);
    *eAdr = 0x00;           // Dummy erase byte
    FCTL1 = FWKEY;          // Clear WRT bit
    FCTL3 = FWKEY|LOCK;     // Set LOCK bit
    FCTL4 = FWKEY|LOCKINFO;
//    RunWdt();
    _EINT();
}



/*******************************************************************************
* Function to write Info memory 
* *Flash_ptr - pointer to memory for write
* *eData     - pointer to data buffer (from)
* cnt        - counter bytes to write
* return     - true = write memory is sucessful
			false = write memory is not sucessful (memory not empty)
*******************************************************************************/
bool eeWrite(void *Flash_ptr, void *eData, uint8_t cnt)
{
uint8_t i;
uint8_t *ptr=Flash_ptr;
uint8_t *buf=eData;

for(i=0; i<cnt; i++)
	{
	//if((ptr[i]&buf[i]) < buf[i]) return false;
	if((ptr[i]&buf[i]) < buf[i])
		{
		eeErase(Flash_ptr);
		break;
		}
	};

__disable_interrupt();
FCTL4 = FWKEY;			// unlock INFO
FCTL3 = FWKEY;			// Clear Lock bit

FCTL1 = FWKEY|WRT;		// Set WRT bit for write operation
while(cnt--) *ptr++ = *buf++;	// Write value to flash
FCTL1 = FWKEY;			// Clear WRT bit
FCTL3 = FWKEY|LOCK;		// Set LOCK bit
FCTL4 = FWKEY|LOCKINFO;	// unlock INFO
__enable_interrupt();
return true;
}



/*******************************************************************************
This code will erase the security fuse in an MSP430F5419.

You will need to program this code into your device before it is secured,
with some way to activate it (eg a port pin or serial command).
 Keep in mind the code as shown does not erase program memory,
 so your application is left unsecured. To avoid this just erase the rest
 of flash before performing the reset.
*******************************************************************************/
void unlockJTAG(void)
{
// segment erase BSL signature
//
//    __disable_interrupt();              // no interrupts during erase
//    WATCHDOG_ENABLE_SLOW;               // set watchdog period to 16s
//    __watchdog_clear ();
    StopWdt();
    _DINT();
    while( ( FCTL3 & WAIT ) == 0 );     // ensure no FLASH write is in progress
    SYSBSLC = 0;                        // enable and unlock BSL segment 3
    FCTL4 = FWKEY;                      // unlock INFO
    FCTL3 = FWKEY;                      // unlock FLASH and INFO A
    FCTL1 = FWKEY | WRT;              // select segment erase mode
    ( *(uint32_t*)0x17fc ) = 0;          // dummy write starts erase
//    while( ( FCTL3 & BUSY ) == BUSY );  // wait for erase complete
//    ( *(uint8_t*)0x17fd ) = 0;          // dummy write starts erase
//    while( ( FCTL3 & BUSY ) == BUSY );  // wait for erase complete
//    ( *(uint8_t*)0x17fe ) = 0;          // dummy write starts erase
//    while( ( FCTL3 & BUSY ) == BUSY );  // wait for erase complete
//    ( *(uint8_t*)0x17ff ) = 0;          // dummy write starts erase
//    while( ( FCTL3 & BUSY ) == BUSY );  // wait for erase complete
    FCTL1 = FWKEY; // Clear WRT bit
    FCTL3 = FWKEY|LOCK;  // Set LOCK bit
    FCTL4 = FWKEY|LOCKINFO;                      // unlock INFO
    SYSBSLC = 0x8000;                        // enable and unlock BSL segment 3
//    WATCHDOG_FORCE_RESET;               // use watchdog to reset processor
// end
}



/*******************************************************************************
This code will erase the security fuse in an MSP430F5419.

You will need to program this code into your device before it is secured,
with some way to activate it (eg a port pin or serial command).
 Keep in mind the code as shown does not erase program memory,
 so your application is left unsecured. To avoid this just erase the rest
 of flash before performing the reset.
*******************************************************************************/
void lockJTAG(void)
{
////
//// segment erase BSL signature
////
    StopWdt();
    _DINT();
//    __disable_interrupt();              // no interrupts during erase
////    WATCHDOG_ENABLE_SLOW;               // set watchdog period to 16s
//    __watchdog_clear ();
//    while( ( FCTL3 & WAIT ) == 0 );     // ensure no FLASH write is in progress
    SYSBSLC = 0;                        // enable and unlock BSL segment 3
    FCTL4 = FWKEY;                      // unlock INFO
    FCTL3 = FWKEY;                      // unlock FLASH and INFO A
    FCTL1 = FWKEY | WRT;              // select segment erase mode
//    FCTL1 = FWKEY | ERASE;              // select segment erase mode
    ( *(uint32_t*)0x17fc ) = 0;          // dummy write starts erase
//        ( *(uint32_t*)0x17fc ) = 4;          // dummy write starts erase
    while( ( FCTL3 & BUSY ) == BUSY );  // wait for erase complete
////    WATCHDOG_FORCE_RESET;               // use watchdog to reset processor
    {
//       printf(" Reset CHIP\n\n");
        _DINT();
        WDTCTL = 0;
        while(1);
    }
// end
}



/*
#ifdef ClearLockJTAG      
   SYSBSLC=SYSBSLSIZE0+SYSBSLSIZE1; //UnLock BSL
   _DINT(); //Write Lock JTAG 
   FCTL3=FWKEY; //Clear Lock
   FCTL1=FWKEY+WRT;
   LockJTAG[0]=0; 
   LockJTAG[1]=0; //Достаточно в одно слово записать
//   LockJTAG[2]=0; 
//   LockJTAG[3]=0;
   FCTL1=FWKEY;
   FCTL3=FWKEY+LOCK; //SET Lock
   _EINT();  
   SYSBSLC=SYSBSLSIZE0+SYSBSLSIZE1+SYSBSLPE;  //Lock BSL  
#endif
   
#ifdef SetLockJTAG      
   SYSBSLC=0; //SYSBSLSIZE0+SYSBSLSIZE1; //UnLock BSL
   _DINT(); //Write Lock JTAG 
   FCTL3=FWKEY; //Clear Lock
   FCTL1=FWKEY+WRT;
   LockJTAG[0]=0x55; 
//   LockJTAG[1]=0x55; //необязательно 
   LockJTAG[2]=0x55;  //Обязательно и во второе слово
//   LockJTAG[3]=0x55; //необязательно
   FCTL1=FWKEY;
   FCTL3=FWKEY+LOCK; //SET Lock
   _EINT();  
   SYSBSLC=SYSBSLSIZE0+SYSBSLSIZE1+SYSBSLPE;  //Lock BSL
#endif
   
#ifdef ReadLockJTAG         
   SYSBSLC=SYSBSLSIZE0+SYSBSLSIZE1; //UnLock BSL
   OutResult[0]=LockJTAG[0]&0xF; OutResult[1]=LockJTAG[0]>>4; OutResult[2]=LockJTAG[1]&0xF; OutResult[3]=LockJTAG[1]>>4; 
   OutResult[4]=LockJTAG[2]&0xF; OutResult[5]=LockJTAG[2]>>4; OutResult[6]=LockJTAG[3]&0xF; OutResult[7]=LockJTAG[3]>>4;
   SYSBSLC=SYSBSLSIZE0+SYSBSLSIZE1+SYSBSLPE;  //Lock BSL
   TempChar=9; DigitToLcd();
   _EINT();
   TimeOut1s(); //1sek 
   _DINT();
#endif*/



void manipulateBSL(void)
{
    uint8_t *ptrToBsl;
    uint16_t  j;
    ptrToBsl = (uint8_t*)0x1800;
    printf(" eeprom = ");
    for(j=0; j<16; j++)
    {
        printf(" %02x",*ptrToBsl++);
    };
    printf("\n");

    SYSBSLC = 0;                        // enable and unlock BSL segment 3

    ptrToBsl = (uint8_t*)0x1000;
    printf(" bslcode == \n@1000\n");
    for(j=0; j < 2048; j++)
    {
        printf(" %02x",*ptrToBsl++);
        if(!((j+1)%16))     printf("\n");
        if(!((j+1)%256))     printf("\n");
    };
    printf("\n");

//    ptrToBsl = (uint8_t*)0x17FC;
//    while( ( FCTL3 & WAIT ) == 0 );     // ensure no FLASH write is in progress
//    SYSBSLC = 0;                        // enable and unlock BSL segment 3
//    FCTL4 = FWKEY;                      // unlock INFO
//    FCTL3 = FWKEY;                      // unlock FLASH and INFO A
//    FCTL1 = FWKEY | WRT;              // select segment erase mode
//    *ptrToBsl = 4;          // dummy write starts erase
//    while( ( FCTL3 & BUSY ) == BUSY );  // wait for erase complete
//    FCTL1 = FWKEY; // Clear WRT bit
//    FCTL3 = FWKEY|LOCK;  // Set LOCK bit
    SYSBSLC |= 0x8000;
//    unlockJTAG();
    SYSBSLC = 0;                        // enable and unlock BSL segment 3
    ptrToBsl = (uint8_t*)0x17FC;
    printf(" bslcode == %08X\n",*ptrToBsl);
    SYSBSLC |= 0x8000;
}



uint16_t  CRC16(uint8_t *buf,uint8_t length)
{
#define CRCINIT 0xFFFF
#define CRCPOLY 0x1021
// compute  x 16 + x 12 + x 5 +1  CRC-CCITT-BR
    /*  mov     #0FFFFh,&CRCINIRES  ; initialize CRC
    mov.b   #00031h,&CRCDIRB_L  ; "1"
    mov.b   #00032h,&CRCDIRB_L  ; "2"
    mov.b   #00033h,&CRCDIRB_L  ; "3"
    mov.b   #00034h,&CRCDIRB_L  ; "4"
    mov.b   #00035h,&CRCDIRB_L  ; "5"
    mov.b   #00036h,&CRCDIRB_L  ; "6"
    mov.b   #00037h,&CRCDIRB_L  ; "7"
    mov.b   #00038h,&CRCDIRB_L  ; "8"
    mov.b   #00039h,&CRCDIRB_L  ; "9"
    cmp     #029B1h,&CRCINIRES  ; compare result
    */
    uint8_t i;
    uint16_t  crc = CRCINIT;
    while(length--)
    {
        crc ^= *buf++ << 8;
        for (i = 0; i < 8; i++)
        {
            crc = crc & 0x8000 ? (crc << 1) ^ CRCPOLY : crc << 1;
        };
    };
    return crc;
}



void PrintInfo(void)
{
uint8_t ReadSingleReg(uint8_t addr);
//    uint8_t j;
printf("\n");
printf("Radio 433MHz Modem\n");
//    printf(" RF Frequency = ");
//    f = (uint16_t )eeStore[eFreqRF]<<8 | eeStore[eFreqRF+1];
//    printf("%u.",f/10);
//    printf("%u MHz",f-10*(f/10));
//    printf(" XT Frequency = ");
//    f = (uint16_t )eeStore[eFreqXT]<<8 | eeStore[eFreqXT+1];
//    printf("%u000 Hz\r",f);
//    printf(" SyncWord = ");
//    printf("%c%c\r",eeStore[eSync+1],eeStore[eSync+0]);
//    printf("SyncWord 0x%02X%02X",ReadSingleReg((uint8_t)SYNC1),ReadSingleReg((uint8_t)SYNC0));
//    f=0x5135;
uint8_t *ChipID_ptr = (uint8_t*) 0x1A04;
printf(" ChipID CC430F%02X%02X",*ChipID_ptr,*(ChipID_ptr+1));
//    f=0x1101;
//    ReadSingleReg(PARTNUM);
printf(" Radio 0x%02X%02X\n",ReadSingleReg(PARTNUM),ReadSingleReg(VERSION));
//    PrintRTCDateTime();
//
printf(" SYNC = 0x%02X%02X (TI)\r\n",ReadSingleReg(SYNC1),ReadSingleReg(SYNC0));
//    WriteSingleReg(SYNC0,eeStore[eSync]);
//    WriteSingleReg(SYNC1,eeStore[eSync+1]);
printf(" FREQ = 0x%02X%02X%02X\r\n",ReadSingleReg(FREQ2), ReadSingleReg(FREQ1), ReadSingleReg(FREQ0));
    
printf(" Compiled Date/Time = %s %s\r", __DATE__, __TIME__);
printf(" Build = %d\r", __BUILD_NUMBER__);
printf(" Version = %d\r", __VER__);
}
//

