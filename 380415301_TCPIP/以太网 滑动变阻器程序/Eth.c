//-----------------------------------------------------------------------------
// Copyright (c) 2002 Jim Brady
// Do not use commercially without author's permission
// Last revised August 2002
// Net ETH.C
//
// This module is the Ethernet layer
//-----------------------------------------------------------------------------
#include <string.h>
#include <stdlib.h>
#include "C8051f.h"
#include "net.h"
#include "serial.h"
#include "arp.h"
#include "ip.h"
#include "eth.h"


#define reg00   XBYTE[0x8000]   //reg00- 10为isa网卡接口的寄存器地址300-310；
#define reg01   XBYTE[0x8001]
#define reg02   XBYTE[0x8002]
#define reg03   XBYTE[0x8003]
#define reg04   XBYTE[0x8004]
#define reg05   XBYTE[0x8005]
#define reg06   XBYTE[0x8006]
#define reg07   XBYTE[0x8007]
#define reg08   XBYTE[0x8008]
#define reg09   XBYTE[0x8009]
#define reg0a   XBYTE[0x800a]
#define reg0b   XBYTE[0x800b]
#define reg0c   XBYTE[0x800c]
#define reg0d   XBYTE[0x800d]
#define reg0e   XBYTE[0x800e]
#define reg0f   XBYTE[0x800f]
#define reg10   XBYTE[0x8010]

bit txd_buffer_select=0;   		//选择网卡的发送缓冲区  
extern UCHAR idata debug;
extern UCHAR xdata arpbuf[];
extern UCHAR xdata my_hwaddr[]; 

void Delay1ms(unsigned char T);

extern UCHAR idata rcve_buf_allocated;
extern UINT volatile event_word;



#define Rtl8019ResetLow 	P5 &= ~(0x4); 	// P52 
#define Rtl8019ResetHigh 	P5 |= 0x4; 		// P52

//------------------------------------------------------------------------
// Initialize the Cirrus Logic 8019 chip
//------------------------------------------------------------------------

void page(unsigned char pagenumber)
{
	unsigned char data temp;
	temp=reg00;		
	temp=temp&0x3B ;
	pagenumber=pagenumber <<6;
	temp=temp | pagenumber;
	reg00=temp;
}

void Rtl8019AS_Reset() //复位网卡
{
	Rtl8019ResetHigh;;  	
	Delay1ms(200);
	Rtl8019ResetLow;
	Delay1ms(200);
}

void ReadRtl8019NodeID(void)//读出网卡的物理地址存到my_ethernet_address.bytes[6]里  
{
	unsigned char data i;
	page(0);
	reg09=0;	//读取网卡的ram的地址为0x0000 
	reg08=0;
	reg0b=0;
	reg0a=12;	//读取12个字节
	reg00=0x0a;	//读ram
	for (i=0;i<6;i++)
	{ 
//    	my_hwaddr[i]=reg10;
//    	my_hwaddr[i]=reg10;
   	}
}

void WriteRtl8019NodeID()
{
	page(1);
	reg01=my_hwaddr[0];
	reg02=my_hwaddr[1];
	reg03=my_hwaddr[2];
	reg04=my_hwaddr[3];
	reg05=my_hwaddr[4];
	reg06=my_hwaddr[5];
	page(0);
}

