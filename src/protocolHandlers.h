#ifndef __PROTOCOL_HANDLERS_H__
#define __PROTOCOL_HANDLERS_H__

typedef struct {
    WORD    dataCnt;
    BYTE*   buff; 
} protocolResponse;

protocolResponse protocolHandler (WORD cmd, WORD data);

#endif //__PROTOCOL_HANDLERS_H__

