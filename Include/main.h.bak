/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  main.h		                                            										*/
/*                                                                           										*/
/*****************************************************************************/

#ifndef __MAIN__
#define __MAIN__

#define ON					1
#define OFF					0

#define _STOP_PWM   	0x00
#define _RUN_PWM   	0xFF
#define _BL_PWM   	0x15
#define _CHG_CURR  	0x14

//ADC
#define P15_BTH   5  //P1.5// for test ryan@20200812
#define P16_BTH   6  //P1.6
#define P17_BAT   7  //P1.7

#define _DontgoingToPD 	0
#define _ReadyToPD 		1
#define _GoingToPD 		2

struct RegisterInfo
{
   int	Min;
   int	Max;
   int	Default;
};
struct LongRegisterInfo
{
   WORD	Min;
   WORD	Max;
   WORD	Default;
};


BYTE keyIn(void);
void delay(BYTE cnt);

BYTE RS_ready(void);
BYTE RS_rx(void);

void RS_tx(BYTE tx_buf);

BYTE RS2_ready(void);
BYTE RS2_rx(void);
void RS2_tx(BYTE tx_buf);


BYTE Asc1Bin(BYTE asc);
BYTE Asc2Bin(PDATA_P BYTE *s);



#if (HS_DEBUG==ON)
	void Prompt(void);
#endif

void WaitPowerOn(void);
void LoadEEPROM (void);
void Hs_InitVars(void);
void enter_isp(void);
	

BYTE mcuLib_ProtocolAppend(BYTE *pucBuf, BYTE* piLen, BYTE ucByte);
BYTE mcuLib_ProtocolSendCmdWithParamNum(BYTE ucType, BYTE* pucParam, BYTE ucSize);
void MCU_SendCmdToDVR(BYTE ucType);
void SetAD5110Step(BYTE newv) ;
extern BYTE ScanPowerkey(void);
extern void MCUTimerInitialTimerEvent(void);
extern void MCUTimerActiveTimerEvent(WORD usTime, BYTE ucEventID);
extern void SysTimerHandler(void);
extern void SysModeHandler(void);
extern void SysPowerHandler(void);
extern void MCUTimerDelayXms(WORD usNum);
extern void SysPowerInitial(void);
extern void UserInterfacePowerSwitch(EnumPowerAction enumSwitch);
extern void SET_MCU_POWER_DOWN_MODE(void);
void EncorderHandler_TELI(void);
DWORD GetRotateNumber(void);
void SaveRotateNumber(DWORD val);


#endif	// __MAIN__
