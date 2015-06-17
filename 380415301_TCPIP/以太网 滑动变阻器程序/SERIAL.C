//-----------------------------------------------------------------------------
// Copyright (c) 2002 Jim Brady
// Do not use commercially without author's permission
// Last revised August 2002
// Net SERIAL.C
//
// This module handles RS-232 messages and associated tasks
//-----------------------------------------------------------------------------
#include "C8051f.h"
#include "intrins.h"
#include "net.h"
#include "serial.h"

bit CommRecDataOverflowFlag,FlagRecComm,SendItComm;

unsigned char CommSendBufferHead, CommSendBufferTail;
unsigned char CommRecBufferHead, CommRecBufferTail;

unsigned char xdata CommSendBuffer[DB_SENDMAXSIZE] _at_ 0 ; //串行口缓冲区定位在内部4K XRAM中
unsigned char xdata CommRecBuffer[DB_RECMAXSIZE] _at_ DB_SENDMAXSIZE; 
 
void ClearCommRecBuffer(void)
{
	unsigned char i;
	CommRecBufferHead=CommRecBufferTail=0;
	CommSendBufferHead=CommSendBufferTail=0;
	FlagRecComm=0;
	for (i=0;i<DB_SENDMAXSIZE;i++)
	{
		CommSendBuffer[i]=0;
	}
	for (i=0;i<DB_RECMAXSIZE;i++)
	{
		CommRecBuffer[i]=0;
	}
}

void init_serial(void)
{
	ClearCommRecBuffer();
	OpenComm();
}

void OpenComm(void) 
{
	PCON |= 0x80;		// SMOD=1 (HW_UART uses Timer 1 overflow with no divide down).
	TMOD |= 0x20;		// Configure Timer 1 for use by UART0
	CKCON |= 0x10;		// Timer 1 derived from SYSCLK

	RCAP2H=(65536-(SYSCLK/BAUDRATE0/32))/256;
	RCAP2L=(65536-(SYSCLK/BAUDRATE0/32))%256;
	TH2=RCAP2H;TL2=RCAP2L;
	CT2=0;				//T2:timer mode	
	TR2=1;
	TCLK=1;RCLK=1;		//说明:52,对于SIO0,可选择T1(TCLK=0,RCLK=0)或T2(TCLK=1,RCLK=1)作为波特率发生器
					    //            SIO1只能用T1作为波特率发生器
					    //baud=OSC/(32*(65536-[RCAP2H,RCAP2L])
	CommSendBufferHead=CommSendBufferTail=0; // set the head and tail to the base of the ring buffer
	CommRecBufferHead=CommRecBufferTail=0;
	FlagRecComm=0;
	RI0=0;					// Clear HW_UART receive and transmit
	TI0=0;					// complete indicators.
	SCON0 = 0x50;			// Configure UART0 for mode 1, receiver enabled.
	ES0=1; 					// allow the serial interrupt
	SendItComm=1;
}
/*
void SendCommChar(char ch)
{
	CommSendBuffer[CommSendBufferTail]=ch; 
	CommSendBufferTail++;
	if (CommSendBufferTail==DB_SENDMAXSIZE)
	{ 	
		CommSendBufferTail=0;
	}
	if (SendItComm)
	{	 
		SendItComm=0;
		SBUF0=CommSendBuffer[CommSendBufferHead]; 
	}
	while (CommSendBufferHead!=CommSendBufferTail);
	return ;
}

code unsigned char hex[]="0123456789ABCDEF";
void SendCommHex(unsigned char senddata)//往串口发送hex码 表示的一个字符 例如senddata=0x3A那么将向串口发送两个字符'3','A'hex[]为转换表，在前面有定义
{
	unsigned char ch;
	ch=senddata>>4;
	SendCommChar(hex[ch]);
	ch=senddata&0x0F;
	SendCommChar(hex[ch]);
}
void SendCommWord(unsigned int asciiword)
//向串口发送一个int型的 hex码表示的字符 例如：asciiword=0x124D 将向串口发送4个字符：'1','2','4','D'
{
	unsigned char ascii;
	ascii=asciiword>>8;
	SendCommHex(ascii);
	ascii=asciiword&0xff;
	SendCommHex(ascii);
}

void SendCommLong(unsigned long asciilong)
{
	SendCommWord(asciilong>>16);
	SendCommWord(asciilong&0xffff);
}
*/
void serial_send(unsigned char *base)
{
	SendCommString(base);
}

