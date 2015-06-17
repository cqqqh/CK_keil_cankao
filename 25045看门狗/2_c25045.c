//整理：聂小猛。该资料来自“51单片机世界”http://go.163.com/~dz2000,欢迎访问。
/***************************************************************
   *  X25043/45 application ; Procedures
   *  absolute one address access
  ***************************************************************
 WARNING: The function with '_' ahead ,user may not use it.as it
 used internal
*/
//使用函数：write_status(status) 写状态，一般写0
//clr_wchdog(void)   清看门狗
//unsigned char	read_byte(address) //读一个字节
//void	write_byte(address,Data)   //写一个字节

#define	ALONE_COMPILE
#ifdef	ALONE_COMPILE
	#include "INTRINS.H"
#endif

#ifndef nop2()
#define nop2()  _nop_();_nop_();_nop_()
#endif

#define WREN    0x06
#define WRDI    0x04
#define RDSR    0x05
#define WRSR    0x01
#define READ    0x03
#define WRITE   0x02

//#define	PORT	P1
sfr	PORT=0x90; //25045的4根io脚接在同一端口，本例为p1
			//请根据实际电路更改引脚定义
#define	_SI	0x80  //si接在p1.3，0x80=00001000b
#define	_SCK	0x40  //sck接在p1.2，0x80=00000100b
#define	_SO	0x20  //so接在p1.1   
#define	_CS	0x10  //cs接在p1.0

//----------------------------------------------------------------
#ifndef	dword
	#define	dword	unsigned long
	#define	word	unsigned int
	#define	byte	unsigned char
	typedef	union{
		word	w;
		byte	bh;
		byte	bl;
	}WordType;

	typedef	union{
		dword	dw;
		word	w[2];
		byte	b[4];
	}DwordType;

#endif

//----------------------------------------------------------------
void	_w_byte(Data)
	char Data;
{
	char i;
		
	PORT &= (_SCK^0xff);
	for(i=0;i<8;i++)
	{
		nop2();nop2();/////////////
		if(Data & 0x80)  PORT |= _SI;
		else PORT &= (_SI^0xff);
               nop2();nop2();/////////////
		PORT |=_SCK;
		nop2();nop2();/////////////
		Data=Data<<1;
		nop2();nop2();/////////////
		PORT &= (_SCK^0xff);
		nop2();nop2();/////////////
	}
}
//----------------------------------------------------------------
char	_r_byte(void)
{
	char i;
	char result;
		
	result=0;
	for(i=0;i<8;i++)
	{
		nop2();nop2();/////////////
		PORT |=_SCK;		
		result=result<<1;
		nop2();nop2();/////////////
		if((PORT & _SO) != 0)
			result |= 0x01;
		nop2();nop2();/////////////
		PORT &=(_SCK^0xff);
		nop2();nop2();/////////////
	}
	return(result);
}
//----------------------------------------------------------------
void	write_status(status)
	char status;
{
	PORT &=(_CS^0xff);
	nop2();nop2();/////////////
	_w_byte(status);
	PORT |=_CS;
	nop2();nop2();/////////////
	return;
}
//----------------------------------------------------------------
void	clr_wchdog(void)
{
	PORT &= (_CS^0xff);
	PORT |=_CS;
}
//----------------------------------------------------------------
void	wait_free(void)
{
	unsigned int  t;

	t=3000;
	while(--t);
}
//----------------------------------------------------------------
void	write_reg(_code)
	char	_code;
{
	write_status(WREN);
	PORT &= (_CS^0xff);
	nop2();nop2();/////////////
	_w_byte(WRSR);
	_w_byte(_code);
	nop2();nop2();/////////////
	PORT |=_CS;
	wait_free();
}
//----------------------------------------------------------------

unsigned char	read_byte(address)
	unsigned int	address;
{	
	char	result;

	PORT &=(_CS^0xff);		// Chip select 		
	nop2();nop2();/////////////
	_w_byte((char)(address>255 ? (0x08|READ): READ));
	_w_byte((char)(address & 0x00ff));
	result=_r_byte();
	nop2();nop2();/////////////
	PORT |=_CS;
					// Chip unselect 	
	return(result);
}
//----------------------------------------------------------------
void	write_byte(address,Data)
	unsigned int address;
	char Data;
{
	write_status(WREN);
	nop2();nop2();/////////////
	PORT &= (_CS^0xff);
	nop2();nop2();/////////////
	_w_byte((unsigned char)(address>255 ? (0x08|WRITE): WRITE));
	_w_byte((unsigned char)(address & 0x00ff));
	_w_byte(Data);
	nop2();nop2();/////////////
	PORT |=_CS;
	wait_free();
	return;
}
/*
//----------------------------------------------------------------
unsigned long	read_data(format,address)
	unsigned char	format;
	unsigned int	address;
{	
	DwordType	result;

	switch(format&0xdf)
	{
	case	'L':
		result.b[0]=read_byte(address);
		result.b[1]=read_byte(address+1);
		result.b[2]=read_byte(address+2);
		result.b[3]=read_byte(address+3);
		break;
	case	'D':
		result.b[2]=read_byte(address);
		result.b[3]=read_byte(address+1);
		break;
	case	'C':
		result.b[3]=read_byte(address);
		break;
	}
	return(result.dw);
}
//----------------------------------------------------------------
void	write_data(format,address,Data)
	unsigned char format;
	unsigned int address;
	DwordType data* Data;
{
	switch(format&0xdf)
	{
	case	'L':
		write_byte(address  , Data->b[0]);
		write_byte(address+1, Data->b[1]);
		write_byte(address+2, Data->b[2]);
		write_byte(address+3, Data->b[3]);
		break;
	case	'D':
		write_byte(address  , Data->b[2]);
		write_byte(address+1, Data->b[3]);
		break;
	case	'C':
		write_byte(address  , Data->b[3]);
		break;
	}

}
//----------------------------------------------------------------
*/