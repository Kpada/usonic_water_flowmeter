#include "stdAfx.h"
#include "tdc-gp22.h"


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
    
    if( appIsFlag( flagShowTemp ) ) {
        sprintf(str, "%.0f %.0f", appDataFull.tempAvg[0], appDataFull.tempAvg[1]);
        BoardLcdClear();
        BoardLcdPutStr((BYTE*)str);
        BoardLcdUpdate();       
    }
    
    else if( appIsFlag( flagShowTof ) ) {
        
        BoardLcdClear();
        
        if( appDataFull.tof.tmoError ) {
            BYTE errStr[] = "-----";
            BoardLcdPutStr((BYTE*)errStr);    
        }
        else {
            sprintf(str, "%.3f", appDataFull.tofAvg);       
            BoardLcdPutStr((BYTE*)str);
        }
        
        BoardLcdUpdate();      
    }
}
/*
static void handleButton (void)
{
        // get button
        boardBtn btnPrsd = BoardButtonGet();
        if( btnPrsd == btn1 ) {
            appSetFlag( flagShowTemp );
            appClearFlag( flagShowTof ); 
        }
        else if( btnPrsd == btn3 ) {
            appSetFlag( flagShowTof );
            appClearFlag( flagShowTemp ); 
        }
            
    
}

//#define PUT_DATA_TO_CHNL

struct {
    union {
        BYTE buff[25];
        struct {
            BYTE  startByte;
            FLOAT tof0, tof1, tofDiff;
            FLOAT res0, res1;
            BOOL  tofTmo;
        } data;       
    } chnl;
} chnlData;


static void putDataPointToChnl (void)
{
#ifdef  PUT_DATA_TO_CHNL
    chnlData.chnl.data.startByte = 0x77;
    chnlData.chnl.data.res0 = appDataFull.temp.val1;
    chnlData.chnl.data.res1 = appDataFull.temp.val2;
    chnlData.chnl.data.tof0 = appDataFull.tof.time0;
    chnlData.chnl.data.tof1 = appDataFull.tof.time1;
    chnlData.chnl.data.tofDiff = appDataFull.tof.timeDiff;
    chnlData.chnl.data.tofTmo = appDataFull.tof.tmoError ? TRUE : FALSE;
    uartPutBytes( (BYTE*)&chnlData.chnl.buff[0], sizeof( chnlData ) );
#endif
}
*/


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
    // btns
    BoardButtonsInit();  
    //lcd
    BoardLcdInit();  
    // data processor
    DpInit();
    // uart
    uartInit();
    
    appSetFlag( flagShowTof );
    
	
    while( 1 ) {
     //   clockCorrFactor = Gp22GetClkCorrectionFactor() * 8000000UL * 1000UL;
		
        // get data
        DpProcess();
        appDataFull = DpGetCurDataPoint();
		appData.r1 = appDataFull.tempAvg[0];
		appData.r2 = appDataFull.tempAvg[1];
		appData.tof1 = appDataFull.tof.time0;
		appData.tof1 = appDataFull.tof.time1;
		
		protocolExecute();
    }
}


