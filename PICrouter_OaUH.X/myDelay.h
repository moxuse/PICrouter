#ifndef _MY_DELAY_HEADER_FILE
#define _MY_DELAY_HEADER_FILE

#ifndef __GENERIC_TYPE_DEFS_H_
#include "GenericTypeDefs.h"
#endif

#define Fosc 80

void delayUs(unsigned int usec);
void delayMs(unsigned int msec);

#endif