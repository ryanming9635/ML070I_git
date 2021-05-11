/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  SysTimer.c                                          											*/
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
#define GET_EVENT_VALID(x)                      (g_stTimerEvent[x].b1EventValid)
#define SET_EVENT_VALID(x)                      (g_stTimerEvent[x].b1EventValid = _TRUE)
#define CLR_EVENT_VALID(x)                      (g_stTimerEvent[x].b1EventValid = _FALSE)

#define GET_EVENT_ID(x)                         (g_stTimerEvent[x].b7EventID)
#define SET_EVENT_ID(x, y)                      (g_stTimerEvent[x].b7EventID = y)

#define GET_EVENT_TIME(x)                       (g_stTimerEvent[x].usTime)
#define SET_EVENT_TIME(x, y)                    (g_stTimerEvent[x].usTime = y)


//****************************************************************************
// VARIABLE DECLARATIONS
//****************************************************************************
bit g_bNotifyTimer0Int = _FALSE;
volatile WORD  g_usTimerCounter = 0;
StructTimerEventTableType g_stTimerEvent[_TIMER_EVENT_COUNT];
WORD BatteryBTH=549/*156*//*650*//*880*/,BatteryVoltage=880;
BYTE WDT_Echo_Count=0;
BYTE STAT1_temp=OFF, STAT2_temp=ON,STAT1_Flag=OFF,STAT2_Flag=ON;

BYTE BattDetect1_COUNT=0, BattDetect2_COUNT=0, BattDetect3_COUNT=0, BattDetect4_COUNT=0,BattDetect5_COUNT=0;
BYTE BattDetectMin_COUNT=0, BattDetectHigh_COUNT=0,BattDetectHighStop_COUNT=0,BattDetectNoStartUp_COUNT=0,BattDetectStartUp_COUNT=0;
BYTE  BATERY_STAT=0xFF,BATERY_STAT_TEMP=0xFE;
BYTE TEMP_NORMAL_COUNT=0,TEMP_LOW_COUNT=0, TEMP_LOW_WARN_COUNT=0, TEMP_HIGH_COUNT=0,TEMP_HIGH_WARN_COUNT=0;
BYTE TEMP_STAT=0xFF;
BYTE PWM_TEMP=0;	
BYTE CHARGE_TEMP_ABNORMAL=0;
BYTE CHARGE_TEMP_NORMAL=0;
//BYTE _BATT_STATUS_CAPACITY_MAX_STOP_Count=0;

#if (_DEBUG_MESSAGE_Monitor==ON)
bit Monitor_flag=OFF;
#endif

extern BYTE PowerFlag;
extern BYTE CameraVolt;
extern BYTE	PWR_START_flag;
extern StructBatteryInfoType g_stBatteryInfo;
extern BYTE EncorderCountPN;
extern BYTE EncorderLenint;
extern long EncorderCount;
extern DWORD EncorderLenfloaat;
extern StructPowerInfoType idata g_stPowerInfo;
extern BYTE day,hour,minute,second;
extern StructDVRInfoType g_stDVRInfo;
extern BYTE Power_down_mode;
extern DWORD ulongRotateNumber;
extern DWORD ulongRotateNumberTELI;

extern BYTE bytFastEncoderMode;
extern long EncorderCountINT;
#if (_BATTERY_CHARGE_STOP==ON)
extern BYTE bytBatteryStopCharge;
extern BYTE bytBatteryStopChargeCount;
#endif
extern WORD bytEncorderCountTemp;
extern BYTE   bytFastEncorderCountTemp;

//--------------------------------------------------
// Definitions of Timer2
//--------------------------------------------------
#define _EVENT_TIME_MAX                         61000	// Please do not modify this value.

//****************************************************************************
// CODE TABLES
//****************************************************************************


//****************************************************************************
// FUNCTION DECLARATIONS
//****************************************************************************
void SysTimerHandler(void);
void SysTimerEventProc(EnumSystemTimerEventID enumEventID);
void MCUTimerInitialTimerEvent(void);
void MCUTimerActiveTimerEvent(WORD usTime, BYTE ucEventID);
void MCUTimerCancelTimerEvent(BYTE ucEventID);
void MCUTimerDelayXms(WORD usNum);
extern WORD GetBatteryBTH(void);
extern WORD GetBatteryVoltage(void);
extern void SetAD5110Step(BYTE newv) ;
extern void UserInterfacePowerSwitch(EnumPowerAction enumSwitch);
extern void SysPowerSwitch(EnumPowerAction enumSwitch);
extern void UserInterfaceBatteryChargeMode(EnumBatteryStatus enumSwitch);
extern void SET_MCU_POWER_DOWN_MODE(void);


//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************
//--------------------------------------------------
// Description  : Timer Delay
// Input Value  : usNum --> Delay in ms
// Output Value : None
//--------------------------------------------------
void MCUTimerDelayXms(WORD usNum)
{
    if(usNum)
    {
        g_bNotifyTimer0Int = _FALSE;

        while(_TRUE)
        {
            if(g_bNotifyTimer0Int)
            {
                g_bNotifyTimer0Int = _FALSE;

                if(--usNum)
                {
                }
                else
                {
                    return;
                }
            }
        }
    }
}

//--------------------------------------------------
// Description  : Get Event Time
// Input Value  : ucEventIndex --> Event which time is needed
// Output Value : Event Time
//--------------------------------------------------
WORD MCUTimerGetEventTime(BYTE ucEventIndex)
{
    return GET_EVENT_TIME(ucEventIndex);
}

//--------------------------------------------------
// Description  : Get Event ID
// Input Value  : Referenced Event Index
// Output Value : Stored Event ID
//--------------------------------------------------
BYTE MCUTimerGetEventID(BYTE ucEventIndex)
{
    return GET_EVENT_ID(ucEventIndex);
}

//--------------------------------------------------
// Description  : Clear Event Time
// Input Value  : ucEventIndex --> Event which time is needed to be cleared
// Output Value : None
//--------------------------------------------------


void MCUTimerClrEventValid(BYTE ucEventIndex)
{
    CLR_EVENT_VALID(ucEventIndex);
}

//--------------------------------------------------
// Description  : Check if Event is Valid
// Input Value  : ucEventIndex --> Event to be checked
// Output Value : Validity(_TRUE/_FALSE)
//--------------------------------------------------
bit MCUTimerCheckEventValid(BYTE ucEventIndex)
{
    return GET_EVENT_VALID(ucEventIndex);;
}

//--------------------------------------------------
// Description  : Get Timer Counter.
// Input Value  : None
// Output Value : Timer Counter
//--------------------------------------------------
WORD MCUTimerGetTimerCounter(void)
{
    WORD usPresentTime = g_usTimerCounter;
   
    // When get counter,timer interrupt maybe change counter.
    // Get twice to avoid it.
    if(abs(g_usTimerCounter - usPresentTime) > 0x7F)
    {
        usPresentTime = g_usTimerCounter;
    } 

    return usPresentTime;
}
//--------------------------------------------------
// Description  : Check timer and events. We have to run this function when setting up a timer for an event.
// Input Value  : usTime    --> Unit in 1ms, range in 0.001 ~ 61sec
// Output Value : Return usPresentTime
//--------------------------------------------------
WORD MCUTimerCheckTimerEvent(WORD usTime)
{
    BYTE ucTimerEventCnt = 0;
    WORD usPresentTime = 0;

    usPresentTime = MCUTimerGetTimerCounter();
    
    if(usTime > _EVENT_TIME_MAX) 
    {
        usTime = _EVENT_TIME_MAX;
    }
   
    if(((usPresentTime + usTime) > _EVENT_TIME_MAX) || // Size Limit of Timer Counter             
       ((usPresentTime + usTime) < usPresentTime)) // Timer Counter Overflow
    { 
        g_usTimerCounter = 0;
    
        // Reset Timer Event Counter
        for(ucTimerEventCnt = 0; ucTimerEventCnt < _TIMER_EVENT_COUNT; ucTimerEventCnt++)
        {
            if(GET_EVENT_VALID(ucTimerEventCnt) == _TRUE)
            {
                if(GET_EVENT_TIME(ucTimerEventCnt) > usPresentTime)
                {
                    SET_EVENT_TIME(ucTimerEventCnt, (GET_EVENT_TIME(ucTimerEventCnt) - usPresentTime));
                }
                else
                {
                    SET_EVENT_TIME(ucTimerEventCnt, 0);
                }
            }
        }
        
        usPresentTime = 0;
    }    

    return usPresentTime;
}

//--------------------------------------------------
// Description  : Initial timer and events. We have to run this function at firmware startup
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void MCUTimerInitialTimerEvent(void)
{
    BYTE ucTimerEventCnt = 0;

    for(ucTimerEventCnt = 0; ucTimerEventCnt < _TIMER_EVENT_COUNT; ucTimerEventCnt++)
    {
        CLR_EVENT_VALID(ucTimerEventCnt);
    }

   // TR2 = 1;
}

//--------------------------------------------------
// Description  : Active Timer Event
// Input Value  : usTime --> Time to active Timer Event
//				  ucEventID --> Event to be actived
// Output Value : None
//--------------------------------------------------
void MCUTimerActiveTimerEvent(WORD usTime, BYTE ucEventID)
{
    BYTE ucEventIndex = 0;

    // Search for Existing Event with the same Event ID
    for(ucEventIndex = 0; ucEventIndex < _TIMER_EVENT_COUNT; ucEventIndex++)
    {
        if(GET_EVENT_VALID(ucEventIndex) == _TRUE)
        {
            if(GET_EVENT_ID(ucEventIndex) == ucEventID)
            {
                return;
            }
        }
    }

    // Choose any Vacant Storage for Event
    for(ucEventIndex = 0; ucEventIndex < _TIMER_EVENT_COUNT; ucEventIndex++)
    {
        if(GET_EVENT_VALID(ucEventIndex) == _FALSE)
        {
            SET_EVENT_VALID(ucEventIndex);
            SET_EVENT_ID(ucEventIndex, ucEventID);
            SET_EVENT_TIME(ucEventIndex, (WORD)((DWORD)(MCUTimerCheckTimerEvent(usTime)) + usTime));
            return;
        }
    }

    //Printf("No More Available Storage for Simultaneous Event!!!!!!");
}

