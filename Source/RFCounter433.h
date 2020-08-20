#define COUNTER



#define maxTxRx     64



//
void InitCounter433(void);
void InitRadio433(void);
void InitWMBus(void);
void Work(void);
void Install(void);
void CheckPowerProfile(void);

extern uint16_t  RadioTimer;
extern uint8_t G_LedMask;
extern uint8_t R_LedMask;

extern uint8_t wmbTxCnt,rfRxCnt,rfRSSI;

extern uint16_t RadioDog_rx, RadioDog_tx;
extern uint16_t PowerDog;

extern uint8_t rfBufRx[],wmbTx[];


extern uint16_t timeout;



enum stateWork
{
    wInit,
    wFindCS,
    wFindEndOfPacket,
    wFindGuard,
    wSendPacket,
    wWaitAck,
    wAnalize
};
extern enum stateWork WorkState;

