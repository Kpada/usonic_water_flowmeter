#include "stdafx.h"
#include "boardLcd.h"

/*

    -----           A   
    |   |       F       B
    |   |
    -----           G
    |   |       E       C
    |   |
    -----   .       D       P
    
*/

// lcd char type
typedef struct {
    BYTE    lcd0;
    BYTE    lcd1;
    BYTE    lcd2;
} lcdChar;

// signs 
#define S_A     0x02
#define S_B     0x04
#define S_C     0x04
#define S_D     0x02
#define S_E     0x01
#define S_F     0x01
#define S_G     0x02
#define S_D     0x02
#define S_P     0x04
#define S_T     0x01

// font 
lcdChar font [] = {
    
// char: '0'
///
    {
        S_A | S_B | S_F, 
        S_C | S_E, 
        S_D
    } , 
    
// char: '1'
///
    {
        S_B, 
        S_C, 
        0
    } ,  
    
// char: '2'
///
    {
        S_A | S_B, 
        S_G | S_E, 
        S_D 
    } ,  
    
// char: '3'
///
    {
        S_A | S_B, 
        S_C | S_G, 
        S_D 
    } , 
    
// char: '4'
///    
    {
        S_B | S_F, 
        S_C | S_G, 
        0 
    } , 
    
// char: '5'
///    
    {
        S_A | S_F, 
        S_C | S_G, 
        S_D 
    } ,
    
// char: '6'
///
    {
        S_A | S_F, 
        S_C | S_G | S_E, 
        S_D 
    } ,
    
// char: '7'
///
    {
        S_B | S_A, 
        S_C, 
        0 
    } ,
    
// char: '8'
///
    {
        S_B | S_A | S_F, 
        S_C | S_G | S_E, 
        S_D 
    } ,
    
// char: '9'
///
    {
        S_B | S_F | S_A, 
        S_C | S_G, 
        S_D
    } ,  

// char: '.'    
///
    { 
        0,
        0,
        S_P
    }, 
    
// char: '-'
///    
    { 
        0,
        S_G,
        0
    }, 

};


