//-----------------------------------------------------------------------------
// Multiple.H
//-----------------------------------------------------------------------------
//
// DEVICE:    Multiple/1/2/3/4/5/6/7
// AUTHOR:    Christopher Menke
// DATE:      06/08/2005
// FUNCTION:  Register/bit definitions for the C8051F34x product family
//           
//
//-----------------------------------------------------------------------------

#ifndef Multiple_H
#define Multiple_H

/*  BYTE Registers  */

sfr  ACC          =  0xE0;    /*  Accumulator  */
sfr  ADC0CF       =  0xBC;    /*  ADC0 Configuration  */
sfr  ADC0CN       =  0xE8;    /*  ADC0 Control  */
sfr  ADC0GTH      =  0xC4;    /*  ADC0 Greater-Than Compare High  */
sfr  ADC0GTL      =  0xC3;    /*  ADC0 Greater-Than Compare Low  */
sfr  ADC0H        =  0xBE;    /*  ADC0 High  */
sfr  ADC0L        =  0xBD;    /*  ADC0 Low  */
sfr  ADC0LTH      =  0xC6;    /*  ADC0 Less-Than Compare Word High  */
sfr  ADC0LTL      =  0xC5;    /*  ADC0 Less-Than Compare Word Low  */
sfr  AMX0N        =  0xBA;    /*  AMUX0 Negative Channel Select  */
sfr  AMX0P        =  0xBB;    /*  AMUX0 Positive Channel Select  */
sfr  B            =  0xF0;    /*  B Register  */
sfr  CKCON        =  0x8E;    /*  Clock Control  */
sfr  CLKMUL       =  0xB9;    /*  Clock Multiplier  */
sfr  CLKSEL       =  0xA9;    /*  Clock Select  */
sfr  CPT0CN       =  0x9B;    /*  Comparator0 Control  */
sfr  CPT0MD       =  0x9D;    /*  Comparator0 Mode Selection  */
sfr  CPT0MX       =  0x9F;    /*  Comparator0 MUX Selection  */
sfr  CPT1CN       =  0x9A;    /*  Comparator1 Control  */
sfr  CPT1MD       =  0x9C;    /*  Comparator1 Mode Selection  */
sfr  CPT1MX       =  0x9E;    /*  Comparator1 MUX Selection  */
sfr  DPH          =  0x83;    /*  Data Pointer High  */
sfr  DPL          =  0x82;    /*  Data Pointer Low  */
sfr  EIE1         =  0xE6;    /*  Extended Interrupt Enable 1  */
sfr  EIE2         =  0xE7;    /*  Extended Interrupt Enable 2  */
sfr  EIP1         =  0xF6;    /*  Extended Interrupt Priority 1  */
sfr  EIP2         =  0xF7;    /*  Extended Interrupt Priority 2  */
sfr  EMI0CF       =  0x85;    /*  External Memory Interface Configuration  */
sfr  EMI0CN       =  0xAA;    /*  External Memory Interface Control  */
sfr  EMI0TC       =  0x84;    /*  External Memory Interface Timing  */
sfr  FLKEY        =  0xB7;    /*  Flash Lock and Key  */
sfr  FLSCL        =  0xB6;    /*  Flash Scale  */
sfr  IE           =  0xA8;    /*  Interrupt Enable  */
sfr  IP           =  0xB8;    /*  Interrupt Priority  */
sfr  IT01CF       =  0xE4;    /*  INT0/INT1 Configuration  */
sfr  OSCICL       =  0xB3;    /*  Internal Oscillator Calibration  */
sfr  OSCICN       =  0xB2;    /*  Internal Oscillator Control  */
sfr  OSCLCN       =  0x86;    /*  Internal Low-Frequency Oscillator Control  */
sfr  OSCXCN       =  0xB1;    /*  External Oscillator Control  */
sfr  P0           =  0x80;    /*  Port 0 Latch  */
sfr  P0MDIN       =  0xF1;    /*  Port 0 Input Mode Configuration  */
sfr  P0MDOUT      =  0xA4;    /*  Port 0 Output Mode Configuration  */
sfr  P0SKIP       =  0xD4;    /*  Port 0 Skip  */
sfr  P1           =  0x90;    /*  Port 1 Latch  */
sfr  P1MDIN       =  0xF2;    /*  Port 1 Input Mode Configuration  */
sfr  P1MDOUT      =  0xA5;    /*  Port 1 Output Mode Configuration  */
sfr  P1SKIP       =  0xD5;    /*  Port 1 Skip  */
sfr  P2           =  0xA0;    /*  Port 2 Latch  */
sfr  P2MDIN       =  0xF3;    /*  Port 2 Input Mode Configuration  */
sfr  P2MDOUT      =  0xA6;    /*  Port 2 Output Mode Configuration  */
sfr  P2SKIP       =  0xD6;    /*  Port 2 Skip  */
sfr  P3           =  0xB0;    /*  Port 3 Latch  */
sfr  P3MDIN       =  0xF4;    /*  Port 3 Input Mode Configuration  */
sfr  P3MDOUT      =  0xA7;    /*  Port 3 Output Mode Configuration  */
sfr  P3SKIP       =  0xDF;    /*  Port 3Skip  */
sfr  P4           =  0xC7;    /*  Port 4 Latch  */
sfr  P4MDIN       =  0xF5;    /*  Port 4 Input Mode Configuration  */
sfr  P4MDOUT      =  0xAE;    /*  Port 4 Output Mode Configuration  */
sfr  PCA0CN       =  0xD8;    /*  PCA Control  */
sfr  PCA0CPH0     =  0xFC;    /*  PCA Capture 0 High  */
sfr  PCA0CPH1     =  0xEA;    /*  PCA Capture 1 High  */
sfr  PCA0CPH2     =  0xEC;    /*  PCA Capture 2 High  */
sfr  PCA0CPH3     =  0xEE;    /*  PCA Capture 3High  */
sfr  PCA0CPH4     =  0xFE;    /*  PCA Capture 4 High  */
sfr  PCA0CPL0     =  0xFB;    /*  PCA Capture 0 Low  */
sfr  PCA0CPL1     =  0xE9;    /*  PCA Capture 1 Low  */
sfr  PCA0CPL2     =  0xEB;    /*  PCA Capture 2 Low  */
sfr  PCA0CPL3     =  0xED;    /*  PCA Capture 3 Low  */
sfr  PCA0CPL4     =  0xFD;    /*  PCA Capture 4 Low  */
sfr  PCA0CPM0     =  0xDA;    /*  PCA Module 0 Mode Register  */
sfr  PCA0CPM1     =  0xDB;    /*  PCA Module 1 Mode Register  */
sfr  PCA0CPM2     =  0xDC;    /*  PCA Module 2 Mode Register  */
sfr  PCA0CPM3     =  0xDD;    /*  PCA Module 3 Mode Register  */
sfr  PCA0CPM4     =  0xDE;    /*  PCA Module 4 Mode Register  */
sfr  PCA0H        =  0xFA;    /*  PCA Counter High  */
sfr  PCA0L        =  0xF9;    /*  PCA Counter Low  */
sfr  PCA0MD       =  0xD9;    /*  PCA Mode  */
sfr  PCON         =  0x87;    /*  Power Control  */
sfr  PFE0CN       =  0xAF;    /*  Prefetch Engine Control  */
sfr  PSCTL        =  0x8F;    /*  Program Store R/W Control  */
sfr  PSW          =  0xD0;    /*  Program Status Word  */
sfr  REF0CN       =  0xD1;    /*  Voltage Reference Control  */
sfr  REG0CN       =  0xC9;    /*  Voltage Regulator Control  */
sfr  RSTSRC       =  0xEF;    /*  Reset Source Configuration/Status  */
sfr  SBCON1       =  0xAC;    /*  UART1 Baud Rate Generator Control  */
sfr  SBRLH1       =  0xB5;    /*  UART1 Baud Rate Generator High  */
sfr  SBRLL1       =  0xB4;    /*  UART1 Baud Rate Generator Low  */
sfr  SBUF0        =  0x99;    /*  UART0 Data Buffer  */
sfr  SBUF1        =  0xD3;    /*  UART1 Data Buffer  */
sfr  SCON0        =  0x98;    /*  UART0 Control  */
sfr  SCON1        =  0xD2;    /*  UART1 Control  */
sfr  SMB0CF       =  0xC1;    /*  SMBus Configuration  */
sfr  SMB0CN       =  0xC0;    /*  SMBus Control  */
sfr  SMB0DAT      =  0xC2;    /*  SMBus Data  */
sfr  SMOD1        =  0xE5;    /*  UART1 Mode  */
sfr  SP           =  0x81;    /*  Stack Pointer  */
sfr  SPI0CFG      =  0xA1;    /*  SPI Configuration  */
sfr  SPI0CKR      =  0xA2;    /*  SPI Clock Rate Control  */
sfr  SPI0CN       =  0xF8;    /*  SPI Control  */
sfr  SPI0DAT      =  0xA3;    /*  SPI Data  */
sfr  TCON         =  0x88;    /*  Timer/Counter Control  */
sfr  TH0          =  0x8C;    /*  Timer/Counter 0 High  */
sfr  TH1          =  0x8D;    /*  Timer/Counter 1 High  */
sfr  TL0          =  0x8A;    /*  Timer/Counter 0 Low  */
sfr  TL1          =  0x8B;    /*  Timer/Counter 1 Low  */
sfr  TMOD         =  0x89;    /*  Timer/Counter Mode  */
sfr  TMR2CN       =  0xC8;    /*  Timer/Counter 2 Control  */
sfr  TMR2H        =  0xCD;    /*  Timer/Counter 2 High  */
sfr  TMR2L        =  0xCC;    /*  Timer/Counter 2 Low  */
sfr  TMR2RLH      =  0xCB;    /*  Timer/Counter 2 Reload High  */
sfr  TMR2RLL      =  0xCA;    /*  Timer/Counter 2 Reload Low  */
sfr  TMR3CN       =  0x91;    /*  Timer/Counter 3Control  */
sfr  TMR3H        =  0x95;    /*  Timer/Counter 3 High  */
sfr  TMR3L        =  0x94;    /*  Timer/Counter 3Low  */
sfr  TMR3RLH      =  0x93;    /*  Timer/Counter 3 Reload High  */
sfr  TMR3RLL      =  0x92;    /*  Timer/Counter 3 Reload Low  */
sfr  USB0ADR      =  0x96;    /*  USB0 Indirect Address Register  */
sfr  USB0DAT      =  0x97;    /*  USB0 Data Register  */
sfr  USB0XCN      =  0xD7;    /*  USB0 Transceiver Control  */
sfr  VDM0CN       =  0xFF;    /*  VDD Monitor Control  */
sfr  XBR0         =  0xE1;    /*  Port I/O Crossbar Control 0  */
sfr  XBR1         =  0xE2;    /*  Port I/O Crossbar Control 1  */
sfr  XBR2         =  0xE3;    /*  Port I/O Crossbar Control 2  */

