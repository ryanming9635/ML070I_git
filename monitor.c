/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  Monitor.c		                                            										*/
/*                                                                           										*/
/*****************************************************************************/
#include "config.h"

#ifdef SERIAL

#include "typedefs.h"
#include "main.h"
#include "i2c.h"
#include "reg.h"
#include "printf.h"
#include "etc_eep.h"
#include "monitor.h"
#include "cpu.h"
#include "Adc.h"

IDATA BYTE MonAddress = 0x8a;		// initialize value should be placed to IDATA area
BYTE MonIndex;
BYTE MonRdata, MonWdata;
BYTE monstr[30];		// buffer for input string
BYTE *argv[7];
BYTE argc=0;
bit	echo=1;

static bit indirect=0;
BYTE Monitor_I2C=0x40;//0x28;//0x12;//EEPROM_24C02_addr;

//extern		  bit	AutoDetect;
extern   BYTE InputSelection;
extern	      BYTE	DebugLevel;
extern  CODE  struct struct_IdName struct_InputSelection[];
extern 		  bit	NoInitAccess;
 		  bit	AccessEnable=1;
extern BYTE day,hour,minute,second;
extern BYTE PowerFlag;
#if (_DEBUG_MESSAGE_Monitor==ON)
extern bit Monitor_flag;
#endif

extern BYTE	PowerFlag;
extern BYTE	CameraVolt;
extern BYTE	PWR_START_flag;
extern WORD BTH_Temp,Battery_Voltage_Temp;
extern WORD BatteryBTH,BatteryVoltage;
extern DWORD ulongRotateNumber;
extern long EncorderCount;
extern DWORD ulongRotateNumberTELI;
extern long EncorderCountINT;
#if (_BATTERY_CHECK_WITH_NO_CHARGE==ON)
extern BYTE bytBatteryVoltageCheck;
#endif
#if (_BATTERY_CHARGE_STOP==ON)
extern BYTE bytBatteryStopCharge;
#endif
//void Loader(BYTE);

//=============================================================================
//
//=============================================================================
#if (_DEBUG_MESSAGE_Monitor==ON)

void Prompt(void)
{

if(day)
{
Printf("\r\nMCU_I2C[%02x][%d:%d:%d:%d]>",(WORD)EEPROMI2CAddress,(WORD)day,(WORD)hour,(WORD)minute,(WORD)second);
}
else if(hour)
{
Printf("\r\nMCU_I2C[%02x][%d:%d:%d]>",(WORD)EEPROMI2CAddress,(WORD)hour,(WORD)minute,(WORD)second);
}
else if(minute)
	{
	Printf("\r\nMCU_I2C[%02x][%d:%d]>",(WORD)EEPROMI2CAddress,(WORD)minute,(WORD)second);
	}
else
	{
	Printf("\r\nMCU_I2C[%02x][%d]>",(WORD)EEPROMI2CAddress,(WORD)second);
	}


}



void Mon_tx(BYTE ch)
{
	RS_tx(ch);
}



//=============================================================================
//		Convert ASCII to Binery                                             
//=============================================================================
DWORD a2i(BYTE *str)
{
	DWORD num=0;
	BYTE i;

	for(i=0; ; i++, str++) {
		if( *str=='\0' || *str==' ' ) break;
		num = num*10 + *str - '0';
	}
	return num;
}

BYTE Asc1Bin(BYTE asc)
{
	if(asc>='0' && asc <='9') return (asc - '0');
	if(asc>='a' && asc <='f') return (asc - 'a' + 0x0a);
	if(asc>='A' && asc <='F') return (asc - 'A' + 0x0a);
	
	return 0;
}

BYTE Asc2Bin(PDATA_P BYTE *s)
{
	WORD bin;

	bin = 0;
	while(*s != '\0' && *s !=' ') {
		bin = bin<<4;
		bin = bin + Asc1Bin(*s);
		s++;
	}
	return (bin);
}
#endif


#if (_DEBUG_MESSAGE_Monitor==ON)

//=============================================================================
BYTE toupper(BYTE ch)
{
	if( ch>='a' && ch<='z' )
		return (ch - 'a' + 'A');
	
	return ch;
}

int stricmp(BYTE *ptr1, BYTE *ptr2)
{
	int		i;
	int		ret;
	
	for(i=0; *ptr1; i++) {
		ret = toupper(*ptr1++) - toupper(*ptr2++);
		if( ret ) return ret;
	}
	return 0;
}
#endif
//=============================================================================
//
//=============================================================================
/*
void SetMonAddress(BYTE addr)
{
	MonAddress = addr;
}
*/
/*
void MonReadI2C(void)
{
	if( argc>=2 ) MonIndex = Asc2Bin( argv[1] );
	else	{
		Printf("   --> Missing parameter !!!");
		return;
	}

	if ( MonAddress == TW88I2CAddress )
		MonRdata = ReadTW88(MonIndex);
	else
		MonRdata = ReadI2C(MonAddress, MonIndex);

	if( echo )
		Printf("\r\nRead %2xh:%2xh", (WORD)MonIndex, (WORD)MonRdata);	
	
	MonWdata = MonRdata;
}

*/
/*
void MonWriteI2C(void)
{
	if( argc<3 ) {
		Printf("   --> Missing parameter !!!");
		return;
	}
	
	MonIndex = Asc2Bin( argv[1] );
	MonWdata = Asc2Bin( argv[2] );
	
	if( echo ) {
		Printf("\r\nWrite %2xh:%2xh ", (WORD)MonIndex, (WORD)MonWdata);
		if ( MonAddress == TW88I2CAddress ) {
			WriteTW88(MonIndex, MonWdata);
			MonRdata = ReadTW88(MonIndex);
		}
		else {
			WriteI2C(MonAddress, MonIndex, MonWdata);
			MonRdata = ReadI2C(MonAddress, MonIndex);
		}
   		Printf("==> Read %2xh:%2xh", (WORD)MonIndex, (WORD)MonRdata);
	}
	else {
		if ( MonAddress == TW88I2CAddress ) {
			WriteTW88(MonIndex, MonWdata);
		}
		else {
			WriteI2C(MonAddress, MonIndex, MonWdata);
		}
	}
}
*/
#if (_DEBUG_MESSAGE_Monitor==ON)

