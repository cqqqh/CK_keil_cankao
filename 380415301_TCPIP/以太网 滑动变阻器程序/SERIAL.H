#ifndef _SERIAL_
#define _SERIAL_

#define	BAUDRATE0		115200		// 用户定义的UART0 波特率
#define DB_SENDMAXSIZE 0x80
#define DB_RECMAXSIZE 0x80

void init_serial(void);
void OpenComm(void);
void SendCommChar(char ch);
void SendCommHex(unsigned char senddata);//往串口发送hex码 表示的一个字符 例如senddata=0x3A那么将向串口发送两个字符'3','A'hex[]为转换表，在前面有定义
void SendCommWord(unsigned int asciiword);
//向串口发送一个int型的 hex码表示的字符 例如：asciiword=0x124D 将向串口发送4个字符：'1','2','4','D'
void SendCommLong(unsigned long asciilong);
void SendCommAddr(char ch);
void SendCommBuffer(unsigned char *base, unsigned char size);
void SendCommString(unsigned char *base);
void serial_send(unsigned char *base);
bit GetCommChar(unsigned char idata *ch); 
bit GetCommCharWait(unsigned char idata *ch,unsigned char t);
void ClearCommRecBuffer(void);
extern bit CommRecDataOverflowFlag,FlagRecComm,SendItComm;
char * itoa(UINT, char *, UCHAR);

#endif



