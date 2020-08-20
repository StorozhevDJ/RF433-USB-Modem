


//
uint8_t AsciiToDigit(uint8_t s)
{
if((s-'0') < 10) return (s-'0');
else return -1;
}



uint8_t ToUpper(uint8_t s)
{
if('a'<=s && s<='z') s += ('A'-'a');
return s;
}



uint8_t AsciiToHex(uint8_t s)
{
if((s-'0') < 10) return (s-'0');
else if((s-'A') <  6) return (s-('A'-10));
else if((s-'a') <  6) return (s-('a'-10));
else return -1;
}



void HexToAscii(uint8_t hex, uint8_t * str)
{
*str  =(hex>>4)  +(((hex>>4  )<0x0a)?'0':('A'-0x0A));
*++str=(hex&0x0F)+(((hex&0x0F)<0x0a)?'0':('A'-0x0A));
}


//
uint8_t ConvertUart(uint8_t s)
{
// to upper
//    if((s == 0x0A) || (s == 0x0D)) return 0x0A;
if((s == 0x09) || (s == ',') || (s == ';')) return ' ';
if((s >= 'a') && (s <= 'z')) return s-('a'-'A');
//    if((s >= 'A') && (s <= 'Z')) return s;
//    if((s >= '0') && (s <= '9')) return s;
return s;
}
