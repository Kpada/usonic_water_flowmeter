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
    // reload
	TIM2->ARR = DP_GET_SPEED_EVERY_msec;
    // irq prinority & enable
	NVIC_SetPriority(TIM2_IRQn, 7); 
	NVIC_EnableIRQ(TIM2_IRQn);
}
//---------------------------------------------------------------------------

/// dp get temperature (resistance) proc
///
static void DpGetTemerature (void)
{   
    DWORD   r1, r2, r3, r4;     // raw data
    FLOAT   val1;               // first
    FLOAT   val2;               // second
    WORD    status;             // status
    
    // get
    if( !gp22GetTemp (&status, &r1, &r2, &r3, &r4) ) {
        // in case error - set error flag
        appSetError (errTdcTmoThermo);
        val1 = 0.f;
        val2 = 0.f;  
    }
    else {
        // otherwise data is correct
        DWORD r34 = ( r3 + r4 ) / 2;
        val1 = (FLOAT)r1 * 1000.f / (FLOAT)r34; 
        val2 = (FLOAT)r2 * 1000.f / (FLOAT)r34; 
    }
    
    // fill
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


/// init tof data buff
///
static void dpInitTofBuff (void)
{
    tofDataBuff.accumulator = 0.f;
    tofDataBuff.buffPtr = 0;
    tofDataBuff.valNum = 0;
}
//---------------------------------------------------------------------------

/// dp tof "dummy filter". sometimes we have a bad data, and we should make sure our data are correct
///
static BOOL dpApplyTofDummyFilter (float* newTof0, float* newTof1, float prevTof0, float prevTof1)
{
    // border conditions
/*
    const float minCtrlTofRatio = 2.f;
    const float minCtrlTofRatioInv = 0.5f;
    const float minPossibleDiff = 300.f;
    // some 
    const float prevDiff = fabs(prevTof0 - prevTof1);
    const float newDiff = fabs(*newTof0 - *newTof1);
    const float diffRatio = prevDiff / newDiff;

    // check the conditions
    if( prevTof0 != 0.f && prevTof1 != 0.f && newDiff > minPossibleDiff ) {
        if( diffRatio < minCtrlTofRatioInv || diffRatio > minCtrlTofRatio ) {
            // apply
            *newTof0 = prevTof0;
            *newTof1 = prevTof1;
            return TRUE;
        }
    }
    */
    const float prevDiff = fabs(prevTof0 - prevTof1);
    const float newDiff = fabs(*newTof0 - *newTof1);
    const float deltaDiff = fabs( newDiff - prevDiff );
    
    if( deltaDiff > 300.f ) {
        // apply
        *newTof0 = prevTof0;
        *newTof1 = prevTof1;
        return TRUE;
    }
      
    return FALSE;
}
//---------------------------------------------------------------------------

/// average tof
///
static float dpTofBuffAddValGetAveraged (float tofValNew)
{ 
    WORD i;

    // if buffer is not full 
    if( tofDataBuff.valNum < DP_TOF_BUFF_SIZE ) {
        // add
        tofDataBuff.tofQueue[tofDataBuff.buffPtr] = tofValNew;
        tofDataBuff.valNum++;
    }
    // otherwise we should pop the oldest element
    else {
        tofDataBuff.tofQueue[tofDataBuff.buffPtr] = tofValNew;
    }    
      
    // update buffPtr
    tofDataBuff.buffPtr = tofDataBuff.buffPtr == ( DP_TOF_BUFF_SIZE - 1 ) ? 0 : tofDataBuff.buffPtr + 1;
    
    // summ
    tofDataBuff.accumulator = 0.f;
    for( i = 0; i < tofDataBuff.valNum; i++ )
        tofDataBuff.accumulator += tofDataBuff.tofQueue[i];
    
    // avg
    return tofDataBuff.accumulator / tofDataBuff.valNum;
}
//---------------------------------------------------------------------------

/// dp get tof proc
///
static void DpGetTof (void)
{  
    DWORD raw0, raw1;               // raw data
    FLOAT time0, time1, time0_1;    // time 0->1, time 1->0, diff
    WORD  status0, status1;         // status 0->1, status 1->0
    BOOL  isTmoError;               // is error?
    
    // get a tof values
    Gp22Tof(&time0, &time1, &status0, &status1, &isTmoError, &raw0, &raw1);  
    
    // error? 
    if( isTmoError ) {       
        appSetError(errTdcTmoTof);
        dpData.tof [0] = 0.f;
        dpData.tof [1] = 0.f;
        time0_1 = 0.f;
    }
    // get a diff
    else {
        // fill with a filter
        dpApplyTofDummyFilter( &time0, &time1, dpData.tof[0], dpData.tof[1] );
        dpData.tof [0] = time0;
        dpData.tof [1] = time1;
        time0_1 = dpData.tof [0] - dpData.tof [1];
    }
    
    // averaged value
    dpData.tofAvg = dpTofBuffAddValGetAveraged(time0_1);
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
    
    dpData.tof[0] = 0.f;
    dpData.tof[1] = 0.f;
    
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

/// data processor TIM2 irq handler
///
void TIM2_IRQHandler (void)
{
    static WORD tempGetCounter = 0;
    const WORD getTempEvery = DP_GET_TEMPERATURE_EVERY_msec / DP_GET_SPEED_EVERY_msec;
    
	// reset interrupt flag
    TIM2->SR &= ~TIM_SR_UIF;
	
    // toggle (test)
	BoardLedToggle();  
    
    // tof
    DpGetTof();
    
    // temperature
    if( 0 == tempGetCounter )
        DpGetTemerature();
    
    tempGetCounter = tempGetCounter < getTempEvery ? tempGetCounter + 1 : 0;
}
//---------------------------------------------------------------------------

