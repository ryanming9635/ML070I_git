/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  typedefs.h                                          											*/
/*                                                                           										*/
/*****************************************************************************/

#ifndef	__TYPEDEFS__
#define	__TYPEDEFS__


#define DATA		data
#define PDATA	pdata
#define IDATA		data
#define CODE		code
#define CODE_P
#define PDATA_P
#define IDATA_P
#define DATA_P

#define INTERRUPT( num, name ) name() interrupt num

#define _between(x, a, b)	(a<=x && x<=b)
#define _swap(a, b)			{ a^=b; b^=a; a^=b; }

#define SEC(x)                                  (1000 * (x))


typedef	unsigned char	Register;
typedef	unsigned char	BYTE;
typedef	unsigned short	WORD;
typedef	unsigned long	DWORD;

#define	TRUE	1
#define	FALSE	0
#define	_TRUE	1
#define	_FALSE	0

//VInputStdDetectMode
#define AUTO	0

//VInputStd
#define NTSC	1			
#define PAL		2
#define SECAM	3
#define NTSC4	4
#define PALM	5
#define PALN	6
#define PAL60	7

#define MAXINPUTSTD	PAL60+1	// 

#define UNKNOWN	0xfe
#define NOINPUT	0	//0xff

//PWM Charge Current
#define Low_Current    0xDA///0xFF//0x28  86% high level
#define High_Current    0x28//0xFF

//#define _CHARGE000mA    0xFF  //minimum
#define _CHARGE000mA    0xE0  //minimum

#define _CHARGE060mA    0xE0  //60mA
//#define _CHARGE060mA    0xFF  //0mA

#if 1//for sean
#define _CHARGE100mA    0xF4
#define _CHARGE300mA    (0x80+25)//0xB0

#define _CHARGE500mA    0x93
#define _CHARGE700mA    (0x80+25)//0xB0//0x5B///0x63

#define _CHARGE1000mA    0x00//0x18
#define _CHARGE1100mA    0x00///maximum

#else
#define _CHARGE100mA    0xF4
#define _CHARGE300mA    0xB0

#define _CHARGE500mA    0x93
#define _CHARGE700mA    0xB0//0x5B///0x63

#define _CHARGE1000mA    0x18
#define _CHARGE1100mA    0x00///maximum

#endif

#define NIL			0xff


//****************************************************************************
// DEFINITIONS / MACROS
//****************************************************************************

#define GET_MODE_STATE()                        		(g_stModeInfo.b5ModeState)
#define SET_MODE_STATE(x)                       	(g_stModeInfo.b5ModeState = x)
#define GET_MODE_STATE_CHANGE()                 (bit)(g_stModeInfo.b1ModeStateChanged)
#define SET_MODE_STATE_CHANGE()                 (g_stModeInfo.b1ModeStateChanged = _TRUE)
#define CLR_MODE_STATE_CHANGE()                 (g_stModeInfo.b1ModeStateChanged = _FALSE)
#define GET_MODE_RESET_TARGET()                 	(g_stModeInfo.b4ModeResetFlag)
#define SET_MODE_RESET_TARGET(x)                (g_stModeInfo.b4ModeResetFlag = x)
#define GET_TARGET_POWER_STATUS()           (g_stPowerInfo.b4PowerAction)
#define SET_TARGET_POWER_STATUS(x)          (g_stPowerInfo.b4PowerAction = x)
#define GET_LED_STATUS()				           (g_stModeInfo.b4LEDStatus)
#define SET_LED_STATUS(x)	         		    (g_stModeInfo.b4LEDStatus = x)


#define GET_BATTERY_STATE()				(g_stBatteryInfo.b5BatteryStatus)
#define SET_BATTERY_STATE(x)				(g_stBatteryInfo.b5BatteryStatus = x)

#define GET_NO_BATTERY()				(bit)(g_stBatteryInfo.b1NoBattery)
#define SET_NO_BATTERY()				(g_stBatteryInfo.b1NoBattery =_TRUE)
#define CLR_NO_BATTERY()				(g_stBatteryInfo.b1NoBattery = _FALSE)