void init_8019(void)
{
	Delay1ms(10);
	Rtl8019AS_Reset();		//复位8019
	R8019_CHIP_SELECT;
	reg00=0x21;				//使芯片处于停止模式,这时进行寄存器设置 停止模式下,将不会发送和接收数据包
	Delay1ms(10);			//延时10毫秒,确保芯片进入停止模式
	page(0);
	reg0a=0x00;	reg0b=0x00;
	reg0c= 0xe0;			//monitor mode (no packet receive)
	reg0d= 0xe2;			//loop back mode 使芯片处于mon和loopback模式,跟外部网络断开  
	reg01=0x4c;	reg02=0x80;	reg03=0x4c;	reg04=0x40;
	reg07=0xff;				//清除所有中断标志位
	reg0f=0x00;				//disable all interrupt
	reg0e=0xc8;				//byte dma 8位dma方式
	page(1);
	reg07=0x4d;	reg08=0x00;	reg09=0x00;	reg0a=0x00;	reg0b=0x00;	
	reg0c=0x00;	reg0d=0x00;	reg0e=0x00;	reg0f=0x00;
	reg00=0x22;				//这时让芯片开始工作
	ReadRtl8019NodeID();	//读出网卡的物理地址48位  
	WriteRtl8019NodeID();	//将网卡地址写入到mar寄存器  
	page(0);
	reg0c=0xcc;				//将网卡设置成正常的模式,跟外部网络连接
	reg0d=0xe0;
	reg00=0x22;				//这时让芯片开始工作
	reg07=0xff;				//清除所有中断标志位
}

//------------------------------------------------------------------------
// This functions checks 8019 status then sends an ethernet
// frame to it by calling an assembler function. 
//------------------------------------------------------------------------

void send_frame(UCHAR xdata * outbuf, UINT len)/*发送一个数据包的命令,长度最小为60字节,最大1514字节*/
{
UCHAR i;
UINT ii;
	page(0);
	if(len<60)len=60;
	txd_buffer_select=!txd_buffer_select;
  	if	(txd_buffer_select)
		reg09=0x40 ;	//txdwrite highaddress
	else
        reg09=0x46 ;	//txdwrite highaddress
	reg08=0x00; 		//read page address low
	reg0b=len>>8;		//read count high
	reg0a=len&0xff;		//read count low;
	reg00=0x12;			//write dma, page0
	for	(ii=0;ii<len;ii++)	//for	(ii=4;ii<len+4;ii++)  //是否加4有待验证
	{
	   reg10=*(outbuf+ii);  
	}

	/* 以下3句为中止dma的操作,可以不要            */
	reg0b=0x00;		//read count high   中止DMA操作
	reg0a=0x00;		//read count low;
	reg00=0x22;		//complete dma page 0

	for(i=0;i<16;i++)	//最多重发16次
	{
		for(ii=0;ii<1000;ii++)	//检查txp为是否为低
		{
			if ((reg00&0x04)==0) break;
		}
		if ((reg04&0x01)!=0) break;	//表示发送成功
		reg00=0x3e;
	}
	reg07=0xff;
	if(txd_buffer_select)
		reg04=0x40;	//txd packet start; 
	else
		reg04=0x46;	//txd packet start; 
	reg06=len>>8;	//high byte counter
	reg05=len&0xff;	//low byte counter
	reg07=0xff;
	reg00=0x3e;		//to sendpacket;  
	free(outbuf);
}

//------------------------------------------------------------------------
// This functions checks the 8019 receive event status
// word to see if an ethernet frame has arrived.  If so,
// set EVENT_ETH_ARRIVED bit in global event_word
//------------------------------------------------------------------------
void query_8019(void)
{   
char bnry,curr;
	page(0);
   	bnry=reg03;		//bnry page have read 读页指针
	page(1);
	curr=reg07;		//curr writepoint 8019写页指针
	page(0);
	if ((curr==0))	return;
	bnry=bnry++;
	if (bnry>0x7f)	bnry=0x4c;
	if (bnry!=curr)	//此时表示有新的数据包在缓冲区里
	{
	    event_word |= EVENT_ETH_ARRIVED;
    	EA = 1;
	}
 	reg0b=0x00; reg0a=0x00;  reg00=0x22;//complete dma page 0
}

