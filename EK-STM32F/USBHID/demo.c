/*----------------------------------------------------------------------------
 * Name:    demo.c
 * Purpose: USB HID Demo
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

#include "demo.h"


U8 InReport;                                      /* HID Input Report         */
                                                  /*   Bit0..6: Buttons       */
                                                  /*   Bit   7: Reserved      */

U8 OutReport;                                     /* HID Out Report           */
                                                  /*   Bit0..3: LEDs          */


/*----------------------------------------------------------------------------
  Get HID Input Report -> InReport
 *----------------------------------------------------------------------------*/
void GetInReport (void) {

  InReport = 0x00;
  if ((GPIOD->IDR & KEY2   ) == 0) InReport |= 0x01; // Key2   pressed means 0
  if ((GPIOD->IDR & KEY3   ) == 0) InReport |= 0x02; // Key3   pressed means 0
  if ((GPIOD->IDR & KEY4_SL) == 0) InReport |= 0x04; // Select pressed means 0
  if ((GPIOD->IDR & KEY4_RI) == 0) InReport |= 0x08; // Right  pressed means 0
  if ((GPIOD->IDR & KEY4_LE) == 0) InReport |= 0x10; // Left   pressed means 0
  if ((GPIOD->IDR & KEY4_UP) == 0) InReport |= 0x20; // Up     pressed means 0
  if ((GPIOD->IDR & KEY4_DN) == 0) InReport |= 0x40; // Down   pressed means 0
}


/*----------------------------------------------------------------------------
  Set HID Output Report <- OutReport
 *----------------------------------------------------------------------------*/
void SetOutReport (void) {

  GPIOC->BRR   = (0xF<<4);                        /* LEDs off                 */
  GPIOC->BSRR  = ((OutReport&0xF)<<4);            /* LEDs on                  */
}


/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {

  /* configure LEDs */
  RCC->APB2ENR |=  RCC_APB2ENR_IOPCEN;            /* Enable GPIOC clock       */

  GPIOC->ODR   &= ~0x000000F0;                    /* switch off LEDs          */
  GPIOC->CRL   &= ~0xFFFF0000;                    /* PC.4..7  clear           */
  GPIOC->CRL   |=  0x33330000;                    /* PC.4..7  is output       */

  /* configure Push Buttons */
  RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;             /* Enable GPIOD clock       */

  GPIOD->CRL   &= ~0x000FF000;                    /* PD.3..4 clear            */
  GPIOD->CRL   |=  0x00044000;                    /* PD.3..4 Floating Input   */

  GPIOD->CRH   &= ~0xFFFFF000;                    /* PD.11..15 clear          */
  GPIOD->CRH   |=  0x44444000;                    /* PD.11..15 Floating Input */

  USB_Init();                                     /* USB Initialization       */
  USB_Connect(__TRUE);                            /* USB Connect              */

  while (1);                                      /* Loop forever             */
}