/*  BIT Registers  */




/*  SPI0CN  0xF8  */        
  sbit  SPIEN   =  SPI0CN  ^  0  ;
  sbit  TXBMT   =  SPI0CN  ^  1  ;
  sbit  NSSMD0  =  SPI0CN  ^  2  ;
  sbit  NSSMD1  =  SPI0CN  ^  3  ;
  sbit  RXOVRN  =  SPI0CN  ^  4  ;
  sbit  MODF    =  SPI0CN  ^  5  ;
  sbit  WCOL    =  SPI0CN  ^  6  ;
  sbit  SPIF    =  SPI0CN  ^  7  ;

/*  B  0xF0  */        
  sbit  B0      =  B  ^  0  ;
  sbit  B1      =  B  ^  1  ;
  sbit  B2      =  B  ^  2  ;
  sbit  B3      =  B  ^  3  ;
  sbit  B4      =  B  ^  4  ;
  sbit  B5      =  B  ^  5  ;
  sbit  B6      =  B  ^  6  ;
  sbit  B7      =  B  ^  7  ;
              
/*  ADC0CN  0xE8  */        
  sbit  AD0CM0  =  ADC0CN  ^  0  ;
  sbit  AD0CM1  =  ADC0CN  ^  1  ;
  sbit  AD0CM2  =  ADC0CN  ^  2  ;
  sbit  AD0WINT =  ADC0CN  ^  3  ;
  sbit  AD0BUSY =  ADC0CN  ^  4  ;
  sbit  AD0INT  =  ADC0CN  ^  5  ;
  sbit  AD0TM   =  ADC0CN  ^  6  ;
  sbit  AD0EN   =  ADC0CN  ^  7  ;
              
