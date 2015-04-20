#include "stdafx.h"
#include "boardLcd.h"

/*  
    -----           A   
    |   |       F       B
    |   |
    -----           G
    |   |       E       C
    |   |
    -----   .       D       H
*/

typedef struct {
    BYTE    lcd0;
    BYTE    lcd1;
    BYTE    lcd2;
} lcdChar;

#define S_A     0x02
#define S_B     0x01
#define S_C     0x01
#define S_D     0x02
#define S_E     0x04
#define S_F     0x04
#define S_G     0x02
#define S_H     0x01
#define S_J     0x04



lcdChar font [] = {
    {
        S_D, 
        S_C | S_E, 
        S_B | S_A | S_F
    } ,    //  0 
    {
        0, 
        S_E, 
        S_F
    } ,    //  1
    {
        S_D, 
        S_G | S_E, 
        S_B | S_A 
    } ,    //  2
    {
        S_D, 
        S_C | S_G, 
        S_B | S_A 
    } ,    //  3 
    {
        0, 
        S_C | S_G, 
        S_B | S_F 
    } ,    //  4 
    {
        S_D, 
        S_C | S_G, 
        S_F | S_A 
    } ,    //  5
    {
        S_D, 
        S_C | S_G | S_E, 
        S_F | S_A 
    } ,    //  6
    {
        0, 
        S_C, 
        S_B | S_A 
    } ,    //  7
    {
        S_D, 
        S_C | S_G | S_E, 
        S_B | S_A | S_F 
    } ,     //  8
    {
        S_D, 
        S_C | S_G, 
        S_B | S_F | S_A 
    } ,     //  9
    
};

void BoardLcdPutChar (BYTE ch, BYTE seg)
{
    BYTE fontPos = ch - 0x30;
   
    WORD offset = 23;
    if( seg == 7 )
        offset = 21;
    
    LCD->RAM[0] |= ( (DWORD)font[fontPos].lcd0 << (offset - seg * 3) );
    LCD->RAM[2] |= ( (DWORD)font[fontPos].lcd1 << (offset - seg * 3) );
    LCD->RAM[4] |= ( (DWORD)font[fontPos].lcd2 << (offset - seg * 3) );
}


#define GPIO_MODER_AF( pin )     ( 0x02UL << ((pin) << 1 ) )
#define GPIO_PP( pin)            ( 0x01UL << (pin) )
#define GPIO_NO_PULL( pin )      ( 0x03UL << (pin) )
#define GPIO_AF11_L( pin )       ( 0x0BUL << ((pin) << 2 ) ) 
#define GPIO_AF11_H( pin )       ( 0x0BUL << (((pin) - 8) << 2 ) )

__inline static void LcdInitPins (void)
{
    
    // ports clock en
    RCC->AHBENR |= (RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN);
    
    // alternative func
    GPIOA->MODER |= GPIO_MODER_AF(1) | GPIO_MODER_AF(2) | GPIO_MODER_AF(3) | GPIO_MODER_AF(8) | GPIO_MODER_AF(9) | GPIO_MODER_AF(10) | GPIO_MODER_AF(15);
    GPIOB->MODER |= GPIO_MODER_AF(0) | GPIO_MODER_AF(1) | GPIO_MODER_AF(3) | GPIO_MODER_AF(4) | GPIO_MODER_AF(5) | GPIO_MODER_AF(8) |
                    GPIO_MODER_AF(10) | GPIO_MODER_AF(11) | GPIO_MODER_AF(12) | GPIO_MODER_AF(13) | GPIO_MODER_AF(14) | GPIO_MODER_AF(15);
    GPIOC->MODER |= GPIO_MODER_AF(0) | GPIO_MODER_AF(1) | GPIO_MODER_AF(2) | GPIO_MODER_AF(3) | GPIO_MODER_AF(4) | GPIO_MODER_AF(5) |
                    GPIO_MODER_AF(6) | GPIO_MODER_AF(7);
    // push-pull
    GPIOA->OTYPER &= ~( GPIO_PP(1) | GPIO_PP(2) | GPIO_PP(3) | GPIO_PP(8) | GPIO_PP(9) | GPIO_PP(10) | GPIO_PP(15) );
    GPIOB->OTYPER &= ~( GPIO_PP(0) | GPIO_PP(1) | GPIO_PP(3) | GPIO_PP(4) | GPIO_PP(5) | GPIO_PP(8) |
                        GPIO_PP(10) | GPIO_PP(11) | GPIO_PP(12) | GPIO_PP(13) | GPIO_PP(14) | GPIO_PP(15) );
    GPIOC->OTYPER &= ~( GPIO_PP(0) | GPIO_PP(1) | GPIO_PP(2) | GPIO_PP(3) | GPIO_PP(4) | GPIO_PP(5) |
                        GPIO_PP(6) | GPIO_PP(7) );
    // no pull
    GPIOA->PUPDR &= ~( GPIO_NO_PULL(1) | GPIO_NO_PULL(2) | GPIO_NO_PULL(3) | GPIO_NO_PULL(8) | GPIO_NO_PULL(9) | GPIO_NO_PULL(10) | GPIO_NO_PULL(15) );
    GPIOB->PUPDR &= ~( GPIO_NO_PULL(0) | GPIO_NO_PULL(1) | GPIO_NO_PULL(3) | GPIO_NO_PULL(4) | GPIO_NO_PULL(5) | GPIO_NO_PULL(8) |
                        GPIO_NO_PULL(10) | GPIO_NO_PULL(11) | GPIO_NO_PULL(12) | GPIO_NO_PULL(13) | GPIO_NO_PULL(14) | GPIO_NO_PULL(15) );
    GPIOC->PUPDR &= ~( GPIO_NO_PULL(0) | GPIO_NO_PULL(1) | GPIO_NO_PULL(2) | GPIO_NO_PULL(3) | GPIO_NO_PULL(4) | GPIO_NO_PULL(5) |
                        GPIO_NO_PULL(6) | GPIO_NO_PULL(7) );
    // af11
    GPIOA->AFR[0] |= GPIO_AF11_L(1) | GPIO_AF11_L(2) | GPIO_AF11_L(3);
    GPIOA->AFR[1] |= GPIO_AF11_H(8) | GPIO_AF11_H(9) | GPIO_AF11_H(10) | GPIO_AF11_H(15);  
    
    GPIOB->AFR[0] |= GPIO_AF11_L(0) | GPIO_AF11_L(1) | GPIO_AF11_L(3) | GPIO_AF11_L(4) | GPIO_AF11_L(5);
    GPIOB->AFR[1] |= GPIO_AF11_H(8) | GPIO_AF11_H(10) | GPIO_AF11_H(11) | GPIO_AF11_H(12) | GPIO_AF11_H(13) | GPIO_AF11_H(14) | GPIO_AF11_H(15);
    
    GPIOC->AFR[0] |= GPIO_AF11_L(0) | GPIO_AF11_L(1) | GPIO_AF11_L(2) | GPIO_AF11_L(3) | GPIO_AF11_L(4) | GPIO_AF11_L(5) |
                     GPIO_AF11_L(6) | GPIO_AF11_L(7);
                    
    // COM: A8, A9, A10
}

