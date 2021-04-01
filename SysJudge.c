/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  SysJudge.c		                                            										*/
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

//--------------------------------------------------
// Power status
//--------------------------------------------------
//#define GET_POWER_STATUS()                 		(g_stPowerInfo.b4PowerStatus)
//#define SET_POWER_STATUS(x)                		(g_stPowerInfo.b4PowerStatus = x)

//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************


//****************************************************************************
// CODE TABLES
//****************************************************************************


//****************************************************************************
// VARIABLE DECLARATIONS
//****************************************************************************
extern StructModeInfoType idata g_stModeInfo;
extern StructPowerInfoType idata g_stPowerInfo;
extern StructBatteryInfoType g_stBatteryInfo;


//****************************************************************************
// FUNCTION DECLARATIONS
//****************************************************************************
void SysJudgeHandler(void);
bit SysJudgeCheckStatus(void);
void SysJudgeStateChangeEvent(EnumModeStatus enumModeState);
//void SysJudgeModeResetProc(void);
WORD SysJudgeGetActiveWaitTime(void);
extern void MCUTimerCancelTimerEvent(BYTE ucEventID);
//extern void SysModeResetState(BYTE ucResetTarget);
extern WORD GetBatteryVoltage(void);

//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************
//--------------------------------------------------
// Description  : Judge Handler
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void SysJudgeHandler(void)
{

#if 1
   //  CLR_MODE_STATE_CHANGE();

    switch(GET_POWER_STATUS())
    {
        case _POWER_STATUS_OFF:

            if(GET_MODE_STATE() != _MODE_STATUS_POWER_OFF)
            {
                SysJudgeStateChangeEvent(_MODE_STATUS_POWER_OFF);
            }
            
            break;
            
        case _POWER_STATUS_SAVING:
//        case _POWER_STATUS_NOSUPPORT_SAVING:

            if(GET_MODE_STATE() != _MODE_STATUS_POWER_SAVING)
            {
                SysJudgeStateChangeEvent(_MODE_STATUS_POWER_SAVING);
            }
            
            break;
            
        case _POWER_STATUS_NORMAL:                    
            
            switch(GET_MODE_STATE())
            {
                case _MODE_STATUS_POWER_OFF:
                    SysJudgeStateChangeEvent(_MODE_STATUS_INITIAL);
                    
                    break;        
                    
                case _MODE_STATUS_INITIAL:

				SysJudgeStateChangeEvent(_MODE_STATUS_SEARCH);
				#if(_DEBUG_MESSAGE_SysJudgeHandler==ON)
				GraphicsPrint(GREEN,"\r\nSysJudgeHandler=_MODE_STATUS_INITIAL");
				#endif
                    break;
                    
                case _MODE_STATUS_SEARCH:

	
                    break;
                    
               // case _MODE_STATUS_DISPLAY_SETTING:

                  //  break;
                    
              //  case _MODE_STATUS_ACTIVE:
             			
                   // break;
                    
              //  case _MODE_STATUS_NOSUPPORT:
     
         //           break;
                    
              //  case _MODE_STATUS_NOSIGNAL:
                case _MODE_STATUS_POWER_SAVING:

                    break;
                
                //case _MODE_STATUS_FACTORY:

                 //   break;
                    
                default:

                    break;
            }    
            
            break;
            
        default:
            break;
    }
#endif
    // Handle Status Check and Manual Reset Requests
    //SysJudgeModeResetProc();    
}


//--------------------------------------------------
// Description  : Check Status
// Input Value  : None.
// Output Value : _FALSE => Status Changed
//--------------------------------------------------
/*
bit SysJudgeCheckStatus(void)
{



    return _TRUE;
}
*/
//--------------------------------------------------
// Description  : Switch to Active State
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void SysJudgeStateChangeEvent(EnumModeStatus enumModeState)
{
    SET_MODE_STATE(enumModeState);
   SET_MODE_STATE_CHANGE();
}
#if 0
void SysJudgeModeResetProc(void)
{        
    switch(GET_MODE_RESET_TARGET())
    {
        case _MODE_ACTION_RESET_TO_SEARCH:   

            if(GET_POWER_STATUS() != _POWER_STATUS_OFF)
            {
          //      SysModeResetState(_MODE_ACTION_RESET_TO_SEARCH);    

                SysJudgeStateChangeEvent(_MODE_STATUS_SEARCH);
            }
        
            break;

        case _MODE_ACTION_RESET_TO_DISPLAY_SETTING:            

            if(GET_MODE_STATE() == _MODE_STATUS_ACTIVE)
            {
               // SysModeResetState(_MODE_ACTION_RESET_TO_DISPLAY_SETTING);

                SysJudgeStateChangeEvent(_MODE_STATUS_DISPLAY_SETTING);
            }
            
            break;


        case _MODE_ACTION_RESET_TO_NOSIGNAL:

            if(GET_POWER_STATUS() != _POWER_STATUS_OFF)
            {                   

                SysJudgeStateChangeEvent(_MODE_STATUS_NOSIGNAL);
							
            }
            
            break;
            
        case _MODE_ACTION_RESET_TO_NOSUPPORT:

            if(GET_POWER_STATUS() != _POWER_STATUS_OFF)
            {
            //    SysModeResetState(_MODE_ACTION_RESET_TO_NOSUPPORT);    
                
                  

                SysJudgeStateChangeEvent(_MODE_STATUS_NOSUPPORT);
            }
            
            break;
            
        case _MODE_ACTION_RESET_TO_FACTORY_MODE:

            SysJudgeStateChangeEvent(_MODE_STATUS_FACTORY);
   
            break;

        default:
            break;            
    }
    
    SET_MODE_RESET_TARGET(_MODE_ACTION_NO_RESET_MODE);     
    /*
    if(SysJudgeCheckStatus() == _FALSE)
    {
        SysModeResetState(_MODE_ACTION_RESET_TO_SEARCH);    

        SysJudgeStateChangeEvent(_MODE_STATUS_SEARCH);
    }    
*/
}
#endif

