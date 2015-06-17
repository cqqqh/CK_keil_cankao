#ifndef _UART_H_
#define _UART_H_

#define NEWLINE {transmitByte(0x0d); transmitByte(0x0a);} /*���У��൱��"\r\n"*/
#define CLS {transmitByte(0x0c);transmitByte(0x0c);} /*0x0cΪwindows�����ն������������������ȷ������*/

/*�Ӵ��ڽ���һ���ֽ�����*/
unsigned char receiveByte(void);

/*�Ӵ��ڷ���һ���ֽ�����*/
void transmitByte(unsigned char);

/*�Ӵ��ڷ����ַ���*/
void transmitString(unsigned char*);

/*��16���Ƹ�ʽ����һ���ֽ�����*/
void transmitHex(unsigned char);

/*�ڳ����ն���ʾ����*/
void displayData(unsigned char *buff, unsigned int len);

#endif
