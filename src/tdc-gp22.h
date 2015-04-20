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

BOOL Gp22Init (void);
BOOL Gp22Tof (float* time0, float* time1, WORD* stat0, WORD* stat1, BOOL* isTimeout, DWORD* raw0, DWORD* raw1);
FLOAT Gp22RawValueToTimeConvert (DWORD val);
gp22Status Gp22ParseStatus (WORD stat);

#endif //__TDC_GP_22_H__

