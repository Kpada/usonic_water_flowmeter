#ifndef __BOARD_LCD_H__
#define __BOARD_LCD_H__

void BoardLcdInit (void);
void BoardLcdPutChar (BYTE ch, BYTE seg);
void BoardLcdUpdate (void);
void BoardLcdClear (void);
void BoardLcdPutFloat (FLOAT value, BYTE afterPoint);
void BoardLcdPutStr (BYTE* str);
#endif // __BOARD_LCD_H__
