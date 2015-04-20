#include "stdAfx.h"
#include "tdc-gp22.h"

/********* settings **********/

static const WORD tdcQuartzFreqKHz = 24000;


// settings
enum {
    // clock
    tdcMainDiv          = 4,    // 1, 2, 4
    
    // fire
    tdcFirePulsesNum    = 10,   // 0 .. 127
    tdcFirePulsesDiv    = 6 ,   // 2 .. 16

    
    // stop
    tdcStopPulsesChnl1  = 3 ,   // 0 .. 4
    tdcStopPulsesChnl2  = 0,    // 0 .. 4
    
    // irq

      
    /// constants defined by setting
    // reg 0
    r0FirePulses = ( tdcFirePulsesNum & 0x0F ) << 28, 
    r0FireDiv    = ( (tdcFirePulsesDiv - 1 ) & 0x0F) << 24,
    r0ClkDiv     = ( ( tdcMainDiv == 1 ) ? 0 : ( ( tdcMainDiv == 2 ) ? 1 : 2 ) ) << 20,
    // reg1
    r1HitIn1     = ( (tdcStopPulsesChnl1 & 0x07 ) << 16 ),
    r1HitIn2     = ( (tdcStopPulsesChnl2 & 0x07 ) << 19 ),
    
    // reg 6
    r6FirePulses = ( tdcFirePulsesNum > 15 ) ? (tdcFirePulsesNum >> 4) << 8 : 0 << 8,
};



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
    
    // additionam
    cmdPowerOnReset = 0x50,
    cmdInit         = 0x70,
    cmdReadId       = 0xB7, //56 bits
    cmdStartCalRes  = 0x03,
    cmdStartCalTDC  = 0x04,
    cmdFire         = 0x05,
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


