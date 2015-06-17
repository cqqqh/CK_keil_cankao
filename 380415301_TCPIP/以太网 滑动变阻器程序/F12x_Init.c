//-----------------------------------------------------------------------------
// F12x_Init.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
// 
// Contains Initialization Routines for the F120.
//
// FID:            
// Target:         C8051F12x
// Tool chain:     Keil C51 7.20 / Keil EVAL C51
//                 Silicon Laboratories IDE version 2.72 
// Command Line:   See Readme.txt
// Project Name:   CP220x_Ethernet_Routines
//
// 
//
// Release 1.0
//    -Initial Release (FB)
//    -30 MAY 2006
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "global.h"

#if(MCU == F020)

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F12x
//-----------------------------------------------------------------------------

sfr16 DP       = 0x82;                 // data pointer
sfr16 ADC0     = 0xbe;                 // ADC0 data
sfr16 ADC0GT   = 0xc4;                 // ADC0 greater than window
sfr16 ADC0LT   = 0xc6;                 // ADC0 less than window
sfr16 RCAP2    = 0xca;                 // Timer2 capture/reload
sfr16 RCAP3    = 0xca;                 // Timer3 capture/reload
sfr16 RCAP4    = 0xca;                 // Timer4 capture/reload
sfr16 TMR2     = 0xcc;                 // Timer2
sfr16 TMR3     = 0xcc;                 // Timer3
sfr16 TMR4     = 0xcc;                 // Timer4
sfr16 DAC0     = 0xd2;                 // DAC0 data
sfr16 DAC1     = 0xd2;                 // DAC1 data
sfr16 PCA0CP5  = 0xe1;                 // PCA0 Module 5 capture
sfr16 PCA0CP2  = 0xe9;                 // PCA0 Module 2 capture
sfr16 PCA0CP3  = 0xeb;                 // PCA0 Module 3 capture
sfr16 PCA0CP4  = 0xed;                 // PCA0 Module 4 capture
sfr16 PCA0     = 0xf9;                 // PCA0 counter
sfr16 PCA0CP0  = 0xfb;                 // PCA0 Module 0 capture
sfr16 PCA0CP1  = 0xfd;                 // PCA0 Module 1 capture

//-----------------------------------------------------------------------------
// Local Constants
//-----------------------------------------------------------------------------
#define SYSCLK  22118400              // System Clock in Hz


//-----------------------------------------------------------------------------
// Local Global Variables 
//-----------------------------------------------------------------------------

static unsigned int timeout;

//-----------------------------------------------------------------------------
// Exported Function Prototypes
//-----------------------------------------------------------------------------
void Reset_Init(void);

void wait_ms(int ms);
void clear_ms_timer_flag(void);
char get_ms_timer_flag(void);
void start_ms_timer();

void CP220x_RST_Low(void);
void CP220x_RST_High(void);
unsigned char AB4_RST_State(void);


//-----------------------------------------------------------------------------
// Local Function Prototypes
//-----------------------------------------------------------------------------

void PORT_Init (void);
void SYSCLK_Init (void);
void EMIF_Init (void);


//-----------------------------------------------------------------------------
// Exported Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Reset_Init
//-----------------------------------------------------------------------------
void Reset_Init(void)
{
   // Enable the VDD Monitor as a reset source
   // Do not use read-modify write instruction on this register
  // RSTSRC = 0x02;

   // Disable Watchdog Timer
   WDTCN = 0xde;
   WDTCN = 0xad;

   // Initialize the MCU
   PORT_Init();
   SYSCLK_Init();
   EMIF_Init();

   EX0 = 1;                            // Enable External Interrupt 0   

 
}

//-----------------------------------------------------------------------------
// External Interrupt 0
//-----------------------------------------------------------------------------
//
// This interrupt service routine is routed to the ethernet interrupt
//
void INT0_ISR(void) interrupt 0
{
   Ethernet_ISR();
}


//-----------------------------------------------------------------------------
// wait_ms
//-----------------------------------------------------------------------------
//
// This routine inserts a delay of <ms> milliseconds.
//
void wait_ms(int ms)
{
   
   T2CON = 0x00;                      // Stop Timer2; Clear TF2;
   // = 0x00;                      // use SYSCLK/12 as timebase

   RCAP2 = -(SYSCLK/1000/12);          // Timer 2 overflows at 1 kHz
   TMR2 = RCAP2;

   ET2 = 0;                            // Disable Timer 2 interrupts

   TR2 = 1;                            // Start Timer 2

   while(ms){
      TF2 = 0;
      while(!TF2);                     // wait until timer overflows
      ms--;                            // decrement ms
   }

   TR2 = 0;                            // Stop Timer 2

}

