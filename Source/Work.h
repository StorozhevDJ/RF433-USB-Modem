

#define CSTIMER 4000


typedef struct
	{
	unsigned long time;
	unsigned long counter;
	unsigned char alarm:1;
	}TSdata_arhive;



void Work(void);