//--------------------------------------------------
// Description  : Cancel an event
// Input Value  : Event     --> Event which we want to cancel
// Output Value : None
//--------------------------------------------------
void MCUTimerCancelTimerEvent(BYTE ucEventID)
{
    BYTE ucEventIndex = 0;
    
    // Search for Existing Event with the same Event ID
    for(ucEventIndex = 0; ucEventIndex < _TIMER_EVENT_COUNT; ucEventIndex++)
    {
        if(GET_EVENT_VALID(ucEventIndex) == _TRUE)
        {
            if(GET_EVENT_ID(ucEventIndex) == ucEventID)
            {
                CLR_EVENT_VALID(ucEventIndex);
                return;
            }
        }
    }      
}

//--------------------------------------------------
// Description  : Reactive a timer for an event. If the event is exist,
//                this function will reset the executing time and restart.
// Input Value  : usTime    --> Unit in 1ms, range in 0.01 ~ 61 sec
//                Event     --> Execute Event while timeup
// Output Value : None
//--------------------------------------------------
void MCUTimerReactiveTimerEvent(WORD usTime, BYTE ucEventID)
{
    BYTE ucEventIndex = 0;

    // Search for Existing Event with the same Event ID
    for(ucEventIndex = 0; ucEventIndex < _TIMER_EVENT_COUNT; ucEventIndex++)
    {
        if(GET_EVENT_VALID(ucEventIndex) == _TRUE)
        {
            if(GET_EVENT_ID(ucEventIndex) == ucEventID)
            {
                SET_EVENT_TIME(ucEventIndex, (WORD)((DWORD)(MCUTimerCheckTimerEvent(usTime)) + usTime));
                return;
            }
        }
    }    

    // Choose any Vacant Storage for Event
    for(ucEventIndex = 0; ucEventIndex < _TIMER_EVENT_COUNT; ucEventIndex++)
    {
        if(GET_EVENT_VALID(ucEventIndex) == _FALSE)
        {
            SET_EVENT_VALID(ucEventIndex);
            SET_EVENT_ID(ucEventIndex, ucEventID);
            SET_EVENT_TIME(ucEventIndex, (WORD)((DWORD)(MCUTimerCheckTimerEvent(usTime)) + usTime));
            return;
        }
    }   
}


void UserInterfaceTimerEventProc(BYTE ucEventID)
{

switch(ucEventID)
{

	case _USER_TIMER_EVENT_OSD_CAMERA_VOLTAGE_INC:
			SetAD5110Step(CameraVolt);
			WriteEEP(EEP_CameraVolt, CameraVolt);  
			#if(_DEBUG_MESSAGE_UserInterfaceTimerEvent==ON)
			GraphicsPrint(YELLOW,"(CAM_INC %d)",(WORD)(CameraVolt));	
			#endif
   			break;

	case _USER_TIMER_EVENT_OSD_CAMERA_VOLTAGE_DEC:
			SetAD5110Step(CameraVolt);	
			WriteEEP(EEP_CameraVolt, CameraVolt);  
			#if(_DEBUG_MESSAGE_UserInterfaceTimerEvent==ON)
			GraphicsPrint(YELLOW,"(CAM_DEC %d)",(WORD)(CameraVolt));	
			#endif
			break;
        case _USER_TIMER_EVENT_OSD_SET_CAMERA_VOLTAGE:
			SetAD5110Step(CameraVolt);	
			WriteEEP(EEP_CameraVolt, CameraVolt);  		
			#if(_DEBUG_MESSAGE_UserInterfaceTimerEvent==ON)
			GraphicsPrint(MAGENTA,"(CAM_VOLT_LEVEL=%d)",(WORD)(CameraVolt));	
			#endif
			break;
        case _USER_TIMER_EVENT_OSD_GET_MCU_VERSION:
			MCU_SendCmdToDVR(MCU_PROTOCOL_CMD_REPLY_MCU_FW);	
			#if(_DEBUG_MESSAGE_UserInterfaceTimerEvent==ON)
			GraphicsPrint(YELLOW,"(GET_MCU_VERSION)");	
			#endif
			break;
	case _USER_TIMER_EVENT_REPLY_ENCODER_COUNT:
			MCU_SendCmdToDVR(MCU_PROTOCOL_CMD_REPLY_ENCODER_COUNT);	
			#if(_DEBUG_MESSAGE_UserInterfaceTimerEvent==ON)
			GraphicsPrint(YELLOW,"(REPLY_ENCODER_COUNT)");	
			#endif
			break;
	case _USER_TIMER_EVENT_RESET_ENCODER_COUNT:
			#if(_DEBUG_MESSAGE_UserInterfaceTimerEvent==ON)
			GraphicsPrint(YELLOW,"(RESET_ENCODER_COUNT)");	
			#endif
			if(bytFastEncoderMode==ON)
				{
			WriteEEP(EEP_RotateNumberL,0);
			WriteEEP(EEP_RotateNumberM,0);
			WriteEEP(EEP_RotateNumberH,0);
			ulongRotateNumberTELI=0;
				}
			else
				{
				WriteEEP(EEP_RotateNumberRL,0);
				WriteEEP(EEP_RotateNumberRM,0);
				WriteEEP(EEP_RotateNumberRH,0);
				ulongRotateNumber=0;
				}
			break;
	case _USER_TIMER_EVENT_OSD_DVR_FACTORY_MODE:

				if(GET_AC_PLUG()==_TRUE)
				{
				MCU_SendCmdToDVR(MCU_PROTOCOL_CMD_ENTRY_FACTORY);			
				#if(_DEBUG_MESSAGE_UserInterfaceTimerEvent==ON)
				GraphicsPrint(RED,"(DVR_FACTORY_MODE)");	
				#endif
				}
			#if(_DEBUG_MESSAGE_UserInterfaceTimerEvent==ON)
			else
				{
				GraphicsPrint(RED,"(DVR_FACTORY_FAIL)");			
				}
			#endif
			
			break;

       case _USER_TIMER_EVENT_OSD_MCU_ENTRY_SLEEP_MODE:
		
				if(GET_DVR_EntrySleepMode()==_FALSE)
				{
				SET_TARGET_POWER_STATUS(_POWER_STATUS_SAVING);
				SET_DVR_EntrySleepMode();
				#if(_DEBUG_MESSAGE_UserInterfaceTimerEvent==ON)
			   	GraphicsPrint(YELLOW,"(MCU_NORMAL_TO_SLEEP_MODE)");    
				#endif				
				}
	   		break;

	case _USER_TIMER_EVENT_OSD_DISTANCE_RESET:
		EncorderCount=0;
		EncorderCountINT=0;
		EncorderLenint=0;
		EncorderLenfloaat=0;
		EncorderCountPN=0;
		bytEncorderCountTemp=0;
		bytFastEncorderCountTemp=0;
		#if(_DEBUG_MESSAGE_UserInterfaceTimerEvent==ON)
		GraphicsPrint(YELLOW,"\r\n ClearEncorder ",0);
		#endif
			 //Printf("(DISTANCE_RESET)");	 
			 break;

	 case _USER_TIMER_EVENT_OSD_DVR_REBOOT:
		 	#if(_DEBUG_MESSAGE_UserInterfaceTimerEvent==ON)
			 GraphicsPrint(YELLOW,"(DVR_REBOOT)");  
			 #endif
				if(GET_DVR_Reboot()==_TRUE) 
					{
					CLR_DVR_Reboot();
					CLR_DVR_SystemReadyNotic();
					 UserInterfaceBatteryChargeMode(_BATT_STATUS_DVR_OFF);
					MCUTimerActiveTimerEvent(SEC(3), _USER_TIMER_EVENT_OSD_DVR_REBOOT);

					P3M1=0x00;//p3.4 and P3.5 set output ryan@20210226
					}
				else
				{
					 UserInterfaceBatteryChargeMode(_BATT_STATUS_DVR_ON);
				}
			  break;

	 case _USER_TIMER_EVENT_OSD_DVR_SHUTDOWN:
			#if(_DEBUG_MESSAGE_UserInterfaceTimerEvent==ON)
			 GraphicsPrint(YELLOW,"(DVR_SHUTDOWN)");  
			#endif
			SET_DVR_Shutdown();
			MCU_SendCmdToDVR(MCU_PROTOCOL_CMD_SYSTEM_SHUTDOWN);
			 MCUTimerActiveTimerEvent(SEC(15), _USER_TIMER_EVENT_OSD_DVR_SHUTDOWN);
			 break;

      case	_USER_TIMER_EVENT_OSD_DVR_ENTER_ISP_MODE:
		#if(_DEBUG_MESSAGE_UserInterfaceTimerEvent==ON)
		   GraphicsPrint(YELLOW,"(MCU_ENTER_ISP_MODE)");			
		#endif
	  	  enter_isp();
		   break;

	case _USER_TIMER_EVENT_OSD_ENCODER_DEVIATION_SETTING:			
			#if(_DEBUG_MESSAGE_UserInterfaceTimerEvent==ON)
			GraphicsPrint(YELLOW,"(ENCODER_SET)");  
			#endif
			break;

	case _USER_TIMER_EVENT_PANEL_BACKLIGHT_ON:
		SET_PWCTRL();
		#if(_DEBUG_MESSAGE_UserInterfaceTimerEvent==ON)
		GraphicsPrint(YELLOW,"(Panel_ON)");	
		#endif
		   UserInterfacePowerSwitch(_POWER_ACTION_PANEL_POWER_ON);	 	
		break;

	case _USER_TIMER_EVENT_OSD_DVR_STOP_REBOOT:
		#if(_DEBUG_MESSAGE_UserInterfaceTimerEvent==ON)
		GraphicsPrint(YELLOW,"(DVR STOP REBOOT)");	
		#endif
		break;

	#if (_DEBUG_MESSAGE_Monitor==ON)
	case _USER_TIMER_EVENT_Monitor_ON:
	 Monitor_flag=_FALSE;
		break;
	#endif

	 default: 	break;
	 

}
	
}