void MonIncDecI2C(BYTE inc)
{

	switch(inc){
	case 0:  MonWdata--;	break;
	case 1:  MonWdata++;	break;
	case 10: MonWdata-=0x10;	break;
	case 11: MonWdata+=0x10;	break;
	}


	if ( MonAddress == TW88I2CAddress ) {
		WriteTW88(MonIndex, MonWdata);
		MonRdata = ReadTW88(MonIndex);
	}
	else {
		WriteI2C(MonAddress, MonIndex, MonWdata);
		MonRdata = ReadI2C(MonAddress, MonIndex);
	}

	if( echo ) {
		Printf("Write %2xh:%2xh ", (WORD)MonIndex, (WORD)MonWdata);
		Printf("==> Read %2xh:%2xh", (WORD)MonIndex, (WORD)MonRdata);
	}

	Prompt();

}
#endif
/*
void MonDumpI2C(void)
{
	BYTE ToMonIndex;
	int  cnt=8;

	if( argc>=2 ) MonIndex = Asc2Bin(argv[1]);
	if( argc>=3 ) ToMonIndex = Asc2Bin(argv[2]);
	else ToMonIndex = MonIndex+cnt;
	if ( ToMonIndex < MonIndex ) ToMonIndex = 0xFF;
	cnt = ToMonIndex - MonIndex + 1;

	if( echo ) {
		if ( MonAddress == TW88I2CAddress ) {
			for ( ; cnt > 0; cnt-- ) {
				MonRdata = ReadTW88(MonIndex);
				Printf("\r\n==> Read %2xh:%2xh", (WORD)MonIndex, (WORD)MonRdata);
				MonIndex++;
			}
		}
		else {
			for ( ; cnt > 0; cnt-- ) {
				MonRdata = ReadI2C(MonAddress, MonIndex);
				Printf("\r\n==> Read %2xh:%2xh", (WORD)MonIndex, (WORD)MonRdata);
				MonIndex++;
			}
		}
	}
	else {
		if ( MonAddress == TW88I2CAddress ) {
			for ( ; cnt > 0; cnt-- ) {
				MonRdata = ReadTW88(MonIndex);
				MonIndex++;
			}
		}
		else {
			for ( ; cnt > 0; cnt-- ) {
				MonRdata = ReadI2C(MonAddress, MonIndex);
				MonIndex++;
			}
		}
	}
}
*/
//-----------------------------------------------------------------------------
/*
void MonNewReadI2C(void)
{
	BYTE Slave;

	if( argc>=3 ) MonIndex = Asc2Bin( argv[2] );
	else	{
		Printf("   --> Missing parameter !!!");
		return;
	}
	Slave = Asc2Bin(argv[1]);

	if ( Slave == TW88I2CAddress )
		MonRdata = ReadTW88(MonIndex);
	else
		MonRdata = ReadI2C(Slave, MonIndex);

	if( echo )
		Printf("\r\n<R>%2x[%2x]=%2x", (WORD)Slave, (WORD)MonIndex, (WORD)MonRdata);
	
	MonWdata = MonRdata;
}
*/
/*
void MonNewWriteI2C(void)
{
	BYTE Slave;

	if( argc<4 ) {
		Printf("   --> Missing parameter !!!");
		return;
	}
	
	MonIndex = Asc2Bin( argv[2] );
	MonWdata = Asc2Bin( argv[3] );
	Slave = Asc2Bin(argv[1]);
	
	if ( Slave == TW88I2CAddress ) {
		WriteTW88(MonIndex, MonWdata);
		MonRdata = ReadTW88(MonIndex);
	}
	else {
		WriteI2C(Slave, MonIndex, MonWdata);
		MonRdata = ReadI2C(Slave, MonIndex);
   	}

	if( echo )
		Printf("\r\n<R>%2x[%2x]=%2x", (WORD)Slave, (WORD)MonIndex, (WORD)MonRdata);

}

void MonNewDumpI2C(void)
{
	BYTE 	ToMonIndex, Slave;
	WORD	i;
	
	if( argc>=2 ) MonIndex = Asc2Bin(argv[2]);
	if( argc>=3 ) ToMonIndex = Asc2Bin(argv[3]);
	Slave = Asc2Bin(argv[1]);

	if( echo ) {
		if ( Slave == TW88I2CAddress ) {
			for(i=MonIndex; i<=ToMonIndex; i++) {
				MonRdata = ReadTW88(i);
        		Printf("\r\n<R>%2x[%2x]=%2x", (WORD)Slave, (WORD)i, (WORD)MonRdata);
			}
		}
		else {
			for(i=MonIndex; i<=ToMonIndex; i++) {
				MonRdata = ReadI2C(Slave, i);
        		Printf("\r\n<R>%2x[%2x]=%2x", (WORD)Slave, (WORD)i, (WORD)MonRdata);
			}
		}
	}
	else {
		if ( Slave == TW88I2CAddress ) {
			for(i=MonIndex; i<=ToMonIndex; i++) {
				MonRdata = ReadTW88(i);
			}
		}
		else {
			for(i=MonIndex; i<=ToMonIndex; i++) {
				MonRdata = ReadI2C(Slave, i);
			}
		}
	}
}
*/
#if (_DEBUG_MESSAGE_Monitor==ON)

