
#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

//#define DEBUG_ON	0

#if DEBUG_ON
	#define Debug	printf
#else
	#define Debug(...)	
#endif


void Init_Debug_UART(void);

#endif //DEBUG_H