//--------------------------------------------------
// Description  : Timer Handler
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void SysTimerHandler(void)
{
    BYTE ucTimerEventIndex = 0;
    BYTE ucTimerEventID = 0;    
    WORD usPresentTime = 0;

    for(ucTimerEventIndex = 0; ucTimerEventIndex < _TIMER_EVENT_COUNT; ucTimerEventIndex++)
    {
        usPresentTime = MCUTimerGetTimerCounter();

        if(MCUTimerCheckEventValid(ucTimerEventIndex) == _FALSE)
        {
            continue;
        }         
        
        if(usPresentTime > MCUTimerGetEventTime(ucTimerEventIndex))
        {
            MCUTimerClrEventValid(ucTimerEventIndex);

            // Get Event ID From Event Index
            ucTimerEventID = MCUTimerGetEventID(ucTimerEventIndex);
            
            if(ucTimerEventID < _USER_TIMER_EVENT_END)
            {
                UserInterfaceTimerEventProc(ucTimerEventID);
                
                continue;
            }

            if(ucTimerEventID < _SYSTEM_TIMER_EVENT_END)
            {
                SysTimerEventProc(ucTimerEventID);
                
                continue;
            }
            
         //   MCUTimerEventProc(ucTimerEventID);
        }
    }

}

//--------------------------------------------------
// Description  : Timer Event Process
// Input Value  : enumEventID --> Event to be processed
// Output Value : None
//--------------------------------------------------
void SysTimerEventProc(EnumSystemTimerEventID enumEventID)
{

switch(enumEventID)
   { 
   case _SYSTEM_TIMER_EVENT_GRN_RED_OFF:
	   PowerLED(_SYSTEM_TIMER_EVENT_GRN_RED_OFF);
	break;

   case _SYSTEM_TIMER_EVENT_GRN_ON_RED_OFF:
	 PowerLED(_SYSTEM_TIMER_EVENT_GRN_ON_RED_OFF);
	break;

   case _SYSTEM_TIMER_EVENT_GRN_OFF_RED_ON:	
   	PowerLED(_SYSTEM_TIMER_EVENT_GRN_OFF_RED_ON);
	break;

   case _SYSTEM_TIMER_EVENT_GRN_RED_ON:
   		PowerLED(_SYSTEM_TIMER_EVENT_GRN_RED_ON);
   	break;

   case _SYSTEM_TIMER_EVENT_GRN_BLINK:
	   PowerLED(_SYSTEM_TIMER_EVENT_GRN_BLINK);
     MCUTimerActiveTimerEvent(SEC(0.25), _SYSTEM_TIMER_EVENT_GRN_BLINK);
    break;

   case _SYSTEM_TIMER_EVENT_GRN_2S_BLINK:
     PowerLED(_SYSTEM_TIMER_EVENT_GRN_BLINK);
	   MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_GRN_2S_BLINK);
   	break;

   case _SYSTEM_TIMER_EVENT_RED_BLINK:
	   PowerLED(_SYSTEM_TIMER_EVENT_RED_BLINK);
     MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_RED_BLINK);

   	break;	

  case _SYSTEM_TIMER_EVENT_GRN_RED_BLINK:
  	 PowerLED(_SYSTEM_TIMER_EVENT_GRN_RED_BLINK);
     MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_GRN_RED_BLINK);

	 break; 

   case _SYSTEM_TIMER_EVENT_GRN_ON_RED_BLINK:
	  PowerLED(_SYSTEM_TIMER_EVENT_GRN_ON_RED_BLINK);
	  MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_GRN_ON_RED_BLINK);
	  break; 
		break;

   case _SYSTEM_TIMER_EVENT_GRN_2S_BLINK_RED_ON:
	   PowerLED(_SYSTEM_TIMER_EVENT_GRN_2S_BLINK_RED_ON);
	   MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_GRN_2S_BLINK_RED_ON);
   	  	break;
 case _SYSTEM_TIMER_EVENT_BATT_LOW_RED_BLINK:
 	PowerLED(_SYSTEM_TIMER_EVENT_GRN_RED_OFF);
	PowerLED(_SYSTEM_TIMER_EVENT_GRN_OFF_RED_ON);
	MCUTimerDelayXms(500);
 	PowerLED(_SYSTEM_TIMER_EVENT_GRN_RED_OFF);
	MCUTimerDelayXms(500);

	PowerLED(_SYSTEM_TIMER_EVENT_GRN_OFF_RED_ON);
	MCUTimerDelayXms(500);
 	PowerLED(_SYSTEM_TIMER_EVENT_GRN_RED_OFF);
	MCUTimerDelayXms(500);

	PowerLED(_SYSTEM_TIMER_EVENT_GRN_OFF_RED_ON);
	MCUTimerDelayXms(500);
 	PowerLED(_SYSTEM_TIMER_EVENT_GRN_RED_OFF);
	MCUTimerDelayXms(500);

	SET_BATTERY_CHARGE_STATE(_BATT_STATUS_NONE);//reset battery state.
 	break;
	
   case _SYSTEM_TIMER_EVENT_JUDGE_CHECK_AC_CABLE:
	
		if(Check_ADAP_IN()==_TRUE)		
		{
			
						if(GET_AC_PLUG()==_FALSE)
						{	
		
						if(MCUTimerCheckEventValid(_SYSTEM_TIMER_EVENT_JUDGE_BATT_MODE)==_TRUE)
							{
								#if(_DEBUG_MESSAGE_SysTimerEvent==ON)
								GraphicsPrint(BLUE,"(AC1)");
								#endif	

							MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_JUDGE_BATT_MODE);  
							MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_JUDGE_BATT_MODE); 
							}
		
						if(MCUTimerCheckEventValid(_SYSTEM_TIMER_EVENT_JUDGE_AC_MODE)==_TRUE)
							{
							MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_JUDGE_AC_MODE);  
							MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_JUDGE_AC_MODE); 
							#if(_DEBUG_MESSAGE_SysTimerEvent==ON)
							GraphicsPrint(BLUE,"(AC2)");
							#endif	
							}
		
						}
			
		}
		else
			{
				if(GET_AC_PLUG()==_TRUE)
				{
							if(MCUTimerCheckEventValid(_SYSTEM_TIMER_EVENT_JUDGE_BATT_MODE)==_TRUE)
							{
							MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_JUDGE_BATT_MODE);  
							MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_JUDGE_BATT_MODE); 
							#if(_DEBUG_MESSAGE_SysTimerEvent==ON)
							GraphicsPrint(BLUE,"(AC3)");
							#endif
							}
		
						if(MCUTimerCheckEventValid(_SYSTEM_TIMER_EVENT_JUDGE_AC_MODE)==_TRUE)
							{
							#if(_DEBUG_MESSAGE_SysTimerEvent==ON)
							GraphicsPrint(BLUE,"(AC4)");
							#endif	
							MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_JUDGE_AC_MODE);  
							MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_JUDGE_AC_MODE); 
							}
				}
			}
					
			MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_JUDGE_CHECK_AC_CABLE);		
			break;
 case _SYSTEM_TIMER_EVENT_REPLAY_REGULAR_DATA:

	 	if(GET_DVR_SystemReadyNotic()==_TRUE)					 
		 MCU_SendCmdToDVR(MCU_PROTOCOL_CMD_REGULAR_DATA);
		
		 MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_REPLAY_REGULAR_DATA);
		 
	break;
	case _SYSTEM_TIMER_EVENT_POWER_OFF_ON_DELAY:
				#if(_DEBUG_MESSAGE_SysTimerEvent==ON)
				GraphicsPrint(RED,"(CLR_DVR_PowerOFFDelay)");
				#endif
			CLR_DVR_PowerOFFDelay();
		break;
		case _SYSTEM_TIMER_EVENT_JUDGE_FIRST_GET_BATT_BTH_STATE:
			#if (_DEBUG_MESSAGE_BATTERY_TEST)
			#else
			BatteryVoltage=GetBatteryVoltage();
