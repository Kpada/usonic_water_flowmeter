#ifndef __SPI_H__
#define __SPI_H__

void SpiInit (void);

void SpiPutByte (BYTE data);

BYTE SpiGetByte (void);

WORD SpiGetWord (BYTE addr);

void SpiPutDword (BYTE addr, DWORD data);

DWORD SpiGetDword (BYTE addr);

QWORD SpiGetQword (BYTE addr);


#endif //__SPI_H__