void MonWriteBit(void)
{
	BYTE mask, i, FromBit, ToBit,  MonMask, val;
	BYTE Slave;
	// b 88 index startbit|endbit data

	if( argc<5 ) {
		Printf("   --> Missing parameter !!!");
		return;
	}
	Slave = Asc2Bin(argv[1]);

	MonIndex = Asc2Bin( argv[2] );
	FromBit  = (Asc2Bin( argv[3] ) >> 4) & 0x0f;
	ToBit  = Asc2Bin( argv[3] )  & 0x0f;
	MonMask  = Asc2Bin( argv[4] );

	if( FromBit<ToBit || FromBit>7 || ToBit>7) {
		Printf("\r\n   --> Wrong range of bit operation !!!");
		return;
	}
	
	mask = 0xff; 
	val=0x7f;
	for(i=7; i>FromBit; i--) {
		mask &= val;
		val = val>>1;
	}

	val=0xfe;
	for(i=0; i<ToBit; i++) {
		mask &= val;
		val = val<<1;
	}

	if ( Slave == TW88I2CAddress ) {
		MonRdata = ReadTW88(MonIndex);
		MonWdata = (MonRdata & (~mask)) | (MonMask & mask);
				
		WriteTW88(MonIndex, MonWdata);
		MonRdata = ReadTW88(MonIndex);
	}
	else {
		MonRdata = ReadI2C(Slave, MonIndex);
		MonWdata = (MonRdata & (~mask)) | (MonMask & mask);
				
		WriteI2C(Slave, MonIndex, MonWdata);
		MonRdata = ReadI2C(Slave, MonIndex);
	}

	if( echo )
		Printf("\r\n<R>%2x[%2x]=%2x", (WORD)Slave, (WORD)MonIndex, (WORD)MonRdata);

}

#if defined( SUPPORT_PC ) || defined( SUPPORT_DTV )
void PclkAccess(void)
{
	DWORD pclk;

	if( argc > 2 ) {
		Printf("   --> Too many parameter !!!");
		return;
	}

	if( argc==2 ) {
		pclk = (DWORD)a2i(argv[1]) * 1000000;
		Printf("\r\n --> Set Pclk:  %ld Hz ", pclk);
//		Printf("\r\n Not Suuport ");
		ChangeInternPLL(pclk);
	}
	
	if( argc==1)   GetPPF();
	
}
#endif


void MonTxDump(void)
{
	WORD	count;

	if ( argc < 2 ) {
		Puts("\r\nInsufficient parameters...");
		return;
	}
	count = (WORD)a2i(argv[1]);
	do {
		RS_tx((count&0x1F)+0x20);
		count--;
	} while ( count != 0 );
}

//=============================================================================
//			Help Message
//=============================================================================

void MonHelp(void)
{
	Printf("\r\n=======================================================");
	Printf("\r\n>>>     Welcome to Techwell Monitor  Rev 1.00     <<<");
	Printf("\r\n=======================================================");
	Printf("\r\n   R ii             ; Read AD5110 data");
	Printf("\r\n   W ii dd          ; Write AD5110 data");
	Printf("\r\n   D [ii] [cc]      ; Dump");
	Printf("\r\n   C aa             ; Change I2C address");
	Printf("\r\n   M ?              ; Current Input");
	Printf("\r\n   M dd             ; Change Input");
	Printf("\r\n   Access [0,1]     ; Program TW88xx Access OFF/ON");
	Printf("\r\n   AutoDetect [0,1] ; PC Automode OFF/ON");
	Printf("\r\n   PCAuto		    ; RUN PC Auto Adjust");
	Printf("\r\n   Debug [0..255]   ; Debuging display level");
	Printf("\r\n   Echo On/Off      ; Terminal Echoing On/Off");
	Printf("\r\n   Delta            ; Init DeltaRGB Panel");
	Printf("\r\n   Delta w ii dh dl; SPI Write to DeltaRGB Panel");
	Printf("\r\n");
	Printf("\r\n=======================================================");
	Printf("\r\n");
}

//=============================================================================
//
//=============================================================================
BYTE MonGetCommand(void)
{
	static BYTE comment=0;
	static BYTE incnt=0, last_argc=0;
	BYTE i, ch;
	BYTE ret=0;

	if( !RS_ready() ) return 0;
	#ifdef HS_DEBUG
	ch = RS_rx();
	#endif

 #if (_DEBUG_MESSAGE_Monitor==ON)
	Monitor_flag=_TRUE;
	#endif
	MCUTimerReactiveTimerEvent(SEC(30), _USER_TIMER_EVENT_Monitor_ON);	

	//----- if comment, echo back and ignore -----
	if( comment ) {
		if( ch=='\r' || ch==0x1b ) comment = 0;
		else { 
			Mon_tx(ch);
			return 0;
		}
	}
	else if( ch==';' ) {
		comment = 1;
		Mon_tx(ch);
		return 0;
	}

	//=====================================
	switch( ch ) {

	case 0x1b:
		argc = 0;
		incnt = 0;
		comment = 0;
		Prompt();
		return 0;

	//--- end of string
	case '\r':

		if( incnt==0 ) {
			Prompt();
			break;
		}

		monstr[incnt++] = '\0';
		argc=0;

		for(i=0; i<incnt; i++) if( monstr[i]!=' ' ) break;

		if( !monstr[i] ) {
			incnt = 0;
			comment = 0;
			Prompt();
			return 0;
		}
		argv[0] = &monstr[i];
		for(; i<incnt; i++) {
			if( monstr[i]==' ' || monstr[i]=='\0' ) {
				monstr[i]='\0';
     			 //Printf("(%s) ",  argv[argc]);
				i++;
				while( monstr[i]==' ' ) i++;
				argc++;
				if( monstr[i] ){
     			 argv[argc] = &monstr[i];
				}
			}
		}

		ret = 1;
		last_argc = argc;
		incnt = 0;
		
		break;

	//--- repeat command
	case '/':
		argc = last_argc;
		ret = 1;
		break;

	//--- back space
	case 0x08:
		if( incnt ) {
			incnt--;
			Mon_tx(ch);
			Mon_tx(' ');
			Mon_tx(ch);
		}
		break;

	//--- decreamental write
	case ',':
		if( incnt ) {
			Mon_tx(ch);
			monstr[incnt++] = ch;
		}
		else
			MonIncDecI2C(0);
		break;

	case '<':
		if( incnt ) {
			Mon_tx(ch);
			monstr[incnt++] = ch;
		}
		else
			MonIncDecI2C(10);
		break;
	//--- increamental write
	case '.':
		if( incnt ) {
			Mon_tx(ch);
			monstr[incnt++] = ch;
		}
		else
			MonIncDecI2C(1);
		break;

	case '>':
		if( incnt ) {
			Mon_tx(ch);
			monstr[incnt++] = ch;
		}
		else
			MonIncDecI2C(11);
		break;

	default:
		Mon_tx(ch);
		monstr[incnt++] = ch;
		break;
	}

	if( ret ) {
		comment = 0;
		last_argc = argc;
		return ret;
	}
	else {
		return ret;
	}
}
#endif