#define GET_AC_PLUG()				(bit)(g_stBatteryInfo.b1AC_PLUG)
#define SET_AC_PLUG()				(g_stBatteryInfo.b1AC_PLUG =_TRUE)
#define CLR_AC_PLUG()				(g_stBatteryInfo.b1AC_PLUG = _FALSE)

#define GET_AC_PLUG_OUT()				(bit)(g_stBatteryInfo.b1AC_PLUG_OUT)
#define SET_AC_PLUG_OUT()				(g_stBatteryInfo.b1AC_PLUG_OUT =_TRUE)
#define CLR_AC_PLUG_OUT()				(g_stBatteryInfo.b1AC_PLUG_OUT = _FALSE)

#define GET_BTH_STATE()					(g_stBatteryInfo.b5BTHStatus)
#define SET_BTH_STATE(x)					(g_stBatteryInfo.b5BTHStatus = x)

#define GET_BATTERY_CAPACITY()  			(g_stBatteryInfo.BatteryCapacity)
#define SET_BATTERY_CAPACITY(x)			(g_stBatteryInfo.BatteryCapacity = x)

#define GET_BATTERY_CAPACITY_LOW_FLAG()  			(bit)(g_stBatteryInfo.b1BatteryCapacityLow)
#define SET_BATTERY_CAPACITY_LOW_FLAG()  			(g_stBatteryInfo.b1BatteryCapacityLow =_TRUE)
#define CLR_BATTERY_CAPACITY_LOW_FLAG()			(g_stBatteryInfo.b1BatteryCapacityLow =_FALSE)

#define GET_BATTERY_CAPACITY_NORMAL_FLAG()  		(bit)(g_stBatteryInfo.b1BatteryCapacityNormal)
#define SET_BATTERY_CAPACITY_NORMAL_FLAG()  		(g_stBatteryInfo.b1BatteryCapacityNormal =_TRUE)
#define CLR_BATTERY_CAPACITY_NORMAL_FLAG()			(g_stBatteryInfo.b1BatteryCapacityNormal =_FALSE)
	
#define GET_BATTERY_CAPACITY_HIGH_FLAG()  			(bit)(g_stBatteryInfo.b1BatteryCapacityHigh)
#define SET_BATTERY_CAPACITY_HIGH_FLAG()  			(g_stBatteryInfo.b1BatteryCapacityHigh =_TRUE)
#define CLR_BATTERY_CAPACITY_HIGH_FLAG()			(g_stBatteryInfo.b1BatteryCapacityHigh =_FALSE)

#define GET_BATTERY_ABNORMAL()  			(bit)(g_stBatteryInfo.b1BatteryAbnormal)
#define SET_BATTERY_ABNORMAL()  			(g_stBatteryInfo.b1BatteryAbnormal=_TRUE)
#define CLR_BATTERY_ABNORMAL()  			(g_stBatteryInfo.b1BatteryAbnormal=_FALSE)

#define GET_BATTERY_HIGH_TMEP_WARN()  			(bit)(g_stBatteryInfo.b1BatteryHighTempWarning)
#define SET_BATTERY_HIGH_TMEP_WARN()  			(g_stBatteryInfo.b1BatteryHighTempWarning=_TRUE)
#define CLR_BATTERY_HIGH_TMEP_WARN()  			(g_stBatteryInfo.b1BatteryHighTempWarning=_FALSE)

#define GET_BATTERY_LOW_TMEP_WARN()  			(bit)(g_stBatteryInfo.b1BatteryLowTempWarning)
#define SET_BATTERY_LOW_TMEP_WARN()  			(g_stBatteryInfo.b1BatteryLowTempWarning=_TRUE)
#define CLR_BATTERY_LOW_TMEP_WARN()  			(g_stBatteryInfo.b1BatteryLowTempWarning=_FALSE)

#define GET_CHARGE_TMEP_ABNORMAL()  			(bit)(g_stBatteryInfo.b1ChargeTempFlag)
#define SET_CHARGE_TMEP_ABNORMAL()  			(g_stBatteryInfo.b1ChargeTempFlag=_TRUE)
#define CLR_CHARGE_TMEP_ABNORMAL()  			(g_stBatteryInfo.b1ChargeTempFlag=_FALSE)