//			BatteryBTH=GetBatteryBTH()+7;
			BatteryBTH=GetBatteryBTH();
			#endif

			if(BatteryVoltage<=BATTERY_CAPACITY_MIN) //<11.6V
				BATERY_STAT=_BATT_STATUS_CAPACITY_MIN;
				else if((BatteryVoltage>BATTERY_CAPACITY_MIN)&&(BatteryVoltage<=BATTERY_CAPACITY_NO_STARTUP))
					BATERY_STAT=_BATT_STATUS_CAPACITY_NO_STARTUP;				
				else if((BatteryVoltage>BATTERY_CAPACITY_NO_STARTUP)&&(BatteryVoltage<=BATTERY_CAPACITY_LEVEL0))
					BATERY_STAT=_BATT_STATUS_CAPACITY_LEVEL0;
				else if((BatteryVoltage>BATTERY_CAPACITY_LEVEL0)&&(BatteryVoltage<=BATTERY_CAPACITY_LEVEL1))
					BATERY_STAT=_BATT_STATUS_CAPACITY_LEVEL1;
				else if((BatteryVoltage>BATTERY_CAPACITY_LEVEL1)&&(BatteryVoltage<=BATTERY_CAPACITY_LEVEL2))
					BATERY_STAT=_BATT_STATUS_CAPACITY_LEVEL2;
				else if((BatteryVoltage>BATTERY_CAPACITY_LEVEL2)&&(BatteryVoltage<=BATTERY_CAPACITY_LEVEL3))
					BATERY_STAT=_BATT_STATUS_CAPACITY_LEVEL3;
				else if((BatteryVoltage>BATTERY_CAPACITY_LEVEL3)&&(BatteryVoltage<=BATTERY_CAPACITY_HIGH))
					BATERY_STAT=_BATT_STATUS_CAPACITY_LEVEL4;
				else if((BatteryVoltage>BATTERY_CAPACITY_HIGH)&&(BatteryVoltage<=BATTERY_CAPACITY_HIGH_STOP)&&(PowerFlag==ON))
					BATERY_STAT=_BATT_STATUS_CAPACITY_MAX;
				else if((BatteryVoltage>BATTERY_CAPACITY_HIGH)&&(BatteryVoltage<=(BATTERY_CAPACITY_HIGH_STOP+9))&&(PowerFlag==OFF))
					BATERY_STAT=_BATT_STATUS_CAPACITY_MAX;
				else 					
					BATERY_STAT=_BATT_STATUS_CAPACITY_MAX_STOP;

			SET_BATTERY_STATE(BATERY_STAT);
		
			SET_BATTERY_CAPACITY(BatteryVoltage);


			if((BatteryBTH<=BTH_TEMP_HIGH)&&(BatteryBTH>BTH_TEMP_MIN)) //High temperature.
				TEMP_STAT=_BATT_STATUS_TEMP_HIGH;
				else if((BatteryBTH>BTH_TEMP_HIGH)&&(BatteryBTH<=BTH_TEMP_HIGH_WARN))//50~55 warnning
					TEMP_STAT=_BATT_STATUS_TEMP_HIGH_WARN;
				else if((BatteryBTH>BTH_TEMP_HIGH_WARN)&&(BatteryBTH<=BTH_TEMP_LOW_WARN))
					TEMP_STAT=_BATT_STATUS_TEMP_NORMAL;
				else if((BatteryBTH>BTH_TEMP_LOW_WARN)&&(BatteryBTH<=BTH_TEMP_LOW))  //0~-5 warnning
					TEMP_STAT=_BATT_STATUS_TEMP_LOW_WARN;
				else if((BatteryBTH>BTH_TEMP_LOW)&&(BatteryBTH<=BATTERY_ACD_no_battery)) /// if(BatteryBTH>BTH_TEMP_LOW)  //-5~-10 warnning
					TEMP_STAT=_BATT_STATUS_TEMP_LOW;
				else{
					SET_NO_BATTERY();
					SET_BATTERY_STATE(_BATT_STATUS_NO_BATT);
					TEMP_STAT=_BATT_STATUS_TEMP_NORMAL; ///no DC12V input
					}

				if(GET_BATTERY_CHARGE_STATE()==_BATT_STATUS_NO_BATT)	
				TEMP_STAT=_BATT_STATUS_TEMP_NORMAL;

				SET_BTH_STATE(TEMP_STAT);	

				STAT1_temp=GET_STAT1();	
				if(STAT1_Flag!=STAT1_temp)
				STAT1_Flag=STAT1_temp;
							
				STAT2_temp=GET_STAT2();	
				if(STAT2_Flag!=STAT2_temp)
				STAT2_Flag=STAT2_temp;				

			/*
			if((BatteryBTH<=CHARGE_TEMP_HIGH)&&(BatteryBTH>CHARGE_TEMP_LOW))  
				{
				if(GET_AC_PLUG()==_TRUE)	
					SET_CHARGE_TMEP_ABNORMAL();

				if(GET_BATTERY_CHARGE_STATE()==_BATT_STATUS_NO_BATT)	
							CLR_CHARGE_TMEP_ABNORMAL();
				}
			*/
			break;
    case _SYSTEM_TIMER_EVENT_JUDGE_BATT_STATE:	

		#if (_DEBUG_MESSAGE_BATTERY_TEST)
		#else
			BatteryVoltage=GetBatteryVoltage();
		#endif
		
			#if 1
		SET_BATTERY_CAPACITY(BatteryVoltage);
			
