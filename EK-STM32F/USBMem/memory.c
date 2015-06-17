/*----------------------------------------------------------------------------
 * Name:    memory.c
 * Purpose: USB Memory Storage Demo
 * Note(s): 
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2009-2011 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "STM32F10x.h"                            /* STM32F10x definitions    */

#include "type.h"

#include "usb.h"
#include "usbcfg_STM32F10x.h"
#include "usbhw_STM32F10x.h"
#include "mscuser.h"

#include "memory.h"

extern U8 Memory[MSC_MemorySize];                 /* MSC Memory in RAM        */

/*----------------------------------------------------------------------------
  Switch on LEDs
 *----------------------------------------------------------------------------*/
void LED_On (unsigned int value) {

  GPIOC->BSRR = (value);                    /* Turn On  LED */
}


/*----------------------------------------------------------------------------
  Switch off LEDs
 *----------------------------------------------------------------------------*/
void LED_Off (unsigned int value) {

  GPIOC->BRR  = (value);                    /* Turn Off LED */
}


/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) {
  U32 n;

  for (n = 0; n < MSC_ImageSize; n++) {           /* Copy Initial Disk Image  */
    Memory[n] = DiskImage[n];                     /*   from Flash to RAM      */
  }

  /* configure LEDs */
  RCC->APB2ENR |=  RCC_APB2ENR_IOPCEN;            /* Enable GPIOC clock       */

  GPIOC->ODR   &= ~0x000000F0;                    /* switch off LEDs          */
  GPIOC->CRL   &= ~0xFFFF0000;                    /* PC.4..7  clear           */
  GPIOC->CRL   |=  0x33330000;                    /* PC.4..7  is output       */

  USB_Init();                                     /* USB Initialization       */
  USB_Connect(__TRUE);                            /* USB Connect              */

  while (1);                                      /* Loop forever             */
}