//------------------------------------------------------------------------
// This function gets an incoming Ethernet frame from the 8019.
// There may be more than 1 waiting but just allocate memory for
// one and read one in.  Use the 8019 to queue incoming packets.
//------------------------------------------------------------------------
UCHAR xdata * rcve_frame(void)//如果收到一个有效的数据包,返回收到的数据,否则返回NULL
{
UCHAR bnry,curr,next_page;

UINT len, ii;
UCHAR temp;
UCHAR xdata * buf;

	page(0);
   	bnry=reg03;		//bnry page have read 读页指针
	page(1);
	curr=reg07;		//curr writepoint 8019写页指针
	page(0);
	if ((curr==0))	return NULL;	//读的过程出错
	next_page=bnry;
	bnry=bnry++;
	if (bnry>0x7f)	bnry=0x4c;
	if (bnry!=curr)	//此时表示有新的数据包在缓冲区里
	{
		//读取一包的前4个字节:4字节的8019头部
		page(0);
 		reg09=bnry;		//read page address high
		reg08=0x00; 	//read page address low
		reg0b=0x00;		//read count high
		reg0a=4;		//read count low;
		reg00=0x0a;		//read dma
		
		temp = reg10;		temp = reg10;
		next_page = temp-1;		//next page start-1
		len = reg10;		temp = reg10;
		len += temp<<8;
	 	reg0b=0x00; reg0a=0x00;  reg00=0x22;//complete dma page 0

   		// Allocate enough memory to hold the incoming frame
		buf = (UCHAR xdata *)malloc(len);
		if (buf == NULL)
		{
			// out of RAM
			// Tell 8019 to skip the frame
			page(1);
			curr=reg07; 	//page1
			page(0);		//切换回page0
	        bnry = curr -1;
	        if	(bnry < 0x4c) bnry =0x7f;
	        reg03=bnry; 	//write to bnry   
			reg07=0xff;		//清除中断状态可以不用
			return NULL;
		}
		// This flag keeps track of allocated rcve memory
		rcve_buf_allocated = TRUE;
		// Call the assembler function to get the incoming frame
		reg09=bnry;	//read page address high
		reg08=4; 	//read page address low
		reg0b=len>>8;	//read count high
		reg0a=len&0xff;	//read count low;
		reg00=0x0a;	//read dma
        for(ii=0;ii<len;ii++)
        {
	    	buf[ii]=reg10;
        }
		reg0b=0x00;	reg0a=0x00; reg00=0x22;	//dma complete  page0     
	   	// Return pointer to start of buffer
		bnry=next_page;
		if (bnry<0x4c)  bnry=0x7f;
		reg03=bnry; 	//write to bnry    
        reg07=0xff;
		return (buf);	
	}
	return NULL;
}



 
		

void eth_send(UCHAR xdata * outbuf, UCHAR * hwaddr, UINT ptype, UINT len)
{
	ETH_HEADER xdata * eth;
   
   eth = (ETH_HEADER xdata *)outbuf;
	  
	// Add 14 byte Ethernet header
	memcpy(eth->dest_hwaddr, hwaddr, 6);
	memcpy(eth->source_hwaddr, my_hwaddr, 6); 
   eth->frame_type = ptype;

   // We just added 14 bytes to length
   send_frame(outbuf, len + 14);
}

//------------------------------------------------------------------------
// This is the handler for incoming Ethernet frames
//	This is designed to handle standard Ethernet (RFC 893) frames
// See "TCP/IP Illustrated, Volume 1" Sect 2.2
//------------------------------------------------------------------------
void eth_rcve(UCHAR xdata * inbuf)
{
   ETH_HEADER xdata * eth;
   
   eth = (ETH_HEADER xdata *)inbuf;
   
   // Reject frames in IEEE 802 format where Eth type field
   // is used for length.  Todo: Make it handle this format
   if (eth->frame_type < 1520)
   {
      if (debug) serial_send("ETH: IEEE 802 pkt rejected\r");
      return;      
   }

   // Figure out what type of frame it is from Eth header
   // Call appropriate handler and supply address of buffer
   switch (eth->frame_type)
   {
	   case ARP_PACKET:
	   arp_rcve(inbuf);
	   break;
		      
	   case IP_PACKET:
	   ip_rcve(inbuf);
      break;

      default:
		if (debug) serial_send("Error: Unknown pkt rcvd\r");
      break;
   }
}





