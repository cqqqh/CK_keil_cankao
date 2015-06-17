/*----------------------------------------------------------------------------
 * Name:    LCD.h
 * Purpose: LCD function definitions
 * Note(s):
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

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __LCD_H
#define __LCD_H

extern void lcd_Init (void);
extern void lcd_Clr  (void);
extern void lcd_WrPos(char*, int);
extern void lcd_WrStr(char *);

#endif // __LCD_H
