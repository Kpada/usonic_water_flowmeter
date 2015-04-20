#ifndef __UART_H__
#define __UART_H__

void uartInit (void);

DWORD uartPutBytes(const BYTE* bytes, DWORD count);
DWORD uartGetBytes(BYTE* pBytes, DWORD count, DWORD timeout);
DWORD uartEstimateGet (DWORD byteCnt);

#endif //__UART_H__

