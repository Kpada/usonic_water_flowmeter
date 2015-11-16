#ifndef USE_EMULATOR
#	include <stdafx.h>
#else
#	include <emulator_stubs.h>
#endif
#pragma hdrstop 

#include "ringbuff.h"


#if defined(USE_ISR_SAFE_RB) && !defined(USE_EMULATOR)
#	ifdef USE_USER_SWI
#		include "userSWI.h"
//#	else
//		extern void __disable_irq(void);	
//		extern void __enable_irq(void);	
#	endif
#endif

// ring buffer initialization 
//
void rbInit(rbHANDLE buff, void* data, DWORD size)
{
	buff->data 	= data;	// data buffer begin
	buff->mask 	= size-1UL;
	buff->in = buff->out = buff->count = 0;
}
//---------------------------------------------------------------------------

// is empty
#ifdef USE_ISR_SAFE_RB
	BOOL rbIsEmpty(rbHANDLE buff)
	{
		BOOL result;
		__disable_irq();
		result = rbIsEmptyFromIsr(buff);
		__enable_irq();

		return result;
	}
//---------------------------------------------------------------------------

	BOOL rbIsEmptyFromIsr(rbHANDLE buff)
#else
	BOOL rbIsEmpty(rbHANDLE buff)	
#endif	// USE_ISR_SAFE_R
	{
		return (0 == buff->count);		
	}
//---------------------------------------------------------------------------

// is full
#ifdef USE_ISR_SAFE_RB
	BOOL rbIsFull(rbHANDLE buff)
	{
		BOOL result;
		__disable_irq();
		result = rbIsFullFromIsr(buff);
		__enable_irq();

		return result;
	}
//---------------------------------------------------------------------------

	BOOL rbIsFullFromIsr(rbHANDLE buff)
#else
	BOOL rbIsFull(rbHANDLE buff)	
#endif	// USE_ISR_SAFE_R
{
	return buff->mask+1 == buff->count;
}
//---------------------------------------------------------------------------

// elements count
#ifdef USE_ISR_SAFE_RB
	DWORD rbGetCount(rbHANDLE buff)
	{
		DWORD result;
		__disable_irq();
		result = rbGetCountFromIsr(buff);
		__enable_irq();

		return result;
	}	
//---------------------------------------------------------------------------

	DWORD rbGetCountFromIsr(rbHANDLE buff)
#else
	DWORD rbGetCount(rbHANDLE buff)	
#endif	// USE_ISR_SAFE_RB
{
 	return buff->count;
}
//---------------------------------------------------------------------------

// flush entire buffer
#ifdef USE_ISR_SAFE_RB
	void rbFlush(rbHANDLE buff)
	{
		__disable_irq();
		rbFlushFromIsr(buff);
		__enable_irq();
	}
//---------------------------------------------------------------------------

	void rbFlushFromIsr(rbHANDLE buff)
#else
	void rbFlush(rbHANDLE buff)	
#endif	// USE_ISR_SAFE_RB
{
	buff->in = buff->out = buff->count = 0;
}
//---------------------------------------------------------------------------

#define RB_IS_NOT_FULL(buff) \
	(buff->mask+1 != buff->count)

// BYTE access
//
#ifdef USE_BYTE_RB

// append byte to buffer waiting timeout for buffer space to become available
BOOL rbPushTimeoutB(rbHANDLE buff, BYTE b, DWORD timeout)
{
	BOOL result = rbPushB(buff, b);
	while( !result && timeout-- )
	{
		DelayUSec(1000);
		result = rbPushB(buff, b);
	}

	return result;
}
//---------------------------------------------------------------------------

// append byte to the end of the buffer, return (false) if fail, (true) on success
#ifdef USE_ISR_SAFE_RB
	BOOL rbPushB(rbHANDLE buff, BYTE b)
	{
		BOOL result;
		__disable_irq();
		result = rbPushFromIsrB(buff, b);
		__enable_irq();

		return result;
	}
//---------------------------------------------------------------------------

	BOOL rbPushFromIsrB(rbHANDLE buff, BYTE b)
#else
	BOOL rbPushB(rbHANDLE buff, BYTE b)	
#endif	// USE_ISR_SAFE_RB
{
	if( RB_IS_NOT_FULL(buff) ) // if we have room in buffer
	{
		((BYTE*)buff->data)[buff->mask & buff->in++] = b;
		++buff->count;
		return TRUE;
	}
	else
		return FALSE;
}
//---------------------------------------------------------------------------

