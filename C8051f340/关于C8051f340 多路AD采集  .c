����C8051f340 ��·AD�ɼ�  

2011-12-05 13:08:05|  ���ࣺ Ĭ�Ϸ��� |�ֺ� ����
����IDE�е�һ��������

sfr16 TMR2RL   = 0xca;                 //��2���Ĵ���������д��
sfr16 TMR2     = 0xcc;
sfr16 ADC0 = 0xBD; // ADC0 �������ݼĴ�����ַ
����AD���ݼĴ����� sfr  ADC0L        =  0xBD;             // ADC0 Low
                                   sfr  ADC0H        =  0xBE;

  �����������д�Ļ�������������õ�AD�ɼ��������ôҪ��������ݼĴ����������ݼĴ����������ڰ�2���Ĵ�����������������ʡ�Ե����㣬�������������ˡ� 

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
  
    AMX0P = 2;                       // �޸��������������һ����������ô�����õ���·AD�����ǾͿ����ñ�������ֵ��OK�� ��
    AMX0N = 0x1F;                        
                                    

   ADC0CF = ((SYSCLK/3000000)-1)<<3;   

   ADC0CF |= 0x00;                   

   EIE1 |= 0x08;                       // enable ADC0 conversion complete int.

   AD0EN = 1;                          // enable ADC0
}


 
void ADC0_ISR (void) interrupt 10         //�ο�USB���Եĳ�����ô���ǿ��԰�AMX0P = 2; �ŵ��ж��������       
{                                                                  
     
   static unsigned long accumulator = 0;     
   static unsigned int measurements = 1;  
    
    AD0INT = 0;   
 accumulator += ADC0;
   measurements--;

   if(measurements==0)
   {  
  