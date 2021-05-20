/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/* Etc_eep.h                                                           										*/
/*                                                                           										*/
/*****************************************************************************/
#ifndef __ETC_EEP__
#define __ETC_EEP__

////eeprom.h
#define  EEP_Powerflag          			0x10   
#define  EEP_CameraVolt          			0x11   
#define	EEP_DC12_PWR_START		0x12	
#define	EEP_LowBattery_Flag			0x13
#define	EEP_PowerOffToOnflag			0x14

#define	EEP_Encorder1                              	0x15
#define 	EEP_Decimal1    				0x16	
#define	EEP_Encorder2                              	0x17
#define 	EEP_Decimal2    				0x18	
#define	EEP_Encorder3                              	0x19
#define 	EEP_Decimal3    				0x20	
#define	EEP_Encorder4                              	0x21
#define 	EEP_Decimal4    				0x22	


#define	EEP_RotateNumberL                     	0x23
#define 	EEP_RotateNumberM			0x24	
#define 	EEP_RotateNumberH			0x25	

#define	EEP_RotateNumberRL                    0x26 //REX
#define 	EEP_RotateNumberRM			0x27	//REX
#define 	EEP_RotateNumberRH			0x28	//REX

#define	EEP_FWREV_MAJOR			0x04	//BYTE	1	F/W Rev.-major(Hex)	
#define	EEP_FWREV_MINOR1			0x05	//BYTE	1	F/W Rev.-minor(Hex)
#define	EEP_FWREV_MINOR2			0x06	//BYTE	1	F/W Rev.-minor(Hex)
#define	EEP_DEBUGLEVEL				0x07	//BYTE	1	DebugLevel

#define	EEP_HARDWARE_VER			0x08	//BYTE	1	Hardware Version
//#if (_BATTERY_CHARGE_STOP==ON)
#define EEP_BatteryStopCharge                   0x29
//#endif

#define	EEP_TEncorder1                             0x30
#define 	EEP_TDecimal1    				0x31	
#define	EEP_TEncorder2                             0x32
#define 	EEP_TDecimal2    				0x33	
#define	EEP_TEncorder3                             0x34
#define 	EEP_TDecimal3    				0x35	
#define	EEP_TEncorder4                             0x36
#define 	EEP_TDecimal4    				0x37	

DWORD GetFWRevEE(void);
void SaveFWRevEE(void);
BYTE GetProjectNameEE(void);

void SaveDebugLevelEE(BYTE);
void ClearBasicEE(void);

#endif	// __ETC_EEP__
