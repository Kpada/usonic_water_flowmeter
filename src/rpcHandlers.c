#include "stdAfx.h"
#include "rpcHandlers.h"

extern applicationData appData;

BYTEARRAY rpcGetData (void)
{
	BYTEARRAY ba;
	
    static BYTE data [sizeof(appData)];
	memcpy( data, (BYTE*)&appData.tof1, sizeof(appData));
	
    ba.size = sizeof(appData);
    ba.data = data;
	
	return ba;
}

