#include <c8051f340.h>
#include "uart.h"

/*------------------------------------------------------------------------------------*/
/*�Ӵ��ڽ���һ���ֽ�����*/
unsigned char receiveByte(void)
{
	unsigned char InData;

	while(!RI0);		//�ж��ַ��Ƿ�����
	InData = SBUF0;		//�ӻ�������ȡ����
	RI0 = 0;			//��RI
	return (InData);	//�����յ����ַ�
}

/*------------------------------------------------------------------------------------*/
/*�Ӵ��ڷ���һ���ֽ�����*/
void transmitByte(unsigned char OutData)
{
	SBUF0 = OutData;	//����ַ�
	while(!TI0);		//�ж��ַ��Ƿ���
	TI0 = 0;			//��TI
}

/*------------------------------------------------------------------------------------*/
/*��16���Ƹ�ʽ����һ���ֽ�����*/
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
/*�Ӵ��ڷ����ַ���*/
void transmitString(char* string)
{
	while(*string)
		transmitByte(*string++);
}

/*------------------------------------------------------------------------------------*/
/*�ڳ����ն���ʾ����*/
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


