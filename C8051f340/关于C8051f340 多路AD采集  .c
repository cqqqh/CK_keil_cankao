关于C8051f340 多路AD采集  

2011-12-05 13:08:05|  分类： 默认分类 |字号 订阅
这是IDE中的一个范例。

sfr16 TMR2RL   = 0xca;                 //把2个寄存器合起来写。
sfr16 TMR2     = 0xcc;
sfr16 ADC0 = 0xBD; // ADC0 采样数据寄存器地址
比如AD数据寄存器， sfr  ADC0L        =  0xBD;             // ADC0 Low
                                   sfr  ADC0H        =  0xBE;

  我们如果这样写的话，比如我们想得到AD采集结果，那么要计算高数据寄存器，低数据寄存器，而现在把2个寄存器合起来，那样就省略掉计算，用起来更方便了。 

void Timer2_Init (void)
{
   TMR2CN  = 0x00;                     // Stop Timer2; Clear TF2;
                                       // use SYSCLK as timebase, 16-bit 
                                       // auto-reload
   CKCON  |= 0x10;                     // select SYSCLK for timer 2 source
   TMR2RL  = 65535 - (SYSCLK / 10000); // init reload value for 10uS
   TMR2    = 0xffff;                   // set to reload immediately
   TR2     = 1;                        // start Timer2
}


void ADC0_Init (void)
{
   ADC0CN = 0x02;                      
                                       
   REF0CN = 0x0f;                       
  
    AMX0P = 2;                       // 修改这个，令它等于一个变量，那么我们用到哪路AD，我们就可以用变量来赋值就OK了 。
    AMX0N = 0x1F;                        
                                    

   ADC0CF = ((SYSCLK/3000000)-1)<<3;   

   ADC0CF |= 0x00;                   

   EIE1 |= 0x08;                       // enable ADC0 conversion complete int.

   AD0EN = 1;                          // enable ADC0
}


 
void ADC0_ISR (void) interrupt 10         //参考USB测试的程序，那么我们可以把AMX0P = 2; 放到中断里来解决       
{                                                                  
     
   static unsigned long accumulator = 0;     
   static unsigned int measurements = 1;  
    
    AD0INT = 0;   
 accumulator += ADC0;
   measurements--;

   if(measurements==0)
   {  
  