
#ifndef MAIN_H
#define MAIN_H









#define  PATABLE_VAL        (0x51)          // 0 dBm output 




typedef struct
	{
	unsigned long counter;
	unsigned short timeout_tick;
	unsigned long  timeout_s;
	char protect:1;
	} TScounter;

typedef enum 
	{
	cmd,
	transparent
	} Emodem_mode;
extern Emodem_mode modem_mode;

#endif //MAIN_H