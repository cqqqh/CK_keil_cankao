#ifndef _UART_H_
#define _UART_H_

#define NEWLINE {transmitByte(0x0d); transmitByte(0x0a);} /*换行，相当于"\r\n"*/
#define CLS {transmitByte(0x0c);transmitByte(0x0c);} /*0x0c为windows超级终端清屏命令，发送两次以确保清屏*/

/*从串口接收一个字节数据*/
unsigned char receiveByte(void);

/*从串口发送一个字节数据*/
void transmitByte(unsigned char);

/*从串口发送字符串*/
void transmitString(unsigned char*);

/*以16进制格式发送一个字节数据*/
void transmitHex(unsigned char);

/*在超级终端显示数据*/
void displayData(unsigned char *buff, unsigned int len);

#endif
