#ifndef _SPI_H_
#define _SPI_H_

#include "c8051f340.h"

sbit CS  = P2^1;

/*延时函数*/
void delay(unsigned int count);

/***************硬件集成SPI总线控制器***************/

#define	FCLK_SLOW()		SPI0CKR = 0xef;delay(100) /* Set slow clock 100k */
#define	FCLK_FAST()		SPI0CKR = 0x01;delay(100) /* Set fast clock (depends on the CSD) */

/*SPI数据发送宏定义*/
#define SPI_transmit_m(c)\
{\	 
	SPI0DAT = c;\
	while(!SPIF);\
	SPIF = 0;\
}

/*SPI数据接收宏定义*/
#define SPI_receive_m(p)\
{\	 
	SPI0DAT = 0xff;\
	while(!SPIF);\
	SPIF = 0;\
	*(p) = SPI0DAT;\
}

/*SPI发送数据函数*/
void SPI_transmit(unsigned char c);

/*SPI接收数据函数*/
unsigned char SPI_receive();


#ifdef _SPI_IO_
/******************IO口模拟SPI总线******************/

/*SPI总线定义, 硬件相关*/
sbit CLK = P0^0;
sbit DI  = P0^1;
sbit DO  = P0^2;

/*软件模拟SPI发送函数*/
void SPI_transmit(unsigned char c); 

/*软件模拟SPI接收函数*/
unsigned char SPI_receive(void);	

#endif /*end _SPI_IO_*/

#endif /*end _SPI_H_*/

