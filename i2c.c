/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  I2C.c 			                                            										*/
/*                                                                           										*/
/*****************************************************************************/


#include "Config.h"

#include "reg.h"
#include "typedefs.h"
#include "cpu.h"
#include "I2C.h"
#include "main.h"
#include "printf.h"
#include "CPU.h"

#define ID_ADDR		0xc5
#define ID_DATA		0xc6

extern	BYTE	DebugLevel;

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

//#define I2C_FAST

#ifdef I2C_FAST
 #define I2Cdelay()
#else
 void I2Cdelay(void)
 {
	 BYTE i;
	 for(i=0; i<1; i++){
	 }
 }
#endif

void WriteEEP(WORD index, BYTE dat)
{
	BYTE addr;

	addr = (EEPROMI2CAddress | (index>>7)) & 0xfe;

	I2CStart();
	I2CWriteData(addr);
	I2CWriteData((BYTE)index);
	I2CWriteData(dat);
	I2CStop();

	#ifdef DEBUG_EEP
		dPrintf("Write %02xh to [Adrs(%02xh)Index(%02xh)\r\n", (WORD)dat, (WORD)addr, (WORD)index );
	#endif

	delay(2);
}

BYTE ReadEEP(WORD index)
{
	BYTE val;
	BYTE addr;

	addr = (EEPROMI2CAddress | (index>>7)) & 0xfe;

	val = ReadI2C(addr,(BYTE)index);

	#ifdef DEBUG_EEP
		dPrintf("Read [Adrs:%02xh,Index:%02xh] %02xh\r\n", (WORD)addr, (WORD)index, (WORD)val);
	#endif
	delay(2);
	return val;
}

BYTE ReadI2C(BYTE addr, BYTE index)
{
	BYTE val;

	I2CStart();
	I2CWriteData(addr);
	I2CWriteData(index);
	I2CStart();
	I2CWriteData(addr | 0x01);
	val = I2CReadData();
	I2CStop();

	return val;
}

#if (_DEBUG_MESSAGE_Monitor==ON)

void WriteI2C(BYTE addr, BYTE index, BYTE val)
{
	I2CStart();
	I2CWriteData(addr);
	I2CWriteData(index);
	I2CWriteData(val);
	I2CStop();
}
#endif

#define dd()  I2Cdelay()

void I2CStart()
{
	I2C_SDA = 1;	dd();
	I2C_SCL = 1;	dd();
	I2C_SDA = 0;	dd();
	I2C_SCL = 0;
}

void I2CStop()
{
	I2C_SDA = 0;	dd();
	I2C_SCL = 1;	dd();
	I2C_SDA = 1;
}

void I2CWriteData(BYTE value)
{

	I2C_SCL=0;	I2C_SDA=(value & 0x80)? 1:0;	I2C_SCL=1;	dd();
	I2C_SCL=0;	I2C_SDA=(value & 0x40)? 1:0;	I2C_SCL=1;	dd();
	I2C_SCL=0;	I2C_SDA=(value & 0x20)? 1:0;	I2C_SCL=1;	dd();
	I2C_SCL=0;	I2C_SDA=(value & 0x10)? 1:0;	I2C_SCL=1;	dd();

	I2C_SCL=0;	I2C_SDA=(value & 0x08)? 1:0;	I2C_SCL=1;	dd();
	I2C_SCL=0;	I2C_SDA=(value & 0x04)? 1:0;	I2C_SCL=1;	dd();
	I2C_SCL=0;	I2C_SDA=(value & 0x02)? 1:0;	I2C_SCL=1;	dd();
	I2C_SCL=0;	I2C_SDA=(value & 0x01)? 1:0;	I2C_SCL=1;	dd();


	I2C_SCL = 0;	// HHY 3.00
	I2C_SCL = 0;	// HHY 3.00
	I2C_SCL = 0;	//	I2Cdelay();
	I2C_SDA = 1;	//	I2Cdelay();

	I2C_SCL = 1;	// HHY 3.00
	I2C_SCL = 1;	// HHY 3.00
	I2C_SCL = 1;	//	I2Cdelay();
	I2C_SCL = 0;
}