void BoardLcdUpdate (void)
{
    LCD_UpdateDisplayRequest();
    while( !LCD_GetFlagStatus(LCD_FLAG_UDD) );   
}

void BoardLcdInit (void)
{
    LCD_InitTypeDef LCD_InitStruct;
    LcdInitPins();

    
    RCC->APB1ENR |= (RCC_APB1ENR_PWREN | RCC_APB1ENR_LCDEN);
    PWR->CR |= PWR_CR_DBP;
    RCC->CSR |= RCC_CSR_RTCRST;
    RCC->CSR &= ~RCC_CSR_RTCRST;
    RCC->CSR |= RCC_CSR_LSEON;
    while(!(RCC->CSR & RCC_CSR_LSERDY));
    RCC->CSR |= RCC_CSR_RTCSEL_LSE;
    
 /*   
    LCD->CR &= ~LCD_CR_BIAS;    // 1/3
    LCD->CR |= LCD_CR_BIAS_1;

    LCD->CR &=~LCD_CR_DUTY;     // 1/3
    LCD->CR |= (LCD_CR_DUTY_1);
    
    //LCD->CR |= LCD_CR_MUX_SEG; 
  
  LCD->FCR &= ~LCD_FCR_PS; 
  LCD->FCR |= (2<<22); 
  
  LCD->FCR &= ~LCD_FCR_DIV; 
  LCD->FCR |= (11<<18); 
  
  LCD->FCR &= ~LCD_FCR_CC;  
  LCD->FCR |= 0x00000C00; // lvl3 
  
  while(!(LCD->SR&LCD_SR_FCRSR)); 
  
  LCD->CR &= ~LCD_CR_VSEL; 
    
  LCD->CR |= LCD_CR_LCDEN; 
  while(!(LCD->SR&LCD_SR_RDY)); 
  while(!(LCD->SR&LCD_SR_ENS)); 
 */ 
  LCD_StructInit(&LCD_InitStruct);
  LCD_InitStruct.LCD_Bias = LCD_Bias_1_3;
  LCD_InitStruct.LCD_Duty = LCD_Duty_1_3;
  LCD_InitStruct.LCD_Prescaler = LCD_Prescaler_2;
  LCD_InitStruct.LCD_Divider = LCD_Divider_20;
  LCD_Init(&LCD_InitStruct);
    LCD_ContrastConfig(LCD_Contrast_Level_3);
 // LCD_BlinkConfig(LCD_BlinkMode_AllSEG_AllCOM,LCD_BlinkFrequency_Div16);
    LCD_PulseOnDurationConfig(LCD_PulseOnDuration_0);
  LCD_DeadTimeConfig(LCD_DeadTime_0);
  LCD_Cmd(ENABLE);
  while( !LCD_GetFlagStatus(LCD_FLAG_ENS) ||  !LCD_GetFlagStatus(LCD_FLAG_RDY) ) {}

    //BoardLcdPutChar('0', 0);
      /*
    BoardLcdPutChar('0', 1);
    BoardLcdPutChar('1', 2);
    BoardLcdPutChar('2', 3);
    BoardLcdPutChar('3', 4);
    BoardLcdPutChar('4', 5);
    BoardLcdPutChar('5', 6);
    BoardLcdPutChar('6', 7);
*/
      LCD_UpdateDisplayRequest();
    while( !LCD_GetFlagStatus(LCD_FLAG_UDD) );
}

void BoardLcdClear (void)
{
    LCD->RAM[0] = 0;
    LCD->RAM[2] = 0;
    LCD->RAM[4] = 0;
 //   BoardLcdUpdate();
}