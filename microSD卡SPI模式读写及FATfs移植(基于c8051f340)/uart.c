#include <c8051f340.h>
#include "uart.h"

/*------------------------------------------------------------------------------------*/
/*从串口接收一个字节数据*/
unsigned char receiveByte(void)
{
	unsigned char InData;

	while(!RI0);		//判断字符是否收完
	InData = SBUF0;		//从缓冲区读取数据
	RI0 = 0;			//清RI
	return (InData);	//返回收到的字符
}

/*------------------------------------------------------------------------------------*/
/*从串口发送一个字节数据*/
void transmitByte(unsigned char OutData)
{
	SBUF0 = OutData;	//输出字符
	while(!TI0);		//判断字符是否发完
	TI0 = 0;			//清TI
}

/*------------------------------------------------------------------------------------*/
/*以16进制格式发送一个字节数据*/
void transmitHex(unsigned char c)
{
	unsigned char i, temp;
	unsigned char dataString[] = "0x  ";

	for(i=2; i>0; i--)
	{
		temp = c % 16;

		if((temp >= 0) && (temp < 10)) 
			dataString[i+1] = temp + 0x30;
		else 
			dataString[i+1] = (temp - 10) + 0x41;

		c = c/16;
	}

	transmitString (dataString);
}

/*------------------------------------------------------------------------------------*/
/*从串口发送字符串*/
void transmitString(char* string)
{
	while(*string)
		transmitByte(*string++);
}

/*------------------------------------------------------------------------------------*/
/*在超级终端显示数据*/
void displayData(unsigned char *buff, unsigned int len)
{
	int i = 0;
	
	NEWLINE;
	for(i=0;i<len;i++)
	{
		transmitHex(buff[i]);
		transmitString("|");
		if(!((i+1)%256))
		{
			transmitString("\r\nPress any key for more...\r\n");
			receiveByte();
			CLS;
		}
	}
}


