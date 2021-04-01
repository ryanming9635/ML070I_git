/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  SysPower.c                                          											*/
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
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************


//****************************************************************************
// CODE TABLES
//****************************************************************************


//****************************************************************************
// VARIABLE DECLARATIONS
//****************************************************************************
StructPowerInfoType idata g_stPowerInfo = {0};
extern StructBatteryInfoType g_stBatteryInfo;
extern StructDVRInfoType g_stDVRInfo;
extern BYTE PowerFlag;
extern BYTE STAT1_Flag;
extern BYTE STAT2_Flag;


//****************************************************************************
// FUNCTION DECLARATIONS
//****************************************************************************
void SysPowerInitial(void);
void SysPowerHandler(void);
void SysPowerSwitch(EnumPowerAction enumSwitch);
void UserInterfacePowerSwitch(EnumPowerAction enumSwitch);
void UserInterfaceBatteryChargeMode(EnumBatteryStatus enumSwitch);

extern void MCUTimerCancelTimerEvent(BYTE ucEventID);
extern bit MCUTimerCheckEventValid(BYTE ucEventIndex);

//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************
//--------------------------------------------------
// Description  : Initial Source Switch Flags
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void SysPowerInitial(void)
{  
	CLR_PCON3V3_P();
	CLR_Panel_EN();
	CLR_PCON5V_P();
	CLR_PCON5V();
	CLR_Panel_EN();
	CLR_PWCTRL();
	CLR_BL_PWM();
	CLR_AC_MODE();
	CLR_BAT_SYS();
	CLR_PCON_CAM();
	CLR_PCON3V3_TW();
	SET_TW8836_RST();	 
	CLR_GREEN();
	CLR_RED();	
	
	MCUTimerDelayXms(200);//delay 200ms

	DEBUG_MESSAGE("\n\rSysPowerInitial..OK!!");		

}

//--------------------------------------------------
// Description  : Power Handler Process
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void SysPowerHandler(void)
{
    // The process will deal with all kinds of power changing by power action flag.  
    switch(GET_TARGET_POWER_STATUS())
    {
        case _POWER_STATUS_NORMAL:

            switch(GET_POWER_STATUS())
            {
                case _POWER_STATUS_OFF:
                    SysPowerSwitch(_POWER_ACTION_OFF_TO_NORMAL);
                    break;

                case _POWER_STATUS_SAVING:
                    SysPowerSwitch(_POWER_ACTION_PS_TO_NORMAL);
                    break;

                case _POWER_STATUS_AC_ON:
                    SysPowerSwitch(_POWER_ACTION_AC_ON_TO_NORMAL);
                    break;

                case _POWER_STATUS_NOSUPPORT_SAVING:
                    break;
                case _POWER_STATUS_NORMAL:		 
                    break;					
                default:
                    break;
            }            
            
            SET_POWER_STATUS(_POWER_STATUS_NORMAL);
            
            break;
            
        case _POWER_STATUS_OFF:

            switch(GET_POWER_STATUS())
            {
                case _POWER_STATUS_SAVING:
                    SysPowerSwitch(_POWER_ACTION_PS_TO_OFF);
                    break;

                case _POWER_STATUS_AC_ON:
                    SysPowerSwitch(_POWER_ACTION_AC_ON_TO_OFF);
                    break;


                case _POWER_STATUS_NORMAL:
                    SysPowerSwitch(_POWER_ACTION_NORMAL_TO_OFF);
                    break;

                default:
                    break;
            }

            SET_POWER_STATUS(_POWER_STATUS_OFF);
            
            break;

        case _POWER_STATUS_SAVING:

            switch(GET_POWER_STATUS())
            {
                case _POWER_STATUS_NORMAL:
                  SysPowerSwitch(_POWER_ACTION_NORMAL_TO_PS);
                    break;

                default:
                    break;                   
            }
                        
            SET_POWER_STATUS(_POWER_STATUS_SAVING);

            break;
        default:
            break;
    }

    // Clear power action to avoid repeat calls in next circle.
    SET_TARGET_POWER_STATUS(GET_POWER_STATUS());

}

