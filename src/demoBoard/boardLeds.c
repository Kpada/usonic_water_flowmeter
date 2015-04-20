#include "stdafx.h"
#include "boardLeds.h"



#define GPIO_INIT_OUTPUT( portNum, pinNum ) \
                    __PASTE(GPIO, portNum)->MODER &= ~( 0x03UL << ((pinNum) << 1) ); \
                    __PASTE(GPIO, portNum)->MODER |=  ( 0x01UL << ((pinNum) << 1) )
                    
#define TURN_LED_ON( portNum, pinNum ) \
                    __PASTE(GPIO, portNum)->ODR |=  ( 0x01UL << (pinNum) )
#define TURN_LED_OFF( portNum, pinNum ) \
                    __PASTE(GPIO, portNum)->ODR &= ~( 0x01UL << (pinNum) )
#define TOGGLE_LED( portNum, pinNum ) \
                    __PASTE(GPIO, portNum)->ODR ^= ( 0x01UL << (pinNum) )
#define LED_IS_ENABLE( portNum, pinNum ) \
                    ( __PASTE(GPIO, portNum)->IDR & ( 0x01UL << (pinNum) ) )

//
#define LED1_PORT   D
#define LED1_PIN    2
#define LED2_PORT   C
#define LED2_PIN    8
#define LED3_PORT   A
#define LED3_PIN    11
#define LED4_PORT   A
#define LED4_PIN    12
#define LED5_PORT   C
#define LED5_PIN    10
#define LED6_PORT   C
#define LED6_PIN    11
#define LED7_PORT   C
#define LED7_PIN    12

void BoardLedsInit (void)
{
    GPIO_INIT_OUTPUT(LED1_PORT, LED1_PIN);
    GPIO_INIT_OUTPUT(LED2_PORT, LED2_PIN);  
    GPIO_INIT_OUTPUT(LED3_PORT, LED3_PIN);  
    GPIO_INIT_OUTPUT(LED4_PORT, LED4_PIN);  
    GPIO_INIT_OUTPUT(LED5_PORT, LED5_PIN);  
    GPIO_INIT_OUTPUT(LED6_PORT, LED6_PIN);  
    GPIO_INIT_OUTPUT(LED7_PORT, LED7_PIN);  
    
    BoardLedsTurnAllOff();
}

void BoardLedsTurnAllOff (void)
{
    TURN_LED_OFF(LED1_PORT, LED1_PIN);
    TURN_LED_OFF(LED2_PORT, LED2_PIN);
    TURN_LED_OFF(LED3_PORT, LED3_PIN);
    TURN_LED_OFF(LED4_PORT, LED4_PIN);
    TURN_LED_OFF(LED5_PORT, LED5_PIN);
    TURN_LED_OFF(LED6_PORT, LED6_PIN);
    TURN_LED_OFF(LED7_PORT, LED7_PIN);   
}

void BoardLedsTurnAllOn (void)
{
    TURN_LED_ON(LED1_PORT, LED1_PIN);
    TURN_LED_ON(LED2_PORT, LED2_PIN);
    TURN_LED_ON(LED3_PORT, LED3_PIN);
    TURN_LED_ON(LED4_PORT, LED4_PIN);
    TURN_LED_ON(LED5_PORT, LED5_PIN);
    TURN_LED_ON(LED6_PORT, LED6_PIN);
    TURN_LED_ON(LED7_PORT, LED7_PIN);    
}

void BoardLedsTurnLedOn (boardLedNum led)
{
    switch( led ) {
        case ledOne:    TURN_LED_ON(LED1_PORT, LED1_PIN); break;
        case ledTwo:    TURN_LED_ON(LED2_PORT, LED2_PIN); break;
        case ledThree:  TURN_LED_ON(LED3_PORT, LED3_PIN); break;
        case ledFour:   TURN_LED_ON(LED4_PORT, LED4_PIN); break;
        case ledFive:   TURN_LED_ON(LED5_PORT, LED5_PIN); break;
        case ledSix:    TURN_LED_ON(LED6_PORT, LED6_PIN); break;
        case ledSeven:  TURN_LED_ON(LED7_PORT, LED7_PIN); break;
    }
}

void BoardLedsTurnLedOff (boardLedNum led)
{
     switch( led ) {
        case ledOne:    TURN_LED_OFF(LED1_PORT, LED1_PIN); break;
        case ledTwo:    TURN_LED_OFF(LED2_PORT, LED2_PIN); break;
        case ledThree:  TURN_LED_OFF(LED3_PORT, LED3_PIN); break;
        case ledFour:   TURN_LED_OFF(LED4_PORT, LED4_PIN); break;
        case ledFive:   TURN_LED_OFF(LED5_PORT, LED5_PIN); break;
        case ledSix:    TURN_LED_OFF(LED6_PORT, LED6_PIN); break;
        case ledSeven:  TURN_LED_OFF(LED7_PORT, LED7_PIN); break;
    }   
}

void BoardLedsToggle (boardLedNum led)
{
     switch( led ) {
        case ledOne:    TOGGLE_LED(LED1_PORT, LED1_PIN); break;
        case ledTwo:    TOGGLE_LED(LED2_PORT, LED2_PIN); break;
        case ledThree:  TOGGLE_LED(LED3_PORT, LED3_PIN); break;
        case ledFour:   TOGGLE_LED(LED4_PORT, LED4_PIN); break;
        case ledFive:   TOGGLE_LED(LED5_PORT, LED5_PIN); break;
        case ledSix:    TOGGLE_LED(LED6_PORT, LED6_PIN); break;
        case ledSeven:  TOGGLE_LED(LED7_PORT, LED7_PIN); break;
    }       
    
}

WORD BoardLedsGetState (void)
{
    WORD result = 0;
    if( LED_IS_ENABLE(LED1_PORT, LED1_PIN) )
        result |= flagLed1;
    if( LED_IS_ENABLE(LED2_PORT, LED2_PIN) )
        result |= flagLed2;
    if( LED_IS_ENABLE(LED3_PORT, LED3_PIN) )
        result |= flagLed3;
    if( LED_IS_ENABLE(LED4_PORT, LED4_PIN) )
        result |= flagLed4;
    if( LED_IS_ENABLE(LED5_PORT, LED5_PIN) )
        result |= flagLed5;
    if( LED_IS_ENABLE(LED6_PORT, LED6_PIN) )
        result |= flagLed6;
    if( LED_IS_ENABLE(LED7_PORT, LED7_PIN) )
        result |= flagLed7;
    
    return result;
}
