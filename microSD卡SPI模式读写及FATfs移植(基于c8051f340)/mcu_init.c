#include "c8051f340.h"
#include "mcu_init.h"

/*************************** c8051f340硬件初始化 *******************************/
void Port_IO_Init(void)//端口配置
{
   P0MDOUT = 0x1D;     		 // enable TX0,SCK,MOSI as a push-pull
   P2MDOUT = 0x02;			 //	enable P2.1 as a push-pull
   XBR0    = 0x03;    		 // UART0 TX and RX pins enabled, SPI enabled
   XBR1    = 0xC0;  // Enable crossbar, disable weak pull-up
}

/*------------------------------------------------------------------------------------*/
void Oscillator_Init()//系统时钟配置
{
	//48M
	int i = 0;
    FLSCL     = 0x90;
    CLKMUL    = 0x80;
    for (i = 0; i < 20; i++);    // Wait 5us for initialization
    CLKMUL    |= 0xC0;
    while ((CLKMUL & 0x20) == 0);
    CLKSEL    = 0x03;
}

/*------------------------------------------------------------------------------------*/
void UART0_Init(void)//串口配置
{
   int xdata sbrl ;

   sbrl =  (0xFFFF - (SYSCLK/BAUDRATE/2)) + 1;
   SCON0 = 0x10;                   

   if (SYSCLK/BAUDRATE/2/256 < 1) 
   {
      TH1 = -(SYSCLK/BAUDRATE/2);
      CKCON |=  0x08;                  // T1M = 1; SCA1:0 = xx
   } 
   else if (SYSCLK/BAUDRATE/2/256 < 4) 
   {
      TH1 = -(SYSCLK/BAUDRATE/2/4);
      CKCON &= ~0x0B;                  
      CKCON |=  0x01;                  // T1M = 0; SCA1:0 = 01
   } 
   else if (SYSCLK/BAUDRATE/2/256 < 12) 
   {
      TH1 = -(SYSCLK/BAUDRATE/2/12);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 00
   } 
   else 
   {
      TH1 = -(SYSCLK/BAUDRATE/2/48);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 10
      CKCON |=  0x02;
   }

   TL1 = TH1;                          // init Timer1
   TMOD &= ~0xf0;                      // TMOD: timer 1 in 8-bit autoreload
   TMOD |=  0x20;
   TR1 = 1;                            // START Timer1
   TI0 = 1;                            // Indicate TX0 ready
}

/*------------------------------------------------------------------------------------*/
void SPI_Init (void)//SPI总线配置
{
	SPI0CFG = 0x70;              // data sampled on rising edge, clk
                                 // active low,
                                 // 8-bit data words, master mode;

	SPI0CN = 0x0F;               // 4-wire mode; SPI enabled; flags cleared
	SPI0CKR = 0xef; 			 // 48m, SPI clk = 100k
}

/*------------------------------------------------------------------------------------*/
//call this routine to initialize all peripherals
void Init_Devices(void)
{
    Port_IO_Init();
    Oscillator_Init();
	UART0_Init();
	SPI_Init();
}
