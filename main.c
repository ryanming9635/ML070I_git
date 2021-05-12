/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  main.c		                                            										*/
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




StructBatteryInfoType g_stBatteryInfo = {0};
StructDVRInfoType g_stDVRInfo = {0};
extern DATA  DWORD tic_Init_time;
extern bit	AccessEnable;

BYTE	PowerFlag;
BYTE	CameraVolt;
BYTE	PWR_START_flag;
BYTE LowBatteryFlag;
BYTE PowerOffToOnFlag;
#if (_BATTERY_CHARGE_STOP==ON)
BYTE bytBatteryStopCharge;
BYTE bytBatteryStopChargeCount=0;
#endif
BYTE DVRChangeCurrent=0;
BYTE Power_down_mode=_DontgoingToPD;
BYTE bytFastEncoderMode=ON;//OFF;

float EncorderLen=0;
WORD EncorderLen_Offset=0;
BYTE EncorderLenint=0,EncorderCountPN=0,EncorderCountPN_offset=0;
DWORD EncorderLenfloaat=0;
//BYTE Encorder1=0.85,Encorder2=0.8,Encorder3=0.75,Encorder4=0.75;
//float Encorder1=100,Encorder2=100,Encorder3=100,Encorder4=100;
float Encorder1=1,Encorder2=1,Encorder3=1,Encorder4=1;
float Decimal1=0,Decimal2=0,Decimal3=0,Decimal4=0;
float TEncorder1=1,TEncorder2=1,TEncorder3=1,TEncorder4=1;
float TDecimal1=0,TDecimal2=0,TDecimal3=0,TDecimal4=0;

DWORD ulongRotateNumber=0;
DWORD ulongRotateNumberTELI=0;

extern WORD BatteryBTH,BatteryVoltage;

#if (_DEBUG_MESSAGE_BatteryBTH==ON)
WORD BTH_Temp=250;
#endif
#if (_DEBUG_MESSAGE_BatteryVoltage==ON)
WORD Battery_Voltage_Temp  =835;
#endif

//extern BYTE FLASH_FLAG;
//extern WORD LED_FLASH_COUNT;
extern bit ChangeKey;
extern long EncorderCount;

extern  DATA  WORD  tic_pc;
extern DATA BYTE	RS_in;
extern DATA BYTE	RS2_in;
extern DATA BYTE RS2_out;
extern StructPowerInfoType idata g_stPowerInfo ;
extern DATA WORD keytic;
extern bit RepeatKey;
extern BYTE IE_Temp;
extern BYTE bytHoldOn3SPowerOff;
extern long EncorderCountINT;

struct RegisterInfo UserRange={0,40,21};
struct RegisterInfo AD5110Range={0,40,21};

BYTE	DebugLevel;

///AD5110 step voltage
BYTE  AD5110_setp[]={0x05,0x06,0x08,0x0A,0x0B,0x0D,0x0F,0x10,0x12,0x14,\
					0x16,0x18,0x1a,0x1c,0x1e,0x20,0x22,0x25,0x27,0x2a,\
					0x2c,0x2f,0x32,0x35,0x38,0x3b,0x3e,0x42,0x45,0x48,\
					0x4b,0x50,0x55,0x58,0x5d,0x61,0x66,0x6b,0x70,0x75,\
					0x7a	};


void isp_write(unsigned char* buf, unsigned char addr_high, unsigned char addr_low, unsigned char len)
{
	IFMT = 0x02;
	IFADRH = addr_high;
	do
	{
		IFADRL = addr_low;
		IFD = *buf;
		SCMD = 0x46;
		SCMD = 0xB9;
	   
		buf++;
		addr_low++;
		if (addr_low == 0)
		{
		   addr_high++;
		   IFADRH = addr_high;
		}
		len--;
	}
	while (len != 0);
}

void isp_init(void)
{
	ISPCR = 0x81;
}

void enter_isp(void)
{
        // Clear the magic mark
        unsigned char mark = 0x00;
        isp_init();
        isp_write(&mark, 0xEF, 0xFF, 1);
        ISPCR = 0x60;
}


BYTE mcuLib_ProtocolAppend(BYTE *pucBuf, BYTE * piLen, BYTE ucByte) {
    if (ucByte == 0x7E || ucByte == 0x7D) {
        pucBuf[*piLen] = 0x7D;
        (*piLen)++;
        pucBuf[*piLen] = 0x50 | (ucByte & 0x0F);
        (*piLen)++;
    } else {
        pucBuf[*piLen] = ucByte;
        (*piLen)++;
    }
    return 0;
}

BYTE mcuLib_ProtocolSendCmdWithParamNum(BYTE ucType, BYTE* pucParam, BYTE ucSize) {
    BYTE ucChecksum = 0;
    BYTE pucBuf[16];
    BYTE iLen;
    BYTE i;
   
    pucBuf[0] = 0x7E;
    iLen = 1;
    ucChecksum ^= ucType;
    mcuLib_ProtocolAppend(pucBuf, &iLen, ucType);
    ucChecksum ^= ucSize;
    mcuLib_ProtocolAppend(pucBuf, &iLen, ucSize);

    for (i = 0; i < ucSize; i++) {
        ucChecksum ^= pucParam[i];
        mcuLib_ProtocolAppend(pucBuf, &iLen, pucParam[i]);
    }

    mcuLib_ProtocolAppend(pucBuf, &iLen, ucChecksum);

	for (i = 0; i < (iLen); i++) {
			RS2_tx(pucBuf[i]);
		}

	
    return 0;
}


