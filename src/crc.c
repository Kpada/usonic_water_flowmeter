#include <stdAfx.h>
#include "crc.h"

// calculate checksum16 as of RFC1071

static DWORD calcSumForChecksum16 (const BYTE* buff, DWORD buffLen)
{
	DWORD result = 0;

	while( buffLen > 1 )  
	{
		result += *(UINT16*)buff;
		buff += 2;
		buffLen -= 2;
	}

	// optional 1 byte left-over
	if( buffLen > 0 )
		result += *buff;

	return result;
}

static WORD checksum16FromSum (DWORD sum)
{
	// fold 32-bit sum to 16 bits
	while( sum / 0x00010000UL )
    sum = (sum & 0x0000FFFFUL) + (sum / 0x00010000UL);

	return ~((UINT16)sum);
}

WORD checksum16 (const BYTE* buff, DWORD buffLen)
{
	return checksum16FromSum( calcSumForChecksum16(buff, buffLen) );
}
