#ifndef __DELAY_H__
#define __DELAY_H__

#include "typedef.h"

// For STM32 make sure HSE_VALUE was defined ( target options -> c/c++ )

void DelayInit (void) ;
void DelayUSec (DWORD usec);
void DelayMSec (DWORD msec);
void DelaySec (DWORD sec);
QWORD GetCurrentTicks (void);
DATETIME GetTimeSince (QWORD prevTicks);
#endif

