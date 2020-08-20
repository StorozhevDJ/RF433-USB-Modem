
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
	//��������� � ������ ������
	case wInit:
		//wmbTxCnt=wmbCreate(wmbTx, RSP_UD); // RSP_UD   0x08
		WorkState=wFindCS;
		//����������� ����������
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
		RadioDog=RadioInterval(5);	//� ������� 30 ��� ����� ����� ��������� �������
		
	break;
	
	//����� �������
	case wFindCS:
		ReceiveOn();	//�������� �����
		//��������� ������� �������
		if (CheckCarrierSense())	//���� �����
			{
			G_LedOn;
			WorkState=wFindEndOfPacket;	//�������� � ������ ������
			}
		else 	//����� ���� �� �����, �������� � ���������� �����
			{
			Radio_PowerDown();
			G_LedOff;
			__bis_SR_register(LPM3_bits + GIE); 
			__no_operation();
			}
	break;
	
	//����� ������ � ��������� � ����� ������ �������, ���� ����� ����� ������
	case wFindEndOfPacket:
		{
		unsigned char i;
		//� ������� ���������� ������� (12/16=0.75�) �������� ������� ��� �����
		for (i=10; i>0; i--)
			{
			//���� ������ �������� ������ �� �������
			if (RFRxBufferLength)
				{
				//���� ������ ���������
				if (wmbCheck(RFRxBuffer, RFRxBufferLength))
					{//� ��� ��� ���
					if (wmbCheckAddr(RFRxBuffer))
						{
						wmbTxCnt=wmbParse(RFRxBuffer, wmbTx);	//�������� ��
						WorkState=wSendPacket;				//� �������
						RadioDog=RadioInterval(5);	//� ������� 5 ��� ����� ���������� ��������
						break;
						}
					//���� ������ �� ��� ���, �������� ����� (� ������ ������ �������)
					else WorkState=wFindCS;
					}
				RFRxBufferLength=0;
				//rfRSSI=rfCorrectGain(0);
				}
			else//���� ������ ���, ����
				{
				LowerPower_3_ENTER;
				__no_operation();
				RunWdt();
				}
			}
		if (i==0) WorkState=wFindCS;	//���� ����� �� ��� ������, �������� � ������ �������
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
	
	//�������� ������ �������������
	case wWaitAck:
		if (RFRxBufferLength)	//���� ����� �������
			{
			//��������� ������������ ������
			if (wmbCheck(RFRxBuffer, RFRxBufferLength))
				{
				//���� ��� ��� 
				if (wmbCheckAddr(RFRxBuffer))
					{
					//� ��� ������������� (���������� ������)
					if(RFRxBuffer[1]==SND_END)
						{
						Radio_PowerDown();		//�������� �����

						RadioDog=timeForRadioSleep(1);//
						}
					else//���� �� �������������, � ����� ������
						{
						wmbTxCnt=wmbParse(RFRxBuffer, wmbTx);	//�������� ��
						WorkState=wSendPacket;				//� �������
						//RadioDog=RadioInterval(5);	//� ������� 5 ��� ����� ���������� ��������
						break;
						}
					}
				else//���� ��� �� ���, �����
					{
					Radio_PowerDown();		//�������� �����

					RadioDog=timeForRadioSleep(1);//-RadioInterval(3);//1������ - 3���
					}
				}
			RFRxBufferLength=0;
			}
		//���� ����� �������� ������ ������������� �����, 
	break;

    default:

        RadioDog=timeForRadioSleep(1);
        break;
    };
//���� ����� ������ ����� �����, �������� ������ � ������ �����
if (RadioDog==0)
	{
	Radio_PowerDown();			//�������� �����
	RadioDog=RadioInterval(10);//timeForRadioSleep(1);//�� ���� ������ �
//	if (WorkState==wWaitAck) RadioDog-=RadioInterval(10);

	WorkState=wInit;			//����� ��������
	}
}
