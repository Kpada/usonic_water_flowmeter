#ifndef _ring_buff_h_
#define _ring_buff_h_

#include "typedef.h"

#ifdef __cplusplus
	extern "C" {
#endif

// the following file defines which ringbuff 
// access implementation to use
#include <config/ringbuffConfig.h>

// ring buffer struct
//
typedef struct {
	void* data;	// data buffer
	DWORD mask; // buffer size mask
	DWORD	in;		// first index of data in buffer
	DWORD	out;	// last index of data in buffer
	DWORD count;
	
} RingBuff;
typedef volatile RingBuff* rbHANDLE;

// ring buffer initialization 
//
// size	is number of elements, not bytes
void rbInit(rbHANDLE buff, void* data, DWORD size);

// ring buffer check
//
// is empty
BOOL rbIsEmpty(rbHANDLE buff);
// is full
BOOL rbIsFull(rbHANDLE buff);
// elements count
DWORD rbGetCount(rbHANDLE buff);

// ring buffer access
//
// flush entire buffer
void rbFlush(rbHANDLE buff); 

//
// all 'pop' services below may have null pointer as input parameter
// therefore, no popped value will be returned
// otherwise, popped value is put into dereferenced ptr slot
//

// unguarded versions to use inside ISR
#ifdef USE_ISR_SAFE_RB
	BOOL rbIsEmptyFromIsr(rbHANDLE buff);
	BOOL rbIsFullFromIsr(rbHANDLE buff);
	DWORD rbGetCountFromIsr(rbHANDLE buff);
	void rbFlushFromIsr(rbHANDLE buff);	
#else	// USE_ISR_SAFE_RB
	#define rbIsEmptyFromIsr	rbIsEmpty
	#define rbIsFullFromIsr		rbIsFull
	#define rbGetCountFromIsr rbGetCount
	#define rbFlushFromIsr		rbFlush	
#endif // USE_ISR_SAFE_RB

// type-dependent part
//
// BYTE access
//
#ifdef USE_BYTE_RB
// append byte to the end of the buffer, return FALSE if fail, TRUE on success
BOOL rbPushB(rbHANDLE buff, BYTE b);
// append byte to buffer waiting timeout for buffer space to become available
BOOL rbPushTimeoutB(rbHANDLE buff, BYTE b, DWORD timeout);

// pop byte from front of the buffer
BOOL rbPopB(rbHANDLE buff, BYTE* b);
// pop byte from the buffer with timeout in ms
BOOL rbPopTimeoutB(rbHANDLE buff, BYTE* b, DWORD timeout);

#ifdef USE_ISR_SAFE_RB
	// append byte to the end of the buffer, return FALSE if fail, TRUE on success
	BOOL rbPushFromIsrB(rbHANDLE buff, BYTE b);
	// pop byte from front of the buffer
	BOOL rbPopFromIsrB(rbHANDLE buff, BYTE* b);
#else // USE_ISR_SAFE_RB	
	#define rbPushFromIsrB rbPushB
	#define rbPopFromIsrB	 rbPopB
#endif // USE_ISR_SAFE_RB

#endif // USE_BYTE_RB

// WORD access
//
#ifdef USE_WORD_RB
// append byte to the end of the buffer, return FALSE if fail, TRUE on success
BOOL rbPushW(rbHANDLE buff, WORD w);
// append byte to buffer waiting timeout for buffer space to become available
BOOL rbPushTimeoutW(rbHANDLE buff, WORD w, DWORD timeout);
// pop byte from front of the buffer
BOOL rbPopW(rbHANDLE buff, WORD* w);
// pop byte from the buffer with timeout in ms
BOOL rbPopTimeoutW(rbHANDLE buff, WORD* w, DWORD timeout);

#ifdef USE_ISR_SAFE_RB
	// append byte to the end of the buffer, return FALSE if fail, TRUE on success
	BOOL rbPushFromIsrW(rbHANDLE buff, WORD w);
	// pop byte from front of the buffer
	BOOL rbPopFromIsrW(rbHANDLE buff, WORD* w);	
#else	// USE_ISR_SAFE_RB
	#define rbPushFromIsrW rbPushW
	#define rbPopFromIsrW	 rbPopW
#endif // USE_ISR_SAFE_RB

#endif // USE_WORD_RB

// DWORD access
//
#ifdef USE_DWORD_RB
// append byte to the end of the buffer, return FALSE if fail, TRUE on success
BOOL rbPushDW(rbHANDLE buff, DWORD dw);
// append byte to buffer waiting timeout for buffer space to become available
BOOL rbPushTimeoutDW(rbHANDLE buff, DWORD dw, DWORD timeout);
// pop byte from front of the buffer
BOOL rbPopDW(rbHANDLE buff, DWORD* dw);
// pop byte from the buffer with timeout in ms
BOOL rbPopTimeoutDW(rbHANDLE buff, DWORD* dw, DWORD timeout);

#ifdef USE_ISR_SAFE_RB
	// append byte to the end of the buffer, return FALSE if fail, TRUE on success
	BOOL rbPushFromIsrDW(rbHANDLE buff, DWORD dw);
	// pop byte from front of the buffer
	BOOL rbPopFromIsrDW(rbHANDLE buff, DWORD* dw);	
#else	// USE_ISR_SAFE_RB
	#define rbPushFromIsrDW rbPushDW
	#define rbPopFromIsrDW	rbPopDW
#endif // USE_ISR_SAFE_RB

#endif // USE_DWORD_RB

// FLOAT access
//
#ifdef USE_FLOAT_RB
// append float to the end of the buffer, return FALSE if fail, TRUE on success
BOOL rbPushF(rbHANDLE buff, FLOAT f);
// append float to buffer waiting timeout for buffer space to become available
BOOL rbPushTimeoutF(rbHANDLE buff, FLOAT f, DWORD timeout);
// pop float from front of the buffer
BOOL rbPopF(rbHANDLE buff, FLOAT* f);
// pop float from the buffer with timeout in ms
BOOL rbPopTimeoutF(rbHANDLE buff, FLOAT* f, DWORD timeout);

#ifdef USE_ISR_SAFE_RB
	// append float to the end of the buffer, return FALSE if fail, TRUE on success
	BOOL rbPushFromIsrF(rbHANDLE buff, FLOAT f);
	// pop float from front of the buffer
	BOOL rbPopFromIsrF(rbHANDLE buff, FLOAT* F);	
#else	// USE_ISR_SAFE_RB
	#define rbPushFromIsrF rbPushF
	#define rbPopFromIsrF	rbPopF
#endif // USE_ISR_SAFE_RB

#endif // USE_FLOAT_RB

#ifdef __cplusplus
	}
#endif

#endif
