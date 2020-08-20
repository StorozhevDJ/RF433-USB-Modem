

//#include "Print_scan/Print.h"


int Parser_FindPos (const char *str, char ch, char num);
int Parser_GetParamChar(const char *str, char ch, char num, char *val);
int Parser_GetParamInt(const char *str, char ch, char num, int  *val);
int Parser_GetParamLong(const char *str, char ch, char num, long *val);
int Parser_GetParamShort(const char *str, char ch, char num, short *val);
int Parser_GetParamHex(const char *str, char ch, char num, short *val);
int Parser_GetParam(const char *str, char ch, char num, char *val);


void Parser_printNumber(unsigned long n, uint8_t base);
void Parser_printFloat(double number, uint8_t digits) ;
void Parser_BufPrintf(char *str, const char *format, ...);


void write(uint8_t b);
//size_t write(uint8_t b);

void itoa(int n, char s[]);
void itoa_dot(int n, char s[], int l);
void ltoa(long int n,char *str);
void ltoa_null(long n, char s[], int l);