BYTE I2CReadData()
{
	int i;
	BYTE value=0;

	I2C_SDA = 1;	dd();
	I2C_SCL = 0;
	
	for(i=0; i<8; i++) {
		I2C_SCL = 1;
		value <<= 1;
		if(I2C_SDA) value |= 1;
		I2C_SCL = 0;
	}
	dd();
	
	I2C_SCL = 1;	// HHY 3.00
	I2C_SCL = 1;	// HHY 3.00
	I2C_SCL = 1;
	I2C_SCL = 0;

	return value;

}


void AD5110_I2CWriteData(BYTE value)
{

	I2C_SCL=0;	I2C_SDA=(value & 0x80)? 1:0;	I2C_SCL=1;	dd();
	I2C_SCL=0;	I2C_SDA=(value & 0x40)? 1:0;	I2C_SCL=1;	dd();
	I2C_SCL=0;	I2C_SDA=(value & 0x20)? 1:0;	I2C_SCL=1;	dd();
	I2C_SCL=0;	I2C_SDA=(value & 0x10)? 1:0;	I2C_SCL=1;	dd();

	I2C_SCL=0;	I2C_SDA=(value & 0x08)? 1:0;	I2C_SCL=1;	dd();
	I2C_SCL=0;	I2C_SDA=(value & 0x04)? 1:0;	I2C_SCL=1;	dd();
	I2C_SCL=0;	I2C_SDA=(value & 0x02)? 1:0;	I2C_SCL=1;	dd();
	I2C_SCL=0;	I2C_SDA=(value & 0x01)? 1:0;	I2C_SCL=1;	dd();


	I2C_SCL = 0;	// HHY 3.00
	I2C_SCL = 0;	// HHY 3.00
	I2C_SCL = 0;	//	I2Cdelay();
	I2C_SDA = 1;	//	I2Cdelay();
	dd();
	I2C_SCL = 1;	// HHY 3.00
	I2C_SCL = 1;	// HHY 3.00
	I2C_SCL = 1;	//	I2Cdelay();
	I2C_SCL = 0;

}
#if (_DEBUG_MESSAGE_Monitor==ON)

BYTE AD5110_I2CReadData(void)
{
	int i;
	BYTE value=0;

	I2C_SDA = 1;	dd();
	I2C_SCL = 0;
	
	for(i=0; i<8; i++) {
		I2C_SCL = 1;
		value <<= 1;
		if(I2C_SDA) value |= 1;
//		dd();
		I2C_SCL = 0;
	}

	I2C_SDA = 1;	//dd();
	
	I2C_SCL = 1;	// HHY 3.00
	I2C_SCL = 1;	// HHY 3.00
	I2C_SCL = 1;
	dd();
	I2C_SCL = 0;
//	dd();

	return value;

}
#endif

void WriteAD5110(WORD index, BYTE dat)
{
	BYTE addr;

	addr = (AD5110I2CAddress /*| (index>>7)*/) & 0xfe;

	I2CStart();
	AD5110_I2CWriteData(addr);
	AD5110_I2CWriteData((BYTE)index);
	AD5110_I2CWriteData(dat);
	I2CStop();

	#ifdef DEBUG_EEP
		dPrintf("Write %02xh to [Adrs(%02xh)Index(%02xh)\r\n", (WORD)dat, (WORD)addr, (WORD)index );
	#endif

	delay(2);
}
#if (_DEBUG_MESSAGE_Monitor==ON)

BYTE ReadAD5110(WORD index)
{
	BYTE val;
	BYTE addr;

	addr = (AD5110I2CAddress /*| (index>>7)*/) & 0xfe;

	I2CStart();
	AD5110_I2CWriteData(addr);
	AD5110_I2CWriteData(index);	
//	AD5110_I2CWriteData(0x01);	
	AD5110_I2CWriteData(0x00);	

	I2CStart();
	AD5110_I2CWriteData(addr | 0x01);
	val = AD5110_I2CReadData();
	I2CStop();

	#ifdef DEBUG_EEP
		dPrintf("Read [Adrs:%02xh,Index:%02xh] %02xh\r\n", (WORD)addr, (WORD)index, (WORD)val);
	#endif
	delay(2);
	return val;
}

#endif


