#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

// basic typedefs
typedef signed char				INT8;
typedef unsigned char			UINT8;
typedef signed short 			INT16;
typedef unsigned short			UINT16;
typedef signed long				INT32;
typedef unsigned long			UINT32;
typedef long long			 	INT64;
typedef unsigned long long	    UINT64;
typedef unsigned int   			BOOL;

typedef float					FLOAT;
typedef double					DOUBLE;

// more human - friendly types
#define BYTE					UINT8
#define WORD					UINT16
#define DWORD					UINT32
#define QWORD					UINT64



// some special
#ifndef NULL
	#define NULL    ((void *)0)
#endif

#ifndef FALSE
	#define FALSE   ((BOOL)0)
#endif

#ifndef TRUE
	#define TRUE    ((BOOL)1)
#endif

typedef struct {
	DWORD   size;
	BYTE*	data;
} BYTEARRAY;

typedef INT64			        DATETIME;

#endif  // __TYPEDEF_H__ 
