


#define  wmbPacketLength(LField)    ((LField+3)+2*((LField+6)/16))
#define  wmbLFields(PacketLength)  (PacketLength - (3+2*((PacketLength+5)/18)) )
#define L_BLOCK1    18
#define L_BLOCK0    12
#define L_PACKET40  25
#define L_PACKET53  25
#define L_PACKET48  12


// PACKET type
#define SND_NR      0x44    // SEND/NO REPLY, meter initiative
#define SND_UD      0x53    // SEND USER DATA
#define SND_UD73    0x73    // SEND USER DATA
#define SND_IR      0x46    // Send manually initiated installation data;
#define SND_IR_ACK  0x06    // ACK for INIT;
#define SND_AD      0x48    // Accsess demand
#define SND_END     0x40    // END of LINK
#define SND_ACK     0x00    // ACK
#define REQ_UD1     0x5A    // REQUEST USER DATA1
#define REQ_UD2     0x5B    // REQUEST USER DATA2

#define RSP_UD		0x08    // Response USER DATA

// Manufacturer ID
#define MAN_ID(X,Y,Z) ((X-64)*32*32+(Y-64)*32+(Z-64))
#define BETAR_ID    MAN_ID('B','T','R')
#define KVADRAT_ID  MAN_ID('K','V','D')
#define SET_ID		MAN_ID('S','E','T')
// device unic number 8 digit BCD


#define MODEMVERSION    0x01
#define COUNTERVERSION  0x01
// Device type
#define wmbOther        0x00
#define wmbHWater       0x06
#define wmbCWater       0x07
// CI Field
#define wmbNoHdr        0x78
#define wmbShortHdr     0x7A
#define wmbLongHdr      0x72
// DIF
// 0 -no 1- 1byte 2-2bytes 4-4(binary)
// 5-4(float) 6-6bytes 7-8bytes
// 0x80 - BCD coding
// VIF
#define VIF_TIMEs       0x20
#define VIF_DATETIME    0x6D
#define VIF_10l         0x14
#define VIF_1l          0x13
#define VIF_Alarm       0x17FD
// DIF
// VIF
// DATA
//
void InitWMBus(void);
uint8_t wmbCreate(uint8_t *txBuf,uint8_t PacketTYPE);
uint8_t wmbCheckAddr(uint8_t *rxBuf);
uint8_t wmbCheck(uint8_t *rxBuf,uint8_t cnt);
uint8_t wmbParse(uint8_t *rxBuf,uint8_t *txBuf);
#define CRC_CALC    0
#define CRC_APPEND  1
#define CRC_CHECK   2
uint16_t wmbCRC(uint8_t *buf,uint8_t cnt,uint8_t mode);
//
//extern uint32_t WaterCounter;
//extern uint8_t AlarmState;
//

#pragma pack(push, 1)
typedef struct
    {
    uint8_t  DIF1;		// 0x04
    uint8_t  VIF1;		// 0x14 - 10Liter, 0x20 - time
    uint8_t  CNT1[4];	// time counter
    uint8_t  DIF2;		// 0x01 1 byte
    uint16_t VIF2;		// 0x17FD
    uint8_t  ALARM;		// alarmState
    } TSblock2;


typedef struct
    {
    uint8_t  LF;		//Length - The field specifies the number of subsequent user data bytes including the control and address bytes and excluding the CRC bytes
    uint8_t  CF;		//Control - It specifies the frame type, see pages "Mode S", "Mode T" & "Mode R2"
    uint16_t MANIDF;	//Manufacturer: Unique User/Manufacturer ID of the meter
					//	The "CEN" is coded according to EN 13757-3 as "C"=(43h-40h) = 3, "E"=5, "N"=14. Thus CEN= 32*32*3+32*5+14 = 3246d  = 0CAEh.
					//	Most significant bit is null since it is a "hard" (i.e. manufacturer unique) address.
    uint32_t ADDRF;		//Address - Manufacturer number (4 bytes - Identification-number; 1 byte - Version; 1 byte - Device type), specified in 5.4, 5.6 & 5.7 of EN 13757-3:2004
    uint8_t  VERF;		//
    uint8_t  TYPEF;		//7 - Water
    uint8_t  CRC0[2];	//
    } TSblock0;

struct wmbPacket
	{
	// Block0
	TSblock0 block0;
	// Block1
	uint8_t  CIF;		// 0x78
	TSblock2 block2[4];
	//uint8_t  CRC1[2];
	};
#pragma pack(pop)
/*
LF 9
cf 5b
MANIDF SET
ADDRF 2
VERF 1
TYPEF 31
*/
