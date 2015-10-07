#include "stdAfx.h"
#include "rpc.h"
#include "rpcHandlers.h"

// rpc handler declaration macro 
#define DECL_RPC_PROCEDURE( id, sig, proc ) { id, sig, proc },

// procedure types declaration
#define DECL_PROC_TYPE_2( p1, p2 )		typedef p1 ( * TYPE_ ## p1 ## _PROC_ ## p2 ) (p2);


// handler type - id, sig, proc
typedef struct {
	WORD 	id;				// the procedure identifier
	BYTE  	sig;			// the procedure call signature
	void* 	proc;			// procedure that should be called
} rpcHandler;



DECL_PROC_TYPE_2(BYTEARRAY, void)



/// the procedure call table
static rpcHandler rpcTable [] = {
		#include "rpcMap.cc"
		{0, 0, NULL}
};

static WORD rpcTableElementsNum = sizeof(rpcTable) / sizeof(rpcHandler);



/// find a procedure with the id
///
static rpcHandler findProc (WORD id)
{
	WORD idx = 0;
	BOOL weGotIt = FALSE;
	
	// try to fint an id
	while( !weGotIt && rpcTable[idx].id > 0 ) {
		if( id == rpcTable[idx].id )
			weGotIt = TRUE;
		else
			idx++;
	}
	
	if( weGotIt )
		return rpcTable[idx];
	else 
		return rpcTable[ rpcTableElementsNum - 1 ]; // empty
	
}


/// rpc execute
///
void rpcExecute (rpcState* state)
{
	void* rpcResult = NULL;
	WORD rpcResultLen = 0;
	
	rpcHandler handler = findProc(state->id);
	WORD signature = handler.sig;
	
	if( NULL == handler.proc ) {
		state->error = TRUE;
		return;
	}
	
	// call a proc
	switch( signature ) {
		// input - void;	output - BYTEARRAY
		case SIG_BYTEARRAY_PROC_void: {
			TYPE_BYTEARRAY_PROC_void procedure = (TYPE_BYTEARRAY_PROC_void)handler.proc;
			BYTEARRAY ba = procedure();	
			rpcResult = (void*)&ba;
			rpcResultLen = ba.size + sizeof(ba.size);
			break;
		}
		
		// unknown signature
		default : {
			state->error = TRUE;
			break;
		}
		
	}
		
    // finally set the params  
	state->dataOut = rpcResult;
	state->dataOutLen = rpcResultLen;
}

