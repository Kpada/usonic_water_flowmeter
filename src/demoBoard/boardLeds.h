#ifndef __BOARD_LEDS_H__
#define __BOARD_LEDS_H__

typedef enum {
    ledOne = 1,
    ledTwo,
    ledThree,
    ledFour,
    ledFive,
    ledSix,
    ledSeven,    
} boardLedNum;

enum boardLedsFlags{
    flagLed1    = 0x0001,
    flagLed2    = 0x0002,
    flagLed3    = 0x0004,
    flagLed4    = 0x0008,
    flagLed5    = 0x0010,
    flagLed6    = 0x0020,
    flagLed7    = 0x0040,
};

void BoardLedsInit (void);

void BoardLedsTurnAllOff (void);

void BoardLedsTurnAllOn (void);

void BoardLedsTurnLedOn (boardLedNum led);

void BoardLedsTurnLedOff (boardLedNum led);

void BoardLedsToggle (boardLedNum led);

WORD BoardLedsGetState (void);

#endif // __BOARD_LEDS_H__
