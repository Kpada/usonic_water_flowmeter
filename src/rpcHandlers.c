#include "stdAfx.h"
#include "rpcHandlers.h"

extern applicationData appData;

BYTEARRAY rpcProc1 (void)
{
	BYTEARRAY ba;
	
    static BYTE data [sizeof(appData)];
	memcpy( data, (BYTE*)&appData.r1, sizeof(appData));
	
    ba.size = sizeof(appData);
    ba.data = data;
	
	return ba;
}

