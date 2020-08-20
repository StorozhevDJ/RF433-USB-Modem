

typedef struct
	{
	uint8_t  LF;		//L-field - Length - The field specifies the number of subsequent user data bytes including the control and address bytes and excluding the CRC bytes
	uint8_t  CF;		//C-field - Control - It specifies the frame type, see pages "Mode S", "Mode T" & "Mode R2"
	uint16_t MANIDF;	//M-field - Manufacturer: Unique User/Manufacturer ID of the meter
					//The "CEN" is coded according to EN 13757-3 as "C"=(43h-40h) = 3, "E"=5, "N"=14. Thus CEN= 32*32*3+32*5+14 = 3246d  = 0CAEh.
					//Most significant bit is null since it is a "hard" (i.e. manufacturer unique) address.
	uint32_t ADDRF;	//A-field - Address - Manufacturer number (4 bytes - Identification-number; 1 byte - Version; 1 byte - Device type), specified in 5.4, 5.6 & 5.7 of EN 13757-3:2004
	uint8_t  VERF;		//A-field - Version
	uint8_t  TYPEF;	//A-field - Device type = 0x36 (system radio converter)
	uint8_t  CRC0[2];	//
	} TSblock0;


extern bool echo;
extern bool wmb;
extern bool lqi;


bool IsCmd (char * str);
void Cmd_execute (char *cmd);
bool Cmd_compare (const char *cmd, char *buf);