void MCU_SendCmdToDVR(BYTE ucType)
{
	BYTE buf[7]={0,0,0,0,0,0,0};
	WORD val;
	DWORD encoder;
switch(ucType)
{
	case MCU_PROTOCOL_CMD_REPLY_MCU_FW:	//Reply MCU firmware version
		buf[0]=ReadEEP(EEP_FWREV_MAJOR);     	//FWVER
		buf[1]=ReadEEP(EEP_FWREV_MINOR1);
		buf[2]=ReadEEP(EEP_FWREV_MINOR2);
		mcuLib_ProtocolSendCmdWithParamNum(MCU_PROTOCOL_CMD_REPLY_MCU_FW,buf,3);	
		break;

	case MCU_PROTOCOL_CMD_REGULAR_DATA:			//Regular data

	//val=(((GET_BATTERY_CAPACITY()*32)/10000)*100)+(((GET_BATTERY_CAPACITY()*32)%10000)/100);
	//val=(val*54)/100;
	#if 1
	val=((((GET_BATTERY_CAPACITY()*59)/*-800*/)/100))+((((GET_BATTERY_CAPACITY()*59)/*-800*/)/100))+((((GET_BATTERY_CAPACITY()*59)/*-800*/)/100));

		if((val/1000))
		val=(val/10)+1;
		else 
		val=(val/10);
/*
	   if(GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_LEVEL0)
		val=121;	
	else if(GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_LEVEL1)
		val=127;	
	else if(GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_LEVEL2)
		val=132;
	else if(GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_LEVEL3)
		val=146;	
	else	 if(GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_LEVEL4)	
		val=155;
	else if(GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_MIN)
		val=115;	
	else if(GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_MAX)
		val=160;	
	else if(GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_MAX_STOP)
		val=164;
	else if(GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_NO_STARTUP)
		val=117;
	else	 if(GET_BATTERY_STATE()==_BATT_STATUS_NO_BATT)	
		val=0;
*/

	#else
	val=((((GET_BATTERY_CAPACITY()*(32+1))/*-800*/)/10000)*100)+((((GET_BATTERY_CAPACITY()*(32+1))/*-800*/)%10000)/100);
	val=(val*(54+1))/100;
	#endif
		
		#if 1
		buf[0]=val;  		 		///battery info.
		#else
		buf[0]=GET_BATTERY_STATE();  		 		///abttery info.
		#endif
		#if 0//for test
		
			buf[2]=(BatteryVoltage>>8);
			
			buf[3]=(BatteryVoltage&0xff);

		#else
		buf[2]=(BatteryBTH>>8);

		buf[3]=(BatteryBTH&0xff);
		#endif
		buf[4]=((EncorderLenint/10)<<4)|(EncorderLenint%10);   

		#if (_2ND_DECIMAL==ON)//ryan@20200505
		buf[5]=((EncorderLenfloaat/10)<<4)|(EncorderLenfloaat%10);
		#else
		buf[5]=(EncorderLenfloaat<<4);
		#endif
		buf[6]=EncorderCountPN;

		if(GET_AC_PLUG()==_TRUE)
		buf[1]|=0x01;

		if(GET_BTH_STATE()==_BATT_STATUS_TEMP_HIGH)
		buf[1]|=0x02;
		
		if(GET_BTH_STATE()==_BATT_STATUS_TEMP_LOW)
		buf[1]|=0x04;

		if(GET_NO_BATTERY()==_TRUE)
		buf[1]|=0x08;

		if(GET_BATTERY_ABNORMAL()==_TRUE)
		buf[1]|=0x10;
		
		if(GET_BATTERY_HIGH_TMEP_WARN()==_TRUE)
		buf[1]|=0x20;
		
		if(GET_BATTERY_LOW_TMEP_WARN()==_TRUE)
		buf[1]|=0x40;
#if (_BATTERY_CHARGE_STOP==ON)
		if(((GET_STAT1()==OFF)&&(GET_STAT2()==ON)&&(BatteryBTH>90))&&(GET_BATTERY_CAPACITY_HIGH_FLAG()==_FALSE)&&\
			( GET_BTH_STATE()==_BATT_STATUS_TEMP_NORMAL)&&(GET_CHARGE_TMEP_ABNORMAL()==_FALSE)&&\
			(GET_NO_BATTERY()==_FALSE)&&(bytBatteryStopCharge==_FALSE))///charging
			{
		buf[1]|=0x80;
			}
#else
if(((GET_STAT1()==OFF)&&(GET_STAT2()==ON)&&(BatteryBTH>90))&&(GET_BATTERY_CAPACITY_HIGH_FLAG()==_FALSE)&&( GET_BTH_STATE()==_BATT_STATUS_TEMP_NORMAL)&&(GET_CHARGE_TMEP_ABNORMAL()==_FALSE)&&(GET_NO_BATTERY()==_FALSE))///charging
	buf[1]|=0x80;
#endif
		mcuLib_ProtocolSendCmdWithParamNum(MCU_PROTOCOL_CMD_REGULAR_DATA,buf,7);												
		break;

	case MCU_PROTOCOL_CMD_SYSTEM_SHUTDOWN:		//system shutdown
		if(GET_AC_PLUG_OUT()==_TRUE)			
			{
			buf[0]=1;	
			CLR_AC_PLUG_OUT();	
			#if(_DEBUG_MESSAGE_SysTimerEvent==ON)
			GraphicsPrint(RED,"\r\n(AC_PLUG_OUT)");
			#endif	
			}
		mcuLib_ProtocolSendCmdWithParamNum(MCU_PROTOCOL_CMD_SYSTEM_SHUTDOWN,buf,1);											
		break;

	case MCU_PROTOCOL_CMD_ENTRY_FACTORY:	//Entry factory mode
		mcuLib_ProtocolSendCmdWithParamNum(MCU_PROTOCOL_CMD_ENTRY_FACTORY,buf,1);	
		break;

	case MCU_PROTOCOL_CMD_SLEEP_WAKE_UP:
		mcuLib_ProtocolSendCmdWithParamNum(MCU_PROTOCOL_CMD_SLEEP_WAKE_UP,buf,1);	
		break;
	case MCU_PROTOCOL_CMD_REPLY_ENCODER_COUNT:
		if(bytFastEncoderMode==ON)
		encoder=ulongRotateNumberTELI;
		else
		encoder=ulongRotateNumber;
		
		buf[0]=encoder/100000;
		buf[0]<<=4;
		encoder%=100000;
		buf[0]|=encoder/10000;
		encoder%=10000;
		
		buf[1]=encoder/1000;		
		buf[1]<<=4;	
		encoder%=1000;		
		buf[1]|=encoder/100;
		encoder%=100;
		
		buf[2]=encoder/10;		
		buf[2]<<=4;	
		encoder%=10;		
		buf[2]|=encoder;

			#if(_DEBUG_MESSAGE_SysTimerEvent==ON)
			if(bytFastEncoderMode==ON)
			GraphicsPrint(RED,"(ulongRotateNumberTELI=%d)",(WORD)ulongRotateNumberTELI);
			else
			GraphicsPrint(RED,"(ulongRotateNumber=%d)",(WORD)ulongRotateNumber);	
			#endif	
			
		mcuLib_ProtocolSendCmdWithParamNum(MCU_PROTOCOL_CMD_REPLY_ENCODER_COUNT,buf,3);	
		break;
default:
	break;
		
}
	
}


void InitialTimerEvent(void)
{
	MCUTimerInitialTimerEvent();  //clear all of timer event	
	MCUTimerActiveTimerEvent(1, _SYSTEM_TIMER_EVENT_JUDGE_FIRST_GET_BATT_BTH_STATE);
    	MCUTimerActiveTimerEvent(1, _SYSTEM_TIMER_EVENT_JUDGE_BATT_STATE);
	MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_JUDGE_AC_MODE);
  	MCUTimerActiveTimerEvent(1, _SYSTEM_TIMER_EVENT_JUDGE_BTH_STATE);
	MCUTimerActiveTimerEvent(1, _SYSTEM_TIMER_EVENT_GRN_BLINK);
	MCUTimerActiveTimerEvent(1, _SYSTEM_TIMER_EVENT_JUDGE_CHECK_AC_CABLE);
	MCUTimerActiveTimerEvent(SEC(10), _SYSTEM_TIMER_EVENT_REPLAY_REGULAR_DATA);
	MCUTimerActiveTimerEvent(SEC(10), _SYSTEM_TIMER_EVENT_CHECK_CHARGE_STATE);	//check charge
	SET_AC_PLUG();

#if(_DEBUG_MESSAGE_WORKING_TIME == ON)
	MCUTimerActiveTimerEvent(SEC(10), _SYSTEM_TIMER_EVENT_SHOW_WORKING_TIMER);
#endif

}

BYTE Mapping1( int fromValue, CODE_P struct RegisterInfo *fromRange,
                                 int * toValue, CODE_P struct RegisterInfo *toRange ){

	// calculate intermediate values
	int a;
	int b;

	// perform mapping
	if ( fromValue <= fromRange->Default ) {
		a = toRange->Default - toRange->Min;
		b = fromRange->Default - fromRange->Min;
		// prevent divide by zero
		if( b==0 )		return (FALSE);
		*toValue = (int) ( (DWORD)fromValue- (DWORD)fromRange->Min ) * a / b 
						+(DWORD)toRange->Min;
	}
	else {
		a = toRange->Max - toRange->Default;
		b = fromRange->Max - fromRange->Default;
		// prevent divide by zero
		if( b==0 )		return (FALSE);
        *toValue = (int) ( (DWORD)fromValue - (DWORD)fromRange->Default ) * a / b
                       + (DWORD)toRange->Default;
	}

	return ( TRUE );
   
}

