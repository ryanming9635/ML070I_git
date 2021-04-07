/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  Config.h                                                            										*/
/*                                                                           										*/
/*****************************************************************************/

#ifndef __CONFIG_H__
#define __CONFIG_H__

//-----------------------------------------------------------------------------
//		Firmware Version
//-----------------------------------------------------------------------------
#define	MCU_FW_Major			0x00	
#define	MCU_FW_Sub0			0x00		
#define	MCU_FW_Sub1			0x37	

//-----------------------------------------------------------------------------
//		Compiler
//-----------------------------------------------------------------------------
#define KEILC						// Keil C 5.0

#if 1
#define HS_DEBUG                        ON
#else
#define HS_DEBUG                        OFF
#endif

#define  _2ND_DECIMAL           			ON
#define  _POWER_DOWN_ENABLE             	ON
#define  _1KHZ_PWM			             	OFF

//-----------------------------------------------------------------------------
//		Battery Charge Configure
//-----------------------------------------------------------------------------
#define BATTERY_LOW_CHARGE_LEVEL    		662
#define BATTERY_NORMAL_CHARGE_LEVEL	688  //>=12V
#define BATTERY_STOP_CHARGE_LEVEL    	885///0x375  >=15.5V

#define BATTERY_ACD_MAX   1023
#define BATTERY_ACD_MIN   50
#define BATTERY_ACD_no_battery  928 


#define BATTERY_CAPACITY_LEVEL0	 		680//678//688//<12V
#define BATTERY_CAPACITY_LEVEL1			763//771//12V-13.5V
#define BATTERY_CAPACITY_LEVEL2			802//811//13.5V-14.2V
#define BATTERY_CAPACITY_LEVEL3			848//860//14.2V-15V
#define BATTERY_CAPACITY_LEVEL4	     		848//860//>15V

#define BATTERY_CAPACITY_NO_STARTUP 	665//675//<11.8

#define BATTERY_CAPACITY_STARTUP 		700//710//>12.5v

#define BATTERY_CAPACITY_MIN	 	    	650///662//11.6	V

#define BATTERY_CAPACITY_HIGH	     		885//15.5V
//#define BATTERY_CAPACITY_HIGH	     		936//885//15.5V//16.4

//#define BATTERY_CAPACITY_HIGH_STOP        	910//15.9V
//#define BATTERY_CAPACITY_HIGH_STOP        	940//16.5V stop charge
//#define BATTERY_CAPACITY_HIGH_STOP        	935//16.4V stop charge
#define BATTERY_CAPACITY_HIGH_STOP        	(927-9)//16.2V stop charge
//#define BATTERY_CAPACITY_HIGH_STOP        	945//16.4V stop charge
//#define BATTERY_CAPACITY_HIGH_STOP        	936//16.4V stop charge

//#define BATTERY_CAPACITY_HIGH_STOP        	960//16.8V stop charge
 
#if 1  //these are sean's test tempature of result
#define BTH_TEMP_MIN 	   		20//90
#define BTH_TEMP_MAX 	 		1023

//#define BTH_TEMP_HIGH 	   		198//55//534// >55 degree
#define BTH_TEMP_HIGH 	   		175///194// >=60 degree //ryan@2021/01/18

//#define BTH_TEMP_HIGH_WARN	   	238//561//  >50 degree
//#define BTH_TEMP_HIGH_WARN	   	195///214//   >=55 degree //ryan@2021/01/18
#define BTH_TEMP_HIGH_WARN	   	222//  >50 degree //ryan@2021/0222


#define BTH_TEMP_LOW_WARN 		(653-18)//789//<0 degree 836//744//   <-5 degree
#define BTH_TEMP_LOW 	   		(746-5)//868//751///<-10 degree


//#define CHARGE_TEMP_HIGH   		258//586///45 degree
//#define CHARGE_TEMP_HIGH   		175///194//>=60 degree //ryan@2021/01/18
#define CHARGE_TEMP_HIGH   		222//>=50 degree //ryan@2021/02/22

//#define CHARGE_TEMP_LOW   		706//<10 degree 789//736///0 degree
#define CHARGE_TEMP_LOW   		789///0 degree

#define TempADC_Counts   				10
#else

#define BTH_TEMP_MIN 	   		20//90
#define BTH_TEMP_MAX 	 		1023

#define BTH_TEMP_HIGH 	   		55//534// >55 degree
#define BTH_TEMP_HIGH_WARN	   	65//561//  >50 degree
#define BTH_TEMP_LOW_WARN 		509//744//   <-5 degree
#define BTH_TEMP_LOW 	   		629//751///<-10 degree


#define CHARGE_TEMP_HIGH   		77//586///45 degree
#define CHARGE_TEMP_LOW   		414//736///0 degree

//#define BTH_TEMP_HIGH 	   		685
//#define BTH_TEMP_HIGH_WARN	   	724
//#define BTH_TEMP_LOW_WARN 		969
//#define BTH_TEMP_LOW 	   		1010
#endif




//-----------------------------------------------------------------------------
//		MPU Clock
//-----------------------------------------------------------------------------
#if 1
//#define CLOCK_11M					// 11.0592MHz
#define CLOCK_22M					// 22.1184MHz
#else 								// INTERNAL_MCU
#define	CLOCK_27M					// 27MHz
#define	REG_START_ADDRESS	0xc000		// register start 0xc000, for page0, 0xc100-page1 
#endif

//-----------------------------------------------------------------------------
//		I2C Device Access Address Mapping
//-----------------------------------------------------------------------------
#define EEPROMI2CAddress		0xa0	// EEPROM (24C16)
#define TW88I2CAddress		0x8a	// TW880x
#define  AD5110I2CAddress    	0x5E//0x58//0x5E//0x58

//-----------------------------------------------------------------------------
//		AD5110 Command Operation Truth table
//-----------------------------------------------------------------------------

#define AD5110_CMD_No_Operation     			0
#define AD5110_CMD_Write_RDAC_To_EEPROM     	1
#define AD5110_CMD_Write_Data_To_RDAC     	2
#define AD5110_CMD_Shutdown     				3
#define AD5110_CMD_Soft_Refresh_RDAC		4
#define AD5110_CMD_Read_RDAC				5
#define AD5110_CMD_Read_EEPROM			        6


#define SERIAL      				// include serial communication routines

#endif	//__CONFIG_H__
