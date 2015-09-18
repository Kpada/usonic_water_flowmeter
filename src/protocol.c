#include "stdAfx.h"
#include "protocol.h"

#include "protocolHandlers.h"

// there is a simple modbus implementation

enum {
    frameSize = 17,
    addDataSize = 8,
    frameBasePartSize = frameSize - addDataSize, // header + lrc + ender
};

#pragma pack(push, 1)

// frame
#pragma anon_unions
struct {
    // frame header
    union {
        BYTE data [frameSize];
        struct {
            struct {
                struct {
                    BYTE marker;
                    WORD slaveAddr;
                    WORD func;
                }    header;
                // main data
                BYTE addData [addDataSize];
                // crc
                WORD crc;
                // frame ender
                WORD ender;
            };
        } parsed;
    };

} frame;

#pragma pack(pop)


enum {
    // frame constants
    correctProtocolMarker   = 0x3A,
    correctSlaveAddr        = 0x3130,   
    correctProtocolEnder    = 0x0A0D,
    
    // misc constants
    protocolHeader_SZE      = sizeof(frame.parsed.header),
    defProtocolTmo          = 50,
    maxTries                = 3,

};



static BOOL findAndCheckHeader (void)
{
    BOOL isFound = FALSE;
    WORD tries = maxTries;
    
    while( !isFound && --tries ) {
        // try to get a header
        DWORD tmo = uartEstimateGet(protocolHeader_SZE) + defProtocolTmo;
        BYTE* buff = (BYTE*)&frame.parsed.header;
        DWORD read;
        read = uartGetBytes((BYTE*)buff, protocolHeader_SZE, tmo);
        // check a size
        if( protocolHeader_SZE == read ) {
            // correct, find the marker
            BOOL hdrMarkerFound = FALSE;
            const BYTE* pos = buff;
            const BYTE* end = buff + protocolHeader_SZE;
            while( !hdrMarkerFound && pos < end ) {
                hdrMarkerFound = *pos == correctProtocolMarker;
                if( !hdrMarkerFound )
                    ++pos;
            }
            
            if( hdrMarkerFound ) { 
                // header marker found in the current protocolHeader_SZE chunk, 
                // check if we're not at the beginning position of the chunk, try to read additional bytes,
                // first moving all including frame marker to the beginning of the buffer
                if( pos != buff ) {
                    DWORD addRead;
                    read = end - pos;
                    memmove(buff, pos, read);
                    addRead = protocolHeader_SZE - read;
                    tmo = uartEstimateGet(addRead) + defProtocolTmo;
                    read = uartGetBytes(buff+read, addRead, tmo);
                    isFound = read == addRead;
                }
                else
                    isFound = TRUE;
            }       
        }
    }
    return isFound;
}

static void swap_ (BYTE* b0, BYTE* b1)
{
    BYTE b;
    
    b = *b1;
    *b1 = *b0;
    *b0 = b;
}

static BOOL additionalDataGet (void)
{
    DWORD dataLen = addDataSize + 2 + 2;
    DWORD tmo = uartEstimateGet(dataLen) + defProtocolTmo;
    BYTE* buff = (BYTE*)&frame.parsed.addData;
    DWORD read;
    read = uartGetBytes((BYTE*)buff, dataLen, tmo);
    
    swap_(&frame.data[13], &frame.data[14]);
    
    return read == dataLen;
}

static WORD calcLrc (const BYTE* buff, WORD len)
{
    WORD result = 0;
    BYTE tempByte;
    BYTE i;
    BYTE lrc = 0;
    
    for( i = 0; i < len/2; i++ ) {
        BYTE b0, b1;
        
		b0 = *buff++;
        b1 = *buff++;
        
        b0 = b0 <= '9' ? b0 - 0x30 : b0 - 0x31;
        b1 = b1 <= '9' ? b1 - 0x30 : b1 - 55;
        
        tempByte = b0 << 4 | b1;
		//parts[i] = tempByte;

		lrc += tempByte;
    }
    
    // 
	lrc = -lrc;
 
    // the high byte
    tempByte = lrc >> 4;
    if( tempByte < 10 )
        result |= tempByte + 0x30;
    else
        result |= tempByte + 55;   
    result <<= 8;

	// handle the low byte
    tempByte = lrc & 0x0F;
    if( tempByte < 10 )
        result |= tempByte + 0x30;
    else
        result |= tempByte + 55;    
      

    return result;
} 

static BOOL ckeckAddress (void)
{
    return correctSlaveAddr == frame.parsed.header.slaveAddr;
}

static BOOL checkLrc (void)
{
    WORD calculatedLrc = calcLrc(&frame.data[1], 12);
    
    return calculatedLrc == frame.parsed.crc;
}

static BOOL checkEnder (void)
{
    return frame.parsed.ender == correctProtocolEnder;
}

static BYTE byteToAscii (BYTE byte)
{
    if( byte < 10 )
        byte += 0x30;
    else
        byte += 55;

    return byte;
}


BYTE responseBuff [64];

// send the same header, responce, lrc and ender
static void sendResponse (protocolResponse response)
{
    
    WORD lrc = 0;
    WORD ender = correctProtocolEnder;
    WORD copyAddr = 0;
    if( response.buff && response.dataCnt ) {
        //BYTE* responseBuff = NULL;
       // responseBuff = (BYTE*) malloc(response.dataCnt + frameBasePartSize);
        // copy header
        memcpy(responseBuff + copyAddr, frame.data, protocolHeader_SZE);
        copyAddr += protocolHeader_SZE;
        // copy data size
        //memcpy(responseBuff + copyAddr, (BYTE*)&response.dataCnt, 2);
       // response.dataCnt /= 2;
        //response.dataCnt /= 2;
        responseBuff[copyAddr]     = (response.dataCnt >> 8 ) / 2 + 0x30;
        responseBuff[copyAddr + 1] = (response.dataCnt & 0x0F ) / 2 + 0x30;
        copyAddr += 2;
        //response.dataCnt *= 2;
        // copy data
        memcpy(responseBuff + copyAddr, response.buff, response.dataCnt );
        copyAddr += response.dataCnt;
        // calc lrc
        lrc = calcLrc(responseBuff + 1, 2 + response.dataCnt + protocolHeader_SZE - 1);
        responseBuff[copyAddr]     = (lrc >> 8);
        responseBuff[copyAddr + 1] = (lrc & 0xFF);
        copyAddr += 2;
        // ender
        memcpy(responseBuff + copyAddr, (BYTE*)&ender, 2);
        copyAddr += 2;
        
        uartPutBytes(responseBuff, copyAddr);
        
        //free(responseBuff);
    }
}

void protocolExecute (void)
{
    // try to get a header
    if( findAndCheckHeader() ) {
        // check a slave address
        if( ckeckAddress() ) {
            // get data
            if( additionalDataGet() ) {
                // check lrc
                if( checkLrc() && checkEnder() ) {
                    // frame is correct
                    WORD data;
                    protocolResponse response;
                    memcpy((BYTE*)&data, &frame.parsed.addData[7], 2);            
                    response = protocolHandler(frame.parsed.header.func, data);
                    // send response
                    sendResponse(response);
                }
            }
        }
    }        
}