void SetAD5110Step(BYTE newv) 
{

	int regv;

	Mapping1( newv, &UserRange , &regv, &AD5110Range);
	WriteAD5110(AD5110_CMD_Write_Data_To_RDAC ,AD5110_setp[regv]);
		        
}

DWORD GetRotateNumber(BYTE index)
{
	DWORD ret=0;

	if(index==ON)
	{
		ret=ReadEEP(EEP_RotateNumberH);
		ret<<=8;
		
		ret|=ReadEEP(EEP_RotateNumberM);
		ret<<=8;	

		ret|=ReadEEP(EEP_RotateNumberL);

		#if(_DEBUG_EncorderHandler==ON)
		Printf("\r\nGetRotateNumberTELI=%02x%04x",(WORD)(ret>>16),(WORD)ret);
		#endif
	}
	else
	{
		ret=ReadEEP(EEP_RotateNumberRH);
		ret<<=8;
		
		ret|=ReadEEP(EEP_RotateNumberRM);
		ret<<=8;	

		ret|=ReadEEP(EEP_RotateNumberRL);

		#if(_DEBUG_EncorderHandler==ON)
		Printf("\r\nGetRotateNumberREX=%02x%04x",(WORD)(ret>>16),(WORD)ret);
	#endif

	}

	
	return ret;
}

void SaveRotateNumber(void)
{
	DWORD temp=0,val;

val=ulongRotateNumberTELI;


	temp=ReadEEP(EEP_RotateNumberH);
	temp<<=8;
	temp|=ReadEEP(EEP_RotateNumberM);
	temp<<=8;	
	temp|=ReadEEP(EEP_RotateNumberL);

	if(temp!=val)
	{	

	if(val>999999)
		{
		val-=999999 ;///>99999 clean to 0

		#if(_DEBUG_EncorderHandler==ON)
		Printf("\r\nSaveRotateNumberTELI>999999");
		#endif
		}
	WriteEEP(EEP_RotateNumberL,(val&0xff));
	val>>=8;
	WriteEEP(EEP_RotateNumberM,(val&0xff));
	val>>=8;
	WriteEEP(EEP_RotateNumberH,(val&0xff));

	#if(_DEBUG_EncorderHandler==ON)
	Printf("\r\nSaveRotateNumberTELI=%d",(WORD)ulongRotateNumberTELI);
	#endif
	}

val=ulongRotateNumber;
	
		temp=ReadEEP(EEP_RotateNumberRH);
		temp<<=8;
		temp|=ReadEEP(EEP_RotateNumberRM);
		temp<<=8;	
		temp|=ReadEEP(EEP_RotateNumberRL);
		
		if(temp!=val)
		{	
		
		if(val>999999)
			{
			val-=999999 ;///>99999 clean to 0



	#if(_DEBUG_EncorderHandler==ON)
			Printf("\r\nSaveRotateNumberREX>999999");
	#endif
			}
		WriteEEP(EEP_RotateNumberRL,(val&0xff));
		val>>=8;
		WriteEEP(EEP_RotateNumberRM,(val&0xff));
		val>>=8;
		WriteEEP(EEP_RotateNumberRH,(val&0xff));
		
		#if(_DEBUG_EncorderHandler==ON)
		Printf("\r\nSaveRotateNumberREX=%d",(WORD)ulongRotateNumber);
		#endif
		}
}
void 	LoadEEPROM (void)
{
	 PowerFlag = ReadEEP(EEP_Powerflag);
	 CameraVolt= ReadEEP(EEP_CameraVolt);
	 PWR_START_flag= ReadEEP(EEP_DC12_PWR_START);
	PowerOffToOnFlag= ReadEEP(EEP_PowerOffToOnflag);

if(PowerFlag>=2)
{
	PowerFlag=ON;
	WriteEEP(EEP_Powerflag,ON);
}

if(PWR_START_flag>=2)
{
	PWR_START_flag=OFF;
	WriteEEP(EEP_DC12_PWR_START,OFF);
}

if(PowerOffToOnFlag>=2)
{
	PowerOffToOnFlag=ON;
	WriteEEP(EEP_PowerOffToOnflag,ON);
}
	
	Encorder1= ((ReadEEP(EEP_Encorder1)>>4)*10)+(0x0f&ReadEEP(EEP_Encorder1));
	Decimal1= ((ReadEEP(EEP_Decimal1)>>4)*10)+(0x0f&ReadEEP(EEP_Decimal1));

	Encorder2= ((ReadEEP(EEP_Encorder2)>>4)*10)+(0x0f&ReadEEP(EEP_Encorder2));
	Decimal2= ((ReadEEP(EEP_Decimal2)>>4)*10)+(0x0f&ReadEEP(EEP_Decimal2));

	Encorder3= ((ReadEEP(EEP_Encorder3)>>4)*10)+(0x0f&ReadEEP(EEP_Encorder3));
	Decimal3= ((ReadEEP(EEP_Decimal3)>>4)*10)+(0x0f&ReadEEP(EEP_Decimal3));

	Encorder4= ((ReadEEP(EEP_Encorder4)>>4)*10)+(0x0f&ReadEEP(EEP_Encorder4));
	Decimal4= ((ReadEEP(EEP_Decimal4)>>4)*10)+(0x0f&ReadEEP(EEP_Decimal4));


	ulongRotateNumberTELI=ReadEEP(EEP_RotateNumberH);
	ulongRotateNumberTELI<<=8;
	ulongRotateNumberTELI|=ReadEEP(EEP_RotateNumberM);	
	ulongRotateNumberTELI<<=8;
	ulongRotateNumberTELI|=ReadEEP(EEP_RotateNumberL);	

		ulongRotateNumber=ReadEEP(EEP_RotateNumberRH);
		ulongRotateNumber<<=8;
		ulongRotateNumber|=ReadEEP(EEP_RotateNumberRM);	
		ulongRotateNumber<<=8;
		ulongRotateNumber|=ReadEEP(EEP_RotateNumberRL);


	
	if(ReadEEP(EEP_LowBattery_Flag))
		SET_BATTERY_CAPACITY_LOW_FLAG();

	if(PWR_START_flag==ON)
	{
		if(PowerFlag==OFF)
		{
		WriteEEP(EEP_Powerflag,ON);
		PowerFlag=ON;
		}
		
		if(PowerOffToOnFlag==ON)
			{
			WriteEEP(EEP_PowerOffToOnflag,OFF);
			PowerOffToOnFlag=OFF;
			}

	}	
	else //PWR_START_flag==OFF
		{
			
			if(PowerOffToOnFlag==ON)
			{
			WriteEEP(EEP_Powerflag,ON);
			PowerFlag=ON;

			
			WriteEEP(EEP_PowerOffToOnflag,OFF);
			PowerOffToOnFlag=OFF;
			}
			else if(PowerFlag==ON)
				{
				WriteEEP(EEP_Powerflag,OFF);
				PowerFlag=OFF;				
				}
		}

#if (_BATTERY_CHARGE_STOP==ON)
bytBatteryStopCharge=ReadEEP(EEP_BatteryStopCharge);

if(bytBatteryStopCharge>=2)
{
	bytBatteryStopCharge=OFF;
	WriteEEP(EEP_BatteryStopCharge,OFF);
}

#endif

}