//*****************************************************************************
//				Monitoring Command
//*****************************************************************************
#if (_DEBUG_MESSAGE_Monitor==ON)//def  HS_DEBUG
void Monitor(void)
{
	BYTE ret;


	if( !MonGetCommand() ) return;

	//--------------------------------------------------
	// Check Indirect command
	/*
	if( !stricmp( argv[0], "I" ) ) 			indirect=1;	
	else if( !stricmp( argv[0], "O" ) ) 	indirect=1;	

	else if( !stricmp( argv[0], "," ) ) 	indirect=0;	
	else if( !stricmp( argv[0], ">" ) ) 	indirect=0;	
	else if( !stricmp( argv[0], "<" ) ) 	indirect=0;	
	else if( !stricmp( argv[0], "." ) ) 	indirect=0;	
	else  indirect=0;	
	*/
	indirect = 0;

	//---------------- Write Register -------------------
	/*
	if( !stricmp( argv[0], "W" ) ) {
		MonWriteI2C();
	}
	else if( !stricmp( argv[0], ")" ) ) {
		indirect=0;
		MonNewWriteI2C();
	}
	*/
	/*
	else if( !stricmp( argv[0], "O" ) ) {			// inderect addressing for 8806
		MonIndex = Asc2Bin( argv[1] );
		MonWdata = Asc2Bin( argv[2] );
		WriteI2C(MonAddress, 0xc5, MonIndex);
		WriteI2C(MonAddress, 0xc6, MonWdata);
		MonRdata = ReadI2C(MonAddress, 0xc6);
		Printf("\r\n==> Read %2xh:%2xh", (WORD)MonIndex, (WORD)MonRdata);
	}
	*/
	//---------------- Read Register --------------------
	/*
	else if ( !stricmp( argv[0], "R" ) ) {
		MonReadI2C();
	}
	*/
	/*
	else if ( !stricmp( argv[0], "(" ) ) {
		indirect=0;
		MonNewReadI2C();
	}
	*/
	/*
	else if( !stricmp( argv[0], "i" ) ) {
		MonIndex = Asc2Bin( argv[1] );
		WriteI2C(MonAddress, 0xc5, MonIndex);
		MonRdata = ReadI2C(MonAddress, 0xc6);
		Printf("\r\n==> Read %2xh:%2xh", (WORD)MonIndex, (WORD)MonRdata);

	}
	*/
	//---------------- Dump Register --------------------
	//else
		if( !stricmp( argv[0], "D" ) ) {
		Printf("\r\ndump AD5110 start");
		ret=ReadAD5110(5);
		Printf("\r\nAD5110 RDAC=%02x",(WORD)ret);
		ret=ReadAD5110(6);
		Printf("\r\nAD5110 EEPROM=%02x",(WORD)ret);	

		Printf("\r\ndump EEPROM reg.");
	//	 PowerFlag = ReadEEP(EEP_Powerflag);
	//	 CameraVolt= ReadEEP(EEP_CameraVolt);
	// 	PWR_START_flag= ReadEEP(EEP_DC12_PWR_START);		

		Printf("\r\nPowerFlag=%02x",(WORD)ReadEEP(EEP_Powerflag));	
		Printf("\r\nCameraVolt=%d",(WORD)ReadEEP(EEP_CameraVolt));	
		Printf("\r\nPWR_START_flag=%02x",(WORD)ReadEEP(EEP_DC12_PWR_START));	
#if (_BATTERY_CHARGE_STOP==ON)
	Printf("\r\nBatteryStopCharge=%02x",(WORD)ReadEEP(EEP_BatteryStopCharge));	
#endif
#if(_HARDWARE_VERSION==ON)
	Printf("\r\nHARDWARE_VER=%02x",(WORD)ReadEEP(EEP_HARDWARE_VER));	
#endif	
	Printf("\r\nEncorder1=%02x",(WORD)ReadEEP(EEP_Encorder1));	
	Printf("\r\nDecimal1=%02x",(WORD)ReadEEP(EEP_Decimal1));	

	Printf("\r\nEncorder2=%02x",(WORD)ReadEEP(EEP_Encorder2));	
	Printf("\r\nDecimal2=%02x",(WORD)ReadEEP(EEP_Decimal2));	

	Printf("\r\nEncorder3=%02x",(WORD)ReadEEP(EEP_Encorder3));	
	Printf("\r\nDecimal3=%02x",(WORD)ReadEEP(EEP_Decimal3));	

	Printf("\r\nEncorder4=%02x",(WORD)ReadEEP(EEP_Encorder4));	
	Printf("\r\nDecimal4=%02x",(WORD)ReadEEP(EEP_Decimal4));	

	Printf("\r\nGetRotateNumber=%01x%04x",(WORD)(ulongRotateNumber>>16),(WORD)ulongRotateNumber);
	Printf("\r\nGetRotateNumberTELI=%01x%04x",(WORD)(ulongRotateNumberTELI>>16),(WORD)ulongRotateNumberTELI);

		
	}
		/*
	else if( !stricmp( argv[0], "&" ) ) {
		indirect=0;
		MonNewDumpI2C();
	}
*/
	//---------------- Bit Operation --------------------
	else if( !stricmp( argv[0], "B" ) ) {// Write bits - B AA II bb DD
		MonWriteBit();
	}
	//---------------- AD5110 CMD --------------------

	else if( !stricmp( argv[0], "PSW" ) ) {
		PowerFlag^=1;
			WriteEEP(EEP_Powerflag,PowerFlag);
		Printf("\r\n(PowerFlag =%02x)",(WORD)PowerFlag);
		Monitor_flag=OFF;
	}

	//---------------- AD5110 CMD --------------------
	else if( !stricmp( argv[0], "W" ) ) {
	#if 0
		WriteAD5110(Asc2Bin(argv[1]),Asc2Bin(argv[2]));

		if(Asc2Bin(argv[1])==2)
		{
//		WriteAD5110(1,0);
//		ret=ReadAD5110(Asc2Bin(argv[1]));
		ret=ReadAD5110(5);
		Printf("\r\nAD5110 RDAC=%02x",(WORD)/*Asc2Bin(argv[2])*/ret);
//		ret=ReadAD5110(6);	
//		Printf("\r\nAD5110 EEPROM=%02x",(WORD)/*Asc2Bin(argv[2])*/ret);
		}
		#endif	
		WriteEEP(Asc2Bin(argv[1]),Asc2Bin(argv[2]));
		Printf("\r\nEEPROM Write add.=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)Asc2Bin(argv[2]));	
		ret=ReadEEP(Asc2Bin(argv[1]));	
		Printf("\r\nEEPROM Read addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)ret);

	
	}
	else if( !stricmp( argv[0], "R" ) ) {
	
		//ret=ReadAD5110(Asc2Bin(argv[1]));
		//Printf("\r\nAD5110 Read addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)/*ReadAD5110(Asc2Bin(argv[1])*/ret);
		ret=ReadEEP(Asc2Bin(argv[1]));
		Printf("\r\nEEPROM Read addr=%02x  data=%02x",(WORD)Asc2Bin(argv[1]),(WORD)/*ReadAD5110(Asc2Bin(argv[1])*/ret);

	}

	/*	no indirect addressing in TW8816
    //---------------------------------------------------
	else if( !stricmp( argv[0], "@" ) ) { // Indirect address bit access

					BYTE Slave, mask, FromBit, ToBit,  MonMask, val,i;
					// @ 8a iaddr idata index  startbit|endbit  data
					// 0  1   2    3     4     5                 6

					if( argc<7 ) {
						Printf("   --> Missing parameter !!!");
						return;
					}

					Slave = Asc2Bin(argv[1]);
					
					FromBit = Asc2Bin( argv[5] );
					FromBit  = ( FromBit >> 4) & 0x0f;
					ToBit = Asc2Bin( argv[5] );
					ToBit  =  ToBit & 0x0f;
					MonMask  = Asc2Bin( argv[6] );

					if( FromBit<ToBit || FromBit>7 || ToBit>7) {
						Printf("\r\n   --> Wrong range of bit operation !!!");
						return;
					}
	
					mask = 0xff; 
					val=0x7f;
					for(i=7; i>FromBit; i--) {
						mask &= val;
						val = val>>1;
					}

					val=0xfe;
					for(i=0; i<ToBit; i++) {
						mask &= val;
						val = val<<1;
					}

					// @ 8a iaddr idata index  startbit|endbit  data
					// 0  1   2    3     4     5                 6
					MonIndex = Asc2Bin(argv[2]);
					MonWdata = Asc2Bin(argv[4]);
					WriteI2C( Slave, MonIndex, MonWdata);

					MonRdata = ReadI2C(Slave, Asc2Bin( argv[3]));
					MonWdata = (MonRdata & (~mask)) | (MonMask & mask);
					MonIndex = Asc2Bin(argv[3]);
					WriteI2C(Slave, MonIndex, MonWdata);

					MonRdata = ReadI2C(Slave, MonIndex);

					Printf("\r\n<R>%2x[%2x]=%2x", (WORD)Slave, (WORD)MonIndex, (WORD)MonRdata);

	}
	//------------------------------------------------------------------				
	else if( !stricmp( argv[0], "!" ) ) //  Indirect address write access
	{
					BYTE Slave;
					// @ 8a iaddr idata index data
					if( argc<6 ) {
						Printf("   --> Missing parameter !!!");
						return;
					}

					Slave = Asc2Bin(argv[1]);

					MonIndex = Asc2Bin(argv[2]);
					MonWdata = Asc2Bin(argv[4]);
					WriteI2C(Slave, MonIndex, MonWdata);

					MonIndex = Asc2Bin(argv[3]);
					MonWdata = Asc2Bin(argv[5]);
					WriteI2C(Slave, MonIndex, MonWdata);

					MonRdata = ReadI2C(Slave, MonIndex);

					Printf("\r\n<R>%2x[%2x]=%2x", (WORD)Slave, (WORD)MonIndex, (WORD)MonRdata);

	}
	*/    

	//---------------- Change I2C -----------------------
	else if( !stricmp( argv[0], "C" ) ) {
		MonAddress = Asc2Bin( argv[1] );
	}

	//---------------- Help -----------------------------
	else if( !stricmp( argv[0], "H" ) || !stricmp( argv[0], "HELP" ) || !stricmp( argv[0], "?" ) ) {
		MonHelp();

	}
	//---------------------------------------------------
	else if( !stricmp( argv[0], "*" ) ) {
			
				if( argc==1 ) {
					Printf("\r\n  * 0 : Program default Loader");
					Printf("\r\n  * 1 : Program external Loader");
					Printf("\r\n  * 2 : Execute Loader");
				}
				else { 
					BYTE mode;
					mode = Asc2Bin(argv[1]);
					//Loader(mode);
				}
	}

	//---------------------------------------------------
#if defined( SUPPORT_PC ) || defined( SUPPORT_DTV )
	else if( !stricmp( argv[0], "PCLK" ) ) {
		PclkAccess();			
	}
#endif	

	//---------------------------------------------------
	else if( !stricmp( argv[0], "POWER" ) ) {
		//LCDPower();
	}

	//---------------------------------------------------
#if defined( SUPPORT_PC ) || defined( SUPPORT_DTV )
	else if( !stricmp( argv[0], "M" ) ) {
		if( argc==2 ) {
			if( !stricmp( argv[1], "?" ) ) {
				BYTE i;
				Printf("\r\nCurrent Input:%d \r\n", (WORD)InputSelection);
				for(i=1; struct_InputSelection[i].Id!=0 ; i++)
						Printf("%s:%d  ",struct_InputSelection[i].Name,(WORD)struct_InputSelection[i].Id );
				Printf("\r\n");
			}
			else
				ChangeInput(Asc2Bin( argv[1] ));
		}
		else
			ChangeInput(GetNextInputSelection());
	}	
	//---------------------------------------------------
	else if( !stricmp( argv[0], "ACCESS" ) ) {
		//if( argc==2 ) {
			AccessEnable = Asc2Bin(argv[1]);
		//}
		if(AccessEnable)
		Printf("\r\nTW88xx AUTO ACCESS = ON");
		else
		Printf("\r\nTW88xx AUTO ACCESS = OFF");

	}
	#if 0
	//---------------------------------------------------
	else if( !stricmp( argv[0], "AUTODETECT" ) ) {
		if( argc==2 ) {
			AutoDetect = Asc2Bin(argv[1]);
		}
		Printf("\r\nPC Auto Detect = %d", (WORD)AutoDetect);
	}	
	#endif
	//---------------------------------------------------
	else if( !stricmp( argv[0], "PCAUTO" ) ) {
		Puts("\r\nPC Auto Measurement");
		AutoAdjust();
	}	
	//---------------------------------------------------
	else if( !stricmp( argv[0], "PCCOLOR" ) ) {
		Puts("\r\nPC Color Auto Measurement");
		AutoColorAdjust();
	}	
	//---------------------------------------------------
	else if( !stricmp( argv[0], "7" ) ) {
		Puts("\r\nGet HPN / VPN");
		GetHpnVpn(DebugLevel);
	}	
#endif
	//---------------- Debug Level ---------------------
	else if ( !stricmp( argv[0], "DEBUG" ) ) {
		if( argc==2 ) {
			DebugLevel = Asc2Bin(argv[1]);
			SaveDebugLevelEE(DebugLevel);
		}
		Printf("\r\nDebug Level = %2x", (WORD)DebugLevel);
	}
	
	//---------------- Display on/off -------------------
	else if ( !stricmp( argv[0], "echo" ) ) {
		if( !stricmp( argv[1], "off" ) ) {
			echo = 0;
			Printf("\r\necho off");
		}
		else {
			echo = 1;
			Printf("\r\necho on");
		}
	}
	//---------------- RESET/ TEST / POWER_DOWN -------------------
	else if ( !stricmp( argv[0], "reset" ) ) {
		if( !stricmp( argv[1], "0" ) ) {
			P3_4 = 0;
			Printf("\r\nTW_reset_pin => 0");
		}
		else if( !stricmp( argv[1], "1" ) ) {
			P3_4 = 1;
			Printf("\r\nTW_reset_pin => 1");
		}
		else {
			Printf("\r\nTW_reset_pin = %d", (WORD)P3_4);
		}
	}
	else if ( !stricmp( argv[0], "test" ) ) {
		if( !stricmp( argv[1], "0" ) ) {
			//P0_7 = 0;
			Printf("\r\nTW_test_pin => 0");
		}
		else if( !stricmp( argv[1], "1" ) ) {
			//P0_7 = 1;
			Printf("\r\nTW_test_pin => 1");
		}
		else {
			Printf("\r\nTW_test_pin = %d", (WORD)P0_7);
		}
	}
	else if ( !stricmp( argv[0], "pdn" ) ) {
		if( !stricmp( argv[1], "0" ) ) {
			P3_7 = 0;
			Printf("\r\nTW_PowerDown_pin => 0");
		}
		else if( !stricmp( argv[1], "1" ) ) {
			P3_7 = 1;
			Printf("\r\nTW_PowerDown_pin => 1");
		}
		else {
			Printf("\r\nTW_PowerDown_pin = %d", (WORD)P3_7);
		}
	}
	//---------------- OSD test for parallel -----------------------------------
	else if( !stricmp( argv[0], "OSD" ) ) {
		if( !stricmp( argv[1], "logo" ) ) {
//			DisplayLogo();
		}
		else if( !stricmp( argv[1], "end" ) ) {
		  	Puts("\r\nIf you'd like to exit, press any Key....");
	 		while ( !RS_ready() ){
				P2 = ReadTW88( 0x95 );
			}
		}
		else if( !stricmp( argv[1], "input" ) ) {
//			DisplayInput();
		}
		else if( !stricmp( argv[1], "off" ) ) {
////			ShowOSDWindowAll(OFF);
		}
		//else if( !stricmp( argv[1], "grid" ) ) {
		//	DisplayGrid();
		//}
#if 0		
#ifndef BANKING
		else if( !stricmp( argv[1], "cone" ) ) {
			DisplayCone();
			DisplayJapanese();
		}
#endif		
#endif
		#if 0///def BANKING
		else if( !stricmp( argv[1], "agrid" ) ) {
		  	BYTE i, j, k;
		  	Puts("\r\nIf you'd like to exit, press any Key....");
			j = 0;
			k = 0;
			DisplayMessage();
			if ( argc == 3 )
				i = Asc2Bin(argv[2]);
			else i = 0;
	 		while ( !RS_ready() ){
				if ( j < 9 )
					DisplayGridBank0(j);
				else
					DisplayGridBank1(j);
				if ( i )
					delay(10*i);
				if (k==0) {
					if (j==16) {
						k = 1;
						j--;
					}
					else j++;
				}
				else {
					if ( j==0 ) {
						k = 0;
						j++;
					}
					else j--;
				}
			}
		}
		else if( !stricmp( argv[1], "cgrid" ) ) {
		  	BYTE i, j, k;
		  	Puts("\r\nIf you'd like to exit, press any Key....");
			j = 0;
			k = 0;
			DisplayChinese();
			if ( argc == 3 )
				i = Asc2Bin(argv[2]);
			else i = 0;
	 		while ( !RS_ready() ){
				if ( j < 9 )
					DisplayGridBank0(j);
				else
					DisplayGridBank1(j);
				if ( i )
					delay(10*i);
				if (k==0) {
					if (j==16) {
						k = 1;
						j--;
					}
					else j++;
				}
				else {
					if ( j==0 ) {
						k = 0;
						j++;
					}
					else j--;
				}
			}
		}
		else if( !stricmp( argv[1], "jgrid" ) ) {
		  	BYTE i, j, k;
		  	Puts("\r\nIf you'd like to exit, press any Key....");
			j = 0;
			k = 0;
			DisplayJapanese();
			if ( argc == 3 )
				i = Asc2Bin(argv[2]);
			else i = 0;
	 		while ( !RS_ready() ){
				if ( j < 9 )
					DisplayGridBank0(j);
				else
					DisplayGridBank1(j);
				if ( i )
					delay(10*i);
				if (k==0) {
					if (j==16) {
						k = 1;
						j--;
					}
					else j++;
				}
				else {
					if ( j==0 ) {
						k = 0;
						j++;
					}
					else j--;
				}
			}
		}

else if( !stricmp( argv[1], "kgrid" ) ) {
		  	BYTE i, j, k;
		  	Puts("\r\nIf you'd like to exit, press any Key....");
			j = 0;
			k = 0;
			DisplayKorean();
			if ( argc == 3 )
				i = Asc2Bin(argv[2]);
			else i = 0;
	 		while ( !RS_ready() ){
				if ( j < 9 )
					DisplayGridBank0(j);
				else
					DisplayGridBank1(j);
				if ( i )
					delay(10*i);
				if (k==0) {
					if (j==16) {
						k = 1;
						j--;
					}
					else j++;
				}
				else {
					if ( j==0 ) {
						k = 0;
						j++;
					}
					else j--;
				}
			}
		}
		#endif
	}
	//---------------- TELI ML070I Test function -----------------------------------
	else if( !stricmp( argv[0], "PWM0" ) ) {
		SET_PWM(_CHG_CURR, Asc2Bin(argv[1]));
		Puts("\r\nSET PWMP1.4");
		/*
		if(Asc2Bin(argv[1])==0)
			{
			P1_4=0;
			Puts("\r\nP1_4=0");
			}
		
		if(Asc2Bin(argv[1])==1)
			{
			P1_4=1;
			Puts("\r\nP1_4=1");
			}
		*/
		#if (_BATTERY_CHECK_WITH_NO_CHARGE==ON)
		bytBatteryVoltageCheck=ON;
			MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_BATTERY_VOLTAGE_READY_CHECK);
			MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_BATTERY_VOLTAGE_DISABLE_CHECK);
		#endif	
		Monitor_flag=_FALSE;
	}
	else if( !stricmp( argv[0], "PWM1" ) ) {
		SET_PWM(_BL_PWM, Asc2Bin(argv[1]));
		Puts("\r\nSET PWMP1.5");
		Monitor_flag=_FALSE;
	}
	else if( !stricmp( argv[0], "PWM" ) ) {
		if(Asc2Bin(argv[1]))
			{
			SET_PWM(_RUN_PWM, _RUN_PWM);
			Puts("\r\nPOEN PWM");
			}
		else
			{
			SET_PWM(_STOP_PWM, _STOP_PWM);
			Puts("\r\nSTOP PWM");
			}
		Monitor_flag=_FALSE;
	}
	else if( !stricmp( argv[0], "BTH" ) ) {
		BatteryBTH=(Asc2Bin(argv[1])<<8)|(Asc2Bin(argv[2]));
		Printf("\r\n(BatteryBTH=%d)", (WORD)BatteryBTH);	
		Monitor_flag=_FALSE;
		}
	else if( !stricmp( argv[0], "BVOL" ) ) {
		BatteryVoltage=(Asc2Bin(argv[1])<<8)|(Asc2Bin(argv[2]));
		Printf("\r\n(BatteryVoltage=%d)", (WORD)BatteryVoltage);
		Monitor_flag=_FALSE;
		}
	else if( !stricmp( argv[0], "Q" ) ) {
		Monitor_flag=_FALSE;
		}
#if (_BATTERY_CHARGE_STOP==ON)
	else if( !stricmp( argv[0], "clear" ) ) {
		//ClearBasicEE();
		//LoadEEPROM();
		WriteEEP(EEP_BatteryStopCharge,OFF);
		bytBatteryStopCharge=ReadEEP(EEP_BatteryStopCharge);
		Printf("(Clear bytBatteryStopCharge=0)");	
		Monitor_flag=_FALSE;
		}
#endif 
		else if( !stricmp( argv[0], "CABLE" ) ) {
		//EncorderCountINT=(Asc2Bin(argv[1])<<8)|(Asc2Bin(argv[2]));
		EncorderCountINT=65535;
		Printf("\r\n(EncorderCountINT=%d)", (WORD)EncorderCountINT);	
		Monitor_flag=_FALSE;
		}
		else if( !stricmp( argv[0], "PWRS" ) ) {


				if(Asc2Bin(argv[1])==1)
					{
					WriteEEP(EEP_DC12_PWR_START,ON);
					PWR_START_flag=ON;
					Puts("\r\nDC12_PWR_START=ON");
					}
				else if(Asc2Bin(argv[1])==0)
					{
					WriteEEP(EEP_DC12_PWR_START,OFF);
					PWR_START_flag=OFF;
					Puts("\r\nDC12_PWR_START=OFF");
					}
				Monitor_flag=_FALSE;

	}
	/*
	else if( !stricmp( argv[0], "CLEAR" ) ) {
		
		EncorderCount=0;
		Printf("(EncorderCount=0)");
		Monitor_flag=_FALSE;
		}
		*/
	else if( !stricmp( argv[0], "PCON" ) ) {
		
		PCON=Asc2Bin(argv[1]);
		Printf("(PCON=%x)", (WORD)PCON);
		Monitor_flag=_FALSE;
		}
	else if( !stricmp( argv[0], "PCON2" ) ) {
			
			PCON2=Asc2Bin(argv[1]);
			Printf("(PCON2=%x)", (WORD)PCON2);
			Monitor_flag=_FALSE;
			}

		else if( !stricmp( argv[0], "SYSREADY" ) ) {

//	SET_DVR_SystemReadyNotic();

	}
		
		else if( !stricmp( argv[0], "WDT" ) ) {

	//	SET_DVR_SystemReadyNotic();
		
		}
		else if( !stricmp( argv[0], "BTH" ) ) {

			if(Asc2Bin(argv[1]==0))
			BTH_Temp=40;
			else if(Asc2Bin(argv[1]==1))
			BTH_Temp=150;
			else if(Asc2Bin(argv[1]==2))
			BTH_Temp=850;
			else if(Asc2Bin(argv[1]==3))
			BTH_Temp=110;
			else
			BTH_Temp=250;

			Printf("\r\n(BTH=%d)", (WORD)BTH_Temp);
			Monitor_flag=_FALSE;

	}
			else if( !stricmp( argv[0], "BBBVT" ) ) {

				if(Asc2Bin(argv[1])==0)
				{
				Battery_Voltage_Temp=150;
				Puts("\r\nBVT=150");
				}
			else if(Asc2Bin(argv[1])==1)
				{
				Battery_Voltage_Temp=650;				
				Puts("\r\nBVT=650");
				}
			else if(Asc2Bin(argv[1]==2))
				{
				Battery_Voltage_Temp=750;			
				Puts("\r\nBVT=750");
				}
			else if(Asc2Bin(argv[1]==3))
				{
				Battery_Voltage_Temp=810;
				Puts("\r\nBVT=810");
				}
			else if(Asc2Bin(argv[1]==4))
				{
				Battery_Voltage_Temp=860;			
				Puts("\r\nBVT=860");
				}
			else
				{
				Battery_Voltage_Temp=900;			
				Puts("\r\nBVT=900");
				}
				
			//Printf("\r\n(Battery_Voltage_Temp=%d)", (WORD)Battery_Voltage_Temp);
			//Printf("\r\n(Battery_Voltage_Temp=%d)", (WORD)Asc2Bin(argv[1]));

			Monitor_flag=_FALSE;

	}
	else if( !stricmp( argv[0], "SLEEP" )){
		MCUTimerActiveTimerEvent(SEC(1),_USER_TIMER_EVENT_OSD_MCU_ENTRY_SLEEP_MODE);
		Monitor_flag=_FALSE;

		}	
	//---------------- serial tx test with some number -----------------------------------
	else if( !stricmp( argv[0], "TX" ) ) {
		MonTxDump();
	}
	//---------------- OSD FONTtest for parallel -----------------------------------
	else if( !stricmp( argv[0], "FONT" ) ) {
		if ( !stricmp( argv[1], "RAM" ) ) {
//			DisplayRAMFont( Asc2Bin(argv[2]) );
		}
		else if ( !stricmp( argv[1], "ROM" ) ) {
		//	DisplayROMFont(Asc2Bin(argv[2]));
		}
		else {
			Puts("\r\n Font ROM # or Font RAM # for testing");
		}
	}
	
	//---------------- Delta RGB Panel Test -------------------------
	#ifdef SUPPORT_DELTA_RGB

	else if( !stricmp( argv[0], "delta" ) ) {
		if( argc==1 )	DeltaRGBPanelInit();
		else {
			
			if( !stricmp( argv[1], "w" ) ) {

				WORD val;
				val= Asc2Bin(argv[3])<<8 | Asc2Bin(argv[4]);
				Printf("\r\n SPI Write: Addr:%2x  Data:%4x", (WORD)Asc2Bin(argv[2]), val);
				WriteSPI(Asc2Bin(argv[2]), val );
			}
			else if( !stricmp( argv[1], "r" ) ) {
				WORD val;
				val = ReadSPI(Asc2Bin(argv[2]));
				Printf("\r\n SPI Read: Addr:%2x  Data:%4x", (WORD)Asc2Bin(argv[2]), val);
			}
		}
	}
	#endif
	//----------------------------------------------------
	else {
		Printf("\r\nInvalid command...");
	}


	Prompt();


}
#endif

//-------------------------------------------------------------------------


#endif