#define GET_CHARGE_START_FLAG()  			(bit)(g_stBatteryInfo.b1ChargeStartFlag)
#define SET_CHARGE_START_FLAG()  			(g_stBatteryInfo.b1ChargeStartFlag=_TRUE)
#define CLR_CHARGE_START_FLAG()  			(g_stBatteryInfo.b1ChargeStartFlag=_FALSE)

#define GET_BATTERY_CHARGE_STATE()  			(g_stBatteryInfo.b5BatteryChargeStatus)
#define SET_BATTERY_CHARGE_STATE(x)			(g_stBatteryInfo.b5BatteryChargeStatus = x)

//#define GET_BATTERY_TEMP_STATE()  			(g_stBatteryInfo.b5BatteryTempStatus)
//#define SET_BATTERY_TEMP_STATE(x)			(g_stBatteryInfo.b5BatteryTempStatus = x)

#define GET_DVR_SystemReadyNotic()				(bit)(g_stDVRInfo.b1DVRSystemReadyNotice)
#define SET_DVR_SystemReadyNotic()				(g_stDVRInfo.b1DVRSystemReadyNotice =_TRUE)
#define CLR_DVR_SystemReadyNotic()				(g_stDVRInfo.b1DVRSystemReadyNotice = _FALSE)

#define GET_DVR_WDTkickDisable()				(bit)(g_stDVRInfo.b1DVRWDTkickDisable)
#define SET_DVR_WDTkickDisable()				(g_stDVRInfo.b1DVRWDTkickDisable =_TRUE)
#define CLR_DVR_WDTkickDisable()				(g_stDVRInfo.b1DVRWDTkickDisable = _FALSE)

#define GET_DVR_EntrySleepMode()				(bit)(g_stDVRInfo.b1DVREntrySleepMode)
#define SET_DVR_EntrySleepMode()				(g_stDVRInfo.b1DVREntrySleepMode =_TRUE)
#define CLR_DVR_EntrySleepMode()				(g_stDVRInfo.b1DVREntrySleepMode = _FALSE)

#define GET_DVR_Reboot()				(bit)(g_stDVRInfo.b1DVRReboot)
#define SET_DVR_Reboot()				(g_stDVRInfo.b1DVRReboot =_TRUE)
#define CLR_DVR_Reboot()				(g_stDVRInfo.b1DVRReboot = _FALSE)

#define GET_DVR_RebootAndPower()				(bit)(g_stDVRInfo.b1DVRRebootAndPower)
#define SET_DVR_RebootAndPower()				(g_stDVRInfo.b1DVRRebootAndPower =_TRUE)
#define CLR_DVR_RebootAndPower()				(g_stDVRInfo.b1DVRRebootAndPower = _FALSE)


#define GET_DVR_PowerOFFDelay()				(bit)(g_stDVRInfo.b1DVRPowerOFFDelay)
#define SET_DVR_PowerOFFDelay()				(g_stDVRInfo.b1DVRPowerOFFDelay =_TRUE)
#define CLR_DVR_PowerOFFDelay()				(g_stDVRInfo.b1DVRPowerOFFDelay = _FALSE)

#define GET_DVR_Stop_Reboot()				(bit)(g_stDVRInfo.b1DVRStopReboot)
#define SET_DVR_Stop_Reboot()				(g_stDVRInfo.b1DVRStopReboot =_TRUE)
#define CLR_DVR_Stop_Reboot()				(g_stDVRInfo.b1DVRStopReboot = _FALSE)

#define GET_DVR_Shutdown()				(bit)(g_stDVRInfo.b1DVRShutdown)
#define SET_DVR_Shutdown()				(g_stDVRInfo.b1DVRShutdown =_TRUE)
#define CLR_DVR_Shutdown()				(g_stDVRInfo.b1DVRShutdown = _FALSE)

#define GET_DVR_DistanceReset()				(bit)(g_stDVRInfo.b1DVRDistanceReset)
#define SET_DVR_DistanceReset()				(g_stDVRInfo.b1DVRDistanceReset =_TRUE)
#define CLR_DVR_DistanceReset()				(g_stDVRInfo.b1DVRDistanceReset = _FALSE)