WORD GetADCValue(BYTE sel)
{
    
ADCTL=0x80|0x08|sel;  //start adc sampling

while(ADCTL&0x08);	// printf("ADC is busying\r\n");  //wating for adc

return (WORD)(((ADCVL&0x03) |(ADCV<<2))&0x03ff);

}

WORD GetBatteryBTH(void)
{
WORD max=0;
max=GetADCValue(P16_BTH);
//max=GetADCValue(P15_BTH);

//max=850;

return max;
}

WORD GetBatteryVoltage(void)
{
WORD max=0;
max=GetADCValue(P17_BAT);

//max=710;	//low
//max=835;  	//normal
//max=900; 	//full

return max;
}


void EncorderHandler(void)
{
WORD EncorderCountTemp,temp_val;
#if (HS_DEBUG==ON)
WORD temp_EncorderCountTemp;
#endif

//BYTE param[4];
float EncorderParaTemp1,EncorderParaTemp2,EncorderParaTemp3,EncorderParaTemp4;
float EncorderOffset1,EncorderOffset2,EncorderOffset3/*,EncorderOffset4*/;
	
	if(ChangeKey)
	{	
		#if (HS_DEBUG==ON)
		GraphicsPrint(RED,"\r\nEncorderCount=%d ",(WORD)EncorderCount);	
		Printf("\r\nEncorderCountINT=%d",(WORD)EncorderCountINT);
		GraphicsPrint(YELLOW,"\r\nGetRotateNumber=%01x%04x",(WORD)(ulongRotateNumber>>16),(WORD)ulongRotateNumber);
		#endif
		//Printf("\r\nEncorderLen_Offset0=%d ",(WORD)EncorderLen_Offset);
		//Printf("\r\nEncorderCountPN0=%d ",(WORD)EncorderCountPN);
		#if 1//ryan@20200430
//		EncorderParaTemp1=(Encorder1+(Decimal1))*0.01f;
//		EncorderParaTemp2=(Encorder2+(Decimal2))*0.01f;
//		EncorderParaTemp3=(Encorder3+(Decimal3))*0.01f;
//		EncorderParaTemp4=(Encorder4+(Decimal4))*0.01f;
		EncorderParaTemp1=(Encorder1+(Decimal1*0.01));
		EncorderParaTemp2=(Encorder2+(Decimal2*0.01));
		EncorderParaTemp3=(Encorder3+(Decimal3*0.01));
		EncorderParaTemp4=(Encorder4+(Decimal4*0.01));

		#else
		EncorderParaTemp1=(Encorder1)*0.01f;
		EncorderParaTemp2=(Encorder2)*0.01f;
		EncorderParaTemp3=(Encorder3)*0.01f;
		EncorderParaTemp4=(Encorder4)*0.01f;
		#endif
		//EncorderParaTemp1=(Encorder1*0.01f)+Decimal1;
		//EncorderParaTemp2=(Encorder2*0.01f)+Decimal2;
		//EncorderParaTemp3=(Encorder3*0.01f)+Decimal3;
		//EncorderParaTemp4=(Encorder4*0.01f)+Decimal4;
		
		#if 0
		Printf("\r\nEncorder1=%d ",(WORD)Encorder1);
		Printf("\r\nEncorder2=%d ",(WORD)Encorder2);
		Printf("\r\nEncorder3=%d ",(WORD)Encorder3);
		Printf("\r\nEncorder4=%d ",(WORD)Encorder4);
		
		Printf("\r\nEncorderParaTemp1=%d ",(WORD)(EncorderParaTemp1*100));
		Printf("\r\nEncorderParaTemp2=%d ",(WORD)(EncorderParaTemp2*100));
		Printf("\r\nEncorderParaTemp3=%d ",(WORD)(EncorderParaTemp3*100));
		Printf("\r\nEncorderParaTemp4=%d ",(WORD)(EncorderParaTemp4*100));
		#endif
		#if 0
		EncorderOffset1=(30.625*EncorderParaTemp1*(240-EncorderLen_Offset))/1000;
		EncorderOffset2=EncorderOffset1+(32.091*EncorderParaTemp2*(450-240-EncorderLen_Offset))/1000;
		EncorderOffset3=EncorderOffset2+(33.504*EncorderParaTemp3*(780-450-EncorderLen_Offset)/1000);
		//EncorderOffset4=EncorderOffset3+(33.504*((EncorderParaTemp3+EncorderParaTemp4)/2)*(780-600)/1000);
		#else
		
		EncorderOffset1=(30.625*EncorderParaTemp1*240)/1000;
		EncorderOffset2=EncorderOffset1+(32.091*EncorderParaTemp2*(450-240))/1000;
		EncorderOffset3=EncorderOffset2+(33.504*EncorderParaTemp3*(780-450)/1000);
		#endif
		#if 0
		Printf("\r\nEncorderOffset1=%d ",(WORD)(EncorderOffset1*100));
		Printf("\r\nEncorderOffset2=%d ",(WORD)(EncorderOffset2*100));
		Printf("\r\nEncorderOffset3=%d ",(WORD)(EncorderOffset3*100));
		#endif
		//Printf("\r\nEncorderOffset4=%d ",(WORD)(EncorderOffset4*100));
		
		if(EncorderCount>=0)
			{
			EncorderCountPN=0;
			EncorderCountTemp=(EncorderCount/2);
			}
		else
			{
			EncorderCountPN=1;
			EncorderCountTemp=(0-EncorderCount)/2;
			}
		//EncorderCountTemp+=450;
		//EncorderCount
		#if 0
		if(EncorderCount>=0)
			EncorderCountPN=0;
		else
			EncorderCountPN=1;
		#endif
		#if 0
		if(EncorderCountTemp>EncorderLen_Offset)
			{
			EncorderCountPN=0;
			temp_EncorderCountTemp=EncorderCountTemp-EncorderLen_Offset;
			}
		else
			{
			EncorderCountPN=1;
			temp_EncorderCountTemp=EncorderLen_Offset-EncorderCountTemp;
			}
		#endif
		if(EncorderCountTemp<=240&&EncorderCountTemp>=0)
			{
			
			#if (HS_DEBUG==ON)
			Printf("\r\nEncorderCountTemp=%d ",(WORD)EncorderCountTemp);
			Printf("\r\nEncorderLen_Offset=%d ",(WORD)EncorderLen_Offset);
			Printf("\r\nEncorderCountPN=%d ",(WORD)EncorderCountPN);
			#endif
			//if(EncorderCountPN==1)
			#if 0
				EncorderLen=(30.625*EncorderParaTemp1*(temp_EncorderCountTemp))/1000;
			#else
			EncorderLen=(30.625*EncorderParaTemp1*EncorderCountTemp)/1000;
			#endif
				//EncorderLen=(30.625*EncorderParaTemp1*(EncorderCountTemp-EncorderLen_Offset))/1000;
				
			//EncorderLen=(30.625*1*EncorderCountTemp)/1000;
			temp_val=30.625*EncorderParaTemp1*EncorderCountTemp;
			EncorderLenint=EncorderLen+0.05f;
			//Printf("\r\nEncorderLenTemp=%d ",(WORD)EncorderLenTemp);
			//EncorderLenint=EncorderLen;		
			#if (_2ND_DECIMAL==ON)//ryan@20200505
			EncorderLenfloaat=(EncorderLen+0.05)*100;
			EncorderLenfloaat%=100;
			#else
			EncorderLenfloaat=(EncorderLen+0.05)*10;
			EncorderLenfloaat%=10;
			#endif
			#if 0
			Printf("\r\nEncorderCountTemp=%d ",(WORD)EncorderCountTemp);
			Printf("\r\nEncorderLen=%d ",(WORD)EncorderLen);
			Printf("\r\nEncorderLenint=%d ",(WORD)EncorderLenint);			
			Printf("\r\ntemp_val=%d ",(WORD)temp_val);
			Printf("\r\nEncorderCount=%d ",(WORD)EncorderCount);
			Printf("\r\nEncorderLenTemp=%d.%dm ",(WORD)EncorderLenint,(WORD)EncorderLenfloaat);
			#endif
			}
		else if(EncorderCountTemp>240&&EncorderCountTemp<=450)
			{
	
			#if 0
			EncorderLen=EncorderOffset1+(32.091*EncorderParaTemp2*(temp_EncorderCountTemp-(240-EncorderLen_Offset))/1000);
			#else
			EncorderLen=EncorderOffset1+(32.091*EncorderParaTemp2*(EncorderCountTemp-240)/1000);
			#endif
			
			#if (HS_DEBUG==ON)
			Printf("\r\ntemp_EncorderCountTemp=%d ",(WORD)(temp_EncorderCountTemp-(240-EncorderLen_Offset)));
			Printf("\r\nEncorderLen_Offset=%d ",(WORD)EncorderLen_Offset);
			Printf("\r\ntemp_EncorderCountTemp=%d ",(WORD)temp_EncorderCountTemp);
			Printf("\r\nEncorderLen=%d ",(WORD)EncorderLen);
			//EncorderLen=7.35+(32.091*1*(EncorderCountTemp-240)/1000);
			Printf("\r\nEncorderCount=%d ",(WORD)EncorderCount);
			#endif
			EncorderLenint=EncorderLen+0.05f;			
			#if (HS_DEBUG==ON)
			Printf("\r\nEncorderLenint=%d ",(WORD)EncorderLenint);
			#endif
			//EncorderLenint=EncorderLen;
			#if (_2ND_DECIMAL==ON)//ryan@20200505
			EncorderLenfloaat=(EncorderLen+0.05)*100;
			EncorderLenfloaat%=100;
			#else
			EncorderLenfloaat=(EncorderLen+0.05)*10;
			EncorderLenfloaat%=10;
			#endif
			
			#if (HS_DEBUG==ON)
			Printf("\r\nEncorderLenTemp=%d.%dm ",(WORD)EncorderLenint,(WORD)EncorderLenfloaat);
			#endif			
			}
		#if 1
		else if(EncorderCountTemp>450&&EncorderCountTemp<=780)
			{
			#if 0
			EncorderLen=EncorderOffset2+(33.504*1*(temp_EncorderCountTemp-(450-EncorderLen_Offset))/1000);
			#else			
			EncorderLen=EncorderOffset2+(33.504*EncorderParaTemp3*(EncorderCountTemp-450)/1000);
			#endif
			
			#if (HS_DEBUG==ON)
			Printf("\r\nEncorderCount=%d ",(WORD)EncorderCount);
			#endif
			
			EncorderLenint=EncorderLen+0.05f;
			//Printf("\r\nEncorderLenTemp=%d ",(WORD)EncorderLenTemp);
			//EncorderLenint=EncorderLen;
			#if (_2ND_DECIMAL==ON)//ryan@20200505
			EncorderLenfloaat=(EncorderLen+0.05)*100;
			EncorderLenfloaat%=100;
			#else
			EncorderLenfloaat=(EncorderLen+0.05)*10;
			EncorderLenfloaat%=10;
			#endif
			
			#if (HS_DEBUG==ON)
			Printf("\r\nEncorderLenTemp=%d.%dm ",(WORD)EncorderLenint,(WORD)EncorderLenfloaat);
			#endif
			}
		#else
		else if(EncorderCountTemp>450&&EncorderCountTemp<=600)
			{
			EncorderLen=14.089+(33.504*EncorderParaTemp3*(EncorderCountTemp-450)/1000);
			//EncorderLen=14.089+(33.504*1*(EncorderCountTemp-450)/1000);
			Printf("\r\nEncorderCount=%d ",(WORD)EncorderCount);

			EncorderLenint=EncorderLen+0.05f;
			//Printf("\r\nEncorderLenTemp=%d ",(WORD)EncorderLenTemp);
			//EncorderLenint=EncorderLen;
			EncorderLenfloaat=(EncorderLen+0.05)*10;
			EncorderLenfloaat%=10;
			Printf("\r\nEncorderLenTemp=%d.%dm ",(WORD)EncorderLenint,(WORD)EncorderLenfloaat);
			}		
		else if(EncorderCountTemp>600&&EncorderCountTemp<=780)
			{
			EncorderLen=19.089+(33.504*((EncorderParaTemp3+EncorderParaTemp4)/2)*(EncorderCountTemp-600)/1000);
			//EncorderLen=14.089+(33.504*1*(EncorderCountTemp-450)/1000);
			Printf("\r\nEncorderCount=%d ",(WORD)EncorderCount);

			EncorderLenint=EncorderLen+0.05f;
			//Printf("\r\nEncorderLenTemp=%d ",(WORD)EncorderLenTemp);
			//EncorderLenint=EncorderLen;
			EncorderLenfloaat=(EncorderLen+0.05)*10;
			EncorderLenfloaat%=10;
			Printf("\r\nEncorderLenTemp=%d.%dm ",(WORD)EncorderLenint,(WORD)EncorderLenfloaat);
			}
		#endif
		else if(EncorderCountTemp>780)
			{
			#if 0
			EncorderLen=EncorderOffset3+(34.896*1*(EncorderCountTemp-(780-EncorderLen_Offset))/1000);
			#else
			EncorderLen=EncorderOffset3+(34.896*EncorderParaTemp4*(EncorderCountTemp-780)/1000);
			#endif

			#if (HS_DEBUG==ON)	
			Printf("\r\nEncorderCount=%d ",(WORD)EncorderCount);

			Printf("\r\nEncorderLen=%d ",(WORD)EncorderLen);
			#endif
			EncorderLenint=EncorderLen+0.05f;
			#if (HS_DEBUG==ON)
			Printf("\r\nEncorderLenint=%d ",(WORD)EncorderLenint);
			#endif
			//EncorderLenint=EncorderLen;
			#if (_2ND_DECIMAL==ON)//ryan@20200505
			EncorderLenfloaat=(EncorderLen+0.05)*100;
			#if (HS_DEBUG==ON)
			Printf("\r\nEncorderLenfloaat=%d ",(WORD)EncorderLenfloaat);
			#endif
			EncorderLenfloaat%=100;
			#if (HS_DEBUG==ON)		
			Printf("\r\nEncorderLenTemp=%d.%dm ",(WORD)EncorderLenint,(WORD)EncorderLenfloaat);
			#endif
			#else
			EncorderLenfloaat=(EncorderLen+0.05)*10;
			#if (HS_DEBUG==ON)
			Printf("\r\nEncorderLenfloaat=%d ",(WORD)EncorderLenfloaat);
			#endif
			EncorderLenfloaat%=10;			
			#if (HS_DEBUG==ON)
			Printf("\r\nEncorderLenTemp=%d.%dm ",(WORD)EncorderLenint,(WORD)EncorderLenfloaat);
			#endif
			#endif			
			}
			
		ChangeKey=0;
		//param[0]=EncorderLenint;
		//param[1]=EncorderLenfloaat;
		//param[2]=EncorderCountPN;
		//protocol_send_cmd_with_param_num(0x0a, (BYTE *)param, 3);
		//protocol_send_cmd_with_param_num(0x80, (BYTE *)param, 3);
		 if(GET_DVR_SystemReadyNotic()==_TRUE)
		 	{
		 MCU_SendCmdToDVR(MCU_PROTOCOL_CMD_REGULAR_DATA);
		  MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_REPLAY_REGULAR_DATA);
		  MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_REPLAY_REGULAR_DATA);
		 	}
		#if (_DEBUG_EncorderHandler==ON)//ryan@20200505
		Printf("\r\nEncorderLenint=%d ",(WORD)EncorderLenint);
		Printf("\r\nEncorderLenfloaat=%d ",(WORD)EncorderLenfloaat);
		#endif
	}
	//GetKey(0);
}

	void EncorderHandler_TELI(void)
	{
	DWORD EncorderCountTemp,temp_val;
#if (HS_DEBUG==ON)
	DWORD  temp_EncorderCountTemp;
#endif
	long  EncorderCount_T;
	
	//BYTE param[4];
	float EncorderParaTemp1,EncorderParaTemp2,EncorderParaTemp3,EncorderParaTemp4;
	float EncorderOffset1,EncorderOffset2,EncorderOffset3/*,EncorderOffset4*/;

	
			
		if(ChangeKey)
		{					
			EncorderCount_T=(EncorderCountINT);
		#if (HS_DEBUG==ON)
			GraphicsPrint(RED,"\r\n(TELI)EncorderCount=%d ",(WORD)EncorderCount);	
			GraphicsPrint(GREEN,"\r\nEncorderCountINT=%d",(WORD)EncorderCountINT);
			Printf("\r\n(TELI)EncorderCount_T=%d",(WORD)((EncorderCount_T)));	
			Printf("\r\nGetRotateNumberTELI=%01x%04x",(WORD)(ulongRotateNumberTELI>>16),(WORD)ulongRotateNumberTELI);			

		#endif
			//Printf("\r\nEncorderLen_Offset0=%d ",(WORD)EncorderLen_Offset);
			//Printf("\r\nEncorderCountPN0=%d ",(WORD)EncorderCountPN);
		#if 1//ryan@20200430
	//		EncorderParaTemp1=(Encorder1+(Decimal1))*0.01f;
	//		EncorderParaTemp2=(Encorder2+(Decimal2))*0.01f;
	//		EncorderParaTemp3=(Encorder3+(Decimal3))*0.01f;
	//		EncorderParaTemp4=(Encorder4+(Decimal4))*0.01f;
			EncorderParaTemp1=(TEncorder1+(TDecimal1*0.01));
			EncorderParaTemp2=(TEncorder2+(TDecimal2*0.01));
			EncorderParaTemp3=(TEncorder3+(TDecimal3*0.01));
			EncorderParaTemp4=(TEncorder4+(TDecimal4*0.01));
	
		#else
			EncorderParaTemp1=(Encorder1)*0.01f;
			EncorderParaTemp2=(Encorder2)*0.01f;
			EncorderParaTemp3=(Encorder3)*0.01f;
			EncorderParaTemp4=(Encorder4)*0.01f;
		#endif
			//EncorderParaTemp1=(Encorder1*0.01f)+Decimal1;
			//EncorderParaTemp2=(Encorder2*0.01f)+Decimal2;
			//EncorderParaTemp3=(Encorder3*0.01f)+Decimal3;
			//EncorderParaTemp4=(Encorder4*0.01f)+Decimal4;
			
		#if 0
			Printf("\r\nEncorder1=%d ",(WORD)Encorder1);
			Printf("\r\nEncorder2=%d ",(WORD)Encorder2);
			Printf("\r\nEncorder3=%d ",(WORD)Encorder3);
			Printf("\r\nEncorder4=%d ",(WORD)Encorder4);
			
			Printf("\r\nEncorderParaTemp1=%d ",(WORD)(EncorderParaTemp1*100));
			Printf("\r\nEncorderParaTemp2=%d ",(WORD)(EncorderParaTemp2*100));
			Printf("\r\nEncorderParaTemp3=%d ",(WORD)(EncorderParaTemp3*100));
			Printf("\r\nEncorderParaTemp4=%d ",(WORD)(EncorderParaTemp4*100));
		#endif
		#if 0
			EncorderOffset1=(30.625*EncorderParaTemp1*(240-EncorderLen_Offset))/1000;
			EncorderOffset2=EncorderOffset1+(32.091*EncorderParaTemp2*(450-240-EncorderLen_Offset))/1000;
			EncorderOffset3=EncorderOffset2+(33.504*EncorderParaTemp3*(780-450-EncorderLen_Offset)/1000);
			//EncorderOffset4=EncorderOffset3+(33.504*((EncorderParaTemp3+EncorderParaTemp4)/2)*(780-600)/1000);
		#else
			
			EncorderOffset1=(30.625*EncorderParaTemp1*240)/1000;
			EncorderOffset2=EncorderOffset1+(32.091*EncorderParaTemp2*(450-240))/1000;
			EncorderOffset3=EncorderOffset2+(33.504*EncorderParaTemp3*(780-450)/1000);
		#endif
		#if 0
			Printf("\r\nEncorderOffset1=%d ",(WORD)(EncorderOffset1*100));
			Printf("\r\nEncorderOffset2=%d ",(WORD)(EncorderOffset2*100));
			Printf("\r\nEncorderOffset3=%d ",(WORD)(EncorderOffset3*100));
		#endif
			//Printf("\r\nEncorderOffset4=%d ",(WORD)(EncorderOffset4*100));
			
			if(EncorderCount_T>=0)
				{
				EncorderCountPN=0;
				EncorderCountTemp=(EncorderCount_T);
				}
			else
				{
				EncorderCountPN=1;
				EncorderCountTemp=(0-EncorderCount_T);
				}
			
			//EncorderCountTemp+=450;
			//EncorderCount
		#if 0
			if(EncorderCount>=0)
				EncorderCountPN=0;
			else
				EncorderCountPN=1;
		#endif
		#if 0
			if(EncorderCountTemp>EncorderLen_Offset)
				{
				EncorderCountPN=0;
				temp_EncorderCountTemp=EncorderCountTemp-EncorderLen_Offset;
				}
			else
				{
				EncorderCountPN=1;
				temp_EncorderCountTemp=EncorderLen_Offset-EncorderCountTemp;
				}
		#endif
			if(EncorderCountTemp<=240&&EncorderCountTemp>=0)
				{
				
			#if (HS_DEBUG==ON)
				Printf("\r\n(TELI)EncorderCountTemp=%d ",(WORD)EncorderCountTemp);
				Printf("\r\n(TELI)EncorderLen_Offset=%d ",(WORD)EncorderLen_Offset);
				Printf("\r\n(TELI)EncorderCountPN=%d ",(WORD)EncorderCountPN);
			#endif
				//if(EncorderCountPN==1)
			#if 0
					EncorderLen=(30.625*EncorderParaTemp1*(temp_EncorderCountTemp))/1000;
			#else
				//EncorderLen=(30.625*EncorderParaTemp1*EncorderCountTemp)/1000;
				//EncorderLen=((143.4)*EncorderParaTemp1*EncorderCountTemp)/1000;
				EncorderLen=(143.4*EncorderParaTemp1*EncorderCountTemp)/100000;
			
			#endif
					//EncorderLen=(30.625*EncorderParaTemp1*(EncorderCountTemp-EncorderLen_Offset))/1000;
					
				//EncorderLen=(30.625*1*EncorderCountTemp)/1000;
				temp_val=30.625*EncorderParaTemp1*EncorderCountTemp;
				EncorderLenint=EncorderLen/*+0.05f*/;
				//Printf("\r\nEncorderLenTemp=%d ",(WORD)EncorderLenTemp);
				//EncorderLenint=EncorderLen;		
			#if (_2ND_DECIMAL==ON)//ryan@20200505
				EncorderLenfloaat=(EncorderLen/*+0.05*/)*100;
				EncorderLenfloaat%=100;
			#else
				EncorderLenfloaat=(EncorderLen+0.05)*10;
				EncorderLenfloaat%=10;
			#endif
			#if 0
				Printf("\r\nEncorderCountTemp=%d ",(WORD)EncorderCountTemp);
				Printf("\r\nEncorderLen=%d ",(WORD)EncorderLen);
				Printf("\r\nEncorderLenint=%d ",(WORD)EncorderLenint);			
				Printf("\r\ntemp_val=%d ",(WORD)temp_val);
				Printf("\r\nEncorderCount=%d ",(WORD)EncorderCount);
				Printf("\r\nEncorderLenTemp=%d.%dm ",(WORD)EncorderLenint,(WORD)EncorderLenfloaat);
			#endif
				}
			else if(EncorderCountTemp>240&&EncorderCountTemp<=450)
				{
		
			#if 0
				EncorderLen=EncorderOffset1+(32.091*EncorderParaTemp2*(temp_EncorderCountTemp-(240-EncorderLen_Offset))/1000);
			#else
				//EncorderLen=EncorderOffset1+(32.091*EncorderParaTemp2*(EncorderCountTemp-240)/1000);
				EncorderLen=(143.4*EncorderParaTemp1*EncorderCountTemp)/100000;
			#endif
				
			#if (HS_DEBUG==ON)
				Printf("\r\n(TELI)temp_EncorderCountTemp=%d ",(WORD)(temp_EncorderCountTemp-(240-EncorderLen_Offset)));
				Printf("\r\n(TELI)EncorderLen_Offset=%d ",(WORD)EncorderLen_Offset);
				Printf("\r\n(TELI)temp_EncorderCountTemp=%d ",(WORD)temp_EncorderCountTemp);
				Printf("\r\n(TELI)EncorderLen=%d ",(WORD)EncorderLen);
				//EncorderLen=7.35+(32.091*1*(EncorderCountTemp-240)/1000);
				Printf("\r\n(TELI)EncorderCount_T=%d ",(WORD)EncorderCount_T);
			#endif
				EncorderLenint=EncorderLen/*+0.05f*/;			
			#if (HS_DEBUG==ON)
				Printf("\r\n(TELI)EncorderLenint=%d ",(WORD)EncorderLenint);
			#endif
				//EncorderLenint=EncorderLen;
			#if (_2ND_DECIMAL==ON)//ryan@20200505
				EncorderLenfloaat=(EncorderLen/*+0.05*/)*100;
				EncorderLenfloaat%=100;
			#else
				EncorderLenfloaat=(EncorderLen+0.05)*10;
				EncorderLenfloaat%=10;
			#endif
				
			#if (HS_DEBUG==ON)
				Printf("\r\n(TELI)EncorderLenTemp=%d.%dm ",(WORD)EncorderLenint,(WORD)EncorderLenfloaat);
			#endif			
				}
		#if 1
			else if(EncorderCountTemp>450&&EncorderCountTemp<=780)
				{
			#if 0
				EncorderLen=EncorderOffset2+(33.504*1*(temp_EncorderCountTemp-(450-EncorderLen_Offset))/1000);
			#else			
				//EncorderLen=EncorderOffset2+(33.504*EncorderParaTemp3*(EncorderCountTemp-450)/1000);
				EncorderLen=(143.4*EncorderParaTemp1*EncorderCountTemp)/100000;
			#endif
				
			#if (HS_DEBUG==ON)
				Printf("\r\n(TELI)EncorderCount_T=%d ",(WORD)EncorderCount_T);
			#endif
				
				EncorderLenint=EncorderLen/*+0.05f*/;
				//Printf("\r\nEncorderLenTemp=%d ",(WORD)EncorderLenTemp);
				//EncorderLenint=EncorderLen;
			#if (_2ND_DECIMAL==ON)//ryan@20200505
				EncorderLenfloaat=(EncorderLen/*+0.05*/)*100;
				EncorderLenfloaat%=100;
			#else
				EncorderLenfloaat=(EncorderLen+0.05)*10;
				EncorderLenfloaat%=10;
			#endif
				
			#if (HS_DEBUG==ON)
				Printf("\r\n(TELI)EncorderLenTemp=%d.%dm ",(WORD)EncorderLenint,(WORD)EncorderLenfloaat);
			#endif
				}
		#else
			else if(EncorderCountTemp>450&&EncorderCountTemp<=600)
				{
				EncorderLen=14.089+(33.504*EncorderParaTemp3*(EncorderCountTemp-450)/1000);
				//EncorderLen=14.089+(33.504*1*(EncorderCountTemp-450)/1000);
				Printf("\r\nEncorderCount=%d ",(WORD)EncorderCount);
	
				EncorderLenint=EncorderLen+0.05f;
				//Printf("\r\nEncorderLenTemp=%d ",(WORD)EncorderLenTemp);
				//EncorderLenint=EncorderLen;
				EncorderLenfloaat=(EncorderLen+0.05)*10;
				EncorderLenfloaat%=10;
				Printf("\r\nEncorderLenTemp=%d.%dm ",(WORD)EncorderLenint,(WORD)EncorderLenfloaat);
				}		
			else if(EncorderCountTemp>600&&EncorderCountTemp<=780)
				{
				EncorderLen=19.089+(33.504*((EncorderParaTemp3+EncorderParaTemp4)/2)*(EncorderCountTemp-600)/1000);
				//EncorderLen=14.089+(33.504*1*(EncorderCountTemp-450)/1000);
				Printf("\r\nEncorderCount=%d ",(WORD)EncorderCount);
	
				EncorderLenint=EncorderLen+0.05f;
				//Printf("\r\nEncorderLenTemp=%d ",(WORD)EncorderLenTemp);
				//EncorderLenint=EncorderLen;
				EncorderLenfloaat=(EncorderLen+0.05)*10;
				EncorderLenfloaat%=10;
				Printf("\r\nEncorderLenTemp=%d.%dm ",(WORD)EncorderLenint,(WORD)EncorderLenfloaat);
				}
		#endif
			else if(EncorderCountTemp>780)
				{
			#if 0
				EncorderLen=EncorderOffset3+(34.896*1*(EncorderCountTemp-(780-EncorderLen_Offset))/1000);
			#else
				//EncorderLen=EncorderOffset3+(34.896*EncorderParaTemp4*(EncorderCountTemp-780)/1000);
				EncorderLen=(143.4*EncorderParaTemp1*EncorderCountTemp)/100000;
			#endif
	
			#if (HS_DEBUG==ON)	
				Printf("\r\n(TELI)EncorderCount_T=%d ",(WORD)EncorderCount_T);
				Printf("\r\n(TELI)EncorderLen=%d ",(WORD)EncorderLen);
			#endif
				EncorderLenint=EncorderLen/*+0.05f*/;

			#if (HS_DEBUG==ON)
				Printf("\r\n(TELI)EncorderLenint=%d ",(WORD)EncorderLenint);
			#endif
				//EncorderLenint=EncorderLen;
			#if (_2ND_DECIMAL==ON)//ryan@20200505
			EncorderLenfloaat=(EncorderLen/*+0.05*/)*100;
			#if (HS_DEBUG==ON)
			Printf("\r\n(TELI)EncorderLenfloaat=%d ",(WORD)EncorderLenfloaat);
			#endif
			EncorderLenfloaat%=100;

			#if (HS_DEBUG==ON)		
			Printf("\r\n(TELI)EncorderLenTemp=%d.%dm ",(WORD)EncorderLenint,(WORD)EncorderLenfloaat);
			#endif
			#else
				EncorderLenfloaat=(EncorderLen/*+0.05*/)*10;
			#if (HS_DEBUG==ON)
				Printf("\r\n(TELI)EncorderLenfloaat=%d ",(WORD)EncorderLenfloaat);
			#endif
				EncorderLenfloaat%=10;			
			#if (HS_DEBUG==ON)
				Printf("\r\n(TELI)EncorderLenTemp=%d.%dm ",(WORD)EncorderLenint,(WORD)EncorderLenfloaat);
			#endif
			#endif			
				}
				
			ChangeKey=0;
			//param[0]=EncorderLenint;
			//param[1]=EncorderLenfloaat;
			//param[2]=EncorderCountPN;
			//protocol_send_cmd_with_param_num(0x0a, (BYTE *)param, 3);
			//protocol_send_cmd_with_param_num(0x80, (BYTE *)param, 3);

				//if(EncorderLenint>=100)
				//{
				//EncorderLenint=99;   			
				//EncorderLenfloaat=99;
				//GraphicsPrint(RED,"\r\n(TELI)EncorderCountOver");				
				//}
			
			 if(GET_DVR_SystemReadyNotic()==_TRUE)
				{
			 MCU_SendCmdToDVR(MCU_PROTOCOL_CMD_REGULAR_DATA);
			  MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_REPLAY_REGULAR_DATA);
			  MCUTimerActiveTimerEvent(SEC(1), _SYSTEM_TIMER_EVENT_REPLAY_REGULAR_DATA);
				}
		#if (_DEBUG_EncorderHandler==ON)//ryan@20200505
			Printf("\r\n(TELI)EncorderLenint=%d ",(WORD)EncorderLenint);
			Printf("\r\n(TELI)EncorderLenfloaat=%d ",(WORD)EncorderLenfloaat);
		#endif
		}
		//GetKey(0);
	}

	//bit CCFL_status;	
