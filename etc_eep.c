/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  Etc_eep.c                                                            									*/
/*                                                                           										*/
/*****************************************************************************/

//eeprom.c
#include "Config.h"
#include "typedefs.h"
#include "main.h"
#include "i2c.h"
#include "printf.h"
#include "etc_eep.h"

DWORD GetFWRevEE()
{
	DWORD rev=0;
/*
	if( ReadEEP(0) != '0' ) return 0;
	if( ReadEEP(1) != '7' ) return 0;
	if( ReadEEP(2) != '0' ) return 0;
	if( ReadEEP(3) != 'I' ) return 0;
*/
rev = ((ReadEEP(EEP_FWREV_MAJOR)<<16)|(ReadEEP(EEP_FWREV_MINOR1)<<8)|(ReadEEP(EEP_FWREV_MINOR2)));

	return rev;
}

BYTE GetProjectNameEE(void)
{
	BYTE rev=1;

	if( ReadEEP(0) != '0' ) return 0;
	if( ReadEEP(1) != '7' ) return 0;
	if( ReadEEP(2) != '0' ) return 0;
	if( ReadEEP(3) != 'I' ) return 0;

return rev;
}

void SaveFWRevEE(void)
{
/*
	WriteEEP(0, '0');
	WriteEEP(1, '7');
	WriteEEP(2, '0');
	WriteEEP(3, 'I');
	*/
	WriteEEP(EEP_FWREV_MAJOR, MCU_FW_Major );
	WriteEEP(EEP_FWREV_MINOR1, MCU_FW_Sub0 );
	WriteEEP(EEP_FWREV_MINOR2, MCU_FW_Sub1);

}

#if (_DEBUG_MESSAGE_Monitor==ON)

void SaveDebugLevelEE(BYTE dl)
{
	WriteEEP(EEP_DEBUGLEVEL, dl);
}
#endif

//===================================================================

void ClearBasicEE(void)
{

	Printf("\r\nClearBasicEE...");

	WriteEEP(0, '0');
	WriteEEP(1, '7');
	WriteEEP(2, '0');
	WriteEEP(3, 'I');

//	WriteEEP(EEP_HARDWARE_VER,Hardware_Version);

	WriteEEP(EEP_Powerflag,ON);

//	WriteEEP(EEP_CameraVolt, 14);  //7.6V
	WriteEEP(EEP_CameraVolt, 39);  //5.1V

	WriteEEP(EEP_DEBUGLEVEL,OFF); 

	WriteEEP(EEP_DC12_PWR_START, OFF); 	

	WriteEEP(EEP_LowBattery_Flag, OFF);	
	
	WriteEEP(EEP_PowerOffToOnflag,ON);
		#if (_BATTERY_CHARGE_STOP==ON)
	WriteEEP(EEP_BatteryStopCharge,OFF);
	#endif
	WriteEEP(EEP_Encorder1,1);
	WriteEEP(EEP_Decimal1,0);

	WriteEEP(EEP_Encorder2,1);
	WriteEEP(EEP_Decimal2,0);

	WriteEEP(EEP_Encorder3,1);
	WriteEEP(EEP_Decimal3,0);

	WriteEEP(EEP_Encorder4,1);
	WriteEEP(EEP_Decimal4,0);

	WriteEEP(EEP_RotateNumberL,0);
	WriteEEP(EEP_RotateNumberM,0);
	WriteEEP(EEP_RotateNumberH,0);

	WriteEEP(EEP_RotateNumberRL,0);
	WriteEEP(EEP_RotateNumberRM,0);
	WriteEEP(EEP_RotateNumberRH,0);

	WriteEEP(EEP_TEncorder1,1);
	WriteEEP(EEP_TDecimal1,0);

	WriteEEP(EEP_TEncorder2,1);
	WriteEEP(EEP_TDecimal2,0);

	WriteEEP(EEP_TEncorder3,1);
	WriteEEP(EEP_TDecimal3,0);

	WriteEEP(EEP_TEncorder4,1);
	WriteEEP(EEP_TDecimal4,0);

		
}
