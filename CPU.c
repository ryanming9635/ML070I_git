/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  CPU.c                                                            										*/
/*                                                                           										*/
/*****************************************************************************/
#include<intrins.h>       ///_nop_(); 
#include "config.h"
#include "typedefs.h"
#include "reg.h"
#include "cpu.h"
#include "main.h"
#include "I2C.h"
#include "Printf.h"
#include "etc_eep.h"
#include <math.h>
#include "main.h"


extern WORD g_usTimerCounter ;	
extern StructPowerInfoType idata g_stPowerInfo;
extern  BYTE DebugLevel;
extern bit g_bNotifyTimer0Int;
extern StructBatteryInfoType g_stBatteryInfo;
extern WORD BatteryBTH;
extern StructBatteryInfoType g_stBatteryInfo ;
extern BYTE	PowerFlag;
extern BYTE Power_down_mode;
extern BYTE DVRChangeCurrent;
extern BYTE bytFastEncoderMode;


BYTE	 year1=0,year2=0,month=0,day=0,hour=0,minute=0,second=0;
BYTE DVR_FACTORY_MODE=0;
BYTE IE_Temp=0;
//===================== Timer =================================================
		DATA WORD keytic=0;
		DATA BYTE Key=0;
		bit KeyReady=0, RepeatKey = ON;

		DATA BYTE tm001;
		DATA  WORD  stopWatch;
		DATA  WORD  tm01=0;
		DATA  BYTE  tic01=0,tic02=0;	//, tic_pc;
		DATA  WORD  tic_pc=0;
		DATA  DWORD	tic_PWR_SAVING=0;
		DATA  DWORD	tic_Init_time=0;
	 volatile bit PWR_SAVING_ACT_FLAG=0;
		extern BYTE Power_Saving_Flag;
		extern bit PWR_OFF_KEY_LOCK;
		DATA DWORD	SystemClock;			//00:00
		BYTE FormatSDCard=1;

#ifdef SERIAL //=======================================================================




static	DATA BYTE	RS_buf[BUF_MAX];
     //   	DATA BYTE   	RS_buf2[BUF_MAX];
		DATA BYTE	DVR_buf[DVR_BUF_MAX];

		
		DATA BYTE   RS_in, RS_out=0;
		DATA BYTE   RS2_in=0,RS2_out=0;
		bit	        RS_Xbusy=0;			// bit RS_Xbusy=0;
		bit 		RS2_Xbusy=0; 		// bit RS_Xbusy=0;
		

#endif	// SERIAL ======================================================================

BYTE 	keyticSec=0;
BYTE	FormatWait=0;
BYTE	PowerDownWait=0;
BYTE  updn_reg[4]={0,0,0,0};

bit ChangeKey;
BYTE Time5ms;
//BYTE FLASH_FLAG;
BYTE bytHoldOn3SPowerOff=0;
BYTE bytHoldOn3SPowerOffMode=0;
WORD LED_FLASH_COUNT,Power_Msg_Count;
short EncorderCount=0;
BYTE bytEncorderCountTemp=0;
extern DWORD ulongRotateNumber;

extern BYTE 	KeyBuffer;

//=============================================================================
//    Main Initialize Routine
//=============================================================================
void main_init (void)
{
	DWORD	rev;

	rev = GetFWRevEE();


	Printf("\r\nPrev.FW:%02x%02x%02x", (WORD) ReadEEP(EEP_FWREV_MAJOR), (WORD) ReadEEP(EEP_FWREV_MINOR1), (WORD) ReadEEP(EEP_FWREV_MINOR2));

if( GetFWRevEE()!=((MCU_FW_Major<<16)|(MCU_FW_Sub0<<8)|(MCU_FW_Sub1)))
		{
		Printf("\r\nCurr.FW:%02x%02x%02x",(WORD) MCU_FW_Major,(WORD) MCU_FW_Sub0,(WORD) MCU_FW_Sub1);
		SaveFWRevEE();
		Printf("\r\nCurr.FW:%02x%02x%02x",(WORD) ReadEEP(EEP_FWREV_MAJOR), (WORD) ReadEEP(EEP_FWREV_MINOR1), (WORD) ReadEEP(EEP_FWREV_MINOR2));		
		}

	//---------- if FW version is not matched, initialize EEPROM data -----------

	if(GetFWRevEE()==((MCU_FW_Major<<16)|(MCU_FW_Sub0<<8)|(MCU_FW_Sub1))) 
	{					// exist EEPROM

		//if( rev !=((MCU_FW_Major<<16)|(MCU_FW_Sub0<<8)|(MCU_FW_Sub1)) )
			{
				if(GetProjectNameEE()==_FALSE)
				ClearBasicEE();
			}
	}
	else 
	{
		Printf("\r\nCannot access EEPROM!!!!");
		Printf("\r\nNo initialize");
		DebugLevel = 0xff;
		return;
	}
	
	delay(100);


	SET_POWER_STATUS(_POWER_STATUS_OFF);
	SET_TARGET_POWER_STATUS(_POWER_STATUS_NORMAL);

}




