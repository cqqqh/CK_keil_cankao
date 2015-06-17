/*----------------------------------------------------------------------------
 * Name:    Mcommand.c
 * Purpose: Time Set Commands for the Remote Measurement Recorder
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2010 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stdio.h>                       /* standard ANSI I/O .h-file         */
#include "STM32F10x.h"
#include "Measure.h"                     /* global project definition file    */


/*----------------------------------------------------------------------------
  Display measurements
 *----------------------------------------------------------------------------*/
void measure_display (struct mrec display)  {
  int i;                                 /* index count for AD1 - AD3         */

  printf ("\rTime: %2d:%02d:%02d.%03d GPIOD:%04X",
           display.time.hour,            /* display hour                      */
           display.time.min,             /* display minute                    */
           display.time.sec,             /* display second                    */
           display.time.msec,            /* display millisecond               */
           (int)display.gpiod);          /* display gpio port D value         */

  for (i = 0; i < 1; i++)  {             /* display AD0 through AD3           */
    printf (" A%d:%4.2fV", i+1, (float)(display.analog[i] * 3.3 / 0xFFF));
  }
}


/*----------------------------------------------------------------------------
  Set Current Time
 *----------------------------------------------------------------------------*/
void set_time (char * buffer)  {
  int hour, min, sec;                    /* temporary time values             */
  int args;                              /* number of arguments               */

  args = sscanf (buffer, "%d:%d:%d",     /* scan input line for               */
                 &hour,                  /* hour, minute and second           */
                 &min,
                 &sec);
  if (hour > 23  ||  min > 59  ||        /* check for valid inputs            */
      sec > 59   ||  args < 2  ||  args == EOF)  {
    printf (ERROR_STR, "INVALID TIME FORMAT");
  }
  else  {                                /* if inputs valid then              */
    NVIC_DisableIRQ(TIM3_IRQn);          /* disable interrupts while          */
    current.time.hour = hour;            /* setting the new time: hours       */
    current.time.min  = min;             /* setting the new time: minutes     */
    current.time.sec  = sec;             /* setting the new time: seconds     */
    current.time.msec = 0;               /* setting the new time: miliseconds */
    interval.min = 0;                    /* force new interval                */
    interval.sec = 0;
    interval.msec = 0;
    NVIC_EnableIRQ(TIM3_IRQn);           /* enable interrupts again           */
  }
}


/*----------------------------------------------------------------------------
   Set Interval Time
 *----------------------------------------------------------------------------*/
void set_interval (char * buffer) {
  struct interval itime;                 /* temporary interval record         */
  int min, sec, msec;
  int args;                              /* number of arguments               */

  args = sscanf (buffer, "%d:%d.%d",     /* scan input line for               */
                 &min,                   /* minute, second and milliseconds   */
                 &sec,
                 &msec);
                                         /* check valid inputs                */
  if (sec > 59  ||  msec > 999  ||  args < 2  || args == EOF)  {
    printf (ERROR_STR, "INVALID INTERVAL FORMAT");
  }
  else  {                                /* if inputs are valid then          */
    itime.min  = min;
    itime.sec  = sec;
    itime.msec = msec;
    if (itime.min != 0 || itime.sec != 0 || itime.msec != 0)  {
      if (itime.msec-- == 0)  {          /* correct interval time for         */
        itime.msec = 999;                /* interrupt routine                 */
        if (itime.sec-- == 0)  {
          itime.sec = 59;
          itime.min--;
        }
      }
    }
    NVIC_DisableIRQ(TIM3_IRQn);          /* disable interrupts while          */
    setinterval = itime;                 /* of the new setinterval time       */
    interval.min = 0;                    /* force new interval                */
    interval.sec = 0;
    interval.msec = 0;
    NVIC_EnableIRQ(TIM3_IRQn);           /* enable interrupts again           */
  }
}
