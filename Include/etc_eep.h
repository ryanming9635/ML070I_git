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

#define	EEP_FWREV_MAJOR			0x04	//BYTE	1	F/W Rev.-major(Hex)	
#define	EEP_FWREV_MINOR1			0x05	//BYTE	1	F/W Rev.-minor(Hex)
#define	EEP_FWREV_MINOR2			0x06	//BYTE	1	F/W Rev.-minor(Hex)
#define	EEP_DEBUGLEVEL				0x07	//BYTE	1	DebugLevel

DWORD GetFWRevEE(void);
void SaveFWRevEE(void);
BYTE GetProjectNameEE(void);

void SaveDebugLevelEE(BYTE);
void ClearBasicEE(void);

#endif	// __ETC_EEP__
