#ifndef __DATA_PROCESSOR_H__
#define __DATA_PROCESSOR_H__

#include "tdc-gp22.h"

#define PUT_STATUS  0

typedef struct {
    DWORD           r1, r2, r3, r4;    // raw data results
    FLOAT           val1;
    FLOAT           val2;
    WORD            status;
    gp22Status      status_parsed;
} thermoPoint;

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
    thermoPoint     temp;
    tofPoint        tof;
    FLOAT           tofAvg;
    FLOAT           tempAvg [2];
} dataProcessorData;
#pragma pack(pop)

enum {
  dataProcessorData_SZE = sizeof (dataProcessorData),  
};

BOOL DpInit (void);

void DpProcess (void);

dataProcessorData DpGetCurDataPoint (void);


#endif //__DATA_PROCESSOR_H__

