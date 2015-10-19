#include "stdAfx.h"
#include "tdc-gp22.h"
#include "config/tdc-gp22Config.h"

/********* settings **********/

static const WORD tdcQuartzFreqKHz = TDC_QUARTZ_FREQ_KHZ;
static const WORD tdcFreqKHz = tdcQuartzFreqKHz / tdcMainDiv;

// settings
     
    /// constants defined by setting
    // reg 0
    static const DWORD r0FirePulses = ( (DWORD)( (BYTE)tdcFirePulsesNum & 0x0F ) ) << 28;
    static const DWORD r0FireDiv    = ( (tdcFirePulsesDiv - 1 ) & 0x0F) << 24;
    static const DWORD r0ClkDiv     = ( ( tdcMainDiv == 1 ) ? 0 : ( ( tdcMainDiv == 2 ) ? 1 : 2 ) ) << 20;
    static const DWORD r0ClkOn_Msr  = ( (tdcStartCLKHS & 0x03) << 18 );
    static const DWORD r0ClkCalRes  = ( (tdcAnzPerCalRes/4) < 3 ? (tdcAnzPerCalRes/4) : 3 ) << 22;
    // reg1
    static const DWORD r1HitIn1     = ( (tdcStopPulsesChnl1 & 0x07 ) << 16 );
    static const DWORD r1HitIn2     = ( (tdcStopPulsesChnl2 & 0x07 ) << 19 );
    // reg2
    static const DWORD r2DelVal1    = ( (DWORD)(  ( (FLOAT)(tdcDelayBeforeStop1Nsec)*32*(tdcFreqKHz) )/ 1000000UL ) & 0x7FFFF ) << 8;
    static const DWORD r2IrqSource  = (DWORD)( (tdcIrqTimeout ? 0x80000000 : 0) | (tdcIrqEndHits ? 0x40000000 : 0) | (tdcIrqALU ? 0x20000000 : 0) );
    // reg3
    static const DWORD r3Delrel     = ( (tdcDelrel1 & 0x3F) << 8 ) | ( (tdcDelrel2 & 0x3F) << 14 ) | ( (tdcDelrel3 & 0x3F) << 20 );
    static const DWORD r3CalcDetect = ( (tdcAutocalcMode2En ? 1 : 0) << 31) | ( (tdcEnFirstWave ? 1 : 0) << 30) | 
            ( (tdcAluErrorVal ? 1 : 0) << 29) | ( (tdcTmoPreddiv & 0x03) << 27 );
    // reg4
    static const DWORD r4Msr        = ( tdcFirstWaveEdge ? ( 0x01 << 15 ) : 0 );
    // reg 6
    static const DWORD r6FirePulses = ( tdcFirePulsesNum > 15 ) ? (tdcFirePulsesNum >> 4) << 8 : 0 << 8;
    static const DWORD r6IrqSource  = ( ((tdcIrqEndEeprom & 0x01UL) << 21) );

 
    #define GET_FIRE_FREQ   ( (TDC_QUARTZ_FREQ_KHZ) / (tdcMainDiv) / (tdcFirePulsesDiv) )
    
    
    DWORD initRegsValBuff [7] = 
    {  
        0x0003E800 | r0ClkDiv | r0FirePulses | r0FireDiv | r0ClkOn_Msr | r0ClkCalRes,
        0x21404000 | r1HitIn1 | r1HitIn2,    
        0x00000000 | r2DelVal1 | r2IrqSource,
        0x00000000 | r3Delrel | r3CalcDetect,
        0x00000000 | r4Msr,
        0x40000000,
        0xC0C06000 | r6FirePulses | r6IrqSource    
    };

  
  
static DWORD calcOffsetValMv (INT8 mv)
{
    DWORD res = 0;
    
    if( mv >= 20 ) {
        mv -= 20;
        res |= 0x01 << 14;
    }
    else if( mv <= -20 ) {
        mv += 20;
        res |= 0x01 << 13;
    }
    
    if( mv >= 0 ) {
        res |= (mv & 0x0F) << 8;
    }
    else {
        res |= ( ( 32 + mv ) & 0x1F ) << 8;
    }
    
    return res;
}

enum {
    // cmd write to register
    cmdWrReg0 = 0x80,
    cmdWrReg1 = cmdWrReg0 | 0x01,
    cmdWrReg2 = cmdWrReg0 | 0x02,
    cmdWrReg3 = cmdWrReg0 | 0x03,
    cmdWrReg4 = cmdWrReg0 | 0x04,
    cmdWrReg5 = cmdWrReg0 | 0x05,
    cmdWrReg6 = cmdWrReg0 | 0x06,
    
    // cmd read registr
    cmdRdReg0 = 0xB0,
    cmdRdReg1 = cmdRdReg0 | 0x01,
    cmdRdReg2 = cmdRdReg0 | 0x02,
    cmdRdReg3 = cmdRdReg0 | 0x03,
    cmdRdReg4 = cmdRdReg0 | 0x04,
    cmdRdReg5 = cmdRdReg0 | 0x05,
    cmdRdReg6 = cmdRdReg0 | 0x06,
    
    // additional
    cmdPowerOnReset = 0x50,
    cmdInit         = 0x70,
    cmdReadId       = 0xB7, //56 bits
    cmdStartCalRes  = 0x03,
    cmdStartCalTDC  = 0x04,
    cmdFire         = 0x05,
    cmdStartTempMsr = 0x02,
};


/******************      helpers    *********************/

__inline static void Gp22SoftReset (void)
{
    SpiPutByte(cmdPowerOnReset);
    
    DelayMSec(1);
}


