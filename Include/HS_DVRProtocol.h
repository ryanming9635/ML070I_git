/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  HS_DVRProtocol.c                                             										*/
/*                                                                           										*/
/*****************************************************************************/

#ifndef _HS_DVRProtocol_
#define _HS_DVRProtocol_

//DVR to MCU command 
#define MCU_PROTOCOL_CMD_SYS_READY_NOTICE		0x01	///DVR system ready notice
#define MCU_PROTOCOL_CMD_WATCH_DOG_KICK			0x02	///DVR WDT kick
#define MCU_PROTOCOL_CMD_ENTRY_SLEEP				0x03	///DVR sleep mode
#define MCU_PROTOCOL_CMD_DISTANCE_RESET			0x04	///DVR distance reset  0M
#define MCU_PROTOCOL_CMD_ENCODER_SET				0x05	///Encoder deviation setting
#define MCU_PROTOCOL_CMD_CAMERA_VOLTAGE			0x06	///Camera voltage adjust
#define MCU_PROTOCOL_CMD_WDT_SETTING                         0x07	///stop retoot 
#define MCU_PROTOCOL_CMD_DC12V_PWR_START               0x08	///DC12v power start

#define MCU_PROTOCOL_CMD_GET_MCU_FW			0x10	///MCU FW version
#define MCU_PROTOCOL_CMD_DVR_REBOOT			0x11	///DVR Reboot
#define MCU_PROTOCOL_CMD_DVR_SHUTDOWN		0x12	///DVR shutdown
#define MCU_PROTOCOL_PROTOCOL_CMD_ENTER_ISP  0x30 ///isp programming MCU

#define MCU_PROTOCOL_CMD_SET_CHARGE_CURRENT	0x85	//change the BAT of charge current for testing , 0:300mA 1:1A

#define MCU_PROTOCOL_CMD_GET_ENCODER_COUNT		0x86	//Get encoding count
#define MCU_PROTOCOL_CMD_RESET_ENCODER_COUNT	0x87	//Reset encoding count
#define MCU_PROTOCOL_CMD_SET_MODELE_TYPE			0x88	//Set module type

//MCU to DVR command 
#define MCU_PROTOCOL_CMD_REPLY_MCU_FW  			0x80	//Reply MCU firmware version
#define MCU_PROTOCOL_CMD_REGULAR_DATA   			0x81	//Regular data
#define MCU_PROTOCOL_CMD_SYSTEM_SHUTDOWN   		0x82	//system shutdown
#define MCU_PROTOCOL_CMD_ENTRY_FACTORY   			0x83	//Entry factory mode
#define MCU_PROTOCOL_CMD_SLEEP_WAKE_UP   			0x84	//notice DVR wake up from the sleep mode


#define PROTOCOL_DATA_LENGTH								256

#define PROTOCOL_STATE_NONE								0
#define PROTOCOL_STATE_TYPE								1
#define PROTOCOL_STATE_LENGTH								2
#define PROTOCOL_STATE_DATA								3
#define PROTOCOL_STATE_CHECKSUM							4

void DvrReceivePaser(void);
extern void MCUTimerDelayXms(WORD usNum);
extern void MCUTimerReactiveTimerEvent(WORD usTime, BYTE ucEventID);
extern void MCUTimerCancelTimerEvent(BYTE ucEventID);
extern void UserInterfaceBatteryChargeMode(EnumBatteryStatus enumSwitch);


#endif

