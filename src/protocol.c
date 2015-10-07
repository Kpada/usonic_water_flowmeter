#include "stdAfx.h"
#include "protocol.h"
#include "rpc.h"


/// there is a simple master-slave rpc protocol implementation
///

enum {
    // frame structure
    protocolHeader_SZE      = 4,    // header size (bytes)
    protocolMaxBuffLen      = 255,  // max buff size (bytes) = 0xFF
    protocolMaxDataBuff_SZE = protocolMaxBuffLen - protocolHeader_SZE, // max data buff size (bytes)
    
    // frame constants
    markerRequest           = 0x9A, // correct master->slave request marker
    markerResponse          = 0x9B, // correct slave->master response marker
   
    // misc constants
    defProtocolTmo          = 50,   // tmo, msec
    maxTries                = 3,    // tries
};


#pragma pack(push, 1)

// frame
#pragma anon_unions
struct {
    union { 
        // non parsed frame, like a buffer
        BYTE buff [protocolMaxBuffLen];
        // parsed frame, like a structure
        struct {
            struct {
                struct {
                    BYTE marker;    // marker
                    BYTE dataLen;   // data buffer len
                    WORD crc;       // checksum
                } header;
                // main data
                BYTE data [protocolMaxDataBuff_SZE]; // data buffer
            };
        } parsedFrame;
    };
    BYTE totalDataLen;
} frame;

#pragma pack(pop)


/// check uart and try to find the correct header structure
///
static BOOL findAndCheckTheHeader (void)
{
    BOOL isFound = FALSE;
    WORD tries = maxTries;
    
    while( !isFound && --tries ) {
        // try to get a header
        DWORD tmo = uartEstimateGet(protocolHeader_SZE) + defProtocolTmo;
        BYTE* buff = (BYTE*)&frame.buff[0];
        DWORD read;
        read = uartGetBytes((BYTE*)buff, protocolHeader_SZE, tmo);
        // check a size
        if( protocolHeader_SZE == read ) {
            // size is correct, try to find the marker
            BOOL hdrMarkerFound = FALSE;
            const BYTE* pos = buff;
            const BYTE* end = buff + protocolHeader_SZE;
            while( !hdrMarkerFound && pos < end ) {
                hdrMarkerFound = *pos == markerRequest;
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
    
    if( isFound ) {
        // check crc
        WORD crcCalculated = checksum16( (BYTE*)&frame.parsedFrame.header.marker, protocolHeader_SZE - 2 );
        if( crcCalculated == frame.parsedFrame.header.crc )
            return TRUE;      
    }
    
    return FALSE;
}


/// get a data buff
///
static BOOL additionalDataGet (void)
{
    // get data len
    DWORD dataLen = frame.parsedFrame.header.dataLen;
    // tmo
    DWORD tmo = uartEstimateGet(dataLen) + defProtocolTmo;
    // buff
    BYTE* buff = (BYTE*)&frame.parsedFrame.data[0];
 
    // get
    DWORD read = uartGetBytes((BYTE*)buff, dataLen, tmo);
    
    // return
    if( read == dataLen ) {
        // calc crc
        WORD crcCalculated = checksum16 (buff, dataLen - 2);
        // get crc
        WORD crcObtained = buff[dataLen-2] | ( buff[dataLen-1] << 8 );    
        // compare it
        return crcObtained == crcCalculated;
    }
    
    return FALSE;
}


BOOL putResponse (void) 
{
    WORD crc = 0;
    WORD totalBufferLen = 0;
    DWORD put = 0;
    
    // header
    frame.parsedFrame.header.marker = markerResponse;
    frame.parsedFrame.header.dataLen = frame.totalDataLen;
    frame.parsedFrame.header.crc = checksum16( (BYTE*)&frame.parsedFrame.header.marker, 2 );
    // data crc
    crc = checksum16( (BYTE*)&frame.parsedFrame.data[0], frame.totalDataLen - 2 );
    frame.parsedFrame.data[frame.totalDataLen - 2] = (BYTE)crc;
    frame.parsedFrame.data[frame.totalDataLen - 1] = (BYTE)( crc >> 8 );
    
    // put
    totalBufferLen = frame.totalDataLen + protocolHeader_SZE;
    put = uartPutBytes( (BYTE*)&frame.buff[0], totalBufferLen );
    
    return put == totalBufferLen;
}

static WORD getRpcId (void)
{
	// the first 2 bytes 
	return frame.parsedFrame.data[0] | frame.parsedFrame.data[1] << 8;
}

static BYTEARRAY getRpcInBuff (void)
{
	BYTEARRAY ba;
	ba.size = frame.parsedFrame.header.dataLen - 2 - 2;
	ba.data = frame.parsedFrame.data + 2;
	return ba;
}

static void prepareTheDataToBeSent (rpcState* state)
{
    const WORD lenId = 2;
    const WORD lenStatus = 2;
    // id
    memcpy(  (void*)&frame.parsedFrame.data[0], (void*)&state->id, lenId );
    // status
    frame.parsedFrame.data [2] = state->error ? 0x01 : 0x00;
    frame.parsedFrame.data [3] = 0x00; 
    // data
    memcpy( (void*)&frame.parsedFrame.data[4], state->dataOut, state->dataOutLen );
    
    frame.totalDataLen = state->dataOutLen + lenId + lenStatus + 2; // 2 is dataCrc
    
    // data allign
    if( frame.totalDataLen % 2 ) {
        frame.parsedFrame.data[ lenId + lenStatus + state->dataOutLen ] = 0; // empty byte
        frame.totalDataLen++;
    }
}

void protocolExecute (void)
{
    // try to get a header
    if( findAndCheckTheHeader() ) {
            // get data
            if( additionalDataGet() ) {
                // frame is correct
				// fill the state 
				rpcState state;
				state.error = FALSE;
				state.id = getRpcId();
				state.dataIn = getRpcInBuff();
				// execute
				rpcExecute(&state);
                // prepare an answer
                prepareTheDataToBeSent(&state);
                // put an answer
                putResponse();
            }
    }        
}