static BOOL Gp22CheckConnection (void)
{
    QWORD id;
    
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

/// gp22 irq handler
///
void EXTI9_5_IRQHandler (void)
{
    if( EXTI->PR & ( 0x01UL << 9 ) ) {
        BoardLedsTurnLedOn(ledFive);
        gp22Irq = TRUE;
        EXTI->PR |= (0x01UL << 9 );
    }
}



DWORD readVal;
float correctionFactor = 0.f;
#include "math.h"

DWORD rawData [24];
WORD dataCnt = 0;
WORD statRegVal = 0;
//BOOL isTimeout = FALSE;



DWORD reg1Val = 0x31404000 | r1HitIn1 | r1HitIn2;
BYTE  reg1HighByte = 0x31;

BOOL Gp22Tof (float* time0, float* time1, WORD* stat0, WORD* stat1, BOOL* isTimeout, DWORD* raw0, DWORD* raw1)
{
    *isTimeout = FALSE;
    
    PUT_TO_REG(1, reg1Val);
    DelayMSec(1);
    
    // put init command
    SpiPutByte(cmdInit);
    DelayMSec(1);
    // fire
    SpiPutByte(0x05);
    // wfi
    while( GPIOB->IDR & (0x01 << 9) );
    // get raw val
    *raw0 = GET_FROM_REG(3);
    DelayMSec(1);
    SpiPutByte(0xB3);
    SpiPutByte(reg1HighByte);
    // wfi
   // while( GPIOB->IDR & (0x01 << 9) );
    // get stat
    *stat0 = SpiGetWord(0xB4);
    *isTimeout |= *stat0 & 0x0600;

    
    // put init command
    SpiPutByte(cmdInit);
    // wfi
    while( GPIOB->IDR & (0x01 << 9) );
    // get raw val
    *raw1 = GET_FROM_REG(3);
    DelayMSec(1);
    SpiPutByte(0xB3);
    SpiPutByte(reg1HighByte);
    // wfi
   // while( GPIOB->IDR & (0x01 << 9) );
    // get stat
    *stat1 = SpiGetWord(0xB4);
    *isTimeout |= *stat1 & 0x0600;
   
  
    return *isTimeout;
}



static const FLOAT tRef = 1.f / (FLOAT)tdcQuartzFreqKHz;

FLOAT Gp22RawValueToTimeConvert (DWORD val)
{
    FLOAT fltVal = val / powf(2.f, 16.f);
    FLOAT result;
    
    result = fltVal * tRef * tdcMainDiv * 1000000.f;
    
    return result;
}

/// init
///
BOOL Gp22Init (void)
{
    // init interrupt (PB9) (irq - low)
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER &= ~( 0x03UL << (9 << 1) );    // input
    GPIOB->PUPDR |=  ( 0x01UL << (9 << 1) );    // pull-up
    EXTI->IMR |= 0x01UL << 9;                   // unmask
    EXTI->FTSR |= 0x01UL << 9;                  // falling edge
    SYSCFG->EXTICR[2] |= 0x01UL << 4 ;          // pb9
    //NVIC_EnableIRQ(EXTI9_5_IRQn);               // irq enable
    
    //startPulseInit();
    
   // TimerPwmInit();
    DelayMSec(200);
    // soft reset
    Gp22SoftReset();
  // DelayMSec(300);
    // check connection
    if( !Gp22CheckConnection() )
        return FALSE;
    
     Gp22SoftReset();
    DelayMSec(200);
    
 // this code works  
    PUT_TO_REG(0, 0x00076800 | r0ClkDiv | r0FirePulses | r0FireDiv);
    PUT_TO_REG(1, 0x21407F00 | r1HitIn1 | r1HitIn2 );
    PUT_TO_REG(2, 0xA0000000);
    PUT_TO_REG(3, 0xB8000000);
    PUT_TO_REG(4, 0x00000000);
    PUT_TO_REG(5, 0x40000000);
    PUT_TO_REG(6, 0x00000000 | r6FirePulses);

/*
    PUT_TO_REG(0, 0x000B6800 | r0ClkDiv | r0FirePulses | r0FireDiv);
    PUT_TO_REG(1, 0x21444000 );
    PUT_TO_REG(2, 0xA0000000);
    PUT_TO_REG(3, 0xD0A24800);
    PUT_TO_REG(4, 0x20004A00);
    PUT_TO_REG(5, 0x40000000);
    PUT_TO_REG(6, 0xC0C06000);
   */
    /*
    SpiPutByte(cmdStartCalRes);
    while( GPIOB->IDR & (0x01 << 9) );
    //gp22Irq = FALSE;
    rawData[0] = GET_FROM_REG(0); 
    correctionFactor = 61.035f / (rawData[0] / powf(2., 16.) );
    */
    PUT_TO_REG(0, 0x0007E800 | r0ClkDiv | r0FirePulses | r0FireDiv);
  // DelayMSec(300);
    PUT_TO_REG(1, reg1Val );
  //  DelayMSec(300);
    PUT_TO_REG(2, 0xA0000000);
   // DelayMSec(300);
    PUT_TO_REG(3, 0xBC000000);
  //  DelayMSec(300);
    PUT_TO_REG(4, 0x10000000);
  //  DelayMSec(300);
    PUT_TO_REG(5, 0x40000000);
  //  DelayMSec(300);
    PUT_TO_REG(6, 0xC0C06000 | r6FirePulses);   
   
    
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


/*
static void TimerPwmInit (void)
{
    GPIO_InitTypeDef PORT;
    
    RCC->APB2ENR |= RCC_AHBENR_GPIOBEN; // pb6, pb7
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    
    PORT.GPIO_Pin = (GPIO_Pin_6 | GPIO_Pin_7);
    PORT.GPIO_Mode = GPIO_Mode_AF;
    PORT.GPIO_Speed = GPIO_Speed_40MHz;
    GPIO_Init(GPIOB, &PORT);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_TIM4);
    
    TIM4->CCER  |=  (TIM_CCER_CC2E | TIM_CCER_CC1E);
    TIM4->CCMR1 |=  (TIM_CCMR1_OC2M_0 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2) | (TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2);

    TIM4->ARR = 26;
    TIM4->CCR2 = 0;
    TIM4->CCR1 = 0;
    TIM4->CNT = 0;    
    TIM4->DIER |= TIM_DIER_CC1IE | TIM_DIER_CC2IE;  
    NVIC_SetPriority(TIM4_IRQn, 0);
    NVIC_EnableIRQ(TIM4_IRQn);
    
    
    //TIM4->CR1 |= TIM_CR1_CEN;
    TIM4->SR &= ~ ( TIM_SR_UIF | TIM_SR_CC1IF | TIM_SR_CC2IF | TIM_SR_CC3IF | TIM_SR_CC4IF);  
}


void TIM4_IRQHandler (void)
{
    static WORD counter = 0;
    
    WORD source = TIM4->SR;
    
    TIM4->SR &= ~ ( TIM_SR_UIF | TIM_SR_CC1IF | TIM_SR_CC2IF | TIM_SR_CC3IF | TIM_SR_CC4IF); 
        
    if( source & (TIM_SR_CC1IF | TIM_SR_CC2IF ) ) {
        counter++;
        if( counter >= 6 ) {
            TIM4->CR1 &= ~TIM_CR1_CEN;
            counter = 0;
        }
    }
    
   
}

void PutPulses0 (void)
{
    TIM4->CNT = 0;
    TIM4->CCR1 = 13;
    TIM4->CCR2 = 0;
    TIM4->CR1 |= TIM_CR1_CEN;
    
    while( ! (TIM4->CR1 & TIM_CR1_CEN ) );
    
}

void PutPulses1 (void)
{
    TIM4->CNT = 0;
    TIM4->CCR1 = 0;
    TIM4->CCR2 = 13;
    TIM4->CR1 |= TIM_CR1_CEN;
    
    while( ! (TIM4->CR1 & TIM_CR1_CEN ) );
    
}

static void startPulseInit (void)
{
    // PA4
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;    
    
    GPIOA->MODER |= 0x01 << (4 << 1);
    GPIOA->ODR   &= ~( 0x01 << 4);
    
}

static void StartPulse (void)
{
   //BYTE i = 5;
    GPIOA->ODR |= (0x01 << 4);
   // while( --i);
    GPIOA->ODR &= ~( 0x01 << 4);
}

#define START_PULSE     StartPulse();
  */    


