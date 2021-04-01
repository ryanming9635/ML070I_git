/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  keyremo.c		                                            										*/
/*                                                                           										*/
/*****************************************************************************/

#include "config.h"
#include "typedefs.h"
#include "keyremo.h"
#include "main.h"
#include "i2c.h"
#include "cpu.h"
#include "printf.h"
#include "reg.h"
#include "etc_eep.h"
#include "HS_DVRProtocol.h"

extern bit	RepeatKey;
extern  DATA  WORD  tic_pc;
extern  bit KeyReady;
extern  DATA BYTE Key;
extern  BYTE 	keyticSec;
extern BYTE DVR_FACTORY_MODE;
extern BYTE PowerFlag;
extern BYTE  updn_reg[4];
extern short EncorderCount;
extern bit ChangeKey;
extern BYTE bytHoldOn3SPowerOff;
extern BYTE bytHoldOn3SPowerOffMode;

void Scankey(void)
{

	if( GetKey(1)) 
	{

	if((DVR_FACTORY_MODE==ON)&&(PowerFlag==ON))
	{
	DVR_FACTORY_MODE=OFF;//reset value
	MCUTimerActiveTimerEvent(SEC(0.5), _USER_TIMER_EVENT_OSD_DVR_FACTORY_MODE);
	}
	else if((bytHoldOn3SPowerOffMode==ON)&&(PowerFlag==ON))
		{
		bytHoldOn3SPowerOff=OFF;
		bytHoldOn3SPowerOffMode=OFF;
		PowerFlag=OFF;
		WriteEEP(EEP_Powerflag,PowerFlag);																
		}
	else
	   MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_JUDGE_PSW_STATE);
	}

}

//=============================================================================
//		Get Key 
//=============================================================================
BYTE GetKey(BYTE repeat)
{

bit ready;

	ready = KeyReady;
	KeyReady = 0;
	
	if( !ready ) return 0;
	if( repeat ) return Key;
	if( !RepeatKey ) return Key;

	return 0;
}