void UserInterfaceBatteryChargeMode(EnumBatteryStatus enumSwitch)
{

	MCUTimerCancelTimerEvent(_SYSTEM_TIMER_EVENT_GRN_BLINK);
	MCUTimerCancelTimerEvent(_SYSTEM_TIMER_EVENT_GRN_2S_BLINK);
	MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_RED_BLINK);
	MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_GRN_RED_BLINK);
	MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_GRN_ON_RED_BLINK);
	MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_GRN_2S_BLINK_RED_ON);

	switch(enumSwitch)
	{
	case  _BATT_STATUS_STOP_CHARGE: 
				SET_PWM(_CHG_CURR,_CHARGE000mA);	
			#if (_DEBUG_MESSAGE_BatteryVoltage==ON)
					GraphicsPrint(MAGENTA,"(1_Stop Charge)");		
			#endif

			SET_BATTERY_CHARGE_STATE(_BATT_STATUS_STOP_CHARGE);

		if((GET_DVR_SystemReadyNotic()==_FALSE)&&(PowerFlag==ON))
			MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_BLINK);
		else if(GET_POWER_STATUS()==_POWER_STATUS_SAVING)
			{
				if(GET_AC_PLUG()==_TRUE)
				MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_2S_BLINK);//Update LED Status		
				else
					{
						if(PowerFlag==OFF)
						MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_RED_OFF);//Update LED Status								
						else if(GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_LEVEL0 )
						MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_RED_BLINK);//Update LED Status		
						else
						MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_2S_BLINK);//Update LED Status		
					}
			}
		else	 if(GET_POWER_STATUS()==_POWER_STATUS_NORMAL)
			{
				if(GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_LEVEL0)
				MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_ON_RED_BLINK);//Update LED Status			
				 else
				 MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_ON_RED_OFF);//Update LED Status
			}
		else 					 
			MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_RED_OFF);//Update LED Status

					break;		
		case  _BATT_STATUS_LOW_CHARGE:
					SET_PWM(_CHG_CURR,_CHARGE300mA); 
			#if (_DEBUG_MESSAGE_BatteryVoltage==ON)
					GraphicsPrint(MAGENTA,"(Low Current Charge)");	
			#endif
					SET_BATTERY_CHARGE_STATE(_BATT_STATUS_LOW_CHARGE);	
			
			if((GET_DVR_SystemReadyNotic()==_FALSE)&&(PowerFlag==ON))
				MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_BLINK);
			else if(GET_POWER_STATUS()==_POWER_STATUS_SAVING)
				{	if(GET_AC_PLUG()==_TRUE)
					MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_2S_BLINK_RED_ON);//Update LED Status		
					else
					MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_BLINK);//Update LED Status		
					
				}
			else
				{
					if((PowerFlag==OFF)&&(STAT1_Flag==OFF)&&(STAT2_Flag==ON))
					MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_OFF_RED_ON);//Update LED Status					
					else if((STAT1_Flag==OFF)&&(STAT2_Flag==ON))
					MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_RED_ON);//Update LED Status
					else
					MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_ON_RED_OFF);//Update LED Status
						
				}
					break;
		case  _BATT_STATUS_HIGH_CHARGE:
					SET_PWM(_CHG_CURR,_CHARGE1000mA);	
			#if (_DEBUG_MESSAGE_BatteryVoltage==ON)
					GraphicsPrint(MAGENTA,"(High Current Charge)"); 
			#endif
		
			SET_BATTERY_CHARGE_STATE(_BATT_STATUS_HIGH_CHARGE);

			
			if((GET_DVR_SystemReadyNotic()==_FALSE)&&(PowerFlag==ON))
				MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_BLINK);
			else if(GET_POWER_STATUS()==_POWER_STATUS_SAVING)
				{	if(GET_AC_PLUG()==_TRUE)
					MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_RED_BLINK);//Update LED Status		
					else
					MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_BLINK);//Update LED Status			
				}
			else
				{
					if((STAT1_Flag==OFF)&&(STAT2_Flag==ON))
					MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_OFF_RED_ON);//Update LED Status
					else
					MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_RED_OFF);//Update LED Status
								
				}
					break;
		case  _BATT_STATUS_NORMAL_CHARGE:
					SET_PWM(_CHG_CURR,_CHARGE700mA); 
			#if (_DEBUG_MESSAGE_BatteryVoltage==ON)
					GraphicsPrint(MAGENTA,"(Normal Current Charge)");	
			#endif
					SET_BATTERY_CHARGE_STATE(_BATT_STATUS_NORMAL_CHARGE);	
			
			if((GET_DVR_SystemReadyNotic()==_FALSE)&&(PowerFlag==ON))
				MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_BLINK);
			else if(GET_POWER_STATUS()==_POWER_STATUS_SAVING)
				{	
					if(GET_AC_PLUG()==_TRUE)
					MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_2S_BLINK_RED_ON);//Update LED Status		
					else
					MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_BLINK);//Update LED Status		
					
				}
			else
				{
					if((PowerFlag==OFF)&&(STAT1_Flag==OFF)&&(STAT2_Flag==ON))
					MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_OFF_RED_ON);//Update LED Status					
					else if((STAT1_Flag==OFF)&&(STAT2_Flag==ON))
					MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_RED_ON);//Update LED Status
					else
					MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_ON_RED_OFF);//Update LED Status
						
				}
					break;					
		case	_BATT_STATUS_NO_BATT:
					SET_PWM(_CHG_CURR,_CHARGE060mA);
			#if (_DEBUG_MESSAGE_BatteryVoltage==ON)
					GraphicsPrint(MAGENTA,"(1_NO BATT)"); 			
			#endif
					SET_BATTERY_CHARGE_STATE(_BATT_STATUS_NO_BATT);
			
			if((GET_DVR_SystemReadyNotic()==_FALSE)&&(PowerFlag==ON))
				MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_BLINK);
			else  if(GET_POWER_STATUS()==_POWER_STATUS_NORMAL)
				 MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_ON_RED_OFF);//Update LED Status
			else if(GET_POWER_STATUS()==_POWER_STATUS_SAVING)	 
				MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_2S_BLINK);//Update LED Status
			else	
				{
				MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_RED_OFF);//Update LED Status
				}
			break;
                 case _BATT_STATUS_DVR_OFF:				 	
				#if (_DEBUG_MESSAGE_PowerTimerEvent==ON)
				  GraphicsPrint(MAGENTA,"(DVR_POWER_OFF)"); 
				#endif

					if(GET_DVR_RebootAndPower()==_TRUE)
					UserInterfacePowerSwitch(_POWER_ACTION_NORMAL_TO_OFF);
					else
					CLR_PCON5V();  

				MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_RED_BLINK);//Update LED Status
						 break;

                 case _BATT_STATUS_DVR_ON:
						#if (_DEBUG_MESSAGE_PowerTimerEvent==ON)
						 GraphicsPrint(MAGENTA,"(DVR_POWER_ON)"); 
						#endif

							if(GET_DVR_RebootAndPower()==_TRUE)
							{
							UserInterfacePowerSwitch(_POWER_ACTION_OFF_TO_NORMAL);
							CLR_DVR_RebootAndPower();
							}
							else
							 SET_PCON5V();	

						if(GET_DVR_SystemReadyNotic()==_TRUE)
						MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_ON_RED_OFF);//Update LED Status
						else
						MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_RED_BLINK);//Update LED Status

						break;		
	}

}