// pop byte from the buffer with timeout in ms
BOOL rbPopTimeoutB(rbHANDLE buff, BYTE* b, DWORD timeout)
{		
	BOOL result = rbPopB(buff, b);

	while( !result && timeout-- )
	{
		DelayUSec(1000);
		result = rbPopB(buff, b);
	}

	return result;
}
//---------------------------------------------------------------------------

// pop byte from front of the buffer
#ifdef USE_ISR_SAFE_RB
	BOOL rbPopB(rbHANDLE buff, BYTE* b)
	{
		BOOL result;
		__disable_irq();
		result = rbPopFromIsrB(buff, b);
		__enable_irq();

		return result;
	}
//---------------------------------------------------------------------------

	BOOL rbPopFromIsrB(rbHANDLE buff, BYTE* b)
#else
	BOOL rbPopB(rbHANDLE buff, BYTE* b)	
#endif	// USE_ISR_SAFE_RB
{
	if( buff->count )
	{
		if(b)
			*b = ((BYTE*)buff->data)[buff->mask & buff->out++];
		else
			++buff->out;
		--buff->count;
		return TRUE;
	}
	else
		return FALSE;
}
//---------------------------------------------------------------------------

#endif // USE_BYTE_RB

// WORD access
//
#ifdef USE_WORD_RB

// append word to the end of the buffer, return FALSE if fail, TRUE on success
#ifdef USE_ISR_SAFE_RB
	BOOL rbPushW(rbHANDLE buff, WORD w)
	{
		BOOL result;
		__disable_irq();
		result = rbPushFromIsrW(buff, w);
		__enable_irq();

		return result;
	}
//---------------------------------------------------------------------------

	BOOL rbPushFromIsrW(rbHANDLE buff, WORD w)
#else	// USE_ISR_SAFE_RB
	BOOL rbPushW(rbHANDLE buff, WORD w)
#endif // USE_ISR_SAFE_RB
{
	if( RB_IS_NOT_FULL(buff) ) // if we have room in buffer
	{
		((WORD*)buff->data)[buff->mask & buff->in++] = w;
		++buff->count;
		return TRUE;
	}
	else
		return FALSE;
}
//---------------------------------------------------------------------------

// append word to buffer waiting timeout for buffer space to become available
BOOL rbPushTimeoutW(rbHANDLE buff, WORD w, DWORD timeout)
{
	BOOL result = rbPushW(buff, w);
	while( !result && timeout-- )
	{
		DelayUSec(1000);
		result = rbPushW(buff, w);
	}

	return result;
}
//---------------------------------------------------------------------------

// pop word from front of the buffer
#ifdef USE_ISR_SAFE_RB
	BOOL rbPopW(rbHANDLE buff, WORD* w)
	{
		BOOL result;
		__disable_irq();
		result = rbPopFromIsrW(buff, w);
		__enable_irq();

		return result;
	}
//---------------------------------------------------------------------------

	BOOL rbPopFromIsrW(rbHANDLE buff, WORD* w)
#else
	BOOL rbPopW(rbHANDLE buff, WORD* w)
#endif
{
	if( buff->count )
	{
		if(w)
			*w = ((WORD*)buff->data)[buff->mask & buff->out++];
		else
			++buff->out;
		--buff->count;
		return TRUE;
	}
	else
		return FALSE;
}
//---------------------------------------------------------------------------

// pop word from the buffer with timeout in ms
BOOL rbPopTimeoutW(rbHANDLE buff, WORD* w, DWORD timeout)
{
	BOOL result = rbPopW(buff, w);

	while( !result && timeout-- )
	{
		DelayUSec(1000);
		result = rbPopW(buff, w);
	}

	return result;
}
//---------------------------------------------------------------------------

#endif // USE_WORD_RB

// DWORD access
//
#ifdef USE_DWORD_RB
// append dword to the end of the buffer, return FALSE if fail, TRUE on success
#ifdef USE_ISR_SAFE_RB
	BOOL rbPushDW(rbHANDLE buff, DWORD dw)
	{
		BOOL result;
		__disable_irq();
		result = rbPushFromIsrDW(buff, dw);
		__enable_irq();

		return result;
	}
//---------------------------------------------------------------------------

	BOOL rbPushFromIsrDW(rbHANDLE buff, DWORD dw)
#else	// USE_ISR_SAFE_RB
	BOOL rbPushDW(rbHANDLE buff, DWORD dw)
