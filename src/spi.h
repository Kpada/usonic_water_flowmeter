#ifndef __SPI_H__
#define __SPI_H__


/// spi initialization procedure
///
void SpiInit (void);

/// put a byte (u8)
///
void SpiPutByte (BYTE data);

/// get a byte (u8)
///
BYTE SpiGetByte (void);

/// get a word (u16)
///
WORD SpiGetWord (BYTE addr);

/// put a word (u16)
/// 
void SpiPutWord (BYTE addr, BYTE data);

/// put a dword (u32)
///
void SpiPutDword (BYTE addr, DWORD data);

/// get a dword (u32)
///
DWORD SpiGetDword (BYTE addr);

/// get a qword (u64)
///
QWORD SpiGetQword (BYTE addr);


#endif //__SPI_H__

