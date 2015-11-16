#include "stdAfx.h"
#include "rpcHandlers.h"

extern applicationData appData;

struct {
    FLOAT tof1, tof2, r1, r2;
} result;


BYTEARRAY rpcGetData (void)
{
	BYTEARRAY ba;
    
	static BYTE data [16]; 
    
    result.tof1 = appData.tof1;
    result.tof2 = appData.tof2;
    result.r1   = appData.r1;
    result.r2   = appData.r2;
    
	memcpy( data, (BYTE*)&result.tof1, 16 );
	
    ba.size = 16;
    ba.data = data;
	
	return ba;
}
//---------------------------------------------------------------------------

