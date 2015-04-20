#ifndef __STD_AFX_H__
#define __STD_AFX_H__

#define Fmcu                    32000000UL
#define TIMER_uSec_PER_TICK     50UL
#define PowerCtlEnterIdle()     
// target
#include "stm32l1xx.h"
// drivers
#include "typedef.h"
//
#include "stm32l1xx_conf.h"

#include "delay.h"
#include "spi.h"
#include "uart.h"

#include "ringbuff.h"
#include "protocol.h"


// board
#include "demoBoard/boardLeds.h"
#include "demoBoard/boardButtons.h"
#include "demoBoard/boardLcd.h"
#include "tdc-gp22.h" 

#include "main.h"

// default
#include <string.h>
#include <stdlib.h>    


#endif // __STD_AFX_H__
