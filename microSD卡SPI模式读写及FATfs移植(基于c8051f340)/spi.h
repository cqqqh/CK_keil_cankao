#ifndef _SPI_H_
#define _SPI_H_

#include "c8051f340.h"

sbit CS  = P2^1;

/*��ʱ����*/
void delay(unsigned int count);

/***************Ӳ������SPI���߿�����***************/

#define	FCLK_SLOW()		SPI0CKR = 0xef;delay(100) /* Set slow clock 100k */
#define	FCLK_FAST()		SPI0CKR = 0x01;delay(100) /* Set fast clock (depends on the CSD) */

/*SPI���ݷ��ͺ궨��*/
#define SPI_transmit_m(c)\
{\	 
	SPI0DAT = c;\
	while(!SPIF);\
	SPIF = 0;\
}

/*SPI���ݽ��պ궨��*/
#define SPI_receive_m(p)\
{\	 
	SPI0DAT = 0xff;\
	while(!SPIF);\
	SPIF = 0;\
	*(p) = SPI0DAT;\
}

/*SPI�������ݺ���*/
void SPI_transmit(unsigned char c);

/*SPI�������ݺ���*/
unsigned char SPI_receive();


#ifdef _SPI_IO_
/******************IO��ģ��SPI����******************/

/*SPI���߶���, Ӳ�����*/
sbit CLK = P0^0;
sbit DI  = P0^1;
sbit DO  = P0^2;

/*���ģ��SPI���ͺ���*/
void SPI_transmit(unsigned char c); 

/*���ģ��SPI���պ���*/
unsigned char SPI_receive(void);	

#endif /*end _SPI_IO_*/

#endif /*end _SPI_H_*/

