#include "stdAfx.h"
#include "protocolHandlers.h"

static protocolResponse ledsHandler (WORD led);
static protocolResponse fowAndTempHandler (WORD val);

typedef struct {
    WORD cmd;                   // command
    protocolResponse (*handler) (WORD);     // handler pointer

} protocolHandlerType;

/* Proc table */

static protocolHandlerType procTable [] = {
    { 0x3130, ledsHandler },
    { 0x3330, fowAndTempHandler },
    
    
    { 0x0000, NULL}
};

protocolResponse protocolHandler (WORD cmd, WORD data)
{
    INT8 i = 0;
    INT8 correctTableIdx = -1;
    protocolResponse result;
    
    // find 
    while( procTable[i].cmd != 0x0000 && correctTableIdx == -1) {
        if( cmd == procTable[i].cmd ) 
            correctTableIdx = i;
        else
            ++i;
    }
    
    if( correctTableIdx != -1 ) {
        result = procTable[i].handler(data); 
    }
    else {
        result.dataCnt = 0;
        result.buff = NULL;
    }
    
    return result;
}

/* Handlers */

static protocolResponse ledsHandler (WORD null)
{
    protocolResponse result;
    //atic WORD ledState; 
    
  //  ledState = BoardLedsGetState();
    
    result.dataCnt = 2;
    result.buff = "40";//(BYTE*)&ledState;
    
    return result;
}

static protocolResponse fowAndTempHandler (WORD val)
{
    static BYTE values [8] = "000F0007";
    protocolResponse result;
    
    result.buff = (BYTE*)&values;
    result.dataCnt = 8;
    
    return result;
    
}
