/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  I2C.h 			                                            										*/
/*                                                                           										*/
/*****************************************************************************/

#ifndef	__I2C_H__
#define	__I2C_H__

void I2CReadByte3(BYTE *);
void I2CRead8Byte(BYTE *);

BYTE I2CWriteByte2(BYTE);
BYTE I2CWriteByte3(BYTE*);
BYTE I2CWrite8Byte(BYTE *);

void I2CStart(void);
void I2CWriteData(BYTE value);
BYTE I2CReadData(void);

void I2CStop(void);
void I2Cdelay(void);

#define WriteTW88(a,b)		WriteI2C(TW88I2CAddress, a, b)
#define ReadTW88(a)			ReadI2C(TW88I2CAddress, a)

void WriteEEP	(WORD, BYTE);
BYTE ReadEEP	(WORD);

BYTE ReadI2C	(BYTE addr, BYTE index);
void WriteI2C	(BYTE addr, BYTE index, BYTE val);

void WriteAD5110	(WORD index, BYTE dat);
BYTE ReadAD5110	(WORD index);
void AD5110_I2CWriteData	(BYTE value);
BYTE AD5110_I2CReadData	(void);


#endif	/* __I2C_H__ */
