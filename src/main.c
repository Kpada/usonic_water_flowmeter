#include "stdAfx.h"

#define ever ( ; ; )

// application state container
applicationState 	appState;
// application data container
applicationData 	appData;

// applications helpers proc - flags
void appSetFlag (WORD flag)         { appState.flags |=  flag; }
void appClearFlag (WORD flag)       { appState.flags &= ~flag; }
BOOL appIsFlag (WORD flag)          { return appState.flags & flag; }
// applications helpers proc - pending
void appSetPending (WORD pending)   { appState.pending |=  pending; }
void appClearPending (WORD pending) { appState.pending &= ~pending; }
BOOL appIsPending (WORD pending)    { return appState.pending & pending; }
// applications helpers proc - errors
void appSetError (WORD err)         { appState.errors |=  err; }
void appClearError (WORD err)       { appState.errors &= ~err; }
BOOL appIsError (WORD err)          { return appState.errors & err; }
    
/// application state init proc
///
void appDataInit (void)
{
    appState.errors  = 0;
    appState.flags   = 0;
    appState.pending = 0;  
}
//---------------------------------------------------------------------------

// 
dataProcessorData appDataFull;

/// tof formatter
///
static void formatTofData (float val, BYTE* str)
{
    //char formatStr [] = "%.xf";
   // const BYTE formatStringPos = 2;
    
    // 2 digits - like a 1.23
    if( val < 10.f ) 
        sprintf((char*)str, "%.2f", val); 
    // 1 - like a 12.3
    else if( val < 100.f )
        sprintf((char*)str, "%.1f", val); 
    // otherwise 0 - like a 123
    else 
        sprintf((char*)str, "%.0f", val); 
}
//---------------------------------------------------------------------------

/// draw 
///
static void showValue (void)
{
    BYTE str [10];
    
    // tof
    if( appIsFlag( flagShowTof ) ) {
        const float tofAvgAbs = fabs(appDataFull.tofAvg);
        // clean
        BoardLcdClear();
        // format
        formatTofData(tofAvgAbs, str);
        BoardLcdPutStr(str);
        // and show
        BoardLcdUpdate();      
    }
}
//---------------------------------------------------------------------------

/// main
///
int main (void)
{
    // clock (default)
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN;
    // delay init
    DelayInit();
    // app
    appDataInit();
    // led
    BoardLedInit();
    //lcd
    BoardLcdInit(); 
    BoardLcdPutStr("012345678");
    BoardLcdUpdate();    
    // data processor
    DpInit();
    // uart
    uartInit();
    
    appSetFlag( flagShowTof );    
    Sleep(250);  
    DpStart();  
    
    for ever {           
     //   clockCorrFactor = Gp22GetClkCorrectionFactor() * 8000000UL * 1000UL;
       
        // get data
        appDataFull = DpGetCurDataPoint();
        appData.r1 = appDataFull.resistanse[0];
		appData.r2 = appDataFull.resistanse[1];
		appData.tof1 = appDataFull.tof[0];
		appData.tof2 = appDataFull.tof[1];
        // show
		showValue();
        // protocol
		protocolExecute();
    }
}
//---------------------------------------------------------------------------


