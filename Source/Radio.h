

#define rfPOWER10   0xC0        // Output power: +10 dBm
#define rfPOWER5    0x84         // Output power: +5 dBm
#define rfPOWER0    0x51         // Output power: +0 dBm


#define halRfWriteReg	WriteSingleReg


extern unsigned char wor_rx;
extern unsigned char IsCarrierSense;
extern unsigned char RF_transmitting;
extern unsigned char RFRxBufferLength;
extern unsigned char RFRxBuffer[];


// Radio Functions
void RadioInit( void );
void InitRadio433(void);
void ResetRadioCore (void);
void WriteRfSettings(void);
uint8_t Strobe(uint8_t strobe);
void WriteSingleReg(uint8_t addr, uint8_t value);
void WriteBurstReg(uint8_t addr, uint8_t *buf, uint8_t cnt);
uint8_t ReadSingleReg(uint8_t addr);
void ReadBurstReg(uint8_t addr, uint8_t *buf, uint8_t cnt);
void WriteSinglePATable(uint8_t Power);
void WriteBurstPATable(uint8_t *buf, uint8_t cnt);
void halRfWriteReg(uint8_t rfReg,uint8_t data);
uint8_t rfSend(uint8_t *buf,uint8_t cnt);
unsigned char TransmitCCA(unsigned char *buffer, unsigned char length);
uint8_t rfReceive(uint8_t *buf,uint8_t cnt,uint16_t ticks);
bool CheckCarrierSense(void);
void ReceiveOff(void);
void ResetRadio(void);
void Radio_PowerDown(void);
void Radio_SxOff(void);
void ReceiveOn(void);
void ReceiveOff(void);
uint8_t rfCorrectGain(uint8_t correct);
int8_t rfCorrectFreq(uint8_t correct);
//uint16_t  wmbCRC(uint8_t *buf,uint8_t cnt);
void rf_receive();
void rf_transmit();
void WriteRfbdr(short bdr);

void DebugRadio_main(void);
