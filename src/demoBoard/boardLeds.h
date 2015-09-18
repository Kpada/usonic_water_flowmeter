#ifndef __BOARD_LEDS_H__
#define __BOARD_LEDS_H__

/// initializaion
///
void BoardLedInit (void);

/// turn led on
///
void BoardLedTurnOn (void);

/// turn led off
///
void BoardLedTurnOff (void);

/// toggle
///
void BoardLedToggle (void);

/// is led emitting?
///
BOOL BoardLedGetState (void);

#endif // __BOARD_LEDS_H__
