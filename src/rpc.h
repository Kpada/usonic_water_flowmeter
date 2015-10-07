#ifndef __PROTOCOL_HANDLERS_H__
#define __PROTOCOL_HANDLERS_H__

#define DECL_PROC_SIGNATURE1( pReturn, p1 )		SIG_ ## pReturn ## _PROC_ ## p1

// rpc data signatures
enum {
	DECL_PROC_SIGNATURE1( void, void ) = 0,
	DECL_PROC_SIGNATURE1( BYTEARRAY, void) ,
};

// rpc status

typedef struct {
	BOOL 		error; 			// is error 
	WORD 		id;				// procedure id
	BYTEARRAY	dataIn;			// input data buffer
	WORD		dataOutLen;		
	void* 		dataOut;		// output data
} rpcState;

void rpcExecute (rpcState* state);




#endif //__PROTOCOL_HANDLERS_H__