/*  ACC  0xE0  */        
  sbit  ACC0    =  ACC  ^  0  ;
  sbit  ACC1    =  ACC  ^  1  ;
  sbit  ACC2    =  ACC  ^  2  ;
  sbit  ACC3    =  ACC  ^  3  ;
  sbit  ACC4    =  ACC  ^  4  ;
  sbit  ACC5    =  ACC  ^  5  ;
  sbit  ACC6    =  ACC  ^  6  ;
  sbit  ACC7    =  ACC  ^  7  ;
              
/*  PCA0CN  0xD8  */        
  sbit  CCF0    =  PCA0CN  ^  0  ;
  sbit  CCF1    =  PCA0CN  ^  1  ;
  sbit  CCF2    =  PCA0CN  ^  2  ;
  sbit  CCF3    =  PCA0CN  ^  3  ;
  sbit  CCF4    =  PCA0CN  ^  4  ;
              
  sbit  CR      =  PCA0CN  ^  6  ;
  sbit  CF      =  PCA0CN  ^  7  ;
              
/*  PSW  0xD0  */        
  sbit  PARITY  =  PSW  ^  0  ;
  sbit  F1      =  PSW  ^  1  ;
  sbit  OV      =  PSW  ^  2  ;
  sbit  RS0     =  PSW  ^  3  ;
  sbit  RS1     =  PSW  ^  4  ;
  sbit  F0      =  PSW  ^  5  ;
  sbit  AC      =  PSW  ^  6  ;
  sbit  CY      =  PSW  ^  7  ;
              
