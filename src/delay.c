#include "stdAfx.h"
#include "delay.h"

#define SYS_TICK_CAPACITY           0xFFFFFF	//24 bit 

#define SYS_TICK_CTRL_ENABLE        (0x01<<0)
#define SYS_TICK_CTRL_INT           (0x01<<1)
#define SYS_TICK_CTRL_SOURCE        (0x01<<2)
#define SYS_TICK_CTRL_COUNT_FLAG    (0x01<<16)

#define SYS_TICK_ENABLE             SysTick->CTRL |= SYS_TICK_CTRL_ENABLE
#define SYS_TICK_DISABLE            SysTick->CTRL &=~SYS_TICK_CTRL_ENABLE
#define SYS_TICK_CLOCK_CPU          SysTick->CTRL |= SYS_TICK_CTRL_SOURCE
#define SYS_TICK_SET(value)         SysTick->LOAD = value
#define SYS_TICK_CLEAR              SysTick->VAL = 0
#define SYS_TICK_CURRENT_VALUE  	(SysTick->VAL&SYS_TICK_CAPACITY)				
#define SYS_TICK_READ_COUNT_FLAG	(SysTick->CTRL&SYS_TICK_CTRL_COUNT_FLAG)

static volatile QWORD delayTicks;

#define Fmcu_kHz    (Fahb/1000)

void DelayInit (void) 
{
    DWORD value;
    
    delayTicks = 0;
    
    SYS_TICK_DISABLE;
    SYS_TICK_CLEAR;
    value = Fmcu_kHz * TIMER_uSec_PER_TICK / 1000 - 1;
    SYS_TICK_SET (value);
    SysTick->CTRL = 0x00000007; // cpu clock, enable, interrupt enable   
}
//---------------------------------------------------------------------------

void SysTick_Handler (void)
{
    delayTicks++;
}
//---------------------------------------------------------------------------

QWORD GetCurrentTicks (void)
{
    return delayTicks;
}
//---------------------------------------------------------------------------

DATETIME GetTimeSince (QWORD prevTicks)
{
    return (delayTicks - prevTicks) * TIMER_uSec_PER_TICK / 1000; // 1000 is usec to msec coeff
}
//---------------------------------------------------------------------------

void DelayUSec (DWORD usec)
{	
    QWORD startTicksVal = GetCurrentTicks();
    // calc ticks
	usec /= TIMER_uSec_PER_TICK;
	if( 0 == usec ) 
		usec = 1;
    // and wait 
	while( delayTicks - startTicksVal <= usec )
		PowerCtlEnterIdle();
}
//---------------------------------------------------------------------------


void DelayMSec (DWORD msec)
{
    DelayUSec(msec * 1000);
}
//---------------------------------------------------------------------------

void DelaySec (DWORD sec)
{
    DelayMSec(sec * 1000);
}
//---------------------------------------------------------------------------

