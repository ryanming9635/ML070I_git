/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  Monitor.h		                                            										*/
/*                                                                           										*/
/*****************************************************************************/
#ifndef __MONITOR__H_
#define __MONITOR__H_


/* monitor.h */


void Mon_tx(BYTE ch);

void MonHelp(void);
BYTE MonGetCommand(void);
void Monitor(void);
void Prompt(void);

BYTE GetMonAddress(void);
void SetMonAddress(BYTE addr);
void MonReadI2C(void);
void MonWriteI2C(void);
extern void SET_PWM(BYTE index, BYTE val);
extern void MCUTimerReactiveTimerEvent(WORD usTime, BYTE ucEventID);
extern void MCUTimerCancelTimerEvent(BYTE ucEventID);


#endif
