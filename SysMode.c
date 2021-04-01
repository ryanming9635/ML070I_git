/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  SysMode.c                                          											*/
/*                                                                           										*/
/*****************************************************************************/



#include <math.h>
#include <stdio.h>

#include "Config.h"

#include "reg.h"
#include "typedefs.h"
#include "main.h"
#include "i2c.h"
#include "adc.h"
#include "etc_eep.h"
#include "Printf.h"
#include "KeyRemo.h"
#include "Monitor.h"
#include "CPU.h"
#include "HS_DVRProtocol.h"


//****************************************************************************
// DEFINITIONS / MACROS
//****************************************************************************


//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************


//****************************************************************************
// CODE TABLES
//****************************************************************************


//****************************************************************************
// VARIABLE DECLARATIONS
//****************************************************************************
//StructModeInfoType idata g_stModeInfo = {0};
//****************************************************************************
// FUNCTION DECLARATIONS
//****************************************************************************
void SysModeHandler(void);
BYTE SysModeGetModeState(void);
bit SysModeDisplaySetting(void);
void SysModeSetUpInputCapture(void);
void SysModeSetUpDisplayCapture(void);
bit SysModeSetUpMDomain(void);
//void SysModeResetState(BYTE ucResetTarget);
void SysModeSetFreeRun(void);
bit SysModeGetModeStateChange(void);
void SysModeSetResetTarget(EnumModeAction enumResetTarget);
void SysModeDisplayTimingInitial(void);    
extern void HS_power_saving_proc(void);//ryan@20160110

//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************
//--------------------------------------------------
// Description  : Mode Reset 
// Input Value  : Reset Target
// Output Value : None
//--------------------------------------------------
//void SysModeResetState(BYTE ucResetTarget)
//{
//	ucResetTarget=1;
  
//}


