
#include <stdint.h>

#include "cc430x513x.h"

#include "Utils\Utils.h"
#include "RFCounter433.h"
#include "Radio.h"
#include "WMBus\WMBus.h"
#include "Board\Board.h"

#include "Install.h"




void Install(void)
{
    uint8_t i,AttemptCounter;
    struct wmbPacket *wP;
#define AttemptsMAX 7
    AttemptCounter=0;
    switch(InstallState)
    {
    case iInit:
        InstallState=iFindCS;
        break;
    case iFindCS:
        ++PowerDog;
        halRfWriteReg(MCSM0,0x18); // When going from IDLE to RX or TX
        rfRxCnt=rfReceive(rfBufRx,L_PACKET40 ,0);
        halRfWriteReg(MCSM0,0x08); // When going from IDLE to RX or TX
        if(rfRxCnt==255)
        {
            DelayXT1(2622); // 24 bytes
            ++PowerDog;
            rfRxCnt=rfReceive(rfBufRx,L_PACKET40 ,0);
        };
        if(rfRxCnt==0)
        {
            InstallState=iFind46;
        };
        break;
    case iFind46:
        DelayXT1(1529); // 14 bytes
        rfRxCnt=rfReceive(rfBufRx,L_PACKET40,1529); // 14 bytes
        PowerDog += 2*(12+2+L_PACKET53);
        if(rfRxCnt=wmbCheck(rfBufRx,rfRxCnt))
        {
            if(rfBufRx[1]==SND_IR)
            {
                for(i=0; i<rfRxCnt; i++) wmbTx[i]=rfBufRx[i];
                wmbTxCnt=rfRxCnt;
                InstallState=iSend06;
//                RadioDog=RadioInterval<<6;
                G_LedMask=MaskProg;
//                PowerDog=0;
            }
        }
        break;
    case iSend06:
        rfSend(rfBufRx,wmbCreate(rfBufRx,SND_IR_ACK));
        PowerDog += 2*(12+2+L_PACKET53);
        if(++AttemptCounter > AttemptsMAX) InstallState=iStoreInfo;
        else InstallState=iWait40;
        break;
    case iWait40:
        rfRxCnt=rfReceive(rfBufRx,L_PACKET40,1529*3); // 14 bytes
        PowerDog += 2*(12+2+L_PACKET53);
        if(wmbCheck(rfBufRx,rfRxCnt))
        {
            if(!wmbParse(rfBufRx,wmbTx))
            {
                if(rfBufRx[1]==SND_IR) InstallState=iSend06;
                else if(rfBufRx[1]==SND_END || rfBufRx[1]==SND_ACK )
                {
                    InstallState=iSend00;
                }
            }
        }
        else InstallState=iSend06;
        break;
    case iSend00:
        rfSend(rfBufRx,wmbCreate(rfBufRx,SND_ACK));
        PowerDog += 2*(12+2+L_PACKET53);
        InstallState=iStoreInfo;
        break;
    case iStoreInfo:
        __disable_interrupt();
        wP=(struct wmbPacket *)wmbTx;
        WaterCounter[0].cnt[0] = wP->block2[0].CNT1[0];
        WaterCounter[0].cnt[1] = wP->block2[0].CNT1[1];
        WaterCounter[0].cnt[2] = wP->block2[0].CNT1[2];
        WaterCounter[0].cnt[3] = wP->block2[0].CNT1[3];
        eeWrite((uint8_t*)eeCustomer, wmbTx, wmbTxCnt);
        __enable_interrupt();
        CounterState = cWait;
        R_LedMask=MaskNorma;
        break;
    default:
        CounterState = cWait;
        RadioDog=RadioInterval(1);
        break;
    };
}
