#include "stdAfx.h"
#include "dataProcessor.h"
#include "tdc-gp22.h"

// config
#include "config/dataProcessorConfig.h"

volatile static dataProcessorData dpData;


/****** Data timer ******/

// timer macro
#define TIMER_ENABLE					TIM2->CR1 |=  0x00000001
#define TIMER_DISABLE					TIM2->CR1 &= ~0x00000001

/// dp timer initiazitation proc
///
static void DpDataTimeInit (void)
{
	// clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	// stop
	TIMER_DISABLE;
    // interrupt source  
    TIM2->DIER |= TIM_DIER_UIE;
	// prescaller
	TIM2->PSC = Fmcu / 1000UL - 1; 
    
	TIM2->ARR = DP_GET_SPEED_EVERY_msec;
    
	NVIC_SetPriority(TIM2_IRQn, 7); 
	NVIC_EnableIRQ(TIM2_IRQn);
}
//---------------------------------------------------------------------------

/// dp get temperature (resistance) proc
///
static void DpGetTemerature (void)
{   
    DWORD   r1, r2, r3, r4;   // raw data
    FLOAT   val1;   
    FLOAT   val2;
    WORD    status;
    
    if( !gp22GetTemp (&status, &r1, &r2, &r3, &r4) ) {
        // in case error - set error flag
        appSetError (errTdcTmoThermo);
        val1 = 0.f;
        val2 = 0.f;  
    }
    else {
        DWORD r34 = ( r3 + r4 ) / 2;
        val1 = (FLOAT)r1 * 1000.f / (FLOAT)r34; 
        val2 = (FLOAT)r2 * 1000.f / (FLOAT)r34; 
    }
    
    dpData.resistanse[0] = val1;
    dpData.resistanse[1] = val2;
}
//---------------------------------------------------------------------------

struct {
    // rb buff
    FLOAT	    tofQueue [DP_TOF_BUFF_SIZE]; 
    // accumulator - sum of a elements inside the rb
    FLOAT       accumulator;
    // num of the elements inside accumulator
    WORD        valNum;
    // buff pointer
    WORD        buffPtr;
} tofDataBuff;


static void dpInitTofBuff (void)
{
    tofDataBuff.accumulator = 0.f;
    tofDataBuff.buffPtr = 0;
    tofDataBuff.valNum = 0;
}

static FLOAT dpTofBuffAddValGetAveraged (FLOAT tofValNew)
{ 
    // if buffer is not full 
    if( tofDataBuff.valNum < DP_TOF_BUFF_SIZE ) {
        // add
        tofDataBuff.tofQueue[tofDataBuff.buffPtr] = tofValNew;
        //
        tofDataBuff.valNum++;
        tofDataBuff.accumulator += tofValNew;
    }
    // otherwise we should pop the oldest element
    else {
        tofDataBuff.accumulator -= tofDataBuff.tofQueue[tofDataBuff.buffPtr];
        tofDataBuff.tofQueue[tofDataBuff.buffPtr] = tofValNew;
        tofDataBuff.accumulator += tofValNew;
    }    
    
    // update pointer value
    tofDataBuff.buffPtr = tofDataBuff.buffPtr == DP_TOF_BUFF_SIZE - 1 ? 0 : tofDataBuff.buffPtr + 1;
    
    return tofDataBuff.accumulator / tofDataBuff.valNum;
}
/*
typedef struct {
    DWORD           raw0, raw1;
    FLOAT           time0, time1; 
    FLOAT           timeDiff;
    WORD            status0, status1;
    gp22Status      status_parsed0, status_parsed1;
    BOOL            tmoError;
} tofPoint;
*/
/// dp get tof proc
///
static void DpGetTof (void)
{  
    DWORD raw0, raw1;
    FLOAT time0, time1, time0_1;
    WORD  status0, status1;
    BOOL  isTmoError;
    
// get a tof value
    Gp22Tof(&time0, &time1, &status0, &status1, &isTmoError, &raw0, &raw1);  
    if( isTmoError ) {
        appSetError(errTdcTmoTof);
        time0 = 0.f;
        time1 = 0.f;
        time0_1 = 0.f;
    }
    else {
        time0_1 = time0 - time1;
    }
   
    dpData.tof [0] = time0;
    dpData.tof [1] = time1;
    dpData.tofAvg = dpTofBuffAddValGetAveraged (time0_1);
}
//---------------------------------------------------------------------------

/// data processor initialzation proc
///
BOOL DpInit (void)
{
    // spi
    SpiInit();
    
    // tdc
    if( !Gp22Init() ) {
        appSetError( errTdcCommunication );
        return FALSE;
    }
    
    // data timer
    DpDataTimeInit();
    
    // ring buffer
    dpInitTofBuff();

    return TRUE;
}
//---------------------------------------------------------------------------

/// data processor start 
///
void DpStart (void)
{
    TIMER_ENABLE;
}
//---------------------------------------------------------------------------

/// data processor stop
///
void DpStop (void)
{
    TIMER_DISABLE;
}
//---------------------------------------------------------------------------


/// dp get a copy of data struct
///
dataProcessorData DpGetCurDataPoint (void)
{
    return dpData;
}
//---------------------------------------------------------------------------

static const WORD getTempEvery = DP_GET_TEMPERATURE_EVERY_msec / DP_GET_SPEED_EVERY_msec;

void TIM2_IRQHandler (void)
{
    static WORD cnt = 0;
	// reset interrupt flag
    TIM2->SR &= ~TIM_SR_UIF;
	
	BoardLedToggle();  
    // tof
    DpGetTof();
    // temperature
    if( 0 == cnt )
        DpGetTemerature();
    
    cnt = cnt < getTempEvery ? cnt + 1 : 0;
}
//---------------------------------------------------------------------------

