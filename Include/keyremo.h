/*****************************************************************************/
/*                                                                           										*/
/*  TELI ML070I   MCU                    													*/
/*                                                                           										*/
/*  keyremo.h		                                            										*/
/*                                                                           										*/
/*****************************************************************************/

#ifndef	__KEYREMO_H__
#define	__KEYREMO_H__

#define UPKEY		    0x20
#define DOWNKEY		    0x40

BYTE GetKey(BYTE repeat);
void Scankey(void);


#endif
