#ifndef __BOARD_BUTTONS_H__
#define __BOARD_BUTTONS_H__

typedef enum {
    btnNone = 0,
    btn1,
    btn2,
    btn3,

} boardBtn;


void BoardButtonsInit (void);

boardBtn BoardButtonGet (void);

#endif // __BOARD_BUTTONS_H__
