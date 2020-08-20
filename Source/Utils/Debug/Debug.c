

#include "cc430x513x.h"

#include "Debug.h"








void Init_Debug_UART(void)
{
#if DEBUG_ON
// Настраиваем порты UART0
P1DIR &= ~BIT5; //RX
P1DIR |= BIT6;  //TX
P1SEL |= BIT5 | BIT6;

UCA0CTL1 = UCSWRST;		// Переводим блок UART в состояние сброса для настройки
UCA0CTL1 |= UCSSEL__SMCLK;	// Задаём источник тактирования

// Записываем в регистры значения, полученные с помощью калькулятора 
//http://mspgcc.sourceforge.net/baudrate.html
//http://www.daycounter.com/Calculators/MSP430-Uart-Calculator.phtml
//14400
UCA0BR0 = 0x45;
UCA0BR1 = 0x00;
UCA0MCTL = 0xAA;

// Переводим UART в рабочее состояние
UCA0CTL1 &= ~UCSWRST;
#endif //DEBUG_ON
}



#if DEBUG_ON
/*int putchar(int c)
{
// Ждём опустошения буфера передачи
while (!(UCA0IFG & UCTXIFG));
// Записываем в буфер следующий байт
UCA0TXBUF = (unsigned char) c;
return c; // Прототип функции требует вернуть записанный байт
}*/
#endif //DEBUG_ON