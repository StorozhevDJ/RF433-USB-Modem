

#include "cc430x513x.h"

#include "Debug.h"








void Init_Debug_UART(void)
{
#if DEBUG_ON
// ����������� ����� UART0
P1DIR &= ~BIT5; //RX
P1DIR |= BIT6;  //TX
P1SEL |= BIT5 | BIT6;

UCA0CTL1 = UCSWRST;		// ��������� ���� UART � ��������� ������ ��� ���������
UCA0CTL1 |= UCSSEL__SMCLK;	// ����� �������� ������������

// ���������� � �������� ��������, ���������� � ������� ������������ 
//http://mspgcc.sourceforge.net/baudrate.html
//http://www.daycounter.com/Calculators/MSP430-Uart-Calculator.phtml
//14400
UCA0BR0 = 0x45;
UCA0BR1 = 0x00;
UCA0MCTL = 0xAA;

// ��������� UART � ������� ���������
UCA0CTL1 &= ~UCSWRST;
#endif //DEBUG_ON
}



#if DEBUG_ON
/*int putchar(int c)
{
// ��� ����������� ������ ��������
while (!(UCA0IFG & UCTXIFG));
// ���������� � ����� ��������� ����
UCA0TXBUF = (unsigned char) c;
return c; // �������� ������� ������� ������� ���������� ����
}*/
#endif //DEBUG_ON