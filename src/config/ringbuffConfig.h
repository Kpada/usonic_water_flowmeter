#ifndef _ringbuff_condig_h_
#define _ringbuff_condig_h_

// define which ringbuff implementation to use
#define USE_BYTE_RB
#define USE_WORD_RB
#define USE_FLOAT_RB

// use additional ISR-aware implementations of ringbuffer API
#define USE_ISR_SAFE_RB

#endif // _ringbuff_condig_h_
