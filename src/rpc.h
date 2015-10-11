#ifndef __PROTOCOL_HANDLERS_H__
#define __PROTOCOL_HANDLERS_H__

#define DECL_PROC_SIGNATURE1( pReturn, p1 )		SIG_ ## pReturn ## _PROC_ ## p1

// rpc data signatures
enum {
	DECL_PROC_SIGNATURE1( void, void ) = 0,
	DECL_PROC_SIGNATURE1( BYTEARRAY, void) ,
};

// rpc buffer type
enum {
    dataType_void = 0,
    dataType_uint8,
    dataType_uint16,
    dataType_uint32,
    dataType_uint64,
    dataType_int8,
    dataType_int16,
    dataType_int32,
    dataType_int64,
    dataType_float,
    dataType_double,
    dataType_BA,   
};

// rpc status

typedef struct {
	BOOL 		error; 			// is error 
	WORD 		id;				// procedure id
	BYTEARRAY	dataIn;			// input data buffer
	WORD		dataOutLen;		
    WORD        dataOutType;
	void* 		dataOut;		// output data
} rpcState;

void rpcExecute (rpcState* state);




#endif //__PROTOCOL_HANDLERS_H__