//   if((GET_BATTERY_CAPACITY()<200)||(BatteryBTH>=935)) //no battery  <3.3V
//	   if(((GET_BATTERY_CAPACITY()<BATTERY_ACD_MIN)||(STAT1_Flag==1))&&(GET_AC_PLUG()==_TRUE)) //no battery  <3.3V
/*	   if((GET_BATTERY_CAPACITY()<BATTERY_ACD_MIN)&&(GET_AC_PLUG()==_TRUE)) //no battery  <3.3V
		   	{
			SET_NO_BATTERY();
			SET_BATTERY_STATE(_BATT_STATUS_NO_BATT); 
		   	}
		else 
		*/	
		{
			if(GET_NO_BATTERY()==_TRUE)
				{
				BattDetect1_COUNT=0;
				BattDetect2_COUNT=0;
				BattDetect3_COUNT=0;
				BattDetect4_COUNT=0;
				BattDetect5_COUNT=0;
				BattDetectMin_COUNT=0;
				BattDetectHigh_COUNT=0;
				BattDetectHighStop_COUNT=0;
				BattDetectNoStartUp_COUNT=0;
				BATERY_STAT=_BATT_STATUS_CAPACITY_LEVEL4;
				}
				#if (_DEBUG_MESSAGE_Power_Supply_TEST==ON)
				CLR_NO_BATTERY();	
				#endif
			if(BatteryVoltage<=BATTERY_CAPACITY_MIN) //<11.6V
			{
			    	BattDetect1_COUNT=0;
			    	BattDetect2_COUNT=0;
				BattDetect3_COUNT=0;
				BattDetect4_COUNT=0;
				BattDetect5_COUNT=0;							
				BattDetectMin_COUNT++;
				BattDetectHigh_COUNT=0;
				BattDetectHighStop_COUNT=0;
				BattDetectNoStartUp_COUNT=0;
			}
			else if((BatteryVoltage>BATTERY_CAPACITY_MIN)&&(BatteryVoltage<=BATTERY_CAPACITY_NO_STARTUP))
			{
			//_BATT_STATUS_CAPACITY_NO_STARTUP
			    	BattDetect1_COUNT=0;
			    	BattDetect2_COUNT=0;
				BattDetect3_COUNT=0;
				BattDetect4_COUNT=0;
				BattDetect5_COUNT=0;							
				BattDetectMin_COUNT=0;
				BattDetectHigh_COUNT=0;
				BattDetectHighStop_COUNT=0;								
				BattDetectNoStartUp_COUNT++;
			}	
			else if((BatteryVoltage>BATTERY_CAPACITY_NO_STARTUP)&&(BatteryVoltage<=BATTERY_CAPACITY_LEVEL0))
			{
			//_BATT_STATUS_CAPACITY_LEVEL0
			    	BattDetect1_COUNT++;
			    	BattDetect2_COUNT=0;
				BattDetect3_COUNT=0;
				BattDetect4_COUNT=0;
				BattDetect5_COUNT=0;							
				BattDetectMin_COUNT=0;
				BattDetectHigh_COUNT=0;
				BattDetectHighStop_COUNT=0;								
				BattDetectNoStartUp_COUNT=0;
			}			
			else if((BatteryVoltage>BATTERY_CAPACITY_LEVEL0)&&(BatteryVoltage<=BATTERY_CAPACITY_LEVEL1))
			{
			  // _BATT_STATUS_CAPACITY_LEVEL1
			  	BattDetect1_COUNT=0;
			       	BattDetect2_COUNT++;
				BattDetect3_COUNT=0;
				BattDetect4_COUNT=0;
				BattDetect5_COUNT=0;
				BattDetectMin_COUNT=0;
				BattDetectHigh_COUNT=0;
				BattDetectNoStartUp_COUNT=0;
				
			}
			else if((BatteryVoltage>BATTERY_CAPACITY_LEVEL1)&&(BatteryVoltage<=BATTERY_CAPACITY_LEVEL2))
			{
			//_BATT_STATUS_CAPACITY_LEVEL2
			  	BattDetect1_COUNT=0;
			        BattDetect2_COUNT=0;
				BattDetect3_COUNT++;
				BattDetect4_COUNT=0;
				BattDetect5_COUNT=0;
				BattDetectMin_COUNT=0;
				BattDetectHigh_COUNT=0;
				BattDetectHighStop_COUNT=0;
				BattDetectNoStartUp_COUNT=0;
				
			}
			else if((BatteryVoltage>BATTERY_CAPACITY_LEVEL2)&&(BatteryVoltage<=BATTERY_CAPACITY_LEVEL3))
			{
   			//_BATT_STATUS_CAPACITY_LEVEL3
			  	BattDetect1_COUNT=0;
			    	BattDetect2_COUNT=0;
				BattDetect3_COUNT=0;
				BattDetect4_COUNT++;
				BattDetect5_COUNT=0;
				BattDetectMin_COUNT=0;
				BattDetectHigh_COUNT=0;
				BattDetectHighStop_COUNT=0;
				BattDetectNoStartUp_COUNT=0;
				
			}
			else if((BatteryVoltage>BATTERY_CAPACITY_LEVEL4)&&(BatteryVoltage<=BATTERY_CAPACITY_HIGH))
			{
   			//_BATT_STATUS_CAPACITY_LEVEL4
			  	BattDetect1_COUNT=0;
				BattDetect2_COUNT=0;
				BattDetect3_COUNT=0;
				BattDetect4_COUNT=0;
				BattDetect5_COUNT++;
				BattDetectMin_COUNT=0;
				BattDetectHigh_COUNT=0;
				BattDetectHighStop_COUNT=0;
				BattDetectNoStartUp_COUNT=0;
				
		
			}
			else if((BatteryVoltage>BATTERY_CAPACITY_HIGH)&&(BatteryVoltage<=BATTERY_CAPACITY_HIGH_STOP)&&(PowerFlag==ON))			
			{
				BattDetect1_COUNT=0;
				BattDetect2_COUNT=0;
				BattDetect3_COUNT=0;
				BattDetect4_COUNT=0;
				BattDetect5_COUNT=0;
				BattDetectMin_COUNT=0;
				BattDetectHigh_COUNT++;	
				BattDetectHighStop_COUNT=0;
				BattDetectNoStartUp_COUNT=0;
				
			}
			else if((BatteryVoltage>BATTERY_CAPACITY_HIGH)&&(BatteryVoltage<=(BATTERY_CAPACITY_HIGH_STOP+9))&&(PowerFlag==OFF))			
			{
				BattDetect1_COUNT=0;
				BattDetect2_COUNT=0;
				BattDetect3_COUNT=0;
				BattDetect4_COUNT=0;
				BattDetect5_COUNT=0;
				BattDetectMin_COUNT=0;
				BattDetectHigh_COUNT++;	
				BattDetectHighStop_COUNT=0;
				BattDetectNoStartUp_COUNT=0;
				
			}
			else///>BATTERY_CAPACITY_HIGH_STOP
			{
				BattDetect1_COUNT=0;
				BattDetect2_COUNT=0;
				BattDetect3_COUNT=0;
				BattDetect4_COUNT=0;
				BattDetect5_COUNT=0;
				BattDetectMin_COUNT=0;
				BattDetectHigh_COUNT=0; 
				BattDetectHighStop_COUNT++;
				BattDetectNoStartUp_COUNT=0;
			}

			if(BatteryVoltage>=BATTERY_CAPACITY_STARTUP)
			BattDetectStartUp_COUNT++;
			else		
			BattDetectStartUp_COUNT=0;
			
			if(BattDetect1_COUNT==20)
			{
				BATERY_STAT=_BATT_STATUS_CAPACITY_LEVEL0;
				BattDetect1_COUNT=0;
			}
			if(BattDetect2_COUNT==20)
			{
				BATERY_STAT=_BATT_STATUS_CAPACITY_LEVEL1;
				BattDetect2_COUNT=0;
			}
			if(BattDetect3_COUNT==20)
			{
				BATERY_STAT=_BATT_STATUS_CAPACITY_LEVEL2;
				BattDetect3_COUNT=0;
			}
			if(BattDetect4_COUNT==20)
			{
				BATERY_STAT=_BATT_STATUS_CAPACITY_LEVEL3;
				BattDetect4_COUNT=0;
			}
			if(BattDetect5_COUNT==20)
			{
				BATERY_STAT=_BATT_STATUS_CAPACITY_LEVEL4;
				BattDetect5_COUNT=0;
			}

			if(BattDetectMin_COUNT==20)
			{
			BATERY_STAT=_BATT_STATUS_CAPACITY_MIN;
			BattDetectMin_COUNT=0;
			}
			
			if(BattDetectHigh_COUNT==20)
			{
			BATERY_STAT=_BATT_STATUS_CAPACITY_MAX;
			BattDetectHigh_COUNT=0;
			}

			if(BattDetectNoStartUp_COUNT==20)
			{
			BATERY_STAT=_BATT_STATUS_CAPACITY_NO_STARTUP;
			BattDetectNoStartUp_COUNT=0;
			}

					if(BattDetectHigh_COUNT==20)
					{
					BATERY_STAT=_BATT_STATUS_CAPACITY_MAX;
					BattDetectHigh_COUNT=0;
					}

				if(BattDetectHighStop_COUNT==20)
				{
				BATERY_STAT=_BATT_STATUS_CAPACITY_MAX_STOP;
				BattDetectHighStop_COUNT=0;
				}

				if(BATERY_STAT_TEMP!=BATERY_STAT)
				{
				BATERY_STAT_TEMP=BATERY_STAT;
				SET_BATTERY_CHARGE_STATE(_BATT_STATUS_NONE);//reset battery state. 
				}
#if (_BATTERY_CHARGE_STOP==ON)
			if((bytBatteryStopCharge==_TRUE)&&((BATERY_STAT<=_BATT_STATUS_CAPACITY_LEVEL0)||(BATERY_STAT==_BATT_STATUS_CAPACITY_MIN)||\
				(BATERY_STAT==_BATT_STATUS_CAPACITY_NO_STARTUP)))
			{
				bytBatteryStopCharge=_FALSE;
			
				if(ReadEEP(EEP_BatteryStopCharge)==ON)
				WriteEEP(EEP_BatteryStopCharge,OFF);
									
				#if(_DEBUG_MESSAGE_Battery_Charge_Debug==ON)
				GraphicsPrint(RED,"(bytBatteryStopCharge=0)");
				#endif
						
				SET_BATTERY_CHARGE_STATE(_BATT_STATUS_NONE);
			}
#endif
			if(BattDetectStartUp_COUNT==20)
			{	
				if(GET_BATTERY_CAPACITY_LOW_FLAG()==_TRUE)
					{
					#if(_DEBUG_MESSAGE_SysTimerEvent==ON)
					 GraphicsPrint(RED,"(Normal Voltage Charge >12.5V)");
					#endif
					CLR_BATTERY_CAPACITY_LOW_FLAG();
					SET_BATTERY_CHARGE_STATE(_BATT_STATUS_NONE);//reset battery state.	

						if(ReadEEP(EEP_LowBattery_Flag))
							WriteEEP(EEP_LowBattery_Flag,OFF);
					}
				
			BattDetectStartUp_COUNT=0;
			}

			SET_BATTERY_STATE(BATERY_STAT);
	}
		//#if (_DEBUG_MESSAGE_SysTimerEvent==ON)
		//GraphicsPrint(CYAN,">");
		//#endif
			  if(PowerFlag==OFF)
		  	MCUTimerActiveTimerEvent(SEC(0.1/*1*/), _SYSTEM_TIMER_EVENT_JUDGE_BATT_STATE);
			  else
		MCUTimerActiveTimerEvent(SEC(0.3/*1*/), _SYSTEM_TIMER_EVENT_JUDGE_BATT_STATE);
			#else
			SET_BATTERY_CAPACITY(BatteryVoltage);
	
				
		   if((GET_BATTERY_CAPACITY()<200)||(BatteryBTH>=935)) //no battery  <3.3V
		   	{
			SET_NO_BATTERY();
			SET_BATTERY_STATE(_BATT_STATUS_NO_BATT); 
		   	}
		else 
			{
			CLR_NO_BATTERY();		
			
	 	if(GET_BATTERY_CAPACITY()<=BATTERY_CAPACITY_LEVEL0)//<12V
			   SET_BATTERY_STATE(_BATT_STATUS_CAPACITY_LEVEL0); 		   
		else if((GET_BATTERY_CAPACITY()>BATTERY_CAPACITY_LEVEL0)&&(GET_BATTERY_CAPACITY()<=BATTERY_CAPACITY_LEVEL1))//12V-13.5V
			SET_BATTERY_STATE(_BATT_STATUS_CAPACITY_LEVEL1);			
		else if((GET_BATTERY_CAPACITY()>BATTERY_CAPACITY_LEVEL1)&&(GET_BATTERY_CAPACITY()<=BATTERY_CAPACITY_LEVEL2))//14.2V-13.5V
			SET_BATTERY_STATE(_BATT_STATUS_CAPACITY_LEVEL2);
		else if((GET_BATTERY_CAPACITY()>BATTERY_CAPACITY_LEVEL2)&&(GET_BATTERY_CAPACITY()<=BATTERY_CAPACITY_LEVEL3))//15V-14.2V
			SET_BATTERY_STATE(_BATT_STATUS_CAPACITY_LEVEL3);	
		else 		
			SET_BATTERY_STATE(_BATT_STATUS_CAPACITY_LEVEL4);//>15V
			}		

		MCUTimerActiveTimerEvent(SEC(5), _SYSTEM_TIMER_EVENT_JUDGE_BATT_STATE);

		#endif

		break;

    case _SYSTEM_TIMER_EVENT_JUDGE_PSW_STATE:
		#if (_DEBUG_MESSAGE_Scankey_CHECK==ON)
		GraphicsPrint(CYAN,"\r\nPOWER key pressed");
		#endif

			if(GET_DVR_EntrySleepMode()==_TRUE)
			{
				SET_TARGET_POWER_STATUS(_POWER_STATUS_NORMAL);
				//CLR_DVR_EntrySleepMode();
				//MCU_SendCmdToDVR(MCU_PROTOCOL_CMD_SLEEP_WAKE_UP);
				#if (_DEBUG_MESSAGE_Scankey_CHECK==ON)
			   	GraphicsPrint(YELLOW,"(MCU_SLEEP_TO_NORMAL_MODE)");    
				#endif				

			}
			#if 0 //for test
			else if(PowerFlag==ON)		
				{
			MCUTimerActiveTimerEvent(SEC(0.1), _USER_TIMER_EVENT_OSD_DVR_SHUTDOWN);
			PowerFlag=OFF;
			WriteEEP(EEP_Powerflag,PowerFlag);
				}
			#else
			else if(PowerFlag==ON)		
			MCUTimerActiveTimerEvent(SEC(0.1), _USER_TIMER_EVENT_OSD_DVR_SHUTDOWN);
			#endif
			else
			{
					if(GET_DVR_PowerOFFDelay()==_FALSE)
					{
						if((GET_BATTERY_CAPACITY_LOW_FLAG()==_FALSE)||(Check_ADAP_IN()/*GET_AC_PLUG()*/==_TRUE))
						{
						PowerFlag=ON;
						WriteEEP(EEP_Powerflag,PowerFlag);
						}
						else
							{
							#if(_DEBUG_MESSAGE_SysTimerEvent==ON)
							GraphicsPrint(RED,"\r\n(Battery voltage low <12.5V...)");
							#endif
							MCUTimerActiveTimerEvent(SEC(0.01), _SYSTEM_TIMER_EVENT_BATT_LOW_RED_BLINK);
							}
						
					#if (_DEBUG_MESSAGE_Scankey_CHECK==ON)
					if(PWR_START_flag==ON)
					   	GraphicsPrint(YELLOW,"(DC12V to Power on)");    
					#endif
					}
					else
					MCUTimerActiveTimerEvent(SEC(3), _SYSTEM_TIMER_EVENT_JUDGE_PSW_STATE);
					
			}
		
		  break;
case _SYSTEM_TIMER_EVENT_BATTERY_LOW_PWR_OFF:
		 if(PowerFlag==ON)	
		MCUTimerActiveTimerEvent(SEC(0.1), _USER_TIMER_EVENT_OSD_DVR_SHUTDOWN);
	break;
    case _SYSTEM_TIMER_EVENT_JUDGE_BTH_STATE:
	
		#if (_DEBUG_MESSAGE_BATTERY_TEST)
		#else
		BatteryBTH=GetBatteryBTH();

