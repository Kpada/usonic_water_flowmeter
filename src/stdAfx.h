#ifndef __STD_AFX_H__
#define __STD_AFX_H__

#define Fmcu                    16000000UL
#define Fahb                    (Fmcu)
#define Fapb1                   (Fmcu)
#define Sleep( msec )           DelayMSec(msec)

#define TIMER_uSec_PER_TICK     500UL
#define PowerCtlEnterIdle()    


// target
#include "stm32l1xx.h"
// drivers
#include "typedef.h"
//
#include "stm32l1xx_conf.h"


#include "spi.h"
#include "uart.h"

#include "ringbuff.h"
#include "delay.h"
#include "protocol.h"
#include "dataProcessor.h"
#include "crc.h"


#include "data.h"

// board
#include "demoBoard/boardLeds.h"
#include "demoBoard/boardLcd.h"

// default
#include <stdio.h>
#include <string.h>
#include <stdlib.h>    

#include "main.h"

#endif // __STD_AFX_H__