__inline static QWORD Gp22GetId (void)
{
    return SpiGetQword(cmdReadId);
}


#define PUT_TO_REG( regNum, cmd )       SpiPutDword( cmdWrReg ## regNum, cmd )
#define GET_FROM_REG( regNum )          SpiGetDword( cmdRdReg ## regNum )

QWORD id;
static BOOL Gp22CheckConnection (void)
{ 
    // get id
    id = Gp22GetId();
    
    // if last byte != 0x00
    if( id & 0xFF ) {
        // clear it
        PUT_TO_REG(6, 0x00);     
    }
    
    // set the last byte = 0x77
    PUT_TO_REG(6, 0x77);  
    
    // get id againt
    id = Gp22GetId();
    
    return ( id & 0xFF ) == 0x77UL;   
}

BOOL gp22Irq = FALSE;

FLOAT tofRawToValue (DWORD raw)
{
    QWORD tmp = (QWORD)raw * (QWORD)tdcMainDiv * (QWORD)1000000;
    FLOAT tmpF = (FLOAT) tmp;
    FLOAT result = ( tmpF ) / tdcQuartzFreqKHz / 65536.f;
    return result;
}

static BOOL Gp22_WFI (void)
{
    //QWORD wait = GetCurrentTicks();
    while( GPIOB->IDR & (0x01 << 9) ) {
      //  if( GetTimeSince(wait) > 150 )
     //           return FALSE;
    } 
	

    return TRUE;
}


double Gp22GetClkCorrectionFactor (void)
{
    DWORD corrFactorDwordVal;
    volatile double mul0 = (double)30.517578125;
    volatile double mul1 = (double)tdcAnzPerCalRes;
    
    volatile double corFactorPart = mul0 * mul1;
    
    // request a data
    SpiPutByte(cmdStartCalRes);
    // wfi
    Gp22_WFI();
    
    corrFactorDwordVal = GET_FROM_REG(0);
    return corFactorPart / tofRawToValue(corrFactorDwordVal);  
}

/// init
///
BOOL Gp22Init (void)
{
    
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER &= ~( 0x03UL << (9 << 1) );    // input
    GPIOB->PUPDR |=  ( 0x01UL << (9 << 1) );    // pull-up
    
    DelayMSec(1);

    // soft reset
    Gp22SoftReset();
    // check connection
    if( !Gp22CheckConnection() )
        return FALSE;
    
    // reset again
    Gp22SoftReset();
    DelayMSec(200);
    
    // init
    initRegsValBuff [4] |= calcOffsetValMv( tdcOffsetValMv );
    
    PUT_TO_REG(0, initRegsValBuff[0] );
    PUT_TO_REG(1, initRegsValBuff[1] );
    PUT_TO_REG(2, initRegsValBuff[2] );
    PUT_TO_REG(3, initRegsValBuff[3] );
    PUT_TO_REG(4, initRegsValBuff[4] );
    PUT_TO_REG(5, initRegsValBuff[5] );
    PUT_TO_REG(6, initRegsValBuff[6] );
    
    DelayMSec(200);
    

        
    return TRUE;
}

BOOL Gp22Tof (float* time0, float* time1, WORD* stat0, WORD* stat1, BOOL* isTimeout, DWORD* raw0, DWORD* raw1)
{    
    // reset tmo flag
    *isTimeout = FALSE;   

// the first step
    // put init command
    SpiPutByte(cmdInit);
    DelayMSec(1);
    SpiPutByte(cmdFire);    
    // wfi
    Gp22_WFI();
    // get stat
    *stat0 = SpiGetWord(0xB4);
    *isTimeout |= *stat0 & 0x0600;
    // get data
    *raw0 = SpiGetDword(0xB3);
    *time0 = tofRawToValue(*raw0);
    
// the second 
    // put init command
    SpiPutByte(cmdInit);
    // wfi
    Gp22_WFI();   
    // get stat
    *stat1 = SpiGetWord(0xB4);
    *isTimeout |= *stat1 & 0x0600;
    // get data
    *raw1 = SpiGetDword(0xB3);
    *time1 = tofRawToValue(*raw1);
    
    return *isTimeout;
}

BOOL gp22GetTemp (WORD* stat, DWORD* r1, DWORD* r2, DWORD* r3, DWORD* r4)
{    
    BOOL err = FALSE;
    
    SpiPutByte(cmdInit);
    DelayMSec(1);
    SpiPutByte(cmdStartTempMsr);
    
    // wfi
    if( !Gp22_WFI() )
        return FALSE;
    
    // get status
    *stat = SpiGetWord(0xB4);
    if( *stat & 0x1E00 || err ) 
        return FALSE;
    
    // data 
    *r1 = SpiGetDword(0xB0);
    *r2 = SpiGetDword(0xB1);
    *r3 = SpiGetDword(0xB2);
    *r4 = SpiGetDword(0xB3);
    return TRUE;
}

gp22Status Gp22ParseStatus (WORD stat)
{
    gp22Status result;
    
    result.aluPtr = stat & 0x07;
    result.hitsChnl1 = (stat >> 3) & 0x07;
    result.hitsChnl2 = (stat >> 6) & 0x07;
    result.timeoutTdc = stat & (0x01 << 9 );
    result.timeoutPrecounter = stat & (0x01 << 10 );
    result.errorOpen = stat & (0x01 << 11 );
    result.errorShort = stat & (0x01 << 12 );
    result.errorEeprom = stat & (0x01 << 13 );
    result.errorDed = stat & (0x01 << 14 );
    result.eepromEqCreg = stat & (0x01 << 15 );
    
    return result; 
}
