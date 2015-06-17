//-----------------------------------------------------------------------------
// Copyright (c) 2002 Jim Brady
// Do not use commercially without author's permission
// Last revised August 2002
// Net ANALOG.C
//
// This module handles the analog inputs which are external temperature
// sensor, the on-chip temperature sensor, and operating voltage.
//-----------------------------------------------------------------------------
#include <string.h>
//#include <stdlib.h>
#include "C8051f340.h"
#include "net.h"
#include "analog.h"

extern char xdata text[];
char a[10];
UINT idata cpu_temperature=3700,cpu_voltage;
UCHAR idata mux_select;
UINT idata temp,ams,temp_l;
sfr16 ADC0     = 0xbe;                 // ADC0 data


void int2str(int x, char* str);			 //在主程序中有定义	整形到字符串转换
void SendString(char *pSendString);		//字符串发送



//--------------------------------------------------------------------------
// Initialize the A/D converter
// 
//--------------------------------------------------------------------------
void init_adc(void)
{
	REF0CN    = 0x0F; 				   //VDD为电压基准 ，内部温度传感器工作，内部偏压发生器工作，内部基准缓冲期被驱动到VREF引脚。
										//REF0CN=0x00时VREF作为电压基准
	mux_select = MUX_CPU_TEMP;	  		// CPU on-chip temp sensor	 片上温度传感器
    AMX0P      = MUX_CPU_TEMP;			//ADC0正输入 vref作为输入  AMX0P=0x13为配置P1.1为输入端
    AMX0N     = 0x1F;					//GND单端方式 （正反输入均需配置）
    ADC0CF    = 0xF8;                   //ADC0配置寄存器数据右对齐
    ADC0CN    = 0x80;					//ADC0使能
}

 /*********************************************************************/
// ADC中断程序  需要是可以打开  此程序暂时不需要进行中断读取
/*********************************************************************/
/*void ADC0_ConvComplete_ISR(void) interrupt 10
{
      temp= ADC0H;						  //高字节
      ams=ADC0L;						  //低字节
	  temp_l=temp<<8+ams;
	  temp_l=temp_l>>4;
      AD0INT = 0;						  //清中断
}*/

//--------------------------------------------------------------------------
// This function is a little state machine which reads one analog
// inputs at a time, out of the 3 possible inputs
//  1. On-chip temperature
//  2. External air temperature
//  3. CPU operating voltage
//--------------------------------------------------------------------------
void read_analog_inputs(void)	 //对输入的读取分析工作 反复读取片内温度 和 P2.5脚的输入情况
{								  //主要输出cpu_temperature和cpu_voltage到主程序	 同时进行字符串的转换及发送
//	ULONG idata temp_long;

    AD0INT = 0;                      // 清转换结束标志位
    AD0BUSY = 1;                     // 开始转换
	while (AD0INT == 0);             // 等待转换结束

	switch (mux_select)
	{
		case MUX_CPU_TEMP:
	      	temp= ADC0H;					//高字节
		    ams=ADC0L;						  //低字节
			temp=temp<<8;
			temp_l=temp+ams;

			temp_l=temp_l*3.125;
	//		temp_l=(temp_l-603)/2.5;
			temp_l=temp_l/0.24;

			int2str(temp_l, a);
			SendString("T:");
			SendString(a);

			cpu_temperature=temp_l;
			AMX0P = MUX_CPU_VOLTS;		// Select AIN1 for next read  选择AIN1作为下一个要读取的数据输入
			REF0CN    = 0x0F; 
			mux_select = MUX_CPU_VOLTS;
		break;

		case MUX_CPU_VOLTS:
			temp= ADC0H;					   //高字节
		    ams=ADC0L;						  //低字节
			temp=temp<<8;
			temp_l=temp+ams;
			//temp_l=temp_l*3.125;
			temp_l=temp_l*10;

			int2str(temp_l, a);
			SendString("V:");
   			SendString(a);

			cpu_voltage = temp_l;
			AMX0P = MUX_CPU_TEMP;		// Select on-chip temp sensor
			REF0CN    = 0x07; 
			mux_select = MUX_CPU_TEMP;
		break;

		default:
			AMX0P = MUX_CPU_TEMP;
			mux_select = MUX_CPU_TEMP;
		break;
  	}
	/*
     temp= ADC0H;						  //高字节
     ams=ADC0L;						  //低字节
	 temp=temp<<8;
	 temp_l=temp+ams;
	 temp_l=temp_l>>4;
   cpu_voltage = temp_l;
   int2str(cpu_voltage, a);
   SendString(a);*/
}
 

 