void main_loop(void)
{

	Printf("\r\nStart Main Loop...");

	ulongRotateNumber=GetRotateNumber(0);
	ulongRotateNumberTELI=GetRotateNumber(1);

	//---------------------------------------------------------------
	//			             Main Loop
	//---------------------------------------------------------------
	while(1) 
	{
	

		#if (_DEBUG_MESSAGE_Monitor==ON)
		while( RS_ready() ) 
			Monitor();				// for new monitor functions		
		#endif		
		
		
		DvrReceivePaser();		
		SysTimerHandler();			

	//	SysJudgeHandler();	
	
		SysPowerHandler();	
		Scankey();
		
		if(bytFastEncoderMode==ON)
		EncorderHandler_TELI();
		else			
		EncorderHandler();
		

	

			if(PowerFlag==OFF)   //// POWEROFF
			break;
		
	}

}

void main(void)
{

InitCPU();

SysPowerInitial();

Printf("\r\nStart Boot...");

main_init();

//set panel 
//select UP
SET_P_IO1();
//Select Left
SET_P_IO2();

//variable initialize
InitVars();

//load eeprom data
LoadEEPROM();

//event initialize
InitialTimerEvent();

SetAD5110Step(CameraVolt);
 Printf("\r\n(CameraVolt=%d)",(WORD)CameraVolt);

//if(PWR_START_flag==ON)
//	PowerFlag=ON;

if((GET_BATTERY_CAPACITY_LOW_FLAG()==_TRUE)&&(Check_ADAP_IN()==_FALSE))
	PowerFlag=OFF;

  if(PowerFlag==ON)
  SET_TARGET_POWER_STATUS(_POWER_STATUS_NORMAL);
else
{
	//UserInterfacePowerSwitch(_POWER_ACTION_AC_ON_TO_OFF);
	SET_POWER_STATUS(_POWER_STATUS_NORMAL);	
  SET_TARGET_POWER_STATUS(_POWER_STATUS_OFF);
  
}
	RepeatKey=OFF;	
//   SysJudgeStateChangeEvent(_MODE_STATUS_POWER_OFF);    

	while(1) {
		if(PowerFlag==ON)
		main_loop();		//exit when power off
		
		WaitPowerOn();
	}
}

