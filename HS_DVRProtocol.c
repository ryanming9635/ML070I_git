/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  HS_DVRProtocol.c                                             										*/
/*                                                                           										*/
/*****************************************************************************/
#include "Config.h"
#include "typedefs.h"
#include "i2c.h"
#include "main.h"
#include "printf.h"
#include "CPU.h"
#include "HS_DVRProtocol.h"
#include "keyremo.h"
#include "Etc_eep.h"
#include "reg.h"

bit protocol_escape_flag;
BYTE protocol_state=0;
BYTE protocol_type;
BYTE protocol_len;
BYTE protocol_param;
BYTE protocol_chksum;
BYTE protocol_data_count;
BYTE protocol_data[PROTOCOL_DATA_LENGTH];


extern DATA  BYTE RS_out;
extern float Encorder1,Encorder2,Encorder3,Encorder4;
extern float Decimal1,Decimal2,Decimal3,Decimal4;
extern StructDVRInfoType g_stDVRInfo;
extern BYTE PowerFlag;
extern BYTE CameraVolt;
extern BYTE PWR_START_flag;
extern StructBatteryInfoType g_stBatteryInfo ;
extern BYTE DVRChangeCurrent;
extern BYTE bytHoldOn3SPowerOff;
extern BYTE bytFastEncoderMode;
extern DWORD ulongRotateNumber;
extern DWORD ulongRotateNumberTELI;
extern WORD bytEncorderCountTemp;
extern BYTE   bytFastEncorderCountTemp;

