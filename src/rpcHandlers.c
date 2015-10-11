#include "stdAfx.h"
#include "rpcHandlers.h"

static dataRpc dataConst = { 100.f, 200.f, 300.f, 400.f };

BYTEARRAY rpcGetData (void)
{
	BYTEARRAY ba;
	static BYTE data [16];  
	memcpy( data, (BYTE*)&dataConst.tof1, 16);
	
    ba.size = sizeof(dataConst);
    ba.data = data;
	
	return ba;
}