void UserInterfacePowerSwitch(EnumPowerAction enumSwitch)
{
	Printf("\r\n(PowerSwitch=%02x)",(WORD)enumSwitch);	

	
    switch(enumSwitch)
    {
        case _POWER_ACTION_AC_ON_TO_NORMAL:
	case _POWER_ACTION_PS_TO_NORMAL:
	case _POWER_ACTION_OFF_TO_NORMAL:
#if 1			
	
			SET_PCON5V();		
			SET_PWCTRL();
			SET_AC_MODE();
			SET_BAT_SYS();
			SET_PCON_CAM();
			SET_PCON3V3_TW();
			MCUTimerDelayXms(20);//delay 20ms Power-On Sequence of Panel		
			CLR_TW8836_RST();
			
 	MCUTimerActiveTimerEvent(SEC(0.5), _SYSTEM_TIMER_EVENT_JUDGE_FIRST_GET_BATT_BTH_STATE);
	MCUTimerActiveTimerEvent(SEC(/*0.5*/1), _USER_TIMER_EVENT_PANEL_BACKLIGHT_ON);	

#else
			SET_PCON3V3_P();
			SET_Panel_EN();
			SET_PCON5V_P();
			SET_PCON5V();		
			SET_PWCTRL();
			SET_BL_PWM();
			SET_AC_MODE();
			SET_BAT_SYS();
			SET_PCON_CAM();
			SET_PCON3V3_TW();
			MCUTimerDelayXms(64);//delay 64ms Power-On Sequence and Reset
			CLR_TW8836_RST();	 
#endif
            break;
		case	_POWER_ACTION_PANEL_POWER_ON:
			SET_PCON3V3_P();
			SET_Panel_EN();
			MCUTimerDelayXms(64);//delay 64ms Power-On Sequence and Reset
			SET_PCON5V_P();
			SET_BL_PWM();

			if(GET_DVR_EntrySleepMode()==_TRUE)
			{
			CLR_DVR_EntrySleepMode();
			MCU_SendCmdToDVR(MCU_PROTOCOL_CMD_SLEEP_WAKE_UP);
			}

			P3M1=0x00|0x30;//p3.4 and P3.5 set push pull mode ryan@20210223
			break;

        case _POWER_ACTION_AC_ON_TO_OFF:
	case _POWER_ACTION_NORMAL_TO_OFF:
	case _POWER_ACTION_PS_TO_OFF:

		SET_TW8836_RST();	
		CLR_PCON3V3_P();
		CLR_PCON5V_P();
		CLR_Panel_EN();
		CLR_PCON5V();
		CLR_Panel_EN();
		CLR_PWCTRL();
		CLR_AC_MODE();
		CLR_BAT_SYS();
		CLR_PCON_CAM();
		//#if (_DEBUG_MESSAGE_Monitor==ON)
		//#else
		//CLR_PCON3V3_TW(); ///關閉debug message UART1 有問題
		//#endif
		CLR_BL_PWM();

		#if 0// (_DEBUG_MESSAGE_Monitor==ON)
		#else
		CLR_PCON3V3_TW(); ///關閉debug message UART1 有問題
		#endif

		SET_DVR_PowerOFFDelay();
		MCUTimerActiveTimerEvent( SEC(3),_SYSTEM_TIMER_EVENT_POWER_OFF_ON_DELAY);
            break;

        case _POWER_ACTION_NORMAL_TO_PS:

			//CLR_PCON3V3_P();
			//CLR_PCON5V_P();
			CLR_Panel_EN();
			CLR_PCON5V_P();
			CLR_PWCTRL();
			CLR_AC_MODE();
			//CLR_BAT_SYS();
			CLR_PCON_CAM();
			CLR_BL_PWM();
			//CLR_PCON5V();
			//SET_TW8836_RST();	
			//CLR_PCON3V3_TW();
			
			P3M1=0x00;//p3.4 and P3.5 set output ryan@20210226
            break;

        default:

            break;
    }

		
SET_BATTERY_CHARGE_STATE(_BATT_STATUS_NONE);//reset battery state.

}

