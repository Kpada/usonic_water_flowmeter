#ifndef _tdc_gp22_config_h
#define _tdc_gp22_config_h

#define TDC_QUARTZ_FREQ_KHZ         8000  // 8 MHz

//#define FIRE_PULSES_FREQ_KHZ        1000  // 1 MHz

// settings
enum {
    // clock
    tdcMainDiv          = 2,    // 1, 2, 4
    // tdc waits after swithing on osc before making a msr 
    tdcStartCLKHS       = 3,    // 0 - osc off, 1 - osc cont on, 2 - 480us, 3 - 1.46ms
    
    // clock calibration
    tdcAnzPerCalRes     = 2,    // 2, 4, 8 or 16 periods of 32KHz resonator
    
    // fire
    tdcFirePulsesNum    = 5 ,   // 0 .. 127
    tdcFirePulsesDiv    = 4,    // 2 .. 16
    
    // stop
    tdcStopPulsesChnl1  = 4 ,   // 0 .. 4
    tdcStopPulsesChnl2  = 0,    // 0 .. 4 , should be 0
    
    
    // tof
    tdcDelayBeforeStop1Nsec = 40000,
    tdcEnFirstWave      = 1,    // 1 - en, 0 - dis
    tdcAutocalcMode2En  = 1,    // 1 - en, 0 - dis
    tdcDelrel1          = 3,    // 3 .. 63
    tdcDelrel2          = 4,    // 4 .. 63 && tdcDelrel2 > tdcDelrel1
    tdcDelrel3          = 5,    // 5 .. 63 && tdcDelrel3 > tdcDelrel2
    tdcAluErrorVal      = 1,    // 1 - en, 0 - dis  - tmo => fill result with 0xffffffff 
    tdcTmoPreddiv       = 3,    // 0, 1, 2, 3  ( 4MHz => 64us, 256us, 1024us, 4096us )
    tdcOffsetValMv      = 15,    // -36 .. +35 mV
    tdcFirstWaveEdge    = 0,    // 0 - rising, 1 - falling
    
    // irq sourse
    tdcIrqTimeout       = 1,    // 1 - en, 0 - dis
    tdcIrqEndHits       = 0,    // 1 - en, 0 - dis
    tdcIrqALU           = 1,    // 1 - en, 0 - dis
    tdcIrqEndEeprom     = 0,    // 1 - en, 0 - dis
    
    
};

#endif // _tdc_gp22_config_h
