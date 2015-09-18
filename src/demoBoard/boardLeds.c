#include "stdafx.h"
#include "boardLeds.h"

// helpers
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


// leds declaration
#define LED1_PORT   A
#define LED1_PIN    12

/// initializaion
///
void BoardLedInit (void)
{
#if A == LED1_PORT
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
#else
    #error 
#endif
    
    GPIO_INIT_OUTPUT(LED1_PORT, LED1_PIN);
    BoardLedTurnOff();
}
//---------------------------------------------------------------------------

/// turn led off
///
void BoardLedTurnOff (void)
{
    TURN_LED_OFF(LED1_PORT, LED1_PIN);
}
//---------------------------------------------------------------------------

/// turn led on
///
void BoardLedTurnOn (void)
{
    TURN_LED_ON(LED1_PORT, LED1_PIN);
}
//---------------------------------------------------------------------------

/// toggle
///
void BoardLedToggle (void)
{
    TOGGLE_LED(LED1_PORT, LED1_PIN); 
}
//---------------------------------------------------------------------------

/// is led emitting?
///
BOOL BoardLedsGetState (void)
{
    return LED_IS_ENABLE(LED1_PORT, LED1_PIN);
}
//---------------------------------------------------------------------------
