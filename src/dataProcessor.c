#include "stdAfx.h"
#include "dataProcessor.h"

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
    
    TIMER_ENABLE;
}
//---------------------------------------------------------------------------

/// dp get temperature (resistance) proc
///
static void DpGetTemerature (void)
{   
    thermoPoint point;
    
    if( !gp22GetTemp (&point.status, &point.r1, &point.r2, &point.r3, &point.r4) ) {
        appSetError(errTdcTmoThermo);
        point.val1 = 0.f;
        point.val2 = 0.f;  
    }
    else {
        DWORD r34 = ( point.r3 + point.r4 ) / 2;
        point.val1 = (FLOAT)point.r1 * 1000.f / (FLOAT)r34; 
        point.val2 = (FLOAT)point.r2 * 1000.f / (FLOAT)r34; 
    }

    point.status_parsed =  Gp22ParseStatus (point.status);   

    dpData.temp = point;
    
    dpData.tempAvg[0] = point.val1;
    dpData.tempAvg[1] = point.val2;
}
//---------------------------------------------------------------------------

struct {
    // rb buff
    FLOAT	    tofQueue [DP_TOF_BUFF_SIZE]; 
    RingBuff	tofBuff;
    // accumulator - sum of a elements inside the rb
    FLOAT       accumulator;
    // num of the elements inside accumulator
    WORD        valNum;

} tofDataBuff;


/// dp get tof proc
///
static void DpGetTof (void)
{  
    FLOAT curValue;
    BOOL timeout = FALSE;
    tofPoint point;
   
// get a tof value
    Gp22Tof(&point.time0, &point.time1, &point.status0, &point.status1, &timeout, &point.raw0, &point.raw1);  
    // parse a status
    point.status_parsed0 =  Gp22ParseStatus (point.status0);
    point.status_parsed1 =  Gp22ParseStatus (point.status1);    
    point.timeDiff = point.time0 - point.time1;
    point.tmoError = timeout;   
    dpData.tof = point;
   
    curValue = point.timeDiff;
    
// average
    // if buffer is not full 
    if( !rbIsFullFromIsr(&tofDataBuff.tofBuff) ) {
        rbPushFromIsrF(&tofDataBuff.tofBuff, curValue);
        tofDataBuff.valNum++;
        tofDataBuff.accumulator += curValue;
    }
    // otherwise we should pop the oldest element
    else {
        FLOAT oldestBVal;
        rbPopFromIsrF(&tofDataBuff.tofBuff, &oldestBVal);
        tofDataBuff.accumulator -= oldestBVal;
        rbPushFromIsrF(&tofDataBuff.tofBuff, curValue);
        tofDataBuff.accumulator += curValue;
    }
    
    // average and return
    dpData.tofAvg = tofDataBuff.accumulator / tofDataBuff.valNum;
}
//---------------------------------------------------------------------------

/// dp initialzation proc
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
    rbInit(&tofDataBuff.tofBuff, (void*)tofDataBuff.tofQueue, DP_TOF_BUFF_SIZE);
    tofDataBuff.accumulator = 0;
    tofDataBuff.valNum = 0;

    return TRUE;
}
//---------------------------------------------------------------------------

/// process
///
void DpProcess (void)
{
    static WORD getTempCnt = 0;
    DpGetTof();
    if( ++getTempCnt >= 5 ) {
        DpGetTemerature();  
        getTempCnt = 0;
    }        
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

