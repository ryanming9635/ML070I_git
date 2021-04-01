/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  CPU.h                                                            										*/
/*                                                                           										*/
/*****************************************************************************/

#ifndef __CPU__
#define __CPU__

#define	I2C_SCL			P1_0
#define	I2C_SDA			P1_1
#define 	BUF_MAX 		30
#define 	DVR_BUF_MAX 	30

//--------------------------------------------------
// Panel  Related MACRO
//--------------------------------------------------
#define GET_P_IO1()	 	(P3_4)	
#define SET_P_IO1()	 	(P3_4 = 1)  	
#define CLR_P_IO1()	 	(P3_4 = 0)	

#define GET_P_IO2()	 	(P3_5)	
#define SET_P_IO2()	 	(P3_5 = 1)  	
#define CLR_P_IO2()	 	(P3_5 = 0)	

#define GET_BL_PWM()	 	(P0_6)	
#define SET_BL_PWM()	 	(P0_6 = 1)  	//ON
#define CLR_BL_PWM()	 	(P0_6 = 0)	//OFF

#define GET_PCON3V3_P()	 (P0_3)	
#define SET_PCON3V3_P()	 (P0_3 = 0)  	 //ON
#define CLR_PCON3V3_P()	 (P0_3 = 1)	//OFF

#define GET_PCON5V_P()	 (P0_4)	
#define SET_PCON5V_P()	 (P0_4 =0)  	 //ON
#define CLR_PCON5V_P()	 (P0_4 = 1)	//OFF

#define GET_PCON5V()	 	(P0_2)	
#define SET_PCON5V()	 	(P0_2 = 0)  	 //ON
#define CLR_PCON5V()	 	(P0_2 = 1)	//OFF


#define GET_Panel_EN()	 (P2_1)	
#define SET_Panel_EN()	 (P2_1 = 1)  	 //ON
#define CLR_Panel_EN()	 (P2_1 = 0)	//OFF

#define GET_CTP_INT()	 	(P2_4)	
#define SET_CTP_INT()	 	(P2_4 = 1)  	 //ON
#define CLR_CTP_INT()	 	(P2_4 = 0)	//OFF

#define GET_CTP_RST()	 	(P2_5)	
#define SET_CTP_RST()	 	(P2_5 = 1)  	 //ON
#define CLR_CTP_RST()	 	(P2_5 = 0)	//OFF

#define GET_PWCTRL()	 	(P3_7)	
#define SET_PWCTRL()	 	(P3_7 = 1)  	 //ON
#define CLR_PWCTRL()	 	(P3_7 = 0)	//OFF

//--------------------------------------------------
// Battery  Related MACRO
//--------------------------------------------------
#define GET_PWR_GOOD()	(P1_5)	  

#define GET_STAT1()		(P0_0)	  
#define GET_STAT2()		(P0_1)	  

#define GET_AC_MODE()	(P4_2)	
#define SET_AC_MODE()	(P4_2 = 1)  	 //ON
#define CLR_AC_MODE()	(P4_2 = 0)	//OFF

//--------------------------------------------------
// DVR Power  Related MACRO
//--------------------------------------------------
#define GET_BAT_SYS()	(P2_6)	
#define SET_BAT_SYS()	(P2_6 = 1)  	 //ON
//#define CLR_BAT_SYS()	(P2_6 = 1/*0*/)	//OFF for EP2 board
#define CLR_BAT_SYS()	(P2_6 = 0)	//OFF for EP2 board ryan@20210311

//--------------------------------------------------
// CAM Power  Related MACRO
//--------------------------------------------------
#define GET_PCON_CAM()	 	(P2_2)	
#define SET_PCON_CAM()	 	(P2_2 = 1)  	 //ON
#define CLR_PCON_CAM()	 	(P2_2 = 0)	//OFF

//--------------------------------------------------
// TW8836 Power  Related MACRO
//--------------------------------------------------
#define GET_PCON3V3_TW()	 	(P3_6)	
#define SET_PCON3V3_TW()	 	(P3_6 =0/*1*/)  	 //ON for EP2 board
#define CLR_PCON3V3_TW()	 	(P3_6 = 1/*0*/)	//OFF	for EP2 board

#define GET_TW8836_RST()	 		(P2_0)	
#define SET_TW8836_RST()	 		(P2_0 = 0)  	 //ON
#define CLR_TW8836_RST()	 		(P2_0 = 1)	//OFF


//--------------------------------------------------
// I/O board  Related MACRO
//--------------------------------------------------

#define GET_GREEN()			 	(P4_0)	
#define SET_GREEN()	 			(P4_0 = 0)  	 
#define CLR_GREEN()	 			(P4_0 = 1)	

#define GET_RED()				 	(P4_1)	
#define SET_RED()		 			(P4_1 = 0)  	 
#define CLR_RED()	 				(P4_1 = 1)	

#define GET_PSW()			 	(P4_3)	
#define SET_PSW()	 			(P4_3 = 1)  	 
#define CLR_PSW()	 			(P4_3 = 0)	

#define GET_ADAP_12()			(P0_5)	
#define SET_ADAP_12()	 		(P0_5 = 1)  	 
#define CLR_ADAP_12()	 		(P0_5 = 0)	


void 	InitCPU(void);
void	InitVars(void);
void	InitTechwell(void);
void 	main_init (void);
void	PowerDown( void );
void 	PowerLED(BYTE flag);
void SET_PWM(BYTE index, BYTE val);
void ShowWorkingTimer(void);
BYTE Check_ADAP_IN(void);	
void SET_MCU_POWER_DOWN_MODE(void);

#endif
