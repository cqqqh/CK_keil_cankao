#ifndef _MCU_INIT_H_
#define _MCU_INIT_H_

#define SYSCLK 		48000000
#define BAUDRATE	115200 /*Baud rate of UART in bps*/

void Init_Devices(void);
void Port_IO_Init(void);
void Oscillator_Init(void);
void UART0_Init(void);
void SPI_Init (void);

#endif