void SendCommString(unsigned char *base) 
{
unsigned char i=0;
	if (base[0]==0) return;
	for (;;)
	{	
		if (base[i]==0) break;
		CommSendBuffer[CommSendBufferTail]=base[i]; 
		CommSendBufferTail++; 
		if (CommSendBufferTail==DB_SENDMAXSIZE)
		{ 						
			CommSendBufferTail=0;
		}
		i++;
	}
	if (SendItComm)
	{	 						
		SendItComm=0;
		SBUF0=CommSendBuffer[CommSendBufferHead];
	}
	while (CommSendBufferHead!=CommSendBufferTail);
}
/*
void SendCommBuffer(unsigned char *base, unsigned char size) 
{
unsigned char i=0;
	if (!size) { return; }	
	while (i<size) 
	{	 
		CommSendBuffer[CommSendBufferTail]=base[i]; 
		i++;
		CommSendBufferTail++; 
		if (CommSendBufferTail==DB_SENDMAXSIZE)
		{ 
			CommSendBufferTail=0;
		}
	}
	if (SendItComm)
	{	 
		SendItComm=0;
		SBUF0=CommSendBuffer[CommSendBufferHead]; 
	}
}
*/
void CommISR(void) interrupt 4
{
	if (_testbit_(TI0))
	{
		TI0=0;
		CommSendBufferHead++; 	
		if (CommSendBufferHead==DB_SENDMAXSIZE)
		{	 
			CommSendBufferHead=0;
		}
		if (CommSendBufferHead!=CommSendBufferTail)
		{	 
			SBUF0=CommSendBuffer[CommSendBufferHead]; // send the next byte
			SendItComm=0;
		}
		else
		{
			SendItComm=1;
		}
	}
	if (_testbit_(RI0))	
	{	 
		RI0=0;
		if (CommRecBufferTail==CommRecBufferHead)
		{
			CommRecDataOverflowFlag=1;				//接收缓冲区溢出
		}
		CommRecBuffer[CommRecBufferTail]=SBUF0;     //receive data           
	    CommRecBufferTail++;
	    if (CommRecBufferTail==DB_RECMAXSIZE)
	    {
	    	CommRecBufferTail=0;
	    }
		FlagRecComm=1;
   	}
}

//从接收缓冲区读数据 ,无数据返回0,有数据返回1
/*bit GetCommChar(unsigned char idata *ch)      
{ 
	if (CommRecBufferTail==CommRecBufferHead) return 0;     
	*ch=CommRecBuffer[CommRecBufferHead];
	CommRecBufferHead++;
	if (CommRecBufferHead==DB_RECMAXSIZE)
	{
		CommRecBufferHead=0;
	}
	if (CommRecBufferTail==CommRecBufferHead) FlagRecComm=0;
	return 1;
}
/*
//在T(0-255)毫秒内从接收缓冲区读数据 ,无数据返回0,有数据返回1
bit GetCommCharWait(unsigned char idata *ch,unsigned char T)  //T ms    
{ 
	Count1ms=T;*ch=0;
	while (Count1ms)
	{
		if (CommRecBufferTail!=CommRecBufferHead) break;
	}
	if (Count1ms==0) return 0;
	*ch=CommRecBuffer[CommRecBufferHead];
	CommRecBufferHead++;
	if (CommRecBufferHead==DB_RECMAXSIZE)
	{
		CommRecBufferHead=0;
	}
	return 1;
}
*/

//------------------------------------------------------------------------
// This function converts an integer to an ASCII string.  It is a 
// normally provided as a standard library function but the Keil
// libraries do not include it.  Caution: The string passed to this
// must be at least 12 bytes long
//------------------------------------------------------------------------
char * itoa(UINT value, char * buf, UCHAR radix)
{
	UINT i;
	char * ptr;
	char * temphold;

  	temphold = buf;
	ptr = buf + 12;
	*--ptr = 0;		// Insert NULL char
	do
	{
	   // First create string in reverse order
	   i = (value % radix) + 0x30;
		if(i > 0x39) i += 7;
		*--ptr = i;
      value = value / radix;
  	} while(value != 0);

	// Next, move the string 6 places to the left
	// Include NULL character
	for( ; (*buf++ = *ptr++); );	
	return(temphold);
}


