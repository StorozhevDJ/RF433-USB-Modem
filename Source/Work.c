
#include <stdint.h>

#include "cc430x513x.h"

#include "Utils\Utils.h"
#include "RFCounter433.h"
#include "Radio.h"
#include "WMBus\WMBus.h"
#include "Board\Board.h"


#include "Work.h"





void Work(void)
{
switch(WorkState)
	{
	//Готовимся к началу работы
	case wInit:
		//wmbTxCnt=wmbCreate(wmbTx, RSP_UD); // RSP_UD   0x08
		WorkState=wFindCS;
		//Настраиваем прерывания
		RF1AIES &= ~BIT9;			// Rising edge of RFIFG9 SYNC
		RF1AIFG &= ~BIT9;			// Clear a pending interrupt
		RF1AIES &= ~BITD;			// Rising edge of RFIFG13 CS
		RF1AIFG &= ~BITD;			// Clear a pending interrupt
		RF1AIE  |= BIT9;	// Enable the interrupt
		//Strobe( RF_SRX );			// Radio is in IDLE following a TX, so strobe SRX to enter Receive Mode
		RF_transmitting=0;
		RFRxBufferLength=0;
		R_LedOff;
		G_LedOff;
		RadioDog=RadioInterval(5);	//В течении 30 сек будем ждать получения запроса
		
	break;
	
	//Поиск несущей
	case wFindCS:
		ReceiveOn();	//Включаем радио
		//Проверяем наличие несущей
		if (CheckCarrierSense())	//Если нашли
			{
			G_LedOn;
			WorkState=wFindEndOfPacket;	//Перейдем к поиску пакета
			}
		else 	//Иначе если не нашли, засыпаем с выключеным радио
			{
			Radio_PowerDown();
			G_LedOff;
			__bis_SR_register(LPM3_bits + GIE); 
			__no_operation();
			}
	break;
	
	//Поиск пакета с переходом в режим поиска несущей, если пакет небыл найден
	case wFindEndOfPacket:
		{
		unsigned char i;
		//В течении некоторого времени (12/16=0.75с) пытаемся поймать наш пакет
		for (i=10; i>0; i--)
			{
			//Если размер принятых данных не нулевой
			if (RFRxBufferLength)
				{
				//Если данные корректны
				if (wmbCheck(RFRxBuffer, RFRxBufferLength))
					{//И они для нас
					if (wmbCheckAddr(RFRxBuffer))
						{
						wmbTxCnt=wmbParse(RFRxBuffer, wmbTx);	//Разберем их
						WorkState=wSendPacket;				//И ответим
						RadioDog=RadioInterval(5);	//В течении 5 сек будем продолжать работать
						break;
						}
					//Если данные не для нас, подождем наших (в режиме поиска несущей)
					else WorkState=wFindCS;
					}
				RFRxBufferLength=0;
				//rfRSSI=rfCorrectGain(0);
				}
			else//Если данных нет, спим
				{
				LowerPower_3_ENTER;
				__no_operation();
				RunWdt();
				}
			}
		if (i==0) WorkState=wFindCS;	//Если пакет не был найден, вернемся к поиску несущей
		}
	break;
	
	//Send Response packet
	case wSendPacket:
		//while (1){rfSend(" 1234567890", 24); RunWdt();};
		TransmitCCA(wmbTx, wmbTxCnt);
//		Transmit(wmbTx,wmbTxCnt);
		WorkState=wWaitAck;
		RFRxBufferLength=0;
		ReceiveOn();
	break;
	
	//Ожидание приема подтверждения
	case wWaitAck:
		if (RFRxBufferLength)	//Если чтото приняли
			{
			//Проверяем правильность пакета
			if (wmbCheck(RFRxBuffer, RFRxBufferLength))
				{
				//Если это нам 
				if (wmbCheckAddr(RFRxBuffer))
					{
					//и это подтверждение (завершение сеанса)
					if(RFRxBuffer[1]==SND_END)
						{
						Radio_PowerDown();		//Выключим радио

						RadioDog=timeForRadioSleep(1);//
						}
					else//Если не подтверждение, а чтото другое
						{
						wmbTxCnt=wmbParse(RFRxBuffer, wmbTx);	//Разберем их
						WorkState=wSendPacket;				//И ответим
						//RadioDog=RadioInterval(5);	//В течении 5 сек будем продолжать работать
						break;
						}
					}
				else//Если уже не нам, уснем
					{
					Radio_PowerDown();		//Выключим радио

					RadioDog=timeForRadioSleep(1);//-RadioInterval(3);//1минута - 3сек
					}
				}
			RFRxBufferLength=0;
			}
		//Если время ожидания приема подтверждения вышло, 
	break;

    default:

        RadioDog=timeForRadioSleep(1);
        break;
    };
//Если время работы радио вышло, завершим работу и пойдем спать
if (RadioDog==0)
	{
	Radio_PowerDown();			//Выключим радио
	RadioDog=RadioInterval(10);//timeForRadioSleep(1);//на одну минуту и
//	if (WorkState==wWaitAck) RadioDog-=RadioInterval(10);

	WorkState=wInit;			//цикла ожидания
	}
}