#endif // USE_ISR_SAFE_RB
{
	if( RB_IS_NOT_FULL(buff) ) // if we have room in buffer
	{
		((DWORD*)buff->data)[buff->mask & buff->in++] = dw;
		++buff->count;
		return TRUE;
	}
	else
		return FALSE;
}
//---------------------------------------------------------------------------

// append dword to buffer waiting timeout for buffer space to become available
BOOL rbPushTimeoutDW(rbHANDLE buff, DWORD dw, DWORD timeout)
{
	BOOL result = rbPushDW(buff, dw);
	while( !result && timeout-- )
	{
		usDelay(1000);
		result = rbPushDW(buff, dw);
	}

	return result;
}
//---------------------------------------------------------------------------

// pop dword from front of the buffer
#ifdef USE_ISR_SAFE_RB
	BOOL rbPopDW(rbHANDLE buff, DWORD* dw)
	{
		BOOL result;
		__disable_irq();
		result = rbPopFromIsrDW(buff, dw);
		__enable_irq();

		return result;
	}
//---------------------------------------------------------------------------

	BOOL rbPopFromIsrDW(rbHANDLE buff, DWORD* dw)
#else
	BOOL rbPopDW(rbHANDLE buff, DWORD* dw)
#endif
{
	if( buff->count )
	{
		if(dw)
			*dw = ((DWORD*)buff->data)[buff->mask & buff->out++];
		else
			++buff->out;
		--buff->count;
		return TRUE;
	}
	else
		return FALSE;
}
//---------------------------------------------------------------------------

// pop dword from the buffer with timeout in ms
BOOL rbPopTimeoutDW(rbHANDLE buff, DWORD* dw, DWORD timeout)
{
	BOOL result = rbPopDW(buff, dw);

	while( !result && timeout-- )
	{
		usDelay(1000);
		result = rbPopDW(buff, dw);
	}

	return result;
}
//---------------------------------------------------------------------------

#endif // USE_DWORD_RB

// FLOAT access
//
#ifdef USE_FLOAT_RB
// append float to the end of the buffer, return FALSE if fail, TRUE on success
#ifdef USE_ISR_SAFE_RB
	BOOL rbPushF(rbHANDLE buff, FLOAT f)
	{
		BOOL result;
		__disable_irq();
		result = rbPushFromIsrF(buff, f);
		__enable_irq();

		return result;
	}
//---------------------------------------------------------------------------

	BOOL rbPushFromIsrF(rbHANDLE buff, FLOAT f)
#else	// USE_ISR_SAFE_RB
	BOOL rbPushF(rbHANDLE buff, FLOAT f)
#endif // USE_ISR_SAFE_RB
{
	if( RB_IS_NOT_FULL(buff) ) // if we have room in buffer
	{
		((FLOAT*)buff->data)[buff->mask & buff->in++] = f;
		++buff->count;
		return TRUE;
	}
	else
		return FALSE;
}
//---------------------------------------------------------------------------

// append float to buffer waiting timeout for buffer space to become available
BOOL rbPushTimeoutF(rbHANDLE buff, FLOAT f, DWORD timeout)
{
	BOOL result = rbPushF(buff, f);
	while( !result && timeout-- )
	{
		DelayUSec(1000);
		result = rbPushF(buff, f);
	}

	return result;
}
//---------------------------------------------------------------------------

// pop float from front of the buffer
#ifdef USE_ISR_SAFE_RB
	BOOL rbPopF(rbHANDLE buff, FLOAT* f)
	{
		BOOL result;
		__disable_irq();
		result = rbPopFromIsrF(buff, f);
		__enable_irq();

		return result;
	}
//---------------------------------------------------------------------------

	BOOL rbPopFromIsrF(rbHANDLE buff, FLOAT* f)
#else
	BOOL rbPopF(rbHANDLE buff, FLOAT* f)
#endif
{
	if( buff->count )
	{
		if(f)
			*f = ((FLOAT*)buff->data)[buff->mask & buff->out++];
		else
			++buff->out;
		--buff->count;
		return TRUE;
	}
	else
		return FALSE;
}
//---------------------------------------------------------------------------

// pop float from the buffer with timeout in ms
BOOL rbPopTimeoutF(rbHANDLE buff, FLOAT* f, DWORD timeout)
{
	BOOL result = rbPopF(buff, f);
	while( !result && timeout-- )
	{
		DelayUSec(1000);
		result = rbPopF(buff, f);
	}

	return result;
}
//---------------------------------------------------------------------------

#endif // USE_FLOAT_RB