void BoardLcdPutChar (BYTE ch, BYTE seg)
{  
    WORD offset;
    WORD fontPosShift;
    
    if( seg > 8 )
        return;
     
    offset = 0 == seg ? 0 : 2; // cuz seg num 3 && seg num 4 used as SPI
    fontPosShift = seg * 3 + offset;
    
    if( ch >= '0' && ch <= '9' ) {
        BYTE fontPos = ch - '0';   
        // apply   
        LCD->RAM[0] |= ( (DWORD)font[fontPos].lcd0 << fontPosShift );
        LCD->RAM[2] |= ( (DWORD)font[fontPos].lcd1 << fontPosShift );
        LCD->RAM[4] |= ( (DWORD)font[fontPos].lcd2 << fontPosShift );
    }
    else if( '.' == ch ) {
        BYTE fontPos = 10;   
        LCD->RAM[4] |= ( (DWORD)font[fontPos].lcd2 << fontPosShift );
    }
    else if( '-' == ch ) {
        BYTE fontPos = 11; 
        LCD->RAM[0] |= ( (DWORD)font[fontPos].lcd0 << fontPosShift );
        LCD->RAM[2] |= ( (DWORD)font[fontPos].lcd1 << fontPosShift );
        LCD->RAM[4] |= ( (DWORD)font[fontPos].lcd2 << fontPosShift );        
    }
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
                    GPIO_MODER_AF(6) | GPIO_MODER_AF(7) | GPIO_MODER_AF(8) | GPIO_MODER_AF(9) | GPIO_MODER_AF(10) | \
                    GPIO_MODER_AF(11) | GPIO_MODER_AF(12);
    // push-pull
    GPIOA->OTYPER &= ~( GPIO_PP(1) | GPIO_PP(2) | GPIO_PP(3) | GPIO_PP(8) | GPIO_PP(9) | GPIO_PP(10) | GPIO_PP(15) );
    GPIOB->OTYPER &= ~( GPIO_PP(0) | GPIO_PP(1) | GPIO_PP(3) | GPIO_PP(4) | GPIO_PP(5) | GPIO_PP(8) |
                        GPIO_PP(10) | GPIO_PP(11) | GPIO_PP(12) | GPIO_PP(13) | GPIO_PP(14) | GPIO_PP(15) );
    GPIOC->OTYPER &= ~( GPIO_PP(0) | GPIO_PP(1) | GPIO_PP(2) | GPIO_PP(3) | GPIO_PP(4) | GPIO_PP(5) |
                        GPIO_PP(6) | GPIO_PP(7) | GPIO_PP(8) | GPIO_PP(9) | GPIO_PP(10) | GPIO_PP(11) | GPIO_PP(12) );
    // no pull
    GPIOA->PUPDR &= ~( GPIO_NO_PULL(1) | GPIO_NO_PULL(2) | GPIO_NO_PULL(3) | GPIO_NO_PULL(8) | GPIO_NO_PULL(9) | GPIO_NO_PULL(10) | GPIO_NO_PULL(15) );
    GPIOB->PUPDR &= ~( GPIO_NO_PULL(0) | GPIO_NO_PULL(1) | GPIO_NO_PULL(3) | GPIO_NO_PULL(4) | GPIO_NO_PULL(5) | GPIO_NO_PULL(8) |
                        GPIO_NO_PULL(10) | GPIO_NO_PULL(11) | GPIO_NO_PULL(12) | GPIO_NO_PULL(13) | GPIO_NO_PULL(14) | GPIO_NO_PULL(15) );
    GPIOC->PUPDR &= ~( GPIO_NO_PULL(0) | GPIO_NO_PULL(1) | GPIO_NO_PULL(2) | GPIO_NO_PULL(3) | GPIO_NO_PULL(4) | GPIO_NO_PULL(5) |
                        GPIO_NO_PULL(6) | GPIO_NO_PULL(7) | GPIO_NO_PULL(8) | GPIO_NO_PULL(9) | GPIO_NO_PULL(10) | GPIO_NO_PULL(11) | GPIO_NO_PULL(12) );
    // af11
    GPIOA->AFR[0] |= GPIO_AF11_L(1) | GPIO_AF11_L(2) | GPIO_AF11_L(3);
    GPIOA->AFR[1] |= GPIO_AF11_H(8) | GPIO_AF11_H(9) | GPIO_AF11_H(10) | GPIO_AF11_H(15);  
    
    GPIOB->AFR[0] |= GPIO_AF11_L(0) | GPIO_AF11_L(1) | GPIO_AF11_L(3) | GPIO_AF11_L(4) | GPIO_AF11_L(5);
    GPIOB->AFR[1] |= GPIO_AF11_H(8) | GPIO_AF11_H(10) | GPIO_AF11_H(11) | GPIO_AF11_H(12) | GPIO_AF11_H(13) | GPIO_AF11_H(14) | GPIO_AF11_H(15);
    
    GPIOC->AFR[0] |= GPIO_AF11_L(0) | GPIO_AF11_L(1) | GPIO_AF11_L(2) | GPIO_AF11_L(3) | GPIO_AF11_L(4) | GPIO_AF11_L(5) |
                     GPIO_AF11_L(6) | GPIO_AF11_L(7);
    GPIOC->AFR[1] |= GPIO_AF11_H(8) | GPIO_AF11_H(9) | GPIO_AF11_H(10) | GPIO_AF11_H(11) | GPIO_AF11_H(12) ;
                    
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

    LCD_StructInit(&LCD_InitStruct);
    LCD_InitStruct.LCD_Bias = LCD_Bias_1_2;
    LCD_InitStruct.LCD_Duty = LCD_Duty_1_3;
    LCD_InitStruct.LCD_Prescaler = 2 << 22;
    LCD_InitStruct.LCD_Divider = 1 << 18;
    LCD_Init(&LCD_InitStruct);
    LCD_ContrastConfig(LCD_Contrast_Level_3);
    LCD_PulseOnDurationConfig(LCD_PulseOnDuration_7);
    LCD_DeadTimeConfig(LCD_DeadTime_0);
    LCD_MuxSegmentCmd(ENABLE);
    LCD_Cmd(ENABLE);
    while( !LCD_GetFlagStatus(LCD_FLAG_ENS) ||  !LCD_GetFlagStatus(LCD_FLAG_RDY) ) {}

    LCD_UpdateDisplayRequest();
    while( !LCD_GetFlagStatus(LCD_FLAG_UDD) );
}

void BoardLcdClear (void)
{
    LCD->RAM[0] = 0;
    LCD->RAM[2] = 0;
    LCD->RAM[4] = 0;
}


void BoardLcdPutFloat (FLOAT value, BYTE afterPoint)
{   
    char str [12];
    BYTE pos = 0;
    BYTE segment = 0;
    // format
    char frmt [5] = "%.xf";
    frmt[2] = afterPoint + '0';
    // convert
    sprintf(str, frmt, value);
    // put
    while( str[pos] != 0 ) { 
        BYTE ch = str[pos++];
        BoardLcdPutChar(ch, segment);
        // check the next char
        if( str[pos] != '.' ) {
            segment++;
        }
    } 
}

void BoardLcdPutStr (BYTE* str)
{
    BYTE pos = 0;
    BYTE segment = 0;
    BYTE len = 9;
    while( str[pos] != 0 && len > 0 ) { 
        BYTE ch = str[pos++];
        BoardLcdPutChar(ch, segment);
        // check the next char
        if( str[pos] != '.' ) {
            segment++;
            len--;
        }
    }     
    
}