/*  TMR2CN  0xC8  */        
  sbit  T2XCLK  =  TMR2CN  ^  0  ;
  sbit  T2CSS   =  TMR2CN  ^  1  ;
  sbit  TR2     =  TMR2CN  ^  2  ;
  sbit  T2SPLIT =  TMR2CN  ^  3  ;
  sbit  T2CE    =  TMR2CN  ^  4  ;
  sbit  TF2LEN  =  TMR2CN  ^  5  ;
  sbit  TF2L    =  TMR2CN  ^  6  ;
  sbit  TF2H    =  TMR2CN  ^  7  ;
              
/*  SMB0CN  0xC0  */        
  sbit  SI      =  SMB0CN  ^  0  ;
  sbit  ACK     =  SMB0CN  ^  1  ;
  sbit  ARBLOST =  SMB0CN  ^  2  ;
  sbit  ACKRQ   =  SMB0CN  ^  3  ;
  sbit  STO     =  SMB0CN  ^  4  ;
  sbit  STA     =  SMB0CN  ^  5  ;
  sbit  TXMODE  =  SMB0CN  ^  6  ;
  sbit  MASTER  =  SMB0CN  ^  7  ;
              
/*  IP  0xB8  */        
  sbit  PX0     =  IP  ^  0  ;
  sbit  PT0     =  IP  ^  1  ;
  sbit  PX1     =  IP  ^  2  ;
  sbit  PT1     =  IP  ^  3  ;
  sbit  PS0     =  IP  ^  4  ;
  sbit  PT2     =  IP  ^  5  ;
  sbit  PSPI0   =  IP  ^  6  ;
              
              
