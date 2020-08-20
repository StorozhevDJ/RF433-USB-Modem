

#include <cc430x513x.h>

#include "MX25L512.h"




void MX25L512_HW_Init(void)
{
P3OUT|= BIT7;			// Set P3.7 (CS) to High Lewel output (No Select CS)
P3DIR|= BIT7;			// Set P3.7 as CS output
//P1OUT|= (BIT2|BIT3|BIT4);//SO|SI|SCK
P1DIR&=~BIT2;			//miSO to input
P1DIR|= (BIT3|BIT4);	//moSI|SCK to output

P1SEL |= (BIT2|BIT3|BIT4); // Настроить пин 2, 3, 4 порта 1 как вывод периферии


}