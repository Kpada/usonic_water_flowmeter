#include "stdAfx.h"


float time0, time1;
WORD stat0, stat1;
BOOL isTimeout;
DWORD raw0, raw1;
BYTE ch1Hits_0 = 0;
BYTE ch2Hits_0 = 0;
BYTE ch1Hits_1 = 0;
BYTE ch2Hits_1 = 0;

gp22Status status0, status1;

FLOAT time0, time1;

const WORD buffSize = 20;

FLOAT   time0Buff [buffSize];
FLOAT   time1Buff [buffSize];
WORD    timeBuffPtr = 0;

BOOL remaped = FALSE;

FLOAT av1 = 0.f, av2 = 0.f;
WORD ii = 0;
WORD iiMAx = 0;
static void FireAndParsePulseResults (void)
{
    isTimeout = FALSE;
    
     
    Gp22Tof (&time0, &time1, &stat0, &stat1, &isTimeout, &raw0, &raw1);
    
    //status0 = Gp22ParseStatus(stat0);
   // status1 = Gp22ParseStatus(stat1);
    
    BoardLcdClear();
    
    BoardLcdPutChar( status0.hitsChnl1 + '0', 1);

    BoardLcdPutChar( status1.hitsChnl1 + '0', 3);

   // time0 = Gp22RawValueToTimeConvert(raw0);
   // time1 = Gp22RawValueToTimeConvert(raw1);
    
    time0Buff [timeBuffPtr] = Gp22RawValueToTimeConvert(raw0);
    time1Buff [timeBuffPtr] = Gp22RawValueToTimeConvert(raw1);
    
    time0 =  time0Buff [timeBuffPtr];
    time1 =  time1Buff [timeBuffPtr];
    
    if( time0Buff [timeBuffPtr] > 100000.f && time1Buff[timeBuffPtr] > 100000.f ) {

        timeBuffPtr++;
        if( timeBuffPtr >= buffSize ) {
            timeBuffPtr = 0;
            remaped = TRUE;
        }
    }        
    
    av1 = 0;
    av2 = 0;
    
    if( remaped )
        iiMAx = buffSize;
    else
        iiMAx = timeBuffPtr;
    
        for( ii = 0; ii < iiMAx; ii++ ) {
            av1 += time0Buff[ii];
            av2 += time1Buff[ii];
        }
    
    av1 /= iiMAx;
    av2 /= iiMAx;
        
        
    
    BoardLcdUpdate();    

    if( isTimeout )
        BoardLedsTurnLedOn(ledOne);
    else
        BoardLedsTurnLedOff(ledOne);
}

const BYTE pony [] = "my little pony\r\n";

int main (void)
{

    // clock
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN;
    // delay init
    DelayInit();
    // spi init
    SpiInit();
    // board init
    BoardLedsInit();
    BoardButtonsInit();
    BoardLcdInit();
    uartInit();
    // gp22 init
    if( !Gp22Init() ) {
             BoardLedsTurnAllOn();
       // while(1);
    }
    
    //FireAndParsePulseResults();
    
    while(1) { /*
        WORD btn1;
        WORD btn2;
        btn1 = BoardButtonGet();
        DelayMSec(200);
        btn2 = BoardButtonGet();
        if( btn1 == btn2 )
            BoardLedsToggle(btn2);
        DelayMSec(200);*/
       /* 
       if( btn1 == btn2 && btn1 == btn3 ) {
           FireAndParsePulseResults();
        }*/

      //  uartPutBytes(pony, sizeof(pony) );
     //   protocolExecute();
        //DelayMSec(200);
        FireAndParsePulseResults();
        DelayMSec(500);
    }
    
}