/*  P3  0xB0  */        
  sbit  P30     =  P3  ^  0  ;
  sbit  P31     =  P3  ^  1  ;
  sbit  P32     =  P3  ^  2  ;
  sbit  P33     =  P3  ^  3  ;
  sbit  P34     =  P3  ^  4  ;
  sbit  P35     =  P3  ^  5  ;
  sbit  P36     =  P3  ^  6  ;
  sbit  P37     =  P3  ^  7  ;
              
/*  IE  0xA8  */        
  sbit  EX0     =  IE  ^  0  ;
  sbit  ET0     =  IE  ^  1  ;
  sbit  EX1     =  IE  ^  2  ;
  sbit  ET1     =  IE  ^  3  ;
  sbit  ES0     =  IE  ^  4  ;
  sbit  ET2     =  IE  ^  5  ;
  sbit  ESPI0   =  IE  ^  6  ;
  sbit  EA      =  IE  ^  7  ;
              
/*  P2  0xA0  */        
  sbit  P20     =  P2  ^  0  ;
  sbit  P21     =  P2  ^  1  ;
  sbit  P22     =  P2  ^  2  ;
  sbit  P23     =  P2  ^  3  ;
  sbit  P24     =  P2  ^  4  ;
  sbit  P25     =  P2  ^  5  ;
  sbit  P26     =  P2  ^  6  ;
  sbit  P27     =  P2  ^  7  ;
              
/*  SCON0  0x98  */        
  sbit  RI0     =  SCON0  ^  0  ;
  sbit  TI0     =  SCON0  ^  1  ;
  sbit  RB80    =  SCON0  ^  2  ;
  sbit  TB80    =  SCON0  ^  3  ;
  sbit  REN0    =  SCON0  ^  4  ;
  sbit  MCE0    =  SCON0  ^  5  ;

  sbit  S0MODE  =  SCON0  ^  7  ;

/*  P1  0x90  */        
  sbit  P10     =  P1  ^  0  ;
  sbit  P11     =  P1  ^  1  ;
  sbit  P12     =  P1  ^  2  ;
  sbit  P13     =  P1  ^  3  ;
  sbit  P14     =  P1  ^  4  ;
  sbit  P15     =  P1  ^  5  ;
  sbit  P16     =  P1  ^  6  ;
  sbit  P17     =  P1  ^  7  ;

/*  TCON  0x88  */        
  sbit  IT0     =  TCON  ^  0  ;
  sbit  IE0     =  TCON  ^  1  ;
  sbit  IT1     =  TCON  ^  2  ;
  sbit  IE1     =  TCON  ^  3  ;
  sbit  TR0     =  TCON  ^  4  ;
  sbit  TF0     =  TCON  ^  5  ;
  sbit  TR1     =  TCON  ^  6  ;
  sbit  TF1     =  TCON  ^  7  ;

/*  P0  0x80  */        
  sbit  P00     =  P0  ^  0  ;
  sbit  P01     =  P0  ^  1  ;
  sbit  P02     =  P0  ^  2  ;
  sbit  P03     =  P0  ^  3  ;
  sbit  P04     =  P0  ^  4  ;
  sbit  P05     =  P0  ^  5  ;
  sbit  P06     =  P0  ^  6  ;
  sbit  P07     =  P0  ^  7  ;

#define SYSCLK 48000000
#endif
