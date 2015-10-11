#ifndef __DATA_PROCESSOR_H__
#define __DATA_PROCESSOR_H__

#include "tdc-gp22.h"

typedef struct {
    DWORD           raw0, raw1;
    FLOAT           time0, time1; 
    FLOAT           timeDiff;
    WORD            status0, status1;
    gp22Status      status_parsed0, status_parsed1;
    BOOL            tmoError;
} tofPoint;

#pragma pack(push,1)
typedef struct {
    tofPoint        tof;
    FLOAT           tofAvg;
    FLOAT           tempAvg [2];
} dataProcessorData;
#pragma pack(pop)

enum {
  dataProcessorData_SZE = sizeof (dataProcessorData),  
};

/// data processor initialzation proc
///
BOOL DpInit (void);

dataProcessorData DpGetCurDataPoint (void);

/// data processor start 
///
void DpStart (void);

/// data processor stop
///
void DpStop (void);

#endif //__DATA_PROCESSOR_H__