//--------------------------------------------------
// Power status
//--------------------------------------------------
#define GET_POWER_STATUS()                 (g_stPowerInfo.b4PowerStatus)
#define SET_POWER_STATUS(x)                (g_stPowerInfo.b4PowerStatus = x)

//--------------------------------------------------
// Enumerations of Timer Events
//--------------------------------------------------

typedef struct
{
    BYTE b1EventValid : 1;
    BYTE b7EventID : 7;
    WORD usTime;
} StructTimerEventTableType;

typedef enum
{
    _USER_TIMER_EVENT_START,
 //   _USER_TIMER_EVENT_KEY_REPEAT_ENABLE = _USER_TIMER_EVENT_START,   
   // _USER_TIMER_EVENT_KEY_SCAN_READY,
    _USER_TIMER_EVENT_OSD_CAMERA_VOLTAGE_INC,
    _USER_TIMER_EVENT_OSD_CAMERA_VOLTAGE_DEC,
    _USER_TIMER_EVENT_OSD_SET_CAMERA_VOLTAGE,   
    _USER_TIMER_EVENT_OSD_GET_MCU_VERSION,
     _USER_TIMER_EVENT_OSD_DVR_FACTORY_MODE,
    _USER_TIMER_EVENT_OSD_MCU_ENTRY_SLEEP_MODE,
    _USER_TIMER_EVENT_OSD_DISTANCE_RESET,
    _USER_TIMER_EVENT_OSD_DVR_REBOOT,
     _USER_TIMER_EVENT_OSD_DVR_STOP_REBOOT,
    _USER_TIMER_EVENT_OSD_DVR_SHUTDOWN,
    _USER_TIMER_EVENT_OSD_DVR_ENTER_ISP_MODE,
    _USER_TIMER_EVENT_OSD_ENCODER_DEVIATION_SETTING,
    _USER_TIMER_EVENT_PANEL_BACKLIGHT_ON,
    _USER_TIMER_EVENT_Monitor_ON,
    _USER_TIMER_EVENT_GET_ENCODER_COUNT,    
    _USER_TIMER_EVENT_RESET_ENCODER_COUNT,	
    _USER_TIMER_EVENT_END,
    _USER_TIMER_EVENT_COUNT = _USER_TIMER_EVENT_END,
} EnumUserTimerEventID;

typedef enum
{
    _SYSTEM_TIMER_EVENT_START = _USER_TIMER_EVENT_COUNT,
    _SYSTEM_TIMER_EVENT_SOURCE_CLEAR_DETECTING = _SYSTEM_TIMER_EVENT_START,
     _SYSTEM_TIMER_EVENT_JUDGE_CHECK_AC_CABLE,
    _SYSTEM_TIMER_EVENT_JUDGE_BATT_STATE,
    _SYSTEM_TIMER_EVENT_JUDGE_POWER_STATE,
    _SYSTEM_TIMER_EVENT_JUDGE_BTH_STATE,
    _SYSTEM_TIMER_EVENT_JUDGE_PSW_STATE,    
    _SYSTEM_TIMER_EVENT_JUDGE_FIRST_GET_BATT_BTH_STATE,
// 
    _SYSTEM_TIMER_EVENT_JUDGE_AC_MODE,
    _SYSTEM_TIMER_EVENT_JUDGE_BATT_MODE,
_SYSTEM_TIMER_EVENT_CHECK_CHARGE_STATE,
   _SYSTEM_TIMER_EVENT_BATTERY_LOW_PWR_OFF,

//
//    _SYSTEM_TIMER_EVENT_JUDGE_DVR_COMM,
  //  _SYSTEM_TIMER_EVENT_JUDGE_TW8836_COMM,
    _SYSTEM_TIMER_EVENT_JUDGE_WDT_ECHO,
     _SYSTEM_TIMER_EVENT_GRN_RED_ON,    
      _SYSTEM_TIMER_EVENT_GRN_RED_OFF,	 
     _SYSTEM_TIMER_EVENT_GRN_ON_RED_OFF,    
     _SYSTEM_TIMER_EVENT_GRN_OFF_RED_ON,    
    _SYSTEM_TIMER_EVENT_GRN_BLINK,    
    _SYSTEM_TIMER_EVENT_GRN_2S_BLINK,	  
    _SYSTEM_TIMER_EVENT_RED_BLINK,	 
    _SYSTEM_TIMER_EVENT_GRN_RED_BLINK,	  
    _SYSTEM_TIMER_EVENT_GRN_ON_RED_BLINK,    
    _SYSTEM_TIMER_EVENT_GRN_2S_BLINK_RED_ON,    
_SYSTEM_TIMER_EVENT_BATT_LOW_RED_BLINK,	   
    _SYSTEM_TIMER_EVENT_BATT_LOW_CHARGE,    
	_SYSTEM_TIMER_EVENT_BATT_NORMAL_CHARGE,	   
    _SYSTEM_TIMER_EVENT_BATT_HIGH_CHARGE,    
    _SYSTEM_TIMER_EVENT_BATT_NO_CHARGE,	 
    _SYSTEM_TIMER_EVENT_NO_BATT,
    _SYSTEM_TIMER_EVENT_SHOW_WORKING_TIMER,  
    _SYSTEM_TIMER_EVENT_REPLAY_REGULAR_DATA,
      _SYSTEM_TIMER_EVENT_POWER_OFF_ON_DELAY,
	_SYSTEM_TIMER_POWER_DOWN_MODE,     
       _SYSTEM_TIMER_EVENT_END,
    _SYSTEM_TIMER_EVENT_COUNT,
} EnumSystemTimerEventID;