//--------------------------------------------------
// Description  : Deal With Power Manage According To Input Pamater
// Input Value  : ucSwitch    --> Power action description.
// Output Value : None
//--------------------------------------------------
void SysPowerSwitch(EnumPowerAction enumSwitch)
{


    switch(enumSwitch)
    {
        case _POWER_ACTION_NORMAL_TO_PS:
   		UserInterfacePowerSwitch(enumSwitch);	
            break;

        case _POWER_ACTION_NORMAL_TO_OFF:
        case _POWER_ACTION_AC_ON_TO_OFF:
	case _POWER_ACTION_DVR_POWER_OFF:	
		  case _POWER_ACTION_PS_TO_OFF:
		UserInterfacePowerSwitch(enumSwitch);	
	break;
            
//       case _POWER_ACTION_PS_TO_OFF:
//            break;

        case _POWER_ACTION_AC_ON_TO_NORMAL:
        case _POWER_ACTION_OFF_TO_NORMAL:
        case _POWER_ACTION_PS_TO_NORMAL:    
         case _POWER_ACTION_PS_TO_BATT:	
	case _POWER_ACTION_DVR_POWER_ON:
	case	_POWER_ACTION_PANEL_POWER_ON:
		// User Power Process
                UserInterfacePowerSwitch(enumSwitch);	 	     
          break;
		  
        default:
            break;
    }
	
}



