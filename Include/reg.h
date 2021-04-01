//
//Registers.h	: Register declaration
//

#ifndef __REGISTERS__
#define __REGISTERS__

#ifdef INTERNAL_MCU
/*  BYTE Registers  */
sfr P0    = 0x80;
sfr P1    = 0x90;
sfr P2    = 0xA0;
sfr P3    = 0xB0;
sfr PSW   = 0xD0;
sfr ACC   = 0xE0;
sfr B     = 0xF0;
sfr SP    = 0x81;
sfr DPL   = 0x82;
sfr DPH   = 0x83;
sfr PCON  = 0x87;
sfr TCON  = 0x88;
sfr TMOD  = 0x89;
sfr TL0   = 0x8A;
sfr TL1   = 0x8B;
sfr TH0   = 0x8C;
sfr TH1   = 0x8D;
sfr IE    = 0xA8;
sfr IP    = 0xB8;
sfr SCON  = 0x98;
sfr SBUF  = 0x99;

//sfr P4    = 0xd8;

/*  8052 Extensions  */
sfr EXIF  = 0x91;		// external interrupt 2,3,4,5 flags should be cleared by software
sfr T2CON = 0xC8;
sfr RCAP2L = 0xCA;
sfr RCAP2H = 0xCB;
sfr TL2   = 0xCC;
sfr TH2   = 0xCD;
sfr EICON = 0xD8;
sfr EIE   = 0xE8;
sfr EIP	  = 0xF8;

/*	TW88 Core Extension */
sfr	BANKREG 	= 0x9A;
sfr	SPICONTROL 	= 0x9B;
sfr	T0HIGH 		= 0x9C;
sfr	T0LOW 		= 0x9D;
sfr	T1HIGH 		= 0x9E;
sfr	T1LOW 		= 0x9F;
sfr	T2HIGH 		= 0x93;
sfr	T2LOW 		= 0x94;

/*  BIT Registers  */
/*  PSW  */
sbit CY    = 0xD7;
sbit AC    = 0xD6;
sbit F0    = 0xD5;
sbit RS1   = 0xD4;
sbit RS0   = 0xD3;
sbit OV    = 0xD2;
sbit P     = 0xD0;

/*  TCON  */
sbit TF1   = 0x8F;
sbit TR1   = 0x8E;
sbit TF0   = 0x8D;
sbit TR0   = 0x8C;
sbit IE1   = 0x8B;
sbit IT1   = 0x8A;
sbit IE0   = 0x89;
sbit IT0   = 0x88;

/*  IE  */
sbit EA    = 0xAF;
sbit ES    = 0xAC;
sbit ET1   = 0xAB;
sbit EX1   = 0xAA;
sbit ET0   = 0xA9;
sbit EX0   = 0xA8;

/*  EIE  */
sbit EWDI  = 0xEC;		// Watch dog timer interrupt enable
sbit EX5   = 0xEB;		// external interrupt 5 enable
sbit EX4   = 0xEA;		// external interrupt 4 enable
//sbit EX3   = 0xE9;		// external interrupt 3 enable
//sbit EX2   = 0xE8;		// external interrupt 2 enable

/*  IP  */
sbit PS    = 0xBC;
sbit PT1   = 0xBB;
sbit PX1   = 0xBA;
sbit PT0   = 0xB9;
sbit PX0   = 0xB8;

/*  EIP  */
sbit PWDI  = 0xFC;		// Watch dog timer interrupt priority
sbit PX5   = 0xFB;		// external interrupt5 priority
sbit PX4   = 0xFA;		// external interrupt4 priority
sbit PX3   = 0xF9;		// external interrupt3 priority
sbit PX2   = 0xF8;		// external interrupt2 priority

/*  P3  */
sbit RD    = 0xB7;
sbit WR    = 0xB6;
sbit T1    = 0xB5;
sbit T0    = 0xB4;
sbit INT1  = 0xB3;
sbit INT0  = 0xB2;
sbit TXD   = 0xB1;
sbit RXD   = 0xB0;

/*  SCON  */
sbit SM0   = 0x9F;
sbit SM1   = 0x9E;
sbit SM2   = 0x9D;
sbit REN   = 0x9C;
sbit TB8   = 0x9B;
sbit RB8   = 0x9A;
sbit TI    = 0x99;
sbit RI    = 0x98;

/*  8052 Extensions  */
/*  IE  */
sbit ET2   = 0xAD;

/*  IP  */
sbit PT2   = 0xBD;

/*  P1  */
sbit T2EX  = 0x91;
sbit T2    = 0x90;
             
/*  T2CON  */
sbit TF2   = 0xCF;
sbit EXF2  = 0xCE;
sbit RCLK  = 0xCD;
sbit TCLK  = 0xCC;
sbit EXEN2 = 0xCB;
sbit TR2   = 0xCA;
sbit C_T2  = 0xC9;
sbit CP_RL2= 0xC8;