#define _SIMULTANEOUS_USER_EVENT_COUNT             10
#define _SIMULTANEOUS_SYSTEM_EVENT_COUNT            10
#define _TIMER_EVENT_COUNT    (_SIMULTANEOUS_USER_EVENT_COUNT + _SIMULTANEOUS_SYSTEM_EVENT_COUNT )


//--------------------------------------------------
// Power action
//--------------------------------------------------

typedef enum
{
    _POWER_ACTION_OFF_TO_NORMAL = 0x00,          
    _POWER_ACTION_AC_ON_TO_NORMAL,         
    _POWER_ACTION_AC_ON_TO_OFF,  ////2          
    _POWER_ACTION_PS_TO_NORMAL,           
    _POWER_ACTION_PS_TO_OFF,              
    _POWER_ACTION_NORMAL_TO_PS,   //5        
    _POWER_ACTION_NORMAL_TO_OFF,          
    _POWER_ACTION_DVR_POWER_ON,		  
    _POWER_ACTION_DVR_POWER_OFF,		  
   _POWER_ACTION_PANEL_POWER_ON,		  

//Hi sharp function
_POWER_ACTION_NO_BATT, 
_POWER_ACTION_BATT_TO_NO_CHARGE, 
_POWER_ACTION_BATT_TO_LOW_CURRENT_CHARGE, 
_POWER_ACTION_BATT_TO_HIGH_CURRENT_CHARGE, 
_POWER_ACTION_BATT_TO_OFF, 
_POWER_ACTION_BATT_TO_PS, 
_POWER_ACTION_BATT_TO_NORMAL, 
_POWER_ACTION_OFF_TO_BATT, 
_POWER_ACTION_OFF_TO_NO_BATT, 
_POWER_ACTION_PS_TO_BATT, 
_POWER_ACTION_PS_TO_NO_BATT, 
    _POWER_ACTION_NONE = 0xFF,
} EnumPowerAction;


//--------------------------------------------------
// Power status
//--------------------------------------------------

typedef enum
{
    _POWER_STATUS_AC_ON = 0x00,
    _POWER_STATUS_OFF,
    _POWER_STATUS_NORMAL,
    _POWER_STATUS_NORMAL_BATTERY_MODE,   
    _POWER_STATUS_NORMAL_NO_BATTERY_MODE,  ///AC mode
    _POWER_STATUS_NORMAL_CHARGE_MODE,  
    _POWER_STATUS_NORMAL_AC_MODE,  
    _POWER_STATUS_SAVING,
    _POWER_STATUS_SAVING_BATTERY_MODE,   
    _POWER_STATUS_SAVING_CHARGE_MODE,   
    _POWER_STATUS_SAVING_AC_MODE,	
    _POWER_STATUS_SAVING_NO_BATTERY_MODE,	 
    _POWER_STATUS_NOSUPPORT_SAVING,
      _POWER_STATUS_NONE = 0xFF,
} EnumPowerStatus;

