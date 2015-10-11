#include "stdAfx.h"

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
    
// appData init proc
void appDataInit (void)
{
    appState.errors  = 0;
    appState.flags   = 0;
    appState.pending = 0;  
}

// 
dataProcessorData appDataFull;

static void showValue (void)
{
    char str [10];
    
    if( appIsFlag( flagShowTof ) ) {
        // clean
        BoardLcdClear();
        // format a string
        if( appDataFull.tof.tmoError ) {
            BYTE errStr[] = "-----";
            BoardLcdPutStr((BYTE*)errStr);    
        }
        else {
            if( appDataFull.tofAvg >= 0.f )
                sprintf(str, " %.3f", appDataFull.tofAvg); 
            else
                sprintf(str, "%.3f", appDataFull.tofAvg); 
            BoardLcdPutStr((BYTE*)str);
        }
        // and show it
        BoardLcdUpdate();      
    }
}

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
    
    while( 1 ) {
        
       
     //   clockCorrFactor = Gp22GetClkCorrectionFactor() * 8000000UL * 1000UL;
		/*
        appDataFull = DpGetCurDataPoint();
		appData.r1 = appDataFull.tempAvg[0];
		appData.r2 = appDataFull.tempAvg[1];
		appData.tof1 = appDataFull.tof.time0;
		appData.tof1 = appDataFull.tof.time1;
        */
		showValue();
		protocolExecute();
    }
}


