/***************************************************************
UART1 is an asynchronous, full duplex serial port offering a variety of data formatting options. A dedicated
baud rate generator with a 16-bit timer and selectable prescaler is included, which can generate a wide
range of baud rates (details in Section “19.1. Baud Rate Generator” on page 220). A received data
FIFO allows UART1 to receive up to three data bytes before data is lost and an overflow occurs.

UART1 has six associated SFRs. Three are used for the Baud Rate Generator (SBCON1, SBRLH1, and
SBRLL1), two are used for data formatting, control, and status functions (SCON1, SMOD1), and one is
used to send and receive data (SBUF1). The single SBUF1 location provides access to both the transmit
holding register and the receive FIFO. Writes to SBUF1 always access the Transmit Holding Register.
Reads of SBUF1 always access the first byte of the Receive FIFO; it is not possible to read data
from the Transmit Holding Register.

With UART1 interrupts enabled, an interrupt is generated each time a transmit is completed (TI1 is set in
SCON1), or a data byte has been received (RI1 is set in SCON1). The UART1 interrupt flags are not
cleared by hardware when the CPU vectors to the interrupt service routine. They must be cleared manually
by software, allowing software to determine the cause of the UART1 interrupt (transmit complete or receive
complete). Note that if additional bytes are available in the Receive FIFO, the RI1 bit cannot be cleared by
software.
****************************************************************/


/*****UART1*****/

#include "C8051F340.h"
#include "MyDef.h"
#include <string.h>
code unsigned char LED_Charecter[] = { 0xC0, 0xf9, 0xA4, 0xB0, 0x99, 0x92, 0X82, 0xF8, 0x80, 0x90};
code char MyBlog[]="hi.baidu.com/helloelectron\n"; 
unsigned int Byte,Stringlen; 
unsigned int leddata;
bit read_flag;
// SCON1 0xD2

                  
void Delay(unsigned int x)
{
unsigned int a,b;
for(a=x;a>0;a--)
    for(b=100;b>0;b--);
}
// Peripheral specific initialization functions,

void Port_IO_Init()
{
    // any else - Unassigned, Open-Drain, Digital
    //P1MDOUT   = 0x16;// P1.1 P1.2 P1.4 - Unassigned, Push-Pull, Digital
    //P2MDOUT   = 0xFF;
    //P0SKIP    = 0xC0;// set Oscillator at P1^6,7
    
    // P0.4 - TX1 (UART1), Push-Pull, Digital
    // P0.5 - RX1 (UART1), Push-Pull, Digital
P0MDOUT   = 0x30;
    // P0.0 - Skipped,     Open-Drain, Digital
    // P0.1 - Skipped,     Open-Drain, Digital
    // P0.2 - Skipped,     Open-Drain, Digital
    // P0.3 - Skipped,     Open-Drain, Digital
    P0SKIP    = 0x0F;
// enable crossbar
    XBR1      = 0x40;
    // enable UART1
    XBR2      = 0x01;

}


/**************Interrupts_Init****************/
void Interrupts_Init()
{
    EIE2      = 0x02;//Enable UART1 interrupt
    IE        = 0x80;//EA=1
}
/*****Initialize display_led***********/
void display_led(unsigned int leddata)
{

   
HC138Sel_com1();
    
P2=LED_Charecter[leddata%10];
HC138Sel_com1();   
Delay(10);
   
P2=LED_Charecter[leddata%100/10];
HC138Sel_com2();
Delay(10);
   
P2=LED_Charecter[leddata%1000/100];
HC138Sel_com3();
Delay(10);

   
P2=LED_Charecter[leddata/1000];
HC138Sel_com4();
Delay(10);    
}

/************************************
Initialize internal Oscillator 24Mhz 
*************************************/

void Oscillator_Init() // 
{
    unsigned int i;
    OSCICN    = 0x83; //Internal H-F Oscillator Enabled，SYSCLK derived from Internal H-F Oscillator divided by 1.
    OSCICL    = 0x00;//(00000b)the oscillator operates at its fastest setting
    /*
   The 4x Clock Multiplier is configured via the CLKMUL register. 
   The procedure for configuring and enabling
   the 4x Clock Multiplier is as follows:

   1. Reset the Multiplier by writing 0x00 to register CLKMUL.
   2. Select the Multiplier input source via the MULSEL bits.
   3. Enable the Multiplier with the MULEN bit (CLKMUL | = 0x80).
   4. Delay for >5 μs.
   5. Initialize the Multiplier with the MULINIT bit (CLKMUL | = 0xC0).
   6. Poll for MULRDY => ‘1’.
*/
CLKMUL    = 0x00;// 1
CLKMUL   = 0x00; //Select Internal Oscillator 2 
CLKMUL   |= ( 1<<7 );//Clock Multiplier enabled.MULEN=1,0x80 3   
    for (i = 0; i < 20; i++);    // Wait 5us for initialization 4
    CLKMUL |= ( 1<<6 ) | ( 1<<7 );//Initialize the Multiplier with the MULINIT bit (CLKMUL | = 0xC0) 5
    while ((!CLKMUL &( 1<<5 )));   //Poll for MULRDY => ‘1’ 6
CLKSEL = 0x03;//USB clock: 48MHz, system clock : 48MHz 
}
// Initialization function for device,

/*****************UART0_Init*********************/
void UART1_Init()
{

    SBRLL1    = 0x3C;
    SBRLH1    = 0xF6;//9600


//REN1=1 UART1 reception enabled.
SCON1     = 0x10;
//SB1RUN=1 Baud Rate Generator is enabled.
//SB1PS1 SB1PS0 11: Prescaler = 1
    SBCON1    = 0x43;
//8位数据位，无奇偶校验，一位停止位
SMOD1    =0X6C;
Interrupts_Init();

}
void UART1_Send_data(unsigned char Send_data)// http://hi.baidu.com/helloelectron
{
SBUF1=Send_data;
while(SCON1&0x02==0); //TI1=1;
   SCON1 = SCON1&0xfd;//TI1=0; 
}
void UART1_Send_Longdata(unsigned char *str, unsigned int Stringlen)
{
     unsigned int i; 
do 
{ 
       UART1_Send_data(*(str+i));
    Delay(30);//延时很重要，否则会产生误码或者丢字节！ 
    i++; 
   } 
while (i<Stringlen);

}
/*********************The main entry**********************/
void main( )
{
  
PCA0MD &= ~( 1<<6 );//C8051F关闭看门狗
    Oscillator_Init();
Port_IO_Init();
UART1_Init();
    UART1_Send_Longdata(MyBlog,strlen(MyBlog));   //发送BLOG字符串
while(1)
{
       
    if (read_flag)              //如果接收完毕标志已置位，就将读到的数从串口发出 
    { 
     read_flag= 0 ;         //接收完毕标志清0 
    
      UART1_Send_data(Byte);   //发送接收到的字符
    } 
    display_led(Byte);  
}     
  
}

void Uart1_Receive_interrupt()interrupt 16
{
    if (SCON1&0x01)
       {
       SCON1&=0xFE;                           // 清接收中断标志RI0
          Byte = SBUF1;                      // 从串口接收一个字符
          read_flag= 1 ; //就置位取数标志
       }
} 