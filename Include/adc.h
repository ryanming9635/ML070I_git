#ifndef __ADC__
#define __ADC__

//=============================================================================
//                             ADC Functions
//=============================================================================

void ADCPowerDown(bit flag);
void SelectADCmux(BYTE sel);
void SetADCMode(BYTE mode);

void EnableADC(void);
void DisableADC(void);

WORD GetCoarse(void);
void SetCoarse(WORD i);
void SetPhase(BYTE j);
BYTE GetPhaseCurrent(void);
BYTE SetVCORange(DWORD _IPF);

//void SetADCGainOffset(BYTE gain, BYTE offset);
void AutoColorAdjust(void);



void AutoColorAdjustForDTV(BYTE flag);

#endif	// __ADC__


























