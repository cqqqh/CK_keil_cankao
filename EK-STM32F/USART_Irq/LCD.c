/*----------------------------------------------------------------------------
 * Name:    LCD.c
 * Purpose: LCD functions
 * Version: V1.00
 * Note(s): TIM4 is used to refresh LCD
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2008 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------
 * History:
 *          V1.00 Initial Version
 *----------------------------------------------------------------------------*/

#include "STM32F10x.h"


/*--------------------------------------------------------------------- 
  15 segment LCD digit is:

               A
      --  -----------
    X \/  |\   |I  /|
         F| H  |  J |B
          |  \ | /  |
          --G-- --K--
          |   /| \  |
        E |  L |  N |C
          | /  |M  \|
          -----------  .DP   
              D

  PE   | 15| 14| 13| 12| 11| 10| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |    
  SEG  |S15|S14|S13|S12|S11|S10|S9 |S8 |S7 |S6 |S5 |S4 |S3 |S2 |S1 |S0 |    
  Pos          3       |       2       |       1       |       0       |
  COM1 |DP | A | I | X |DP | A | I | X |DP | A | I | X |DP | A | I | X |
  COM2 | B | J | H | F | B | J | H | F | B | J | H | F | B | J | H | F |
  COM3 | C | K | G | E | C | K | G | E | C | K | G | E | C | K | G | E |
  COM4 | D | N | M | L | D | N | M | L | D | N | M | L | D | N | M | L |


  Character coding:  unsinged short c;
                     c 0.. 3 -> COM1
                     c 4.. 7 -> COM2
                     c 8..11 -> COM3
                     c12..15 -> COM4

  Example:          'A' = E, F, A, B, C, G, K   -> 0x0F94
                    '0' = E, F, A, B, C, D      -> 0x8994   

  ---------------------------------------------------------------------*/ 

unsigned short lcdChar[128] = {
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, //   0..9    unused
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, //  10..19   unused
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, //  20..29   unused
  0x0000, 0x0000, 0x0000, 0x2002, 0x0012, 0x0000, 0xAE16, 0x0000, 0x0000, 0x0002, //  30..39   unused           
  0x0000, 0x0000, 0x7662, 0x2602, 0x2000, 0x0600, 0x0000, 0x1040, 0x99D4, 0x08C0, //  40..47   unused, '0', '1'           
  0x8784, 0x8C84, 0x0E90, 0x8E14, 0x8F14, 0x1044, 0x8F94, 0x8E94, 0x0000, 0x0000, //  '2'..'9', 58..59 unused           
  0x4040, 0x8600, 0x1020, 0x2494, 0x0000, 0x0F94, 0x8F10, 0x8114, 0x8F80, 0x8314, //  60..65   unused, 'A'..'E'           
  0x0314, 0x8D14, 0x0F90, 0x2002, 0x4880, 0x4350, 0x8110, 0x09F0, 0x49B0, 0x8994, //  'F'..'O'           
  0x0794, 0xC994, 0x4394, 0x8E14, 0x2006, 0x8990, 0x8990, 0x5990, 0x5060, 0x0160, //  'P'..'Y'          
  0x9044, 0x8114, 0x4020, 0x8884, 0x0000, 0x8000, 0x0010, 0x0F94, 0x8F10, 0x8700, //  'Z', 91..96 unused, 'a'..'c'           
  0x8F80, 0x8314, 0x0314, 0x8D14, 0x0F90, 0x2002, 0x4880, 0x4350, 0x8110, 0x09F0, //  'e'..'m'           
  0x49B0, 0x8F00, 0x0794, 0xC994, 0x0300, 0x8E14, 0x2006, 0x8990, 0x8990, 0x5900, //  'n'..'w'           
  0x5060, 0x0160, 0x9044, 0x0000, 0x2002, 0x0000, 0x0000, 0x0000};                //  'x'..'z', 123..127 unused           



unsigned short lcdBuffer[4] = {0, 0, 0, 0};             // LCD representation
unsigned int   lcdState     = 0;                        // write or clear
unsigned int   lcdWrState   = 0;                        // normal or inverted
unsigned int   lcdLine      = 0;                        // line 0..4


/*----------------------------------------------------------------------------
  clear LCD 
 *----------------------------------------------------------------------------*/
void lcd_Clr (void) {
  int lineNr;

  for (lineNr = 0; lineNr < 4; lineNr++) {
    lcdBuffer[lineNr] = 0;
  }

}

