#include "stdAfx.h"
#include "boardButtons.h"

#define ADC_CHNL_NUM   0

void BoardButtonsInit (void)
{
    
    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR&RCC_CR_HSIRDY));
   // ADC clock enable
   RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
   // port A clock enable
   RCC->APB2ENR |= RCC_AHBENR_GPIOAEN;
   // cofig PA0 as ADC input
   GPIOA->MODER |= 0x03UL << 0; // analog mode
   
   ADC1->CR2 |= ADC_CR2_ADON;  
    
   ADC1->SQR5 |= ( ADC_CHNL_NUM & 0x0F );
   ADC1->SMPR3 |= ( 0x07UL << (ADC_CHNL_NUM*3) ); // 384 ticks
   ADC1->CR1 &= ~ADC_CR1_RES; // 12 bit
                
      
}

#define DECL_CNT_MAX_AND_MIN( btnNum ) \
        cntMaxBtn##btnNum = cntBtn##btnNum * (100 + cntDeviationPercent) / 100, \
        cntMinBtn##btnNum = cntBtn##btnNum * (100 - cntDeviationPercent) / 100,

enum {
    cntBtnNone  = 4095 * 8 / 9,
    cntBtn1     = 4095 * 0,
    cntBtn2     = 4095 / 2,
    cntBtn3     = 4095 * 2 / 3,
    cntBtn4     = 4095 * 3 / 4,
    cntBtn5     = 4095 * 4 / 5,
    cntBtn6     = 4095 * 5 / 6,
    cntBtn7     = 4095 * 6 / 7,
    cntError    = 0xFFFF,
    
    cntDeviationPercent = 1,
    
    DECL_CNT_MAX_AND_MIN(1)
    DECL_CNT_MAX_AND_MIN(2)
    DECL_CNT_MAX_AND_MIN(3)
    DECL_CNT_MAX_AND_MIN(4)
    DECL_CNT_MAX_AND_MIN(5)
    DECL_CNT_MAX_AND_MIN(6)
    DECL_CNT_MAX_AND_MIN(7)
    
};

static WORD AdcGetCounts (void)
{
    WORD errCnt = 0xFFFF;
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while (!(ADC1->SR & ADC_SR_EOC)) {
        if( --errCnt == 0 )
            return cntError;
    }
    return ADC1->DR;	
}

WORD adcCounts;
boardBtn result;

boardBtn BoardButtonGet (void)
{
    
    adcCounts = AdcGetCounts();
    
    
    if( cntError == adcCounts )
        return btnNone;
    
    if( adcCounts > cntMinBtn1 && adcCounts < cntMaxBtn1 )
        result = btn1;
    else if( adcCounts > cntMinBtn2 && adcCounts < cntMaxBtn2 )
        result = btn2;
    else if( adcCounts > cntMinBtn3 && adcCounts < cntMaxBtn3 )
        result = btn3;
    else if( adcCounts > cntMinBtn4 && adcCounts < cntMaxBtn4 )
        result = btn4;
    else if( adcCounts > cntMinBtn5 && adcCounts < cntMaxBtn5 )
        result = btn5;
    else if( adcCounts > cntMinBtn6 && adcCounts < cntMaxBtn6 )
        result = btn6;
    else if( adcCounts > cntMinBtn7 && adcCounts < cntMaxBtn7 )
        result = btn7;
    else 
        result = btnNone;
    
    return result;
}