//--------------------------------------------------
// Battery status
//--------------------------------------------------

typedef enum
{
	_BATT_STATUS_CAPACITY_LEVEL4=0x00,   //batter is more than 15V
	_BATT_STATUS_CAPACITY_LEVEL3,  ///1
	_BATT_STATUS_CAPACITY_LEVEL2,///2
	_BATT_STATUS_CAPACITY_LEVEL1, ///3
	_BATT_STATUS_CAPACITY_LEVEL0,///4  ///battery is less than 12V
	_BATT_STATUS_AC_MODE,   ///5
	_BATT_STATUS_TEMP_HIGH,	///6
	_BATT_STATUS_TEMP_NORMAL,  ///7
	_BATT_STATUS_TEMP_LOW, ////8
 	_BATT_STATUS_NO_BATT,    ///9
	_BATT_STATUS_CAPACITY_LOW,
	_BATT_STATUS_CAPACITY_HIGH,
	_BATT_STATUS_ABNORMAL_MODE,
	_BATT_STATUS_HIGH_CHARGE,     //13
	_BATT_STATUS_LOW_CHARGE,    //14
	_BATT_STATUS_NORMAL_CHARGE,	///15
	_BATT_STATUS_STOP_CHARGE,  ///16
	_BATT_STATUS_START_CHARGE,  ///17
	_BATT_STATUS_DVR_OFF,	///18
	_BATT_STATUS_DVR_ON,	//19	
    	_BATT_STATUS_NONE,    //20
	_BATT_STATUS_CAPACITY_MIN,///21 <11.6V
	_BATT_STATUS_CAPACITY_MAX,///22>15.5
	_BATT_STATUS_CAPACITY_MAX_STOP,///23>15.9
	_BATT_STATUS_CAPACITY_NO_STARTUP,///24 <11.6V
	_BATT_STATUS_TEMP_HIGH_WARN, ////25
	_BATT_STATUS_TEMP_LOW_WARN ////26
   	
} EnumBatteryStatus;

typedef struct
{
    BYTE b5BatteryStatus:5;
    BYTE b1NoBattery:1;
    BYTE b1AC_PLUG:1;
    BYTE b1AC_PLUG_OUT:1;
    BYTE b5BTHStatus:5;
    BYTE b1BatteryCapacityLow:1;
    BYTE b1BatteryCapacityHigh:1;
    BYTE b1BatteryCapacityNormal:1;
    BYTE b1BatteryAbnormal:1;
    BYTE b1BatteryHighTempWarning:1;
    BYTE b1BatteryLowTempWarning:1;
    BYTE b1ChargeTempFlag:1;
    BYTE b1ChargeStartFlag:1;
    BYTE b5BatteryChargeStatus:5;
    WORD BatteryCapacity;
} StructBatteryInfoType;

typedef struct
{
    BYTE b1DVRSystemReadyNotice:1;
    BYTE b1DVRWDTkickDisable:1;
    BYTE b1DVREntrySleepMode:1;
    BYTE b1DVRReboot:1;
    BYTE b1DVRRebootAndPower:1;
    BYTE b1DVRPowerOFFDelay:1;
    BYTE b1DVRShutdown:1;
    BYTE b1DVRDistanceReset:1;
   BYTE b1DVRStopReboot:1;
} StructDVRInfoType;


typedef struct
{
    BYTE b4PowerAction : 4;
    BYTE b4PowerStatus : 4;
    BYTE b1GDIIsolationFlag : 1;
    BYTE b1TopIsolationFlag : 1;
    BYTE b1AnalogIsolationFlag : 1;
} StructPowerInfoType;


#endif	/* __TYPEDEFS__ */
