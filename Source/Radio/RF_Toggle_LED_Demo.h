//#include "cc430x513x.h"
//#include "Radio\RF1A.h"
#include "Radio\hal_pmm.h"

/*******************
 * Function Definition
 */
void Transmit(unsigned char *buffer, unsigned char length);
void ReceiveOn(void);
void ReceiveOff(void);

void DebugRadio_main(void);
