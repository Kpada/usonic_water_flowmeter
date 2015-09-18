#ifndef __TDC_GP_22_H__
#define __TDC_GP_22_H__

typedef struct {
    BYTE aluPtr;
    BYTE hitsChnl1;
    BYTE hitsChnl2;
    BOOL timeoutTdc;
    BOOL timeoutPrecounter;
    BOOL errorOpen;
    BOOL errorShort;
    BOOL errorEeprom;
    BOOL errorDed;
    BOOL eepromEqCreg;
} gp22Status;

// init procedure
BOOL Gp22Init (void);

// msr the differnce between 2 signals
BOOL Gp22Tof (float* time0, float* time1, WORD* stat0, WORD* stat1, BOOL* isTimeout, DWORD* raw0, DWORD* raw1);

// msr resistance values 
BOOL gp22GetTemp (WORD* stat, DWORD* r1, DWORD* r2, DWORD* r3, DWORD* r4);

// convert the status to the user-friendly structure
gp22Status Gp22ParseStatus (WORD stat);

// 
double Gp22GetClkCorrectionFactor (void);

#endif //__TDC_GP_22_H__

