
#include <stdarg.h>

#include "UART.h"




volatile TSusart usart;


void (*USART_Rx_complete)(char *buf, char cnt);		//Pointer to receiv function




/*******************************************************************************
*
*******************************************************************************/
void USART_Init(void (*rx_funct_ptr)(char *buf, char cnt))
{
USART_Rx_complete=rx_funct_ptr;

PMAPPWD = 0x02D52;				// Get write-access to port mapping regs
P1MAP5 = PM_UCA0RXD;			// Map UCA0RXD output to P1.5
P1MAP6 = PM_UCA0TXD;			// Map UCA0TXD output to P1.6
PMAPPWD = 0;					// Lock port mapping registers
P1DIR |= BIT6|BIT3|BIT7;			// Set P1.6 as TX output, P1.3 as RTS output, P1.7 as DTR
P1REN&=~(BIT5|BIT6);
P1REN|=BIT3|BIT7;
P1SEL |= BIT5 + BIT6;			// Select P1.5 & P1.6 to UART function
//
UCA0CTL0 = 0; // UART 8bit
UCA0CTL1 |= UCSWRST; // **Put state machine in reset**
UCA0CTL1 |= UCSSEL_2; // SMCLK
UCA0BR0   = FSYS/BAUD;  // 1.00MHz 38400 (see User's Guide)
UCA0BR1   = 0;          // 1.00MHz 38400
UCA0MCTL |= UCBRS_1 + UCBRF_0; // Modulation UCBRSx=1, UCBRFx=0
UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**

UCA0IE |= UCRXIE;
}



/*******************************************************************************
* USART reciev interrupt handler
* usart_rx_buf - buffer to recieved data
* usart_rx_cnt - pointer to usart_rx_buf
*******************************************************************************/
#pragma vector=USCI_A0_VECTOR
__interrupt void USART_Interrupt(void)
{
//Программа обработки принятого байта
if (UCA0IV & UCTXIFG) USART_RxBufEnqueue(UCA0RXBUF);
}



/*******************************************************************************
* Send Char to USART
* data - char to send
* Return - none
*******************************************************************************/
inline void USART_SendChar(char data)
{
while (!(UCA0IFG & UCTXIFG)); //Ожидание опустошения буфера приема (окончания прошлой передачи)
UCA0TXBUF = data; //Начало передачи данных			        
}



/*******************************************************************************
* Send String to USART.
* str - pointer to transmitted string
* Return - transmitted lenght
*******************************************************************************/
char USART_SendString(void * str)
{
UART_SET_RTS;
unsigned char *ptr=(unsigned char *)str;
unsigned char len=0;
while (*ptr)
	{
	USART_SendChar(*ptr++);
	len++;
	}
UART_RESET_RTS;
return len;
}



/*******************************************************************************
* Send Buffer to USART.
* buf - pointer to transmitted buffer
* len - lenght data for transmit
*******************************************************************************/
void USART_SendBuf(char * buf, unsigned char len)
{
UART_SET_RTS;//new
for (; len; len--) USART_SendChar(*buf++);
UART_RESET_RTS;
}



/*******************************************************************************
*
*******************************************************************************/
inline uint8_t USART_GetC(void)
{
#ifdef NON_BLOCKING_UART_RX
    return RxBufferDequeue();
#else
    while (!(UCA0IFG & UCRXIFG));    // RX a byte?
    return UCA0RXBUF;
#endif
}



/*******************************************************************************
*
*******************************************************************************/
int putchar(int c)
{
// Ждём опустошения буфера передачи
while (!(UCA0IFG & UCTXIFG));
// Записываем в буфер следующий байт
UCA0TXBUF = (unsigned char) c;
return c; // Прототип функции требует вернуть записанный байт
}



/*******************************************************************************
*
*******************************************************************************/
int getchar(void)
{
return USART_GetC();
}



/*******************************************************************************
* Determination of end receive data
*******************************************************************************/
inline void USART_Timer10ms(void)
{
if (usart.rx.timeout) usart.rx.timeout--;	//Dicrement received timer
else	//Timeout receive
	{
	if (usart.rx.overflow==true)
		{
		usart.rx.overflow=false;
		usart.rx.ptr=0;
		usart.rx.cnt=0;
//		R_LedOff;
		}
	else if ((usart.rx.cnt)&&(usart.mode==BIN))
		{
		usart.rx.complete=true;	//Receive completed
		usart.rx.ptr=0;
		USART_Rx_complete((char *)usart.rx.buf, usart.rx.cnt);	//вызовим функцию обработки
		usart.rx.cnt=usart.rx.ptr;
//		R_LedOff;
		}
	}
}



/*******************************************************************************
* Addition of bytes in the buffer queue
*******************************************************************************/
inline void USART_RxBufEnqueue(char byte)
{
if (usart.rx.ptr<RX_BUF_SIZE) usart.rx.buf[usart.rx.ptr++]=byte;
if (usart.rx.ptr==0)
	{
	usart.rx.overflow=true;
	usart.rx.ptr--;
	//USART_Rx_complete((char *)usart.rx.buf, usart.rx.cnt);
	}
if (usart.rx.cnt<RX_BUF_SIZE) usart.rx.cnt++;
usart.rx.timeout=RX_TIMEOUT_VAL;

//Если режим передачи ASCII символов
if (usart.mode==ASCII)
	{
	R_LedOn; 
	 //если первый байт CR или LF, пропустим его
	if ((usart.rx.ptr==1)&&((byte=='\r')||(byte=='\n')))
		{
		usart.rx.ptr=0;
		usart.rx.cnt=0;
		return;
		}
	//и приняли символ конца строки или символ 0
	if ((byte==RX_END_STR_CHAR)||(byte==0))
		{
		usart.rx.complete=true;	//выставим флаг "прием завершен"
		usart.rx.ptr=0;
		if (usart.rx.cnt<RX_BUF_SIZE)
			{
			usart.rx.buf[usart.rx.cnt]=0;//Add end string
			//usart.rx.cnt++;
			}
		USART_Rx_complete((char *)usart.rx.buf, usart.rx.cnt);	//вызовим функцию обработки
		usart.rx.cnt=usart.rx.ptr;
		R_LedOff;
		}
	}
}



/*******************************************************************************
* Change receive mode to ASCII or Binary
*******************************************************************************/
inline void USART_SetMode(Emode_data m)
{
usart.mode=m;
}