/*----------------------------------------------------------------------------
  write a character to LCD position (0..3). Position 0 is left, 3 is right 
 *----------------------------------------------------------------------------*/
void lcd_WrPos (char* c, int pos) {
  int            lineNr;
  unsigned short cLcd;                                  // character coding for LCD

  cLcd = lcdChar[*c & 0x7F];                            // we only handel character 0..127

  for (lineNr = 0; lineNr < 4; lineNr++) {
    lcdBuffer[lineNr] = (lcdBuffer[lineNr] & ~(0xF<<(4*pos))) | (((cLcd>>(lineNr*4))& 0xF)<<(4*pos));
  }

}

/*----------------------------------------------------------------------------
  write a string to LCD position (0..3). max chars written is 4 
 *----------------------------------------------------------------------------*/
void lcd_WrStr (char* s) {
  int i;

  for (i = 0; i < 4; i++) {
    lcd_WrPos (s+i, i);
  }

}


/*----------------------------------------------------------------------------
  initialize LCD -> configure GPIO, TIM4 
 *----------------------------------------------------------------------------*/
void lcd_Init (void) {

  /* initialize GPIO for COM1..4 */
  RCC->APB2ENR |=  RCC_APB2ENR_IOPCEN;            /* Enable GPIOC clock       */

  GPIOC->ODR   &= ~0x00000F00;                    /* switch off               */
  GPIOC->CRH   &= ~0x0000FFFF;                    /* PC.8..11  clear          */
  GPIOC->CRH   |=  0x00003333;                    /* PC.8..11  is output      */

  /* initialize GPIO for SEG0..15 */
  RCC->APB2ENR |=  RCC_APB2ENR_IOPEEN;            /* Enable GPIOE clock       */

  GPIOE->CRL    = 0x33333333;                     /* PE.0..7   is output      */
  GPIOE->CRH    = 0x33333333;                     /* PC.8..15  is output      */

  /* initialize TIM4 used for LCD */
  RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;             /* enable clock for TIM4    */

  TIM4->PSC   = ( 7200 - 1);                      /* set prescaler   = 10KHz  */
  TIM4->ARR   = (   20 - 1);                      /* set auto-reload =  2ms  */

  TIM4->DIER = TIM_DIER_UIE;                      /* Update Interrupt enable  */
  NVIC_EnableIRQ(TIM4_IRQn);                      /* TIM4   Interrupt enable  */

  TIM4->CR1  |= TIM_CR1_CEN;                      /* timer enable             */
}


/*----------------------------------------------------------------------------
  TIM4_IRQHandler
  ticks every 2ms
 *----------------------------------------------------------------------------*/
void TIM4_IRQHandler(void)
{
  if (TIM4->SR & (1<<0)) {                              // UIF set?
//--------------------------------
  GPIOC->CRH &= 0xFFFF0000;
  GPIOC->CRH |= 0x00004444;                             // COM1..4 input floating
  GPIOC->CRH &= ~(0xF<<(4*(3-lcdLine))); 
  GPIOC->CRH |=  (0x3<<(4*(3-lcdLine)));                // COMx    ouput push pull 50 MHz

  switch (lcdState){
    case 0:                                             // write state
      switch (lcdWrState) {
        case 0:                                         // write nomal data stat
          GPIOE->ODR  = lcdBuffer[lcdLine];             // write data
          GPIOC->BRR  = (1<<(8+(3-lcdLine)));           // COMx low

          lcdWrState = 1;                               // next state is write inverted data                       
        break;
        case 1:                                         // write inverted data state
          GPIOE->ODR  = ~lcdBuffer[lcdLine];            // write inverted data
          GPIOC->BSRR = (1<<(8+(3-lcdLine)));           // COMx high

          lcdWrState = 0;                               // next state is write normal data
          lcdLine = ((lcdLine+1) == 4) ? 0 : lcdLine+1; // handle next COM
        break;
      } // end switch (lcdWrState)

      lcdState = 1;                                     // next state is clearState
    break;

    case 1:                                             // clear state
      GPIOC->CRH &= 0xFFFF0000;
      GPIOC->CRH |= 0x00003333;                         // COM1..4 ouput push pull 50 MHz

      GPIOE->ODR  =  0x0000;                            // all Segments off
      GPIOC->BRR  = (0xF<<8);                           // all Lines off

      lcdState = 0;                                     // next state is writeState
    break;

  } // end switch (lcdState)
//--------------------------------
    TIM4->SR &= ~(1<<0);                                // clear UIF flag
  }

}
