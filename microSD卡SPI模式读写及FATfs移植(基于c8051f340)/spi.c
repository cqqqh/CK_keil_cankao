#include "c8051f340.h"
#include "spi.h"

void delay(unsigned int count)
{
	while(count--);
}

/*SPI发送数据函数*/
void SPI_transmit(unsigned char c)
{	 
	SPI0DAT = c;
	while(!SPIF);
	SPIF = 0;
}

/*SPI接收数据函数*/
unsigned char SPI_receive()
{	 
	SPI0DAT = 0xff;
	while(!SPIF);
	SPIF = 0;
	return SPI0DAT;
}

#ifdef _SPI_IO_
/*软件模拟SPI发送函数*/
void SPI_transmit(unsigned char c)
{
	unsigned int cnt = 0;
	
	for (cnt = 0; cnt<8; cnt++)
	{
		CLK = 0;				
		
		if(c & 0x80)			
			DO = 1;				
		else
			DO = 0;				
				
		CLK = 1;			
		delay(1);
		
		c = c << 1;	
	}
}

/*软件模拟SPI接收函数*/
unsigned char SPI_receive(void)
{
	unsigned char cnt = 0, c = 0xff;
	
	for (cnt = 0;cnt < 8; cnt++)
	{
		CLK = 0;					
		delay(1);
		CLK = 1;				
		c = c << 1;		
								
		if(DI) 
			c++;					
	}
	
	return c;
}

#endif /*end _SPI_IO_*/