//=============================================================================
//                            Initialize WINBOND CPU                                                   
//=============================================================================
void InitCPU(void)
{
	/*----- Initialize interrupt -------------*/

//	TH1 = 0xfD;//38400 		//    SMOD = 0      SMOD =1	//andy AWT 980924					
	TH1 = 0xf4; //9600		//    SMOD = 0      SMOD =1	//andy AWT 980924					
						// 0ffh :57600 bps				
						// 0fdh : 9600 bps	
						//0fdh :19200 bps				
						// 0fah : 4800 bps								
						// 0f4h : 2400 bps								
						// 0e8h : 1200 bps								

	SCON = 0x50;		// 0100 0000 mode 1 - 8 bit UART				
						// Enable serial reception						
    TMOD = 0x22;		// 0010 0010 timer 0 - 8 bit auto reload		
						// timer 1 - baud rate generator				
    TCON = 0x55;		// 0101 0001 timer 0,1 run						
						// int 0,  edge triggered						
						// int 1,  edge triggered						
						// TF1 TR1 TF0 TR0	EI1 IT1 EI0 IT0				
#if (_1KHZ_PWM==ON)	
TH0 = TL0 = (255-6); 	// PWM 1Khz frequency

#else
	TH0 = TL0 = (64); 	// 64=4608 Hz at 11.0592MHz
#endif
	PCON = 0x80;		// 0000 0000 SMOD(double baud rate bit) = 1		
	IP	 = 0x02;		// 0000 0000 interrupt priority					
						// -  - PT2 PS PT1 PX1 PT0 PX0	 		         

	#ifdef SERIAL
	IE	 = 0x92;		// 1001 0010 interrupt enable:Serial,TM0		
  						// EA - ET2 ES ET1 EX1 ET0 EX0					

	TI	 = 1;			// LJY000724 // For Starting Serial TX 
	ES   = 1;			// LJY000724
	#else
	IE   = 0x82;
	#endif // SERIAL

	//------------ Timer 2 for Remocon --------------------------------
	#if (_1KHZ_PWM==ON)
	TL2=0x64-20;//0xE3;
	TH2=0xFF;
	RCAP2L=0x64-20;//0xE3;
	RCAP2H=0xFF;

	T2CON  = 0x00;				// Timer2 Clear
	TR2	   = 0;
	ET2    = 1;

	T2CON = 0x04;				// Timer2 run
	AUXR |=0x80;	//Timer1 for UART2	
	#else
	T2CON  = 0x00;				// Timer2 Clear
	TR2	   = 0;
	ET2    = 1;
	#endif
	//-----------------------------------------------------------------

	//Uart2
	//P0M0=0x7F;
	//P1M0=0x04;
       S2CON=0x50;                                 //Set UART Mode 1
       S2BRT=0xf4;                                 //Set 2UART overflow rate
    	//AUXR2=0x08;                                 //SMOD = 1,TX12 = 0 (/12)
    	//AUXR2|=0x10;                                //S2TR = 1
	AUXR2 |= 0x18;

	// Enable serial interrupt
	AUXIE |= 0x10;///EX2=1

	RS2_Xbusy=0;
	//PCA counter..
	CL=0; 
	CH=0;
	//AUXR2|=0x80;	//Fosc/12 for timer-0
	//CMOD=0x00; // //PCA clock is Sysclk / 12   7.2KHz
	//CMOD=0x02; // //PCA clock is Sysclk / 2   //43Khz
	CMOD=0x04; // //PCA clock is timer0  37HZ
	CCON|=	0x40;				//PCA Run
	//set pwm port 1.4 & 1.5
  	//PCAPWM3 = 0x80;

	//P1M0=0x80;					//set input p1.6 
	//P1M0=0xC0;///add ADC AIN P1.6 & P1.7  (input mode)
  	//P2M0=0x01;
	//P2M1=0x01;
	//P3M0=0x00;
	//P3M1=0x20;
	P0M0|=0x20|0x80;  //p0_5  //P0_7

	//P1M0|=0x04|0x40|0x20; ////set input p1.6 and P1.5 for adc function
	P1M0|=(0x04|0x80|0x40);///adc p1.7,P1.6
		
	P2M1=0x40;		//|P2_6 set push pull mode ryan@20210312

	P3M0=0x0C;
	P3M1=0x00;//|0x30;//p3.4 and P3.5 set push pull mode ryan@20210222
	P4M0=0x0C;
	P4M1=0x00;

}

#if (_POWER_DOWN_ENABLE==ON)
void SET_MCU_POWER_DOWN_MODE(void)
{

	Printf("(SET_MCU_POWER_DOWN_MODE) ");

	 EX2 = 1;
	 IE2 = 0;  ///INT2 wakeup
	 IT2=1;

	EX3 = 1;
	IE3= 0;  ///INT3 wakeup
	IT3=1;

	IE_Temp=IE;
	IE=0x80;

	Power_down_mode=1;
	P4_0 = 1;	//LED RED off
	P4_1 = 1;	//LED GREEN off
	PCON=0x82;	//inter power down  mode
	_nop_(); 
	_nop_(); 
	_nop_(); 

//	InitCPU();

}
#endif

BYTE Check_ADAP_IN(void)	
{

if((GET_ADAP_12()==_TRUE)||(P4_2==0))		
{
	MCUTimerDelayXms(50);
	
	if((GET_ADAP_12()==_TRUE)||(P4_2==0))		
		return _TRUE;
	else
		return _FALSE;
}
else
	return _FALSE;
}

void SET_PWM(BYTE index, BYTE val)
{
//#if (_DEBUG_MESSAGE_PWM_TEST)
	Printf("\r\n(SET_PWM=%02x val=%02x)",(WORD)index,(WORD)val);
//#endif
switch(index)
{
case _CHG_CURR:  //P1_4
	#if 1
		CCAP2L=0x80;  
		CCAP2H =255-val;	
		CCAPM2=0x42;

			if(val==_CHARGE1100mA)
			{
			CCAPM2=0x00;	
			P1_4=0;
			}
			
		if(DVRChangeCurrent==0x10)	
			{			
			CCAP2L=0x80;  
			CCAP2H =255-_CHARGE300mA;	
			CCAPM2=0x42;	
			Printf("(*DVRChangeCurrent=300mA)");
			}
		else if(DVRChangeCurrent==0x11)	
			{
			CCAP2L=0x80;  
			CCAP2H =255-_CHARGE1000mA;	
			CCAPM2=0x42;	
			Printf("(*DVRChangeCurrent=1000mA)");
			}
		
/*	
		if(val==High_Current)
			{
			CCAPM2=0x00;	
			P1_4=0;
			}

		else
			{
			CCAPM2=0x00;	
			P1_4=1;

			}
*/
	#else
		#if 0
		if(val==High_Current)
			P1_4=0;
		else
			P1_4=1;
		#else
			CCAP2L=0x80;  
			CCAP2H =255-val;	
			CCAPM2=0x42;   	

			if(val==High_Current)
				{
				CCAPM2=0x00;	
				P1_4=0;
				}
	#endif		
		#endif
			 break; 
 case _BL_PWM:  //P1_5
			 CCAP3L=0x80;  
			 CCAP3H =255-val;	 
		 	CCAPM3=0x42;				
			 break;
 case _RUN_PWM:
		 	CCON	|=	0x40;				//RUN  PCA Counter	
		 	break;
 case _STOP_PWM:
		 	CCON	&=	~(0xBF);				//STOP PCA Counter
	 	    	break;
 	
default: 
		  break;
}


}


#define _ReadKey()		 ((~P4>>3)& 0x01)  //PSW P4.3


////////////////////////////////

