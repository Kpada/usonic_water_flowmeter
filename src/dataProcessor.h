#ifndef __DATA_PROCESSOR_H__
#define __DATA_PROCESSOR_H__


typedef struct {
    FLOAT           tofAvg;
    FLOAT           tof [2];
    FLOAT           resistanse [2];
} dataProcessorData;


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