void WaitPowerOn(void)
{

	Printf("\r\n(Going to power off...)");
	SET_TARGET_POWER_STATUS(_POWER_STATUS_OFF);

	if(GET_DVR_EntrySleepMode()==TRUE)
		CLR_DVR_EntrySleepMode();

	CLR_DVR_SystemReadyNotic();	
	MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_JUDGE_WDT_ECHO);		
	CLR_DVR_Shutdown();
	MCUTimerCancelTimerEvent( _USER_TIMER_EVENT_OSD_DVR_SHUTDOWN);

	//CLR_BATTERY_CAPACITY_HIGH_FLAG();
	Power_down_mode=_DontgoingToPD;
	P3M1=0x00;//p3.4 and P3.5 set output ryan@20210226
	bytHoldOn3SPowerOff=OFF;

	//if(bytFastEncoderMode==ON)
	SaveRotateNumber();
	
	while(1) 
	{

			
				#if (_DEBUG_MESSAGE_Monitor==ON)
				while( RS_ready() ) 
				Monitor();				// for new monitor functions		
				#endif	

			SysPowerHandler();	
			SysTimerHandler();
			Scankey();
			
			if(PowerFlag==ON)   //// POWER-ON
			break;

			#if (_POWER_DOWN_ENABLE==ON)

			if((Power_down_mode==_DontgoingToPD)&&(GET_POWER_STATUS()==_POWER_STATUS_OFF)&&(GET_AC_PLUG()/*Check_ADAP_IN()*/==_FALSE))
			{
			Power_down_mode=_ReadyToPD;
			MCUTimerActiveTimerEvent(SEC(10), _SYSTEM_TIMER_POWER_DOWN_MODE);
			}	
			else if((Check_ADAP_IN()==_TRUE)||(((~P4>>3)& 0x01) ))
				{
				Power_down_mode=_DontgoingToPD;
				MCUTimerCancelTimerEvent( _SYSTEM_TIMER_POWER_DOWN_MODE);
				}
			else	if(Power_down_mode==_GoingToPD)
				SET_MCU_POWER_DOWN_MODE();				

			#endif	
			
	}


	WriteEEP(EEP_PowerOffToOnflag,ON);
	EncorderCount=0;
	EncorderLenint=0;
	EncorderLenfloaat=0;
	EncorderCountPN=0;
	 RS2_in=0; //clean DVR CMD.
	RS2_out=0;//clean DVR CMD.
	//CLR_BATTERY_CAPACITY_LOW_FLAG();	
        CLR_BATTERY_ABNORMAL();
	SET_BATTERY_CHARGE_STATE(_BATT_STATUS_NONE);
	SET_TARGET_POWER_STATUS(_POWER_STATUS_NORMAL);	
	Printf("\r\n(Start up  Power On Mode...)");

	ISPCR = 0x20;/// reboot to AP.

}