//INTERRUPT(1, timer0_int)
void timer0_int(void) interrupt 1 using 1			// interrupt number1, bank register 2
{

 #if  (_1KHZ_PWM==ON)
 #else
BYTE TempKey;

	tm001++;

    	//Timer Counter 
	  tic02++;
	

	if(tic02>=10)   
	{
	g_usTimerCounter += 1;     ///per 103us  interrupt
	tic02=0;
	g_bNotifyTimer0Int=_TRUE;
	}	

		if(Time5ms)
			Time5ms--;
		if(Time5ms==0)
		{
			//TempKey=(( (P2&0xC0) >> 6) & 0x03);
			TempKey=(( (P3&0x0C) >> 2) & 0x03); //ENA&ENB, P3.2, P3.3 
			if(updn_reg[3] !=TempKey )// (( (P2&0x60) >> 5) & 0x03))	//Abel for HS SW key ......951102										
			{																						   
				updn_reg[0] = updn_reg[1];																
				updn_reg[1] = updn_reg[2];																
				updn_reg[2] = updn_reg[3];																
				updn_reg[3] =TempKey;// ( (P2&0x60) >> 5) & 0x03; 
				ChangeKey=1;		
			//else			//william-v1.42-961130
			//ChangeKey=0;	//william-v1.42-961130
		if(bytFastEncoderMode==ON)	
			Time5ms=48/*12*/;
		else
			Time5ms=48;

			if((updn_reg[0] == 0) && (updn_reg[1] == 1) && (updn_reg[2] == 3)&& (updn_reg[3] == 2))  //UP Key					
				{
				   EncorderCount++; 		   
				   bytEncorderCountTemp++;
				}
			if((updn_reg[0] == 1) && (updn_reg[1] == 3) && (updn_reg[2] == 2)&& (updn_reg[3] == 0))  //UP Key	  
				{
				   EncorderCount++; 		 
				   bytEncorderCountTemp++;
				}
			 if((updn_reg[0] == 3) && (updn_reg[1] == 2) && (updn_reg[2] == 0)&& (updn_reg[3] == 1))	//UP Key	
			 	{
				  EncorderCount++;				  
				  bytEncorderCountTemp++;
			 	}
			 if((updn_reg[0] == 2) && (updn_reg[1] == 0) && (updn_reg[2] == 1)&& (updn_reg[3] == 3))  //UP Key	  
			 	{
			   	EncorderCount++; 					
				bytEncorderCountTemp++;
			 	}
			if((updn_reg[0] == 0) && (updn_reg[1] == 2)&& (updn_reg[2] == 3)&& (updn_reg[3] == 1))	//DN Key	 
				{
				   EncorderCount--; 	 
				   bytEncorderCountTemp++;
				}
			 if((updn_reg[0] == 2) && (updn_reg[1] == 3)&& (updn_reg[2] == 1)&& (updn_reg[3] == 0))  //DN Key	  
			 	{
				   EncorderCount--; 						   
				   bytEncorderCountTemp++;
			 	}
			if((updn_reg[0] == 3) && (updn_reg[1] == 1)&& (updn_reg[2] == 0)&& (updn_reg[3] == 2))	//DN Key	
				{
				   EncorderCount--;				   
				   bytEncorderCountTemp++;
				}
			if((updn_reg[0] == 1) && (updn_reg[1] == 0)&& (updn_reg[2] == 2)&& (updn_reg[3] == 3))	//DN Key	
				{
				   EncorderCount--; 
				   bytEncorderCountTemp++;
				}
			
			if(bytEncorderCountTemp==200)	
				{
				  ulongRotateNumber++;
				  bytEncorderCountTemp=0;
				}
			}
		}

		//Kane @HS 2007 0814 Ver1.31<<<<
		
		//---------- 0.01 sec timer ------------
		
	#ifdef CLOCK_11M
		if( tm001 > 48 ) {			// LJY001220 0.01sec
	#elif defined CLOCK_22M
		if( tm001 > ((48*2)) ) {		// LJY001220 0.01sec
	#endif
	
			stopWatch++;
			tm001 = 0;
			tic01++;
		
			if( tic01==100 ) 
			{				// 1 sec
				
				tic01 = 0;
				//second++;
				
			if(Key)
				keyticSec++;
			else
				keyticSec=0;
	
			}
			if( tic_pc!=0xffff ) 
				tic_pc++;
	
	
	if(tic_Init_time)
		tic_Init_time--;
	if(Power_Msg_Count)
		Power_Msg_Count--;

	///////////////////////////////////////////	
#if 1
	#if 0
if( _ReadKey() )
{		if (keytic==50)
		{
		Key = ON;
		RepeatKey = ON;
		KeyReady = ON;
		//keytic = 0;
		}
		else if( keytic==800 ) {//8sec into DVR factory mode
		Key = ON;
		RepeatKey = 1;
		KeyReady = 1;
		keytic = 0;///50;
		DVR_FACTORY_MODE=ON;
		}
		
	keytic++;
}
else 
	{
		/*
		if( (keytic>=50)&&(DVR_FACTORY_MODE==OFF) ) {  //0.5sec push
		Key = ON;
		RepeatKey = OFF;
		KeyReady = ON;
		keytic = 0;
		}
		else
			*/
		{
		DVR_FACTORY_MODE=OFF;
		keytic = 0;
		Key=OFF;
		}
	}
	#else
 if( _ReadKey() )
{		
		if (keytic==50)
		{
			if((PowerFlag==OFF)||(GET_POWER_STATUS()==_POWER_STATUS_SAVING))
			{
			Key = ON;
			RepeatKey=OFF;	
			KeyReady = ON;
			}
		}
		else if( (keytic==300)&& (bytHoldOn3SPowerOff==ON)&&(PowerFlag==ON))///hold on the switch for 3S that power off
			{
				if(RepeatKey==ON)
				{
				Key = ON;
				KeyReady = ON;	
				bytHoldOn3SPowerOffMode=ON;
				RepeatKey=OFF;	
				}
			}
		else if( keytic==800 ) {//8sec into DVR factory mode	

			if(RepeatKey==ON)
			{
			Key = ON;
			KeyReady = ON;			
			DVR_FACTORY_MODE=ON;
			RepeatKey=OFF;	
			}
				
		keytic = 51;///51;
		}
		
	keytic++;
}
else 
	{
		if( (keytic>=50)&&(PowerFlag==ON)&&(RepeatKey==ON)) {  //0.5sec push
		Key = ON;
		KeyReady = ON;
		}
		else
		{
		Key=OFF;
		KeyReady = OFF;
		}
		
		RepeatKey = ON;
		keytic = 0;		
	}	
	#endif	
#else
	if( _ReadKey() )
	{
			if( keytic==800 ) {//8sec into DVR factory mode
			Key = ON;
			RepeatKey = 1;
			KeyReady = 1;
			keytic = 50;
			DVR_FACTORY_MODE=ON;
			}
		keytic++;
	}
	else 
		{
		    	if( (keytic>=50)&&(DVR_FACTORY_MODE==OFF) ) {  //0.5sec push
			Key = ON;
			RepeatKey = OFF;
			KeyReady = ON;
			keytic = 0;
			}
			else
			{
			DVR_FACTORY_MODE=OFF;
			keytic = 0;
			Key=OFF;
			}
		}
#endif	
	///////////////////////////////////////////	
	}
	#endif	
}

//=============================================================================//
//                     ShowWorkTimer                                                 										//
//=============================================================================//
void ShowWorkingTimer(void)
{
WORD val;

	GraphicsPrint(RED,"\r\n==================================");

	if(day)
	{
	GraphicsPrint(YELLOW,"\r\nSystem has been working for %d days, %d hours, %d minutes and %d seconds",(WORD)day,(WORD)hour,(WORD)minute,(WORD)second);
	}
	else if(hour)
	{
	GraphicsPrint(YELLOW,"\r\nSystem has been working for %d hours, %d minutes and %d seconds",(WORD)hour,(WORD)minute,(WORD)second);
	}
	else if(minute)
		{
		GraphicsPrint(YELLOW,"\r\nSystem has been working for %d minutes and %d seconds",(WORD)minute,(WORD)second);
		}
	else
		{
		GraphicsPrint(YELLOW,"\r\nSystem has been working for %d seconds",(WORD)second);
		}

	

	GraphicsPrint(GREEN,"\r\n(BatteryBTH=%d(%d.%dV))",(WORD)BatteryBTH\
			,(WORD)((BatteryBTH*32)/10000)\
		,(WORD)((BatteryBTH*32)%10000));

	if(GET_BTH_STATE()==_BATT_STATUS_TEMP_HIGH)
		GraphicsPrint(RED,"(TEMP_HIGH)");
	else if(GET_BATTERY_HIGH_TMEP_WARN() ==_TRUE)
		GraphicsPrint(YELLOW,"(HIGH_TMEP_WARN)");	
///	else if((GET_BTH_STATE()==_BATT_STATUS_TEMP_LOW)||(BatteryBTH>=1010))
	else if((GET_BTH_STATE()==_BATT_STATUS_TEMP_LOW))
		GraphicsPrint(BLUE,"(TEMP_LOW)");
	else if(GET_BATTERY_LOW_TMEP_WARN() ==_TRUE)
		GraphicsPrint(YELLOW,"(LOW_TMEP_WARN)");	
	else 
		GraphicsPrint(GREEN,"(TEMP_NORMAL)");
	#if 0	
GraphicsPrint(MAGENTA,"\r\n(BatteryCAP=%d(%d.%dV))",(WORD)GET_BATTERY_CAPACITY()\
		,(WORD)((GET_BATTERY_CAPACITY()*(32-1))/10000)\
		,(WORD)((GET_BATTERY_CAPACITY()*(32-1))%10000));
#else
	if(GET_BATTERY_CHARGE_STATE()==_BATT_STATUS_NO_BATT)   
	{
	val=((((GET_BATTERY_CAPACITY()*32)-150)/10000)*100)+((((GET_BATTERY_CAPACITY()*32)-150)%10000)/100);
	val=(val*(54+1))/100;
	
	}
	else
	{
	//val=((((GET_BATTERY_CAPACITY()*32)/*-800*/)/10000)*100)+((((GET_BATTERY_CAPACITY()*32)/*-800*/)%10000)/100);
	//val=(val*(54+1))/100;

	val=((((GET_BATTERY_CAPACITY()*59)/*-800*/)/100))+((((GET_BATTERY_CAPACITY()*59)/*-800*/)/100))+((((GET_BATTERY_CAPACITY()*59)/*-800*/)/100));

		if((val/1000))
		val=(val/10)+1;
		else 
		val=(val/10);
		
	//val=(GET_BATTERY_CAPACITY()*176/10000);
	//val=GET_BATTERY_CAPACITY();
	}

	GraphicsPrint(MAGENTA,"\r\n(BatteryCAP=%d(%d.%04dV))(%dV)",(WORD)GET_BATTERY_CAPACITY()\
			,(WORD)((GET_BATTERY_CAPACITY()*(32+1))/10000)\
			,(WORD)(((GET_BATTERY_CAPACITY()*(32+1))/*-200*/)%10000),(WORD)val);
#endif

	if(GET_BATTERY_CHARGE_STATE()==_BATT_STATUS_HIGH_CHARGE)
		GraphicsPrint(GREEN,"(HIGH_CHARGE)");	
	else if(GET_BATTERY_CHARGE_STATE()==_BATT_STATUS_LOW_CHARGE)	
		GraphicsPrint(YELLOW,"(LOW_CHARGE)"); 
	else if(GET_BATTERY_CHARGE_STATE()==_BATT_STATUS_NORMAL_CHARGE)	
		GraphicsPrint(MAGENTA,"(NORMAL_CHARGE)"); 
	else if(GET_BATTERY_CHARGE_STATE()==_BATT_STATUS_STOP_CHARGE)	
		GraphicsPrint(RED,"(STOP_CHARGE)"); 
	else if(GET_BATTERY_CHARGE_STATE()==_BATT_STATUS_NO_BATT)	
		GraphicsPrint(RED,"(CHARGE_NO_BATT)"); 

   if(GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_LEVEL0)
	GraphicsPrint(GREEN,"(_BATT_STATUS_CAPACITY_LEVEL0)");	
else if(GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_LEVEL1)
	GraphicsPrint(GREEN,"(_BATT_STATUS_CAPACITY_LEVEL1)");	
else if(GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_LEVEL2)
	GraphicsPrint(GREEN,"(_BATT_STATUS_CAPACITY_LEVEL2)");	
else if(GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_LEVEL3)
	GraphicsPrint(GREEN,"(_BATT_STATUS_CAPACITY_LEVEL3)");	
else	 if(GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_LEVEL4)	
	GraphicsPrint(GREEN,"(_BATT_STATUS_CAPACITY_LEVEL4)");	
else if(GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_MIN)
	GraphicsPrint(BLUE,"(_BATT_STATUS_CAPACITY_MIN)");	
else if(GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_MAX)
	GraphicsPrint(CYAN,"(_BATT_STATUS_CAPACITY_MAX)");	
else if(GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_MAX_STOP)
	GraphicsPrint(MAGENTA,"(_BATT_STATUS_CAPACITY_MAX_STOP)");	
else if(GET_BATTERY_STATE()==_BATT_STATUS_CAPACITY_NO_STARTUP)
	GraphicsPrint(MAGENTA,"(BATTERY_CAPACITY_NO_STARTUP)");	
else	 if(GET_BATTERY_STATE()==_BATT_STATUS_NO_BATT)	
	GraphicsPrint(RED,"(_BATT_STATUS_NO_BATT)");	

	if(GET_CHARGE_TMEP_ABNORMAL()==_TRUE)
		GraphicsPrint(RED,"(CHARGE_TMEP_ABNORMAL)");	

if(GET_POWER_STATUS()==7)	
	GraphicsPrint(CYAN,"\r\n(_POWER_STATUS_SAVING)");
else if(GET_POWER_STATUS()==1)	
	GraphicsPrint(CYAN,"\r\n(_POWER_STATUS_OFF)");
else if(GET_POWER_STATUS()==2)	
	GraphicsPrint(CYAN,"\r\n(_POWER_STATUS_NORMAL)");
else
GraphicsPrint(CYAN,"\r\n(POWER_STATUS=%d)",(WORD)GET_POWER_STATUS());

#if 0
if(GET_BATTERY_CHARGE_STATE()==_BATT_STATUS_NO_BATT)   
{
val=((((GET_BATTERY_CAPACITY()*32)-150)/10000)*100)+((((GET_BATTERY_CAPACITY()*32)-150)%10000)/100);
val=(val*(54+1))/100;

}
else
{
val=((((GET_BATTERY_CAPACITY()*32)/*-800*/)/10000)*100)+((((GET_BATTERY_CAPACITY()*32)/*-800*/)%10000)/100);
val=(val*(54+1))/100;
}
#endif
//GraphicsPrint(MAGENTA,"\r\n(BatteryCAP=%d(%dV))",(WORD)GET_BATTERY_CAPACITY(),(WORD)val);


GraphicsPrint(RED,"\r\n==================================");

}




//------------------------------------------------------------------
// void	InitVars(void) - variable initialize
//------------------------------------------------------------------
void	InitVars(void)
{
BYTE j;
for(j=0;j<29;j++)
		DVR_buf[j] = 0 ;	

}


//=============================================================================
//                            Power LED ON / OFF                                                   
//=============================================================================
void PowerLED(EnumSystemTimerEventID flag)
{
   
 switch(flag)
   { 
case   _SYSTEM_TIMER_EVENT_GRN_RED_OFF:
	CLR_RED();
	CLR_GREEN();
	break;
case _SYSTEM_TIMER_EVENT_GRN_OFF_RED_ON:
	CLR_GREEN();
	SET_RED();
	break;
   case _SYSTEM_TIMER_EVENT_GRN_ON_RED_OFF:
	   SET_GREEN();
	   CLR_RED();
 	break;
   case _SYSTEM_TIMER_EVENT_GRN_RED_ON:
	 	SET_GREEN();
		SET_RED();
   	break;
   case _SYSTEM_TIMER_EVENT_GRN_BLINK:
              if(GET_GREEN())		 	
   		{
		SET_GREEN();
		CLR_RED();
   		}
		else
		{
		CLR_GREEN();
		CLR_RED();
		}
   	break;
   case _SYSTEM_TIMER_EVENT_RED_BLINK:  	
   	 	 if(GET_RED())
   		{
		 SET_RED();
		 CLR_GREEN();
   		}
		else
		{
		CLR_GREEN();
		CLR_RED();
		}
   	break;	
  case _SYSTEM_TIMER_EVENT_GRN_RED_BLINK:
                if(GET_GREEN())		 	
   		{
		SET_GREEN();
		SET_RED();
   		}
		else
		{
		CLR_GREEN();
		CLR_RED();
		}
	 break;  
   case _SYSTEM_TIMER_EVENT_GRN_ON_RED_BLINK:
		if(GET_RED())
	   	{
		SET_RED();
	 	  }
	   	else
	   	{
	   	CLR_RED();
	   	}
		
	   	SET_GREEN();

	 break;   
	case _SYSTEM_TIMER_EVENT_GRN_2S_BLINK_RED_ON:
		if(GET_GREEN())
	   	{
		SET_GREEN();
	 	  }
	   	else
	   	{
	   	CLR_GREEN();
	   	}
		
	   	SET_RED();

		
		break;
   default:
            break;
 }	
}


//------------------------ common routines with interrupts --------------------

/*****************************************************************************/
/*      Ext Int 1 Interrupt                                                  */
/*****************************************************************************/
//INTERRUPT(2, ext1_int)
void	ext1_int(void) interrupt 2 using 1
{
	EX1 = 0;
}

//****************************************************************************/
//      Timer 0 Interrupt                                                  
//			If TL0 overflow,
//			 .Invoke this interrupt
//			 .TL0 <- TH0
//			TL0 is incremented every machine cycle
//			Every machine cycle is 12*Tosc(11.0592MHz)
//
//			Every machine cycle = 1.085us
//			Interrupt interval = 208us ( 1.085*(256-64(TH0)) )
//			When tm001==48, it's 0.01sec.  48*208us
//							
//****************************************************************************/


//*****************************************************************************
//      Serial Interrupt                                                   
//*****************************************************************************
#ifdef SERIAL

//INTERRUPT(4, serial_int)
void serial_int(void) interrupt 4 using 1		// register bank 1
{
	
	//day++;
	//year2=RI;
	//year1=TI;
	if( RI ) {					//--- Receive interrupt ----
		#if 0///def Hs_debug
			RS_buf[RS_in]=SBUF;
			RS_in++;
			if( RS_in>=BUF_MAX) RS_in = 0;
		#else
		
			#ifdef USE_HI3521_UART2
			#else
			RS_buf[RS_in]= SBUF;//andy awt 980928
	    		RS_in++;
			if( RS_in>=DVR_BUF_MAX) RS_in = 0;
			#endif	
		
		#endif
		RI = 0;
		//month++;
	}

	if( TI ) {					//--- Transmit interrupt ----
		TI = 0;
		RS_Xbusy=0;
	}
}

#if 1
//INTERRUPT(12, serial2_int)
void serial2_int(void) interrupt 12 using 1		
{
	//--- Receive interrupt ----
	//day++;
	//year2=S2CON&0x0f;
	if ((S2CON & 0x01) == 0x01)
		{					
		// Clear reception flag
		S2CON = S2CON&0xfe;
		//#ifdef USE_HI3521_UART2
		DVR_buf[RS2_in] = S2BUF;
		//#endif
		//month++;
	    RS2_in++;
		if( RS2_in>=/*BUF_MAX*/DVR_BUF_MAX) RS2_in = 0;
	}
	
	if ((S2CON & 0x02) == 0x02)
	{
		S2CON = S2CON&0xfd;
		RS2_Xbusy = 0;
	}
}

#endif

//=============================================================================
//		Serial RX Check 												   
//=============================================================================
#if (_DEBUG_MESSAGE_Monitor==ON)

BYTE RS_ready(void)
{
	if( RS_in == RS_out ) return 0;
	else return 1;
}

//=============================================================================
//		Serial RX														   
//=============================================================================
BYTE RS_rx(void)
{
	BYTE	ret;
		
	ES = 0;
	ret = RS_buf[RS_out];
	RS_out++;
		if(RS_out >= BUF_MAX) 
		RS_out = 0;
	ES = 1;

	return ret;
}
#endif

//=============================================================================
//		Serial RX2 Check 												   
//=============================================================================
BYTE RS2_ready(void)
{
	if( RS2_in == RS2_out ) return 0;
	else return 1;
}
//=============================================================================
//		Serial RX2														   
//=============================================================================
BYTE RS2_rx(void)
{
	BYTE	ret;
		
	AUXIE&=0xEF;  //ES2=0
	ret = DVR_buf[RS2_out];
	RS2_out++;
		if(RS2_out >= DVR_BUF_MAX) 
		RS2_out = 0;
	AUXIE|=0x10;  //ES2=1

	return ret;
}

//=============================================================================
//		Serial TX														   
//=============================================================================
#if (_DEBUG_MESSAGE_Monitor==ON)
void RS_tx(BYTE tx_buf)
{
	while(RS_Xbusy);
	SBUF = tx_buf;
	RS_Xbusy=1;
}
#endif
//=============================================================================
//		Serial2 TX														   
//=============================================================================

void RS2_tx(BYTE tx_buf)
{

	while(RS2_Xbusy);	
		S2BUF=tx_buf;
		RS2_Xbusy=1;
}


#endif	// SERIAL



//****************************************************************************/
//      Timer 2 Interrupt                                                  
//			If TH2 and TL2 are overflowed,
//			 .Invoke this interrupt
//			 .TH2 <- RCAP2H
//			 .TL2 <- RCAP2L
//			TL2 is incremented every machine cycle
//			Every machine cycle is 12*Tosc(11.0592MHz)
//
//			Every machine cycle = 1.085us
//			Interrupt interval  
//				1) REMO_RC5 
//					221.34us  ( 1.085*204 )		// (256-52) (0x10000-0xff34)
//
//					data length: 14bit (2sync bits, 1 control bit, 11 data bits) 24,889ms
//
//								+----+
//					1 is coded:      |    |
//									 +----+   
//								  T    T
//
//									 +----+
//					0 is coded: |    |
//					            +----+	  
//								  T    T					T = 889us
//
//					*) DongYang
//						209.62	( 1.085*193 )	// (256-63) (0x10000-0xff3f)
//-----------------------------------------------------------------------------
//				2) REMO_NEC
//					187.714us ( 1.085*173 )     // (256-83) (0x10000-0xff53)
//							
//****************************************************************************/
//INTERRUPT(5, timer2_int)
void timer2_int(void) interrupt 5 using 1			// using register block 3
{
	#if  (_1KHZ_PWM==ON)
	BYTE TempKey;

	TF2 = 0;					// clear overflow
        ET2=0;
#if 1
		tm001++;
	
			//Timer Counter 
		  tic02++;
	
	//	if(tic02>=(10*22))	 
			if(tic02>=(10))	 
		{
		g_usTimerCounter += 1;	   ///per 103us  interrupt
		tic02=0;
		g_bNotifyTimer0Int=_TRUE;
		}	
#endif
	//tm01++;

#if 1

			if(Time5ms)
				Time5ms--;

			if(Time5ms==0)
			{
				//TempKey=(( (P2&0xC0) >> 6) & 0x03);
				TempKey=(( (P3&0x0C) >> 2) & 0x03); //ENA&ENB, P3.2, P3.3 				
				
				if(updn_reg[3] !=TempKey )// (( (P2&0x60) >> 5) & 0x03))	//Abel for HS SW key ......951102										
				{																						   
					updn_reg[0] = updn_reg[1];																
					updn_reg[1] = updn_reg[2];																
					updn_reg[2] = updn_reg[3];																
					updn_reg[3] =TempKey;// ( (P2&0x60) >> 5) & 0x03; 
					ChangeKey=1;		
				//else			//william-v1.42-961130
				//ChangeKey=0;	//william-v1.42-961130
				Time5ms=(48);
		
				if((updn_reg[0] == 0) && (updn_reg[1] == 1) && (updn_reg[2] == 3)&& (updn_reg[3] == 2))  //UP Key	  
					   EncorderCount++; 		   
				if((updn_reg[0] == 1) && (updn_reg[1] == 3) && (updn_reg[2] == 2)&& (updn_reg[3] == 0))  //UP Key	  
					   EncorderCount++; 		 
				 if((updn_reg[0] == 3) && (updn_reg[1] == 2) && (updn_reg[2] == 0)&& (updn_reg[3] == 1))	//UP Key	
					  EncorderCount++;
				 if((updn_reg[0] == 2) && (updn_reg[1] == 0) && (updn_reg[2] == 1)&& (updn_reg[3] == 3))  //UP Key	  
					EncorderCount++;		
				 
				if((updn_reg[0] == 0) && (updn_reg[1] == 2)&& (updn_reg[2] == 3)&& (updn_reg[3] == 1))	//DN Key	 
					   EncorderCount--; 	 
				 if((updn_reg[0] == 2) && (updn_reg[1] == 3)&& (updn_reg[2] == 1)&& (updn_reg[3] == 0))  //DN Key	  
					   EncorderCount--; 		
				if((updn_reg[0] == 3) && (updn_reg[1] == 1)&& (updn_reg[2] == 0)&& (updn_reg[3] == 2))	//DN Key	
					   EncorderCount--;
				if((updn_reg[0] == 1) && (updn_reg[1] == 0)&& (updn_reg[2] == 2)&& (updn_reg[3] == 3))	//DN Key	
					   EncorderCount--; 
	
				}
				
			}
			
#endif

#if 1
	//---------- 0.01 sec timer ------------
		
	#ifdef CLOCK_11M
		if( tm001 > 48 ) {			// LJY001220 0.01sec
	#elif defined CLOCK_22M
//		if( tm001 > ((48*2)*22) ) {		// LJY001220 0.01sec
		if( tm001 > ((48*2)) ) { 	// LJY001220 0.01sec
	#endif
	
			stopWatch++;
			tm001 = 0;
			tic01++;
		
			if( tic01==100 ) 
			{				// 1 sec
				
				tic01 = 0;
				//second++;
				
			if(Key)
				keyticSec++;
			else
				keyticSec=0;
	
			}
			if( tic_pc!=0xffff ) 
				tic_pc++;
	
	
	if(tic_Init_time)
		tic_Init_time--;
	if(Power_Msg_Count)
		Power_Msg_Count--;

	///////////////////////////////////////////	

#if 1
 if( _ReadKey() )
{		
		if (keytic==50)
		{
			if((PowerFlag==OFF)||(GET_POWER_STATUS()==_POWER_STATUS_SAVING))
			{
			Key = ON;
			RepeatKey=OFF;	
			KeyReady = ON;
			}
		}
		else if( keytic==800 ) {//8sec into DVR factory mode	

			if(RepeatKey==ON)
			{
			Key = ON;
			KeyReady = ON;			
			DVR_FACTORY_MODE=ON;
			RepeatKey=OFF;	
			}
				
		keytic = 51;///51;
		}
		
	keytic++;
}
else 
	{
		if( (keytic>=50)&&(PowerFlag==ON)&&(RepeatKey==ON)) {  //0.5sec push
		Key = ON;
		KeyReady = ON;
		}
		else
		{
		Key=OFF;
		KeyReady = OFF;
		}
		
		RepeatKey = ON;
		keytic = 0;		
	}	
#endif

	///////////////////////////////////////////	
	}


#endif

TF2 = 0;	

ET2=1;	
	#else
	TF2 = 0;					// clear overflow

	tm01++;
	#endif
}

void delay(BYTE cnt)
{
	WORD ttic01;

	ttic01 =  ( tic01 + cnt ) % 100;
	do {
		;
	} while( tic01 != ttic01 );
}

#if (_POWER_DOWN_ENABLE==ON)

void int2_int(void) interrupt 6 using 1			// using register block 3
{
	EX2=0;
	IE2=0;
	PCON=0x80;
	IE=IE_Temp;
	Power_down_mode=OFF;
}

void int3_int(void) interrupt 7 using 1			// using register block 3
{
	EX3=0;
	IE3=0;
	PCON=0x80;
	IE=IE_Temp;
	Power_down_mode=OFF;
}




#endif