void DvrReceivePaser(void)
{

if ( RS2_ready())
	{
		BYTE c= RS2_rx();
		
		if (c == 0x7E)
		{	
			#if(_DEBUG_MESSAGE_UART_Protocol==ON)
			GraphicsPrint(CYAN,"\r\n(GOT UART DATA)");
			#endif
			protocol_escape_flag = 0;
			protocol_state = PROTOCOL_STATE_TYPE;
			protocol_chksum = 0;
			return;	
		}
		
		if (c == 0x7D)
		{
			protocol_escape_flag = 1;
			return;
		}
		
		if (protocol_escape_flag)
		{
			protocol_escape_flag = 0;
			c |= 0x70;
		}

		if (protocol_state != PROTOCOL_STATE_CHECKSUM)
		{
			protocol_chksum ^= c;
		}

		switch (protocol_state)
		{
			case PROTOCOL_STATE_TYPE:
				protocol_type = c;
				switch (protocol_type)
				{
				case MCU_PROTOCOL_CMD_SYS_READY_NOTICE:
				case MCU_PROTOCOL_CMD_WATCH_DOG_KICK:
				case MCU_PROTOCOL_CMD_ENTRY_SLEEP:
				case MCU_PROTOCOL_CMD_DISTANCE_RESET:
				case MCU_PROTOCOL_CMD_ENCODER_SET:				   
				case MCU_PROTOCOL_CMD_CAMERA_VOLTAGE:
				case MCU_PROTOCOL_CMD_WDT_SETTING:
				case MCU_PROTOCOL_CMD_GET_MCU_FW:
				case MCU_PROTOCOL_CMD_DVR_REBOOT:
				case MCU_PROTOCOL_CMD_DVR_SHUTDOWN:
				case	MCU_PROTOCOL_PROTOCOL_CMD_ENTER_ISP:
				case MCU_PROTOCOL_CMD_DC12V_PWR_START:	
				case	MCU_PROTOCOL_CMD_SET_CHARGE_CURRENT:				
				case	MCU_PROTOCOL_CMD_GET_ENCODER_COUNT:
				case	MCU_PROTOCOL_CMD_RESET_ENCODER_COUNT:
				case	MCU_PROTOCOL_CMD_SET_MODELE_TYPE:

				      protocol_state = PROTOCOL_STATE_LENGTH;
				      break;
					default:
					   protocol_state = PROTOCOL_STATE_NONE;
					   break;
				}
				break;

			case PROTOCOL_STATE_LENGTH:
				protocol_len = c;
				switch (protocol_type)
				{
					case MCU_PROTOCOL_CMD_SYS_READY_NOTICE:
					case MCU_PROTOCOL_CMD_WATCH_DOG_KICK:
					case MCU_PROTOCOL_CMD_ENTRY_SLEEP:
					case MCU_PROTOCOL_CMD_DISTANCE_RESET:
					case MCU_PROTOCOL_CMD_ENCODER_SET:				   
					case MCU_PROTOCOL_CMD_CAMERA_VOLTAGE:
					case MCU_PROTOCOL_CMD_WDT_SETTING:
					case MCU_PROTOCOL_CMD_GET_MCU_FW:
					case MCU_PROTOCOL_CMD_DVR_REBOOT:
					case MCU_PROTOCOL_CMD_DVR_SHUTDOWN:
				case	MCU_PROTOCOL_PROTOCOL_CMD_ENTER_ISP:	
				case	MCU_PROTOCOL_CMD_DC12V_PWR_START:
				case	MCU_PROTOCOL_CMD_SET_CHARGE_CURRENT:
					case	MCU_PROTOCOL_CMD_GET_ENCODER_COUNT:
					case	MCU_PROTOCOL_CMD_RESET_ENCODER_COUNT:
					case	MCU_PROTOCOL_CMD_SET_MODELE_TYPE:
					
						if (protocol_len == 0)
						{
							protocol_state = PROTOCOL_STATE_CHECKSUM;
						}
						else
						{		
							protocol_data_count = 0;
							protocol_state = PROTOCOL_STATE_DATA;
							
						#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
						GraphicsPrint(CYAN,"(1)");
						 #endif
						}
						break;						
					default:
						protocol_state = PROTOCOL_STATE_NONE;
						break;
				}
				break;

			case PROTOCOL_STATE_DATA:
				switch (protocol_type) {
					case MCU_PROTOCOL_CMD_SYS_READY_NOTICE:
					case MCU_PROTOCOL_CMD_WATCH_DOG_KICK:
					case MCU_PROTOCOL_CMD_ENTRY_SLEEP:
					case MCU_PROTOCOL_CMD_DISTANCE_RESET:
					case MCU_PROTOCOL_CMD_ENCODER_SET:				   
					case MCU_PROTOCOL_CMD_CAMERA_VOLTAGE:
					case MCU_PROTOCOL_CMD_WDT_SETTING:
					case MCU_PROTOCOL_CMD_GET_MCU_FW:
					case MCU_PROTOCOL_CMD_DVR_REBOOT:
					case MCU_PROTOCOL_CMD_DVR_SHUTDOWN:
				case	MCU_PROTOCOL_PROTOCOL_CMD_ENTER_ISP:						
					case	MCU_PROTOCOL_CMD_DC12V_PWR_START:
					case	MCU_PROTOCOL_CMD_SET_CHARGE_CURRENT:
						case	MCU_PROTOCOL_CMD_GET_ENCODER_COUNT:
						case	MCU_PROTOCOL_CMD_RESET_ENCODER_COUNT:
						case	MCU_PROTOCOL_CMD_SET_MODELE_TYPE:
						

				      protocol_data[protocol_data_count] = c;
				   	protocol_data_count++;
					   if (protocol_data_count == protocol_len) {
							protocol_state = PROTOCOL_STATE_CHECKSUM;
							#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
							   GraphicsPrint(CYAN,"(2)");
							#endif
						}
						#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
					   GraphicsPrint(CYAN,"(3)");
					   #endif
					
					   break;
					default:
						protocol_state = PROTOCOL_STATE_NONE;
						break;
				}
				break;

			case PROTOCOL_STATE_CHECKSUM:
				if (protocol_chksum == c)
				{

			#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
			GraphicsPrint(CYAN,"\r\n(Work CMD=%02x)",(WORD)protocol_type);
			#endif				
					switch (protocol_type)
					{
					case MCU_PROTOCOL_CMD_SYS_READY_NOTICE:						
						#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
						 GraphicsPrint(CYAN,"\r\n(CMD:SYS READY)");
						#endif
						SET_DVR_SystemReadyNotic();
						SET_BATTERY_CHARGE_STATE(_BATT_STATUS_NONE);
							if(GET_DVR_Reboot()==_TRUE) 
							{
							 CLR_DVR_Reboot();	
							 UserInterfaceBatteryChargeMode(_BATT_STATUS_DVR_ON);
							 MCUTimerCancelTimerEvent( _USER_TIMER_EVENT_OSD_DVR_REBOOT);
							}
						if(GET_DVR_WDTkickDisable()==_FALSE)
						MCUTimerActiveTimerEvent(SEC(10), _SYSTEM_TIMER_EVENT_JUDGE_WDT_ECHO);
						P3M1=0x00|0x30;//p3.4 and P3.5 set push pull mode ryan@20210223
						break;
					case MCU_PROTOCOL_CMD_WATCH_DOG_KICK:
						#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
						GraphicsPrint(CYAN,"\r\n(CMD:WDT kick)");	
						#endif
						MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_JUDGE_WDT_ECHO);	
						if(GET_DVR_WDTkickDisable()==_FALSE)
						MCUTimerActiveTimerEvent(SEC(10), _SYSTEM_TIMER_EVENT_JUDGE_WDT_ECHO);	
						break;
					case MCU_PROTOCOL_CMD_ENTRY_SLEEP: 	
						#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
						GraphicsPrint(CYAN,"\r\n(CMD:ENTRY_SLEEP)");	
						#endif						
						//SET_DVR_EntrySleepMode();
						MCUTimerActiveTimerEvent(SEC(1),_USER_TIMER_EVENT_OSD_MCU_ENTRY_SLEEP_MODE);
						break;
					case MCU_PROTOCOL_CMD_DISTANCE_RESET:
						#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
						GraphicsPrint(CYAN,"\r\n(CMD:DISTANCE_RESET)");	
						#endif												
						//SET_DVR_DistanceReset();					
						MCUTimerActiveTimerEvent(SEC(0.1),_USER_TIMER_EVENT_OSD_DISTANCE_RESET);
						break;
					case MCU_PROTOCOL_CMD_ENCODER_SET: 	
						#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
						GraphicsPrint(CYAN,"\r\n (CMD:ENCODER_SET)");	
						Printf("\r\n Encorder1=%02x",(WORD)protocol_data[0]);
						Printf("\r\n Decimal1=%02x",(WORD)protocol_data[1]);				
						Printf("\r\n Encorder2=%02x",(WORD)protocol_data[2]);
						Printf("\r\n Decimal2=%02x",(WORD)protocol_data[3]);				
						Printf("\r\n Encorder3=%02x",(WORD)protocol_data[4]);
						Printf("\r\n Decimal3=%02x",(WORD)protocol_data[5]);				
						Printf("\r\n Encorder4=%02x",(WORD)protocol_data[6]);
						Printf("\r\n Decimal4=%02x",(WORD)protocol_data[7]);				
						#endif

						Encorder1=((protocol_data[0]>>4)*10)+(0x0f&protocol_data[0]);
						Decimal1=((protocol_data[1]>>4)*10)+(0x0f&protocol_data[1]);
						
						Encorder2=((protocol_data[2]>>4)*10)+(0x0f&protocol_data[2]);
						Decimal2=((protocol_data[3]>>4)*10)+(0x0f&protocol_data[3]);
						
						Encorder3=((protocol_data[4]>>4)*10)+(0x0f&protocol_data[4]);
						Decimal3=((protocol_data[5]>>4)*10)+(0x0f&protocol_data[5]);
						
						Encorder4=((protocol_data[6]>>4)*10)+(0x0f&protocol_data[6]);
						Decimal4=((protocol_data[7]>>4)*10)+(0x0f&protocol_data[7]);

						WriteEEP(EEP_Encorder1,protocol_data[0]);
						WriteEEP(EEP_Decimal1,protocol_data[1]);

						WriteEEP(EEP_Encorder2,protocol_data[2]);
						WriteEEP(EEP_Decimal2,protocol_data[3]);

						WriteEEP(EEP_Encorder3,protocol_data[4]);
						WriteEEP(EEP_Decimal3,protocol_data[5]);

						WriteEEP(EEP_Encorder4,protocol_data[6]);
						WriteEEP(EEP_Decimal4,protocol_data[7]);

						MCUTimerActiveTimerEvent(SEC(1),_USER_TIMER_EVENT_OSD_ENCODER_DEVIATION_SETTING);						

						break;

					case MCU_PROTOCOL_CMD_CAMERA_VOLTAGE:	
						#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
						GraphicsPrint(CYAN,"\r\n(CMD:CAMERA_VOLTAGE)");	
						#endif		

						#if 1
						if(protocol_data[0]<5)
						protocol_data[0]=5;
						
						CameraVolt=((protocol_data[0]-5)*10)+(protocol_data[1]);

						if(CameraVolt>40)
							CameraVolt=40;
						else 
						      CameraVolt=(40-CameraVolt);
						MCUTimerReactiveTimerEvent(SEC(0.5),_USER_TIMER_EVENT_OSD_SET_CAMERA_VOLTAGE);
						#else
						if(protocol_data[0])
							{
								if(CameraVolt)
								{
								CameraVolt--;
								}
								else
								{
								CameraVolt=0;
								}
								MCUTimerReactiveTimerEvent(SEC(0.5),_USER_TIMER_EVENT_OSD_CAMERA_VOLTAGE_INC);
							}
						else
							{
								if(CameraVolt<40)
									{
								CameraVolt++;
								}
								else
								{
									CameraVolt=40;
								}
							MCUTimerReactiveTimerEvent(SEC(0.5),_USER_TIMER_EVENT_OSD_CAMERA_VOLTAGE_DEC);
							}
						#endif
						break;

					case MCU_PROTOCOL_CMD_WDT_SETTING: 	
						#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
						GraphicsPrint(CYAN,"\r\n(CMD:WDT_SETTING)");	
						#endif									

						if(protocol_data[0]==0)
							{
							CLR_DVR_WDTkickDisable();
							#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
							GraphicsPrint(CYAN,"\r\n(CMD:WDT_Enable=1)");	
							#endif
							}
						else if(protocol_data[0]==1)
							{
							MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_JUDGE_WDT_ECHO);
							SET_DVR_WDTkickDisable();
							#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
							GraphicsPrint(CYAN,"\r\n(CMD:WDT_Enable=0)");	
							#endif
							}
						break;

					case MCU_PROTOCOL_CMD_GET_MCU_FW:						
						#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
						GraphicsPrint(CYAN,"\r\n(CMD:GET_MCU_FW)");	
						#endif												
						MCUTimerActiveTimerEvent(SEC(1),_USER_TIMER_EVENT_OSD_GET_MCU_VERSION);
						break;

					case MCU_PROTOCOL_CMD_DVR_REBOOT: 
						#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
						GraphicsPrint(CYAN,"\r\n(CMD:DVR_REBOOT)");	
						#endif	
						
							if(protocol_data[0]==0)
							{
							CLR_DVR_SystemReadyNotic();
							MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_JUDGE_WDT_ECHO);
							//MCUTimerActiveTimerEvent(SEC(60), _SYSTEM_TIMER_EVENT_JUDGE_WDT_ECHO);	

							MCUTimerCancelTimerEvent(_SYSTEM_TIMER_EVENT_GRN_BLINK);
							MCUTimerCancelTimerEvent(_SYSTEM_TIMER_EVENT_GRN_2S_BLINK);
							MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_RED_BLINK);
							MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_GRN_RED_BLINK);
							MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_GRN_ON_RED_BLINK);
							MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_GRN_2S_BLINK_RED_ON);

							MCUTimerActiveTimerEvent(SEC(0.1), _SYSTEM_TIMER_EVENT_GRN_RED_BLINK);//Update LED Status
							
							}
							else if(protocol_data[0]==1)
							{
							SET_DVR_Reboot();
							MCUTimerActiveTimerEvent(SEC(1),_USER_TIMER_EVENT_OSD_DVR_REBOOT);
							MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_JUDGE_WDT_ECHO);
							}
							else if(protocol_data[0]==2)
							{
							SET_DVR_Reboot();
							SET_DVR_RebootAndPower();
							MCUTimerActiveTimerEvent(SEC(1),_USER_TIMER_EVENT_OSD_DVR_REBOOT);
							MCUTimerCancelTimerEvent( _SYSTEM_TIMER_EVENT_JUDGE_WDT_ECHO);
							}

						break;

					case MCU_PROTOCOL_CMD_DVR_SHUTDOWN:
						#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
						GraphicsPrint(CYAN,"\r\n(CMD:DVR_SHUTDOWN)");	
						#endif	
						
							if((protocol_data[0]==1))
								{
								CLR_DVR_Shutdown();
								bytHoldOn3SPowerOff=OFF;
								#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
								GraphicsPrint(GREEN,"\r\n(CMD:bytHoldOn3SPowerOff=OFF)");	
								#endif
								}
							else if((PowerFlag==ON)&&(protocol_data[0]==0))
							{
							CLR_DVR_Shutdown();
							PowerFlag=OFF;
							WriteEEP(EEP_Powerflag,PowerFlag);

							if(GET_DVR_EntrySleepMode()==TRUE)
							CLR_DVR_EntrySleepMode();
							}
							if((PowerFlag==ON)&&(protocol_data[0]==2))
							{
							SET_BATTERY_CAPACITY_LOW_FLAG();
							CLR_DVR_Shutdown();
							PowerFlag=OFF;
							WriteEEP(EEP_Powerflag,PowerFlag);
							if(ReadEEP(EEP_LowBattery_Flag)==OFF)
							WriteEEP(EEP_LowBattery_Flag,ON);
							#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
							GraphicsPrint(RED,"\r\n(CMD:LOW Batt volt. shutdown)");	
							#endif	
							if(GET_DVR_EntrySleepMode()==TRUE)
							CLR_DVR_EntrySleepMode();
							}
							else if((PowerFlag==ON)&&(protocol_data[0]==3))
								{
								bytHoldOn3SPowerOff=ON;
								#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
								GraphicsPrint(RED,"\r\n(CMD:bytHoldOn3SPowerOff)");	
								#endif
								}

						MCUTimerCancelTimerEvent(_USER_TIMER_EVENT_OSD_DVR_SHUTDOWN);							

						if(bytHoldOn3SPowerOff==ON)						
						MCUTimerActiveTimerEvent(SEC(15), _USER_TIMER_EVENT_OSD_DVR_SHUTDOWN);

						break;

					case	MCU_PROTOCOL_PROTOCOL_CMD_ENTER_ISP:	
						#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
						GraphicsPrint(CYAN,"\r\n(CMD:MCU_ENTER_ISP_MODE)");	
						#endif							
						MCUTimerActiveTimerEvent(SEC(1),_USER_TIMER_EVENT_OSD_DVR_ENTER_ISP_MODE);
						break;

					case	MCU_PROTOCOL_CMD_DC12V_PWR_START:

						if(protocol_data[0]==ON)
							{
							WriteEEP(EEP_DC12_PWR_START,ON);
							PWR_START_flag=ON;
							}
						else
							{
							WriteEEP(EEP_DC12_PWR_START,OFF);
							PWR_START_flag=OFF;
							}

						#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
						GraphicsPrint(CYAN,"\r\n(CMD:MCU_DC12V_PWR_START=%d)",(WORD)ReadEEP(EEP_DC12_PWR_START));	
						#endif	

						
						break;
					case	MCU_PROTOCOL_CMD_SET_CHARGE_CURRENT:
							if(protocol_data[0]==ON)										
								DVRChangeCurrent=0x11;
							else
								DVRChangeCurrent=0x10;

					#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
					GraphicsPrint(GREEN,"\r\n(CMD:SET_CHARGE_CURRENT=%02x)",(WORD)DVRChangeCurrent);	
					#endif	
					SET_BATTERY_CHARGE_STATE(_BATT_STATUS_NONE);///reset charge current 


						break;
					case MCU_PROTOCOL_CMD_GET_ENCODER_COUNT:
							#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
							GraphicsPrint(GREEN,"\r\n(CMD:MCU_PROTOCOL_CMD_GET_ENCODER_COUNT)");	
							#endif
							MCUTimerActiveTimerEvent(SEC(1),_USER_TIMER_EVENT_REPLY_ENCODER_COUNT);
						break;
					case MCU_PROTOCOL_CMD_RESET_ENCODER_COUNT:
							#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
							GraphicsPrint(GREEN,"\r\n(CMD:MCU_PROTOCOL_CMD_RESET_ENCODER_COUNT)");	
							#endif
							MCUTimerActiveTimerEvent(SEC(1),_USER_TIMER_EVENT_RESET_ENCODER_COUNT);
						break;
					case MCU_PROTOCOL_CMD_SET_MODELE_TYPE:

							if(protocol_data[0]==ON)
								{
								bytFastEncoderMode=ON;
								IP	 |= 0x01;		// INT0 high priority 				
								IPH	 |= 0x01;		// INT0 high priority				
								IE	 |= 0x01;		// Enable INT0
								bytFastEncorderCountTemp=0;
								}
							else
								{
								bytFastEncoderMode=OFF;
								IP	 &= ~(0x01);		// clear INT0 high priority 				
								IPH	 &= ~(0x01);		// clear INT0 high priority				
								IE	 &= ~(0x01);		// clear Enable INT0
								bytEncorderCountTemp=0;
								}

						

							#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
							GraphicsPrint(CYAN,"\r\n(CMD:MCU_PROTOCOL_CMD_SET_MODELE_TYPE=%d)",(WORD)bytFastEncoderMode);	
							#endif
						break;
					default:
						break;
					}
					
				}
				else
						{
						#if(_DEBUG_MESSAGE_UART_Protocol==ON)	
						DEBUG_MESSAGE("(CS error)");
						#endif
						}
				protocol_state = PROTOCOL_STATE_NONE;
				break;
			default:
				break;
		}
	}


}