sfr	CHPENR = 0xF6;
sfr	CHPCON = 0xBF;
sfr	SFRAL  = 0xC4;
sfr	SFRAH  = 0xC5;
sfr	SFRFD  = 0xC6;
sfr	SFRCN  = 0xC7;

sbit P0_0  = 0x80;
sbit P0_1  = 0x81;
sbit P0_2  = 0x82;
sbit P0_3  = 0x83;
sbit P0_4  = 0x84;
sbit P0_5  = 0x85;
sbit P0_6  = 0x86;
sbit P0_7  = 0x87;

sbit P1_0  = 0x90;
sbit P1_1  = 0x91;
sbit P1_2  = 0x92;
sbit P1_3  = 0x93;
sbit P1_4  = 0x94;
sbit P1_5  = 0x95;
sbit P1_6  = 0x96;
sbit P1_7  = 0x97;

sbit P2_0  = 0xa0;
sbit P2_1  = 0xa1;
sbit P2_2  = 0xa2;
sbit P2_3  = 0xa3;
sbit P2_4  = 0xa4;
sbit P2_5  = 0xa5;
sbit P2_6  = 0xa6;
sbit P2_7  = 0xa7;

sbit P3_0  = 0xb0;
sbit P3_1  = 0xb1;
sbit P3_2  = 0xb2;
sbit P3_3  = 0xb3;
sbit P3_4  = 0xb4;
sbit P3_5  = 0xb5;
sbit P3_6  = 0xb6;
sbit P3_7  = 0xb7;

#else	// external MCU=WINBOND
/*  BYTE Registers  */
sfr P0    = 0x80;
sfr P1    = 0x90;
sfr P2    = 0xA0;
sfr P3    = 0xB0;
sfr PSW   = 0xD0;
sfr ACC   = 0xE0;
sfr B     = 0xF0;
sfr SP    = 0x81;
sfr DPL   = 0x82;
sfr DPH   = 0x83;
sfr PCON  = 0x87;
sfr TCON  = 0x88;
sfr TMOD  = 0x89;
sfr TL0   = 0x8A;
sfr TL1   = 0x8B;
sfr TH0   = 0x8C;
sfr TH1   = 0x8D;
sfr CKCON = 0x8E;		//clock control register, default =0x01
sfr IE    = 0xA8;
sfr IP    = 0xB8;
sfr SCON  = 0x98;
sfr SBUF  = 0x99;
sfr P4 = 0xE8; //Megawin Ryan@970716
//sfr P4    = 0xa5;      //SST MCU
//sfr P4    = 0xd8;		//winbond<960820>
sfr PCON2  = 0xC7;

//INT2 configuration
sfr XICON = 0xC0;
sbit IT2  = 0xC0;
sbit IE2  = 0xC1;
sbit EX2  = 0xC2;
sbit PX2  = 0xC3;
sbit IT3  = 0xC4;
sbit IE3  = 0xC5;
sbit EX3  = 0xC6;
sbit PX3  = 0xC7;

/*  8052 Extensions  */
sfr ADCTL = 0xC5;		//william-20111017
sfr ADCV = 0xC6;		//william-20111017
sfr ADCVL = 0xE8;		//william-20111017
sfr T2CON = 0xC8;
sfr RCAP2L = 0xCA;
sfr RCAP2H = 0xCB;
sfr TL2   = 0xCC;
sfr TH2   = 0xCD;

// for UART2
sfr S2CON  = 0xAA;
sfr S2BRT  = 0xBA;
sfr S2BUF  = 0x9A;
sfr AUXR2  = 0xA6;
sfr AUXIE  = 0xAD;

sfr AUXR  = 0x8E;///timer 1 for UART2


sfr P1M0  = 0x91;
sfr P1M1  = 0x92;
sfr P0M0  = 0x93;
sfr P0M1  = 0x94;
sfr P2M0  = 0x95;
sfr P2M1  = 0x96;
sfr P3M0  = 0xB1;
sfr P3M1  = 0xB2;
sfr P4M0  = 0xB3;
sfr P4M1  = 0xB4;


// for PCA
sfr CCON   = 0xD8;
sfr CMOD   = 0xD9;
sfr CCAPM0 = 0xDA;
sfr CCAPM1 = 0xDB;
sfr CCAPM2 = 0xDC;
sfr CCAPM3 = 0xDD;
sfr CCAPM4 = 0xDE;
sfr CCAPM5 = 0xDF;
sfr CL     = 0xE9;
sfr CH     = 0xF9;
sfr CCAP0L = 0xEA;
sfr CCAP0H = 0xFA;
sfr CCAP1L = 0xEB;
sfr CCAP1H = 0xFB;
sfr CCAP2L = 0xEC;
sfr CCAP2H = 0xFC;
sfr CCAP3L = 0xED;
sfr CCAP3H = 0xFD;
sfr CCAP4L = 0xEE;
sfr CCAP4H = 0xFE;
sfr CCAP5L = 0xEF;
sfr CCAP5H = 0xFF;
sfr PCAPWM0= 0xF2;
sfr PCAPWM1= 0xF3;
sfr PCAPWM2= 0xF4;
sfr PCAPWM3= 0xF5;
sfr PCAPWM4= 0xF6;
sfr PCAPWM5= 0xF7;