//		if(GET_BATTERY_CHARGE_STATE()==_BATT_STATUS_NO_BATT)
//			BatteryBTH=549;
		#endif
		
		//if((GET_BATTERY_CHARGE_STATE()==_BATT_STATUS_STOP_CHARGE)&&(GET_BTH_STATE()==_BATT_STATUS_TEMP_NORMAL))
		//SET_BTH_STATE(_BATT_STATUS_TEMP_NORMAL);
		//else
		//{

		#if 1
		if((BatteryBTH<=BTH_TEMP_HIGH)&&(BatteryBTH>BTH_TEMP_MIN)) //<11.6V
			{			
	 		TEMP_NORMAL_COUNT=0;
	   		TEMP_LOW_COUNT=0;
			TEMP_LOW_WARN_COUNT=0;
			TEMP_HIGH_COUNT++;
			TEMP_HIGH_WARN_COUNT=0;
			}
			else if((BatteryBTH>BTH_TEMP_HIGH)&&(BatteryBTH<=BTH_TEMP_HIGH_WARN))//50~55 warnning
			{
		 	TEMP_NORMAL_COUNT=0;
	   		TEMP_LOW_COUNT=0;
			TEMP_LOW_WARN_COUNT=0;
			TEMP_HIGH_COUNT=0;
			TEMP_HIGH_WARN_COUNT++;
			}		
			else if((BatteryBTH>BTH_TEMP_HIGH_WARN)&&(BatteryBTH<=BTH_TEMP_LOW_WARN))
			{
			TEMP_NORMAL_COUNT++;
			TEMP_LOW_COUNT=0;
			TEMP_LOW_WARN_COUNT=0;
			TEMP_HIGH_COUNT=0;
			TEMP_HIGH_WARN_COUNT=0;
			}
			else if((BatteryBTH>BTH_TEMP_LOW_WARN)&&(BatteryBTH<=BTH_TEMP_LOW))  //0~-5 warnning
				{
				TEMP_NORMAL_COUNT=0;
				TEMP_LOW_COUNT=0;
				TEMP_LOW_WARN_COUNT++;
				TEMP_HIGH_COUNT=0;
				TEMP_HIGH_WARN_COUNT=0;

				}
			else if((BatteryBTH>BTH_TEMP_LOW)&&(BatteryBTH<=BATTERY_ACD_no_battery))  //-5~-10 warnning
				{
				TEMP_NORMAL_COUNT=0;
				TEMP_LOW_COUNT++;
				TEMP_LOW_WARN_COUNT=0;
				TEMP_HIGH_COUNT=0;
				TEMP_HIGH_WARN_COUNT=0;
				}
			else
				{
				#if (_DEBUG_MESSAGE_Power_Supply_TEST==ON)
				#else
				SET_NO_BATTERY();
				SET_BATTERY_STATE(_BATT_STATUS_NO_BATT);
				#endif
 				TEMP_STAT=_BATT_STATUS_TEMP_NORMAL; ///no DC12V input
			TEMP_NORMAL_COUNT=0;
	   		TEMP_LOW_COUNT=0;
			TEMP_LOW_WARN_COUNT=0;
			TEMP_HIGH_COUNT=0;
			TEMP_HIGH_WARN_COUNT=0; 				
				}

		if(((TEMP_NORMAL_COUNT>3)||(TEMP_LOW_COUNT>3)||(TEMP_LOW_WARN_COUNT>3)||(TEMP_HIGH_COUNT>3)||(TEMP_HIGH_WARN_COUNT>3)\
			)&&(PWR_START_flag==ON)&&(PowerFlag==OFF)&&(GET_NO_BATTERY()==_TRUE))
		PowerFlag=ON;

			
		if(TEMP_HIGH_COUNT==TempADC_Counts)
		{
			TEMP_STAT=_BATT_STATUS_TEMP_HIGH;
			TEMP_HIGH_COUNT=0;
			CLR_BATTERY_HIGH_TMEP_WARN();
			CLR_BATTERY_LOW_TMEP_WARN();	
			CLR_NO_BATTERY();
		}

		if(TEMP_HIGH_WARN_COUNT==TempADC_Counts)
		{
			TEMP_STAT=_BATT_STATUS_TEMP_HIGH_WARN;
			TEMP_HIGH_WARN_COUNT=0;
			SET_BATTERY_HIGH_TMEP_WARN();
			CLR_BATTERY_LOW_TMEP_WARN();
			CLR_NO_BATTERY();		
		}

		if(TEMP_NORMAL_COUNT==TempADC_Counts)
		{
			TEMP_STAT=_BATT_STATUS_TEMP_NORMAL;
			TEMP_NORMAL_COUNT=0;
			CLR_BATTERY_LOW_TMEP_WARN();
			CLR_BATTERY_HIGH_TMEP_WARN();
			CLR_NO_BATTERY();		
		}

		if(TEMP_LOW_WARN_COUNT==TempADC_Counts)
		{
			TEMP_STAT=_BATT_STATUS_TEMP_LOW_WARN;
			TEMP_LOW_WARN_COUNT=0;
			SET_BATTERY_LOW_TMEP_WARN();
			CLR_BATTERY_HIGH_TMEP_WARN();
			CLR_NO_BATTERY();			
		}

		if(TEMP_LOW_COUNT==TempADC_Counts)
		{
			TEMP_STAT=_BATT_STATUS_TEMP_LOW;
			TEMP_LOW_COUNT=0;
			CLR_BATTERY_LOW_TMEP_WARN();
			CLR_BATTERY_HIGH_TMEP_WARN();	
			CLR_NO_BATTERY();		
		}
/*
		if(GET_BATTERY_CHARGE_STATE()==_BATT_STATUS_NO_BATT)
			{
				TEMP_STAT=_BATT_STATUS_TEMP_NORMAL;
				CLR_BATTERY_LOW_TMEP_WARN();
				CLR_BATTERY_HIGH_TMEP_WARN();

			}
		*/
		SET_BTH_STATE(TEMP_STAT);


			if((BatteryBTH<=CHARGE_TEMP_HIGH)||(BatteryBTH>CHARGE_TEMP_LOW))  
				{
				CHARGE_TEMP_ABNORMAL++;
				CHARGE_TEMP_NORMAL=0;
				}
				else
				{
				CHARGE_TEMP_ABNORMAL=0;
				CHARGE_TEMP_NORMAL++;
				//CLR_CHARGE_TMEP_ABNORMAL();
				}
				
				if(CHARGE_TEMP_ABNORMAL==TempADC_Counts)
					{
					CHARGE_TEMP_ABNORMAL=0;
						if((GET_AC_PLUG()==_TRUE)&&(GET_NO_BATTERY()==_FALSE))			
						SET_CHARGE_TMEP_ABNORMAL();
					
					//	if(GET_BATTERY_CHARGE_STATE()==_BATT_STATUS_NO_BATT)	
					//		CLR_CHARGE_TMEP_ABNORMAL();											
					}
				else	if(CHARGE_TEMP_NORMAL==TempADC_Counts)
					{
					CHARGE_TEMP_NORMAL=0;
					CLR_CHARGE_TMEP_ABNORMAL();
					}
		#else
				if((BatteryBTH<=BTH_TEMP_HIGH)&&(BatteryBTH>BTH_TEMP_MIN)) //>55
				SET_BTH_STATE(_BATT_STATUS_TEMP_HIGH);					
			else if((BatteryBTH>BTH_TEMP_HIGH)&&(BatteryBTH<=BTH_TEMP_HIGH_WARN))//50~55 warnning
				{
				SET_BATTERY_HIGH_TMEP_WARN();		
				CLR_BATTERY_LOW_TMEP_WARN();	
				}
			else if((BatteryBTH>BTH_TEMP_HIGH_WARN)&&(BatteryBTH<=BTH_TEMP_LOW_WARN))
				{
				SET_BTH_STATE(_BATT_STATUS_TEMP_NORMAL);				
				CLR_BATTERY_LOW_TMEP_WARN();
				CLR_BATTERY_HIGH_TMEP_WARN();
				}
			else if((BatteryBTH>BTH_TEMP_LOW_WARN)&&(BatteryBTH<=BTH_TEMP_LOW))  //0~-5 warnning
				{
				//SET_BATTERY_LOW_TMEP_WARN();
				//CLR_BATTERY_HIGH_TMEP_WARN();	
				}
			else if(BatteryBTH>BTH_TEMP_LOW)  //-5~-10 warnning
				{
				SET_BTH_STATE(_BATT_STATUS_TEMP_LOW);				
				CLR_BATTERY_LOW_TMEP_WARN();
				CLR_BATTERY_HIGH_TMEP_WARN();
				}
			else 
				SET_BTH_STATE(_BATT_STATUS_TEMP_NORMAL);
			

			#endif	
		//}
		  if(PowerFlag==OFF)
		  	MCUTimerActiveTimerEvent(SEC(0.1/*1*/), _SYSTEM_TIMER_EVENT_JUDGE_BTH_STATE);
		  	else
		MCUTimerActiveTimerEvent(SEC(0.2/*0.3*//*1*/), _SYSTEM_TIMER_EVENT_JUDGE_BTH_STATE);
		break;

    case _SYSTEM_TIMER_EVENT_JUDGE_WDT_ECHO:


			if(GET_DVR_WDTkickDisable()==_FALSE)
			{
			#if(_DEBUG_MESSAGE_WDT_KICK_CHECK == ON)		
			GraphicsPrint(RED,"\r\n(WDT kick over 10 seconds, Try to reboot the DVR)");	
			#endif
			SET_DVR_Reboot();
			SET_DVR_RebootAndPower();
			MCUTimerActiveTimerEvent(SEC(1), _USER_TIMER_EVENT_OSD_DVR_REBOOT);
			}
		break;

    case _SYSTEM_TIMER_EVENT_JUDGE_AC_MODE:
		
				#if(_DEBUG_MESSAGE_SysTimerEvent==ON)
				GraphicsPrint(CYAN,"(AC)");
				if(GET_STAT1()==ON)
					GraphicsPrint(GREEN,"(S1=1)");
				else
					GraphicsPrint(GREEN,"(S1=0)");
				
				if(GET_STAT2()==ON)
					GraphicsPrint(MAGENTA,"(S2=1)");
				else
					GraphicsPrint(MAGENTA,"(S2=0)");
					
				#endif
				
						if(Check_ADAP_IN()==_TRUE)
						{
							if(GET_AC_PLUG()!=_TRUE)
							{
								SET_AC_PLUG();
								//SET_AC_MODE();	// turn on AC mode
								CLR_AC_MODE();	// turn on AC mode
								//MCUTimerDelayXms(100);//delay 50ms		
								//CLR_BAT_SYS();// turn off battery output							
								//SET_PWM(_CHG_CURR,Low_Current);

								STAT1_temp=GET_STAT1();	
								if(STAT1_Flag!=STAT1_temp)
								STAT1_Flag=STAT1_temp;
								
								STAT2_temp=GET_STAT2();	
								if(STAT2_Flag!=STAT2_temp)
								STAT2_Flag=STAT2_temp;

							}					
						
						if(GET_NO_BATTERY()==_TRUE)
							{
						MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_NO_BATT);								
							}
						else
							{
							
						if(GET_POWER_STATUS()==_POWER_STATUS_NORMAL)
						{
							
							if((GET_BTH_STATE()==_BATT_STATUS_TEMP_NORMAL)&&(GET_BATTERY_CAPACITY_HIGH_FLAG()==_FALSE)&&(GET_BATTERY_ABNORMAL()==_FALSE)&&(GET_CHARGE_TMEP_ABNORMAL()==_FALSE)/*&&(GET_CHARGE_START_FLAG()==_TRUE)*/)
								{
									if(GET_BATTERY_CAPACITY_LOW_FLAG()==_FALSE)
									MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_BATT_NORMAL_CHARGE);										
									else
									MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_BATT_LOW_CHARGE);	
								}
							else if(GET_BATTERY_CHARGE_STATE()==_BATT_STATUS_NO_BATT)
							MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_NO_BATT); 
							else
							MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_BATT_NO_CHARGE);		
						}
						else if(GET_POWER_STATUS()==_POWER_STATUS_SAVING)
							{
							
							if((GET_BTH_STATE()==_BATT_STATUS_TEMP_NORMAL)&&(GET_BATTERY_CAPACITY_HIGH_FLAG()==_FALSE)&&(GET_BATTERY_ABNORMAL()==_FALSE)&&(GET_CHARGE_TMEP_ABNORMAL()==_FALSE)/*&&(GET_CHARGE_START_FLAG()==_TRUE)*/)
								{
								if(GET_BATTERY_CAPACITY_LOW_FLAG()==_FALSE)
								MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_BATT_NORMAL_CHARGE); 									
								else
								MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_BATT_LOW_CHARGE);
								}
								else if(GET_BATTERY_CHARGE_STATE()==_BATT_STATUS_NO_BATT)
								MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_NO_BATT); 
								else
								MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_BATT_NO_CHARGE);	
							}
						else if(GET_POWER_STATUS()==_POWER_STATUS_OFF)
							{
								
								if((GET_BTH_STATE()==_BATT_STATUS_TEMP_NORMAL)&&(GET_BATTERY_CAPACITY_HIGH_FLAG()==_FALSE)&&(GET_BATTERY_ABNORMAL()==_FALSE)&&(GET_CHARGE_TMEP_ABNORMAL()==_FALSE)/*&&(GET_CHARGE_START_FLAG()==_TRUE)*/)
									{
									if(GET_BATTERY_CAPACITY_LOW_FLAG()==_FALSE)
										MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_BATT_HIGH_CHARGE);	
									else
										MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_BATT_LOW_CHARGE);	
									}
								else if(GET_BATTERY_CHARGE_STATE()==_BATT_STATUS_NO_BATT)
								MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_NO_BATT); 
								else
								MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_BATT_NO_CHARGE);	
							}
										
						
							}
					
						MCUTimerActiveTimerEvent(SEC(5), _SYSTEM_TIMER_EVENT_JUDGE_AC_MODE);   //repeat on AC mode


						}
					else
						{
						CLR_AC_PLUG();
						CLR_AC_MODE();	// turn off AC mode
						SET_AC_PLUG_OUT();
						CLR_BATTERY_CAPACITY_HIGH_FLAG();	
						if((PWR_START_flag==ON)&&(PowerFlag==ON))
						MCU_SendCmdToDVR(MCU_PROTOCOL_CMD_SYSTEM_SHUTDOWN);///SET_AC_PLUG_OUT							
						MCUTimerActiveTimerEvent(SEC(0.5), _SYSTEM_TIMER_EVENT_BATT_NO_CHARGE);	 //change battery mode					
						MCUTimerActiveTimerEvent(SEC(0.5), _SYSTEM_TIMER_EVENT_JUDGE_BATT_MODE);  //going to battery mode
						}

					CLR_AC_MODE();	
    					//CLR_BAT_SYS();
		break;
