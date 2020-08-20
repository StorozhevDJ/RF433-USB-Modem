

#define sbit(byte,bit)     (byte|= (1<<bit))
#define cbit(byte,bit)     (byte&=~(1<<bit))
#define tbit(byte,bit)     (byte&  (1<<bit))
#define fbit(byte,bit)     (byte^= (1<<bit))
//
#define FromBCD(s)          ((s&0x0F)+10*(s>>4))
#define ToBCD(s)            ((s%10)|((s/10)<<4))
//
#define MIN(A,B) if(A<B)?A:B
#define MAX(A,B) if(A>B)?A:B

uint8_t ToUpper(uint8_t s);
uint8_t AsciiToDigit(uint8_t s);
uint8_t AsciiToHex(uint8_t s);
void HexToAscii(uint8_t hex, uint8_t * str);
void BCDprint(uint8_t *buf, uint32_t W, uint8_t digit);
void Binprint(uint8_t *buf, uint32_t W, uint8_t digit);
uint8_t ConvertUart(uint8_t s);
//


