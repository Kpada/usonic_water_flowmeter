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
#define VOID					void
#define CHAR					INT8
#define BYTE					UINT8
#define WORD					UINT16
#define DWORD					UINT32
#define QWORD					UINT64

typedef int						INT;
typedef unsigned int			UINT;
typedef INT64			        DATETIME;
typedef INT64			        TIMESPAN;

typedef char					ES_ASCII_CHAR;
typedef const ES_ASCII_CHAR*	ES_ASCII_CSTR;
typedef ES_ASCII_CHAR*			ES_ASCII_STR;

// ecosf special data types
// the remote reflection engine implementation
#define esI8                    INT8
#define esU8                    UINT8
#define esI16                   INT16
#define esU16                   UINT16
#define esI32                   INT32
#define esU32                   UINT32
#define esI64                   INT64
#define esU64                   UINT64
#define esDT                    DATETIME
#define esTS                    TIMESPAN
#define esF                     FLOAT
#define esD                     DOUBLE
#define esBL       				BOOL

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

    
/* HANDLE */
    
typedef volatile void*		                                busHANDLE;	

// define port handle type
#define DEF_VOLATILE_HANDLE(Name)	                        typedef volatile void* Name
// special cast for port handles and structures
#define CAST_PORT_HANDLE(PortStructType, PortHandle)	    ((volatile PortStructType*)(PortHandle))
// define port struct pointer var
#define DEF_PORT_STRUCT_VAR(PortStructType, PortHandle)     volatile PortStructType* ps = CAST_PORT_HANDLE(PortStructType, PortHandle)
    
#ifndef INVALID_HANDLE
	#define INVALID_HANDLE 		NULL
#endif

// const array items count
#ifndef CONST_ARRAY_COUNT
#	define CONST_ARRAY_COUNT( array )	(sizeof(array)/sizeof(array[0]))
#endif


typedef struct {
	DWORD   size;
	BYTE*	data;

} BYTEARRAY;
#define esBA                    BYTEARRAY

#define BCDBYTE					UINT8


#pragma pack(push, 1)

// power status structure, returned
// from power status rpc call
//
typedef struct {
	UINT16  minCode;	 // device - dependent power codes
	UINT16  curCode;
	UINT16  maxCode;
	UINT8	flags;		// power flags mask (optional, may be 0)

} PowerStatus;


typedef struct {
	UINT16 count;
	UINT16 frac;
	UINT8 unit; // = msuXXX
	UINT8 dummy;
	
} EsMemSpaceInfo;

// file system result
typedef WORD FsResult;

// file system item struct. used in rpc
// directory listing, rpc FS functions
//
typedef struct {
	UINT32		fsize;			// file size
	DATETIME	fts;			// file time stamp
	UINT8		attrs;			// file attributes
	char		fname[13];	    // file name (short)

} FsItem;

// file system enumeration|search result struct
//
typedef struct {
	FsResult	result;		    // file operation result
	FsItem		item;

} FsItemResult;

// file system space info
//
typedef struct {
	FsResult	    result;		// file operation result
	EsMemSpaceInfo  total;
	EsMemSpaceInfo  free;

} FsSpaceInfo;


// firmware | software version information
typedef struct
{
	BYTE major;
	BYTE minor;

} VerInfo;

// device production number
typedef struct 
{
	WORD year;		// year of production
	WORD ser;		// device production (serial) number within year

} DeviceNum;

// basic software | firmware identification block
typedef struct
{
	WORD 			type;		// device type (model) id
 	DeviceNum num;		// device production number
	VerInfo 	ver;		// firmware major|minor version

} BasicFirmwareID;

typedef BYTE UID[32];			// (G)UID	in string representation, 
													// no delimiters, commas, or brackets

// extended core firmware identification structure
// which may be returned for the core firmware id query
typedef struct
{
	BasicFirmwareID id;
	UID key;
	DWORD funcMask;

}	HubFirmwareID;

// software CRC information struct
typedef struct
{
	UINT32 crcPrev;	// reserved
	UINT32 crcCur;		// current CRC32 (as calculated by CRC32 IEEE 802.3)
	DATETIME ts;	// change timestamp
	
} SoftwareCrcInfo;

typedef struct
{
	char name[64];
	char id[32];
	SoftwareCrcInfo fw;

} FwSwInfo;

// software information structure
// which is returned as a response to the standard RPC "software id get" request
typedef struct
{
	FwSwInfo fwSwInfo;
	SoftwareCrcInfo cal;
	UINT8 filler[64];
	
} SoftwareInfo;

#pragma pack(pop)

// appliance firmware id (aliased BasicFirmwareID)
#define ApplianceFirmwareID BasicFirmwareID

enum { 
	Type_SZE = sizeof(UINT16),
	DevNum_SZE = sizeof(DeviceNum),
	VerInfo_SZE = sizeof(VerInfo),
	ApplianceFirmwareID_NoFw_SZE = DevNum_SZE+Type_SZE,
	BasicFirmwareID_SZE = sizeof(BasicFirmwareID),
	UID_SZE = sizeof(UID),
	HubFirmwareID_SZE = sizeof(HubFirmwareID),
	SoftwareCrcInfo_SZE = sizeof(SoftwareCrcInfo),
	FwSwInfo_SZE = sizeof(FwSwInfo),
	SoftwareInfo_SZE = sizeof(SoftwareInfo),
};

#define ApplianceFirmwareID_SZE BasicFirmwareID_SZE


#endif  // __TYPEDEF_H__ 