//-----------------------------------------------------------------------------
// reset_timout
//-----------------------------------------------------------------------------
//
// This routine resets the global timeout.
//
void reset_timeout(unsigned int ms)
{
   timeout = ms;
   start_ms_timer();
      
}


//-----------------------------------------------------------------------------
// timeout_expired
//-----------------------------------------------------------------------------
//
// This routine manages the timeout and returns TRUE if timeout expired.
//
unsigned char timeout_expired(void)
{
   if(get_ms_timer_flag()){
      clear_ms_timer_flag();
      if(timeout > 0){
         timeout--;
      }
   }
   
   return (timeout == 0);
     
   
}
//-----------------------------------------------------------------------------
// start_ms_timer
//-----------------------------------------------------------------------------
//
// This routine starts a timer with a 1kHz overflow rate (1ms period).
//
void start_ms_timer()
{

   T2CON = 0x00;                      // Stop Timer2; Clear TF2;
  // TMR2CF = 0x00;                      // use SYSCLK/12 as timebase

   RCAP2 = -(SYSCLK/1000/12);          // Timer 2 overflows at 1 kHz
   TMR2 = RCAP2;

   ET2 = 0;                            // Disable Timer 2 interrupts

   TR2 = 1;                            // Start Timer 2

}

//-----------------------------------------------------------------------------
// get_ms_timer_flag()
//-----------------------------------------------------------------------------
//
// This routine returns the state of the ms_timer overflow flag.
//
char get_ms_timer_flag(void)
{

   return TF2;

}

//-----------------------------------------------------------------------------
// clear_ms_timer_flag()
//-----------------------------------------------------------------------------
//
// This routine returns the state of the ms_timer overflow flag.
//
void clear_ms_timer_flag(void)
{

   TF2 = 0;

}


//-----------------------------------------------------------------------------
// CP220x_RST_Low
//-----------------------------------------------------------------------------
//
// Drives the CP220x's Reset Pin Low.
//
void CP220x_RST_Low(void)
{
   P4 &= ~0x20;                        // Set P4.5 Low
}

//-----------------------------------------------------------------------------
// CP220x_RST_High
//-----------------------------------------------------------------------------
//
// Drives the CP220x's Reset Pin High.
//
void CP220x_RST_High(void)
{
   P4 |= 0x20;                        // Set P4.5 High
}

//-----------------------------------------------------------------------------
// AB4_RST_State
//-----------------------------------------------------------------------------
//
// Returns the state of the AB4's reset pin.
//
unsigned char AB4_RST_State(void)
{
   char rst_pin_state;
   rst_pin_state = P4 & 0x20;          // Get P4.5 State
   return rst_pin_state;
}

//-----------------------------------------------------------------------------
// Local Initialization Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Configure UART1, Interrupts, Crossbar and GPIO ports
//
void PORT_Init (void)
{

   P1MDOUT |= 0x40;                 // Set P1.6(TB_LED) to push-pull
   P2MDOUT |= 0x08;                 // Set P2.2(AB4_LED1)
   P74OUT = 0xFF;
   P4 = 0xC0;                       // /WR, /RD, are high, RESET is low
   P5 = 0xFF;
   P6 = 0xFF;                       // P5, P6 contain the address lines
   P7 = 0xFF;                       // P7 contains the data lines

   TCON &= ~0x01;                   // Make /INT0 level triggered

   // Enable UART0, CP0, and /INT0. This puts /INT0 on P0.3.
   XBR0 = 0x84;
   XBR1 = 0x04;
   XBR2 = 0x40;
 
 
   TCON &= ~0x01;                   // Make /INT0 level triggered


}

//-----------------------------------------------------------------------------
// SYSCLK_Init
//-----------------------------------------------------------------------------
//
// This routine initializes the system clock.
//
void SYSCLK_Init (void)
{
    int i;                              // delay counter

   OSCXCN = 0x67;                      // start external oscillator with
                                       // 22.1184MHz crystal

   for (i=0; i < 256; i++) ;           // wait for oscillator to start

   while (!(OSCXCN & 0x80)) ;          // Wait for crystal osc. to settle

   OSCICN = 0x88;                      // select external oscillator as SYSCLK
                                       // source and enable missing clock
                                       // detector
}

//-----------------------------------------------------------------------------
// EMIF_Init
//-----------------------------------------------------------------------------
//
// Configure the External Memory Interface for both on and off-chip access.
//
void EMIF_Init (void)
{

   EMI0CF = 0xFB;             // Split-mode, non-multiplexed on P0 - P3

   EMI0TC = 0xef;      // This constant may be modified
                              // according to SYSCLK to meet the
                              // timing requirements for the CP2200

   EMI0CN = 0x20;     // Page of XRAM accessed by EMIF

}



#endif // MCU == F020

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------