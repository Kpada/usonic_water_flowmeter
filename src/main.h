#ifndef __MAIN_H__
#define __MAIN_H__

typedef struct {
    WORD            errors;
    WORD            flags;
    WORD            pending;    
} applicationState;

enum tag_appErrors{
    // tdc errors
    errOk                   = 0x00,
    errTdcCommunication     = 0x01,
    errTdcTmoThermo         = 0x02,
    errTdcTmoTof            = 0x04,
    errTdcBreakThermo       = 0x08,
    errTdcBreakTof          = 0x10,
    
};

enum tag_appFlags {
    flagShowTemp            = 0x01,
    flagShowTof             = 0x02,
};


void appSetFlag (WORD flag);
void appClearFlag (WORD flag);
BOOL appIsFlag (WORD flag);

void appSetPending (WORD pending);
void appClearPending (WORD pending);
BOOL appIsPending (WORD pending);

void appSetError (WORD err);
void appClearError (WORD err);
BOOL appIsError (WORD err);

#endif // __MAIN_H__