/*  BIT Registers  */
/*  PSW  */
sbit CY    = 0xD7;
sbit AC    = 0xD6;
sbit F0    = 0xD5;
sbit RS1   = 0xD4;
sbit RS0   = 0xD3;
sbit OV    = 0xD2;
sbit P     = 0xD0;

/*  TCON  */
sbit TF1   = 0x8F;
sbit TR1   = 0x8E;
sbit TF0   = 0x8D;
sbit TR0   = 0x8C;
sbit IE1   = 0x8B;
sbit IT1   = 0x8A;
sbit IE0   = 0x89;
sbit IT0   = 0x88;

/*  IE  */
sbit EA    = 0xAF;
sbit ES    = 0xAC;
sbit ET1   = 0xAB;
sbit EX1   = 0xAA;
sbit ET0   = 0xA9;
sbit EX0   = 0xA8;

/*  IP  */
sbit PS    = 0xBC;
sbit PT1   = 0xBB;
sbit PX1   = 0xBA;
sbit PT0   = 0xB9;
sbit PX0   = 0xB8;

/*  P3  */
sbit RD    = 0xB7;
sbit WR    = 0xB6;
sbit T1    = 0xB5;
sbit T0    = 0xB4;
sbit INT1  = 0xB3;
sbit INT0  = 0xB2;
sbit TXD   = 0xB1;
sbit RXD   = 0xB0;

/*  SCON  */
sbit SM0   = 0x9F;
sbit SM1   = 0x9E;
sbit SM2   = 0x9D;
sbit REN   = 0x9C;
sbit TB8   = 0x9B;
sbit RB8   = 0x9A;
sbit TI    = 0x99;
sbit RI    = 0x98;

/*  8052 Extensions  */
/*  IE  */
sbit ET2   = 0xAD;

/*  IP  */
sbit PT2   = 0xBD;

/*  P1  */
sbit T2EX  = 0x91;
sbit T2    = 0x90;
             
/*  T2CON  */
sbit TF2   = 0xCF;
sbit EXF2  = 0xCE;
sbit RCLK  = 0xCD;
sbit TCLK  = 0xCC;
sbit EXEN2 = 0xCB;
sbit TR2   = 0xCA;
sbit C_T2  = 0xC9;
sbit CP_RL2= 0xC8;

sfr	CHPENR = 0xF6;
sfr	CHPCON = 0xBF;
sfr	SFRAL  = 0xC4;
sfr	SFRAH  = 0xC5;
sfr	SFRFD  = 0xC6;
sfr	SFRCN  = 0xC7;

sbit P0_0  = 0x80;
sbit P0_1  = 0x81;
sbit P0_2  = 0x82;
sbit P0_3  = 0x83;
sbit P0_4  = 0x84;
sbit P0_5  = 0x85;
sbit P0_6  = 0x86;
sbit P0_7  = 0x87;

sbit P1_0  = 0x90;
sbit P1_1  = 0x91;
sbit P1_2  = 0x92;
sbit P1_3  = 0x93;
sbit P1_4  = 0x94;
sbit P1_5  = 0x95;
sbit P1_6  = 0x96;
sbit P1_7  = 0x97;

sbit P2_0  = 0xa0;
sbit P2_1  = 0xa1;
sbit P2_2  = 0xa2;
sbit P2_3  = 0xa3;
sbit P2_4  = 0xa4;
sbit P2_5  = 0xa5;
sbit P2_6  = 0xa6;
sbit P2_7  = 0xa7;

sbit P3_0  = 0xb0;
sbit P3_1  = 0xb1;
sbit P3_2  = 0xb2;
sbit P3_3  = 0xb3;
sbit P3_4  = 0xb4;
sbit P3_5  = 0xb5;
sbit P3_6  = 0xb6;
sbit P3_7  = 0xb7;

//sbit P4_0  = P4^0;
//sbit P4_1  = P4^1;
//sbit P4_2  = P4^2;
//sbit P4_3  = P4^3;

sbit P4_0  = 0xE8;
sbit P4_1  = 0xE9;
sbit P4_2  = 0xEA;
sbit P4_3  = 0xEB;

//isp mode
sfr IFD    = 0xE2;
sfr IFADRH = 0xE3; 
sfr IFADRL = 0xE4;
sfr IFMT   = 0xE5;
sfr SCMD   = 0xE6;
sfr ISPCR  = 0xE7;



#endif	// INTERNAL_MCU

#endif  //__REGISTERS__