//#if (_BATTERY_CHARGE_STOP==ON)
		case _SYSTEM_TIMER_EVENT_CHECK_BATTERY_STOP_STATE:

								if((GET_STAT1()==ON)&&(GET_STAT2()==ON)&&(bytBatteryStopCharge==_FALSE)&&(Check_ADAP_IN()==_TRUE)&&((GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_MAX)||\
								(GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_MAX_STOP)))
								{
									bytBatteryStopCharge=_TRUE;
									if(ReadEEP(EEP_BatteryStopCharge)==OFF)
									WriteEEP(EEP_BatteryStopCharge,ON);
									
										#if(_DEBUG_MESSAGE_Battery_Charge_Debug==ON)
								  		GraphicsPrint(RED,"(*bytBatteryStopCharge=1)");
										#endif
										SET_BATTERY_CHARGE_STATE(_BATT_STATUS_NONE);
								}
	
									
						break;
//#endif
                case _SYSTEM_TIMER_EVENT_CHECK_CHARGE_STATE:

							STAT1_temp=GET_STAT1();
							STAT2_temp=GET_STAT2();
						
							#if(_DEBUG_MESSAGE_SysTimerEvent==ON)
							GraphicsPrint(YELLOW,"(STAT1_temp=%02x)",(WORD)STAT1_temp);
							#endif	
							#if(_DEBUG_MESSAGE_SysTimerEvent==ON)
							GraphicsPrint(GREEN,"(STAT2_temp=%02x)",(WORD)STAT2_temp);
							#endif	
							
							if((STAT1_Flag!=STAT1_temp)&&(GET_NO_BATTERY()==_FALSE))
								{
								STAT1_Flag=STAT1_temp;
								SET_BATTERY_CHARGE_STATE(_BATT_STATUS_NONE);//reset battery state.							
								}
							
								if((STAT2_Flag!=STAT2_temp)&&(GET_NO_BATTERY()==_FALSE))
								{
								STAT2_Flag=STAT2_temp;
								SET_BATTERY_CHARGE_STATE(_BATT_STATUS_NONE);//reset battery state.							
								}
						#if 0			
							if((STAT1_Flag==ON)&&(STAT2_Flag==ON)&&((GET_BATTERY_CHARGE_STATE()==_BATT_STATUS_HIGH_CHARGE)||\
								(GET_BATTERY_CHARGE_STATE()==_BATT_STATUS_LOW_CHARGE)||(GET_BATTERY_CHARGE_STATE()==_BATT_STATUS_NORMAL_CHARGE))&&\
								(GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_MAX))
								{
								if(_BATT_STATUS_CAPACITY_MAX_STOP_Count>20)
									{
								SET_BATTERY_STATE(_BATT_STATUS_CAPACITY_MAX_STOP);
									bytBatteryStopCharge=_TRUE;
									if(ReadEEP(EEP_BatteryStopCharge)==OFF)
									WriteEEP(EEP_BatteryStopCharge,ON);
									
										#if(_DEBUG_MESSAGE_Battery_Charge_Debug==ON)
								  		GraphicsPrint(RED,"(bytBatteryStopCharge=1)");
										#endif
										SET_BATTERY_CHARGE_STATE(_BATT_STATUS_NONE);
									}
								else
									{

									_BATT_STATUS_CAPACITY_MAX_STOP_Count++;
							#if(_DEBUG_MESSAGE_Battery_Charge_Debug==ON)
						  		GraphicsPrint(RED,"(_BATT_STATUS_CAPACITY_MAX_STOP_Count=%d)",(WORD)_BATT_STATUS_CAPACITY_MAX_STOP_Count);
							#endif
									
									}
								
								}
								else
									_BATT_STATUS_CAPACITY_MAX_STOP_Count=0;
							#endif
					#if 1
							if(GET_BATTERY_ABNORMAL()==_FALSE)
							{	
									if(GET_BATTERY_CAPACITY()>=BATTERY_ACD_MAX)//>17		
									{
									#if(_DEBUG_MESSAGE_SysTimerEvent==ON)
									GraphicsPrint(RED,"( HigH Voltage >17V shut down)");
									#endif
									SET_BATTERY_ABNORMAL();
									MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_JUDGE_PSW_STATE);						
									}
						
							}

				  				switch(GET_BATTERY_STATE())
				  				{

								case _BATT_STATUS_CAPACITY_MAX_STOP:
									
										if((GET_BATTERY_CAPACITY_HIGH_FLAG()==_FALSE)/*&&(_BATT_STATUS_CAPACITY_MAX_STOP_Count>20)*/)
										{
										#if(_DEBUG_MESSAGE_SysTimerEvent==ON)
										GraphicsPrint(RED,"(HIGH Voltage >16.4V stop charge)");
										#endif	
										SET_BATTERY_CAPACITY_HIGH_FLAG();		
										}
										/*
										else
											{
											_BATT_STATUS_CAPACITY_MAX_STOP_Count++;
												if(_BATT_STATUS_CAPACITY_MAX_STOP_Count>200)
													_BATT_STATUS_CAPACITY_MAX_STOP_Count=0;
											}
										*/
									 if(GET_BATTERY_CAPACITY_LOW_FLAG()==_TRUE)
									 {		 
										#if(_DEBUG_MESSAGE_SysTimerEvent==ON)
										 GraphicsPrint(RED,"(2.Normal Voltage Charge >12V)");
										#endif
									 CLR_BATTERY_CAPACITY_LOW_FLAG();	 
									 SET_BATTERY_CHARGE_STATE(_BATT_STATUS_NONE);//reset battery state.  
									 	if(ReadEEP(EEP_LowBattery_Flag))
											WriteEEP(EEP_LowBattery_Flag,OFF);
									 }
									 CLR_CHARGE_START_FLAG();

									#if 0
								 	if(bytBatteryStopCharge==_FALSE)
								 	{

									if(bytBatteryStopChargeCount>20)
										{
										bytBatteryStopCharge=_TRUE;
										if(ReadEEP(EEP_BatteryStopCharge)==OFF)
										WriteEEP(EEP_BatteryStopCharge,ON);

										#if(_DEBUG_MESSAGE_Battery_Charge_Debug==ON)
								  		GraphicsPrint(RED,"(bytBatteryStopCharge=1)");
										#endif
										}
									else
										{	
										bytBatteryStopChargeCount++;
										if(bytBatteryStopChargeCount>200)
											bytBatteryStopChargeCount=0;

										#if(_DEBUG_MESSAGE_Battery_Charge_Debug==ON)
								  		GraphicsPrint(RED,"(bytBatteryStopChargeCount=%d)",(WORD)bytBatteryStopChargeCount);
										#endif
										}
								 	}
									 	#endif	
									break;
								case _BATT_STATUS_CAPACITY_MIN:
								case _BATT_STATUS_CAPACITY_NO_STARTUP:
										if((GET_BATTERY_CAPACITY_LOW_FLAG()==_FALSE)&&(GET_DVR_SystemReadyNotic()==_FALSE)&&(GET_AC_PLUG()==_FALSE))
										{
										SET_BATTERY_CAPACITY_LOW_FLAG();	
										
										if(ReadEEP(EEP_LowBattery_Flag)==OFF)
										WriteEEP(EEP_LowBattery_Flag,ON);
										
										#if(_DEBUG_MESSAGE_SysTimerEvent==ON)
										GraphicsPrint(RED,"( Low Voltage <11.8V or < 11.6V shut down)");
										#endif	
										MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_JUDGE_PSW_STATE);	
										}
										else	if((GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_MIN)&&(PowerFlag==ON)&&(GET_AC_PLUG()==_FALSE))
										{
										PowerFlag=OFF;
										WriteEEP(EEP_Powerflag,PowerFlag);
									
										if(GET_DVR_EntrySleepMode()==TRUE)
										CLR_DVR_EntrySleepMode();
										}

										if(GET_DVR_EntrySleepMode()==_TRUE)
										{
										SET_TARGET_POWER_STATUS(_POWER_STATUS_NORMAL);
										#if (_DEBUG_MESSAGE_SysTimerEvent==ON)
				   						GraphicsPrint(RED,"(MCU_SLEEP_TO_NORMAL_MODE: Low Voltage <12V)");    
										#endif	
										}
										
										CLR_BATTERY_CAPACITY_HIGH_FLAG();				
										SET_CHARGE_START_FLAG();
									break;

								 case _BATT_STATUS_CAPACITY_LEVEL0:
									 CLR_BATTERY_CAPACITY_HIGH_FLAG();	 
									//  CLR_BATTERY_CAPACITY_LOW_FLAG();	
										if(GET_DVR_EntrySleepMode()==_TRUE)
										{
										SET_TARGET_POWER_STATUS(_POWER_STATUS_NORMAL);
										#if (_DEBUG_MESSAGE_SysTimerEvent==ON)
				   						GraphicsPrint(RED,"(MCU_SLEEP_TO_NORMAL_MODE: Low Voltage <12V)");    
										#endif	
										}
								 	break;
								 case _BATT_STATUS_CAPACITY_LEVEL1:								 	
								 case _BATT_STATUS_CAPACITY_LEVEL2:
								 case _BATT_STATUS_CAPACITY_LEVEL3:
								 case _BATT_STATUS_CAPACITY_LEVEL4:
								 case _BATT_STATUS_CAPACITY_MAX:

									 SET_CHARGE_START_FLAG();
									 /*
									 if(GET_BATTERY_CAPACITY_LOW_FLAG()==_TRUE)
									 {		 
										#if(_DEBUG_MESSAGE_SysTimerEvent==ON)
										 GraphicsPrint(RED,"(Normal Voltage Charge >12V)");
										#endif
									 CLR_BATTERY_CAPACITY_LOW_FLAG();	 
									 SET_BATTERY_CHARGE_STATE(_BATT_STATUS_NONE);//reset battery state.  
									 }	
									 */
									 CLR_BATTERY_CAPACITY_HIGH_FLAG();	 
										break;
								//case _BATT_STATUS_CAPACITY_MAX:
								//	CLR_CHARGE_START_FLAG();
									
									//break;
				  				}
			#else
							if(GET_BATTERY_CAPACITY_LOW_FLAG()==_FALSE)
							{	
								if(GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_MIN)//<11.6V 		
								{
								SET_BATTERY_CAPACITY_LOW_FLAG();																			
								#if(_DEBUG_MESSAGE_SysTimerEvent==ON)
								GraphicsPrint(RED,"( Low Voltage <11.6V shut down)");
								#endif																			
								MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_JUDGE_PSW_STATE);						
								}							
								
							}

	
							if(GET_BATTERY_ABNORMAL()==_FALSE)
							{	
									if(GET_BATTERY_CAPACITY()>=BATTERY_ACD_MAX)//>17		
									{
									#if(_DEBUG_MESSAGE_SysTimerEvent==ON)
									GraphicsPrint(RED,"( HigH Voltage >17V shut down)");
									#endif
									SET_BATTERY_ABNORMAL();
									MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_JUDGE_PSW_STATE);						
									}
						
							}
				//}

								if(GET_BATTERY_CAPACITY_LOW_FLAG()==_TRUE)
								{		
									if(GET_BATTERY_STATE()!=_BATT_STATUS_CAPACITY_MIN)//<11.6V		
									{
									#if(_DEBUG_MESSAGE_SysTimerEvent==ON)
									GraphicsPrint(RED,"(Normal Voltage Charge >12V)");
									#endif
									CLR_BATTERY_CAPACITY_LOW_FLAG();	
									SET_BATTERY_CHARGE_STATE(_BATT_STATUS_NONE);//reset battery state.	
									}					
								}
						//	if(((GET_BATTERY_CAPACITY()>675)||(GET_BATTERY_CAPACITY()<1023))&&(GET_BATTERY_ABNORMAL()==_TRUE))		
							//      CLR_BATTERY_ABNORMAL();

							if(GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_MAX)//>16.4V
							{
										if(GET_BATTERY_CAPACITY_HIGH_FLAG()==_FALSE)
										{
										#if(_DEBUG_MESSAGE_SysTimerEvent==ON)
										GraphicsPrint(RED,"(HIGH Voltage >15.5V stop charge)");
										#endif	
										SET_BATTERY_CAPACITY_HIGH_FLAG();		
										//SET_BATTERY_CHARGE_STATE(_BATT_STATUS_NONE);//reset battery state.
										}
										
							}
							else
								CLR_BATTERY_CAPACITY_HIGH_FLAG();	
							
			#endif				
					MCUTimerActiveTimerEvent(SEC(10-5), _SYSTEM_TIMER_EVENT_CHECK_CHARGE_STATE);  //check charge
					break;
		case     _SYSTEM_TIMER_EVENT_JUDGE_BATT_MODE:
			#if(_DEBUG_MESSAGE_SysTimerEvent==ON)
			GraphicsPrint(CYAN,"(BATT)");
				if(GET_STAT1()==ON)
					GraphicsPrint(MAGENTA,"(S1=1)");
				else
					GraphicsPrint(GREEN,"(S1=0)");
				
				if(GET_STAT2()==ON)
					GraphicsPrint(MAGENTA,"(S2=1)");
				else
					GraphicsPrint(GREEN,"(S2=0)");
			#endif
		
			if(Check_ADAP_IN()==_TRUE)
				{

					if((PWR_START_flag==ON)&&(PowerFlag==OFF))
					{
					MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_JUDGE_PSW_STATE);
					}

				SET_BATTERY_CHARGE_STATE(_BATT_STATUS_NONE);//reset battery state.
				MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_JUDGE_AC_MODE);	
				}
			else 
				{
				if(GET_BAT_SYS()!=ON)
					{
					//SET_BAT_SYS();   // turn on  battery mode
					//CLR_BAT_SYS();   // turn on  battery mode
					//MCUTimerDelayXms(100);//delay 50ms					
					CLR_AC_MODE();	// turn off AC mode
					}

					//if((GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_LEVEL0)&&(PowerFlag==ON))
					//MCUTimerActiveTimerEvent(SEC(0.1), _USER_TIMER_EVENT_OSD_DVR_SHUTDOWN);

					MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_BATT_NO_CHARGE); 			


				MCUTimerActiveTimerEvent(SEC(5), _SYSTEM_TIMER_EVENT_JUDGE_BATT_MODE);
				}

					CLR_AC_MODE();	
    					//CLR_BAT_SYS();
			break;

     	case _SYSTEM_TIMER_EVENT_BATT_LOW_CHARGE:	
				if(GET_BATTERY_CHARGE_STATE()!=_BATT_STATUS_LOW_CHARGE)
				UserInterfaceBatteryChargeMode(_BATT_STATUS_LOW_CHARGE);
		break;

	case  _SYSTEM_TIMER_EVENT_BATT_HIGH_CHARGE:  	
				if(GET_BATTERY_CHARGE_STATE()!=_BATT_STATUS_HIGH_CHARGE)
				UserInterfaceBatteryChargeMode(_BATT_STATUS_HIGH_CHARGE);					
				break;
	case  _SYSTEM_TIMER_EVENT_BATT_NORMAL_CHARGE: 	
			if(GET_BATTERY_CHARGE_STATE()!=_BATT_STATUS_NORMAL_CHARGE)
				UserInterfaceBatteryChargeMode(_BATT_STATUS_NORMAL_CHARGE);							
			break;

	case  _SYSTEM_TIMER_EVENT_BATT_NO_CHARGE: 	
				if(GET_BATTERY_CHARGE_STATE()!=_BATT_STATUS_STOP_CHARGE)
				UserInterfaceBatteryChargeMode(_BATT_STATUS_STOP_CHARGE);					
			break;


	case  _SYSTEM_TIMER_EVENT_NO_BATT:	
				if(GET_BATTERY_CHARGE_STATE()!=_BATT_STATUS_NO_BATT)
				UserInterfaceBatteryChargeMode(_BATT_STATUS_NO_BATT);					
 				break;
			

        case _SYSTEM_TIMER_EVENT_SHOW_WORKING_TIMER:
	
			second+=10;
		
			if(second==60)
			{
				second=0;
				minute++;
			}
	
			if(minute==60)
			{
				minute=0;
				hour++;
			}
	
			if(hour==24)
			{
				hour=0;
				day++;
			}	

		ShowWorkingTimer();

		MCUTimerActiveTimerEvent(SEC(10), _SYSTEM_TIMER_EVENT_SHOW_WORKING_TIMER);
			 break;
#if (_POWER_DOWN_ENABLE==ON)
    case _SYSTEM_TIMER_POWER_DOWN_MODE:
		//SET_MCU_POWER_DOWN_MODE();
		Power_down_mode=2;
				
		break;
#endif
   default:
            break;
 }
}

