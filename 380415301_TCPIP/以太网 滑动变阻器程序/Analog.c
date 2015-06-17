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


void int2str(int x, char* str);			 //�����������ж���	���ε��ַ���ת��
void SendString(char *pSendString);		//�ַ�������



//--------------------------------------------------------------------------
// Initialize the A/D converter
// 
//--------------------------------------------------------------------------
void init_adc(void)
{
	REF0CN    = 0x0F; 				   //VDDΪ��ѹ��׼ ���ڲ��¶ȴ������������ڲ�ƫѹ�������������ڲ���׼�����ڱ�������VREF���š�
										//REF0CN=0x00ʱVREF��Ϊ��ѹ��׼
	mux_select = MUX_CPU_TEMP;	  		// CPU on-chip temp sensor	 Ƭ���¶ȴ�����
    AMX0P      = MUX_CPU_TEMP;			//ADC0������ vref��Ϊ����  AMX0P=0x13Ϊ����P1.1Ϊ�����
    AMX0N     = 0x1F;					//GND���˷�ʽ ����������������ã�
    ADC0CF    = 0xF8;                   //ADC0���üĴ��������Ҷ���
    ADC0CN    = 0x80;					//ADC0ʹ��
}

 /*********************************************************************/
// ADC�жϳ���  ��Ҫ�ǿ��Դ�  �˳�����ʱ����Ҫ�����ж϶�ȡ
/*********************************************************************/
/*void ADC0_ConvComplete_ISR(void) interrupt 10
{
      temp= ADC0H;						  //���ֽ�
      ams=ADC0L;						  //���ֽ�
	  temp_l=temp<<8+ams;
	  temp_l=temp_l>>4;
      AD0INT = 0;						  //���ж�
}*/

//--------------------------------------------------------------------------
// This function is a little state machine which reads one analog
// inputs at a time, out of the 3 possible inputs
//  1. On-chip temperature
//  2. External air temperature
//  3. CPU operating voltage
//--------------------------------------------------------------------------
void read_analog_inputs(void)	 //������Ķ�ȡ�������� ������ȡƬ���¶� �� P2.5�ŵ��������
{								  //��Ҫ���cpu_temperature��cpu_voltage��������	 ͬʱ�����ַ�����ת��������
//	ULONG idata temp_long;

    AD0INT = 0;                      // ��ת��������־λ
    AD0BUSY = 1;                     // ��ʼת��
	while (AD0INT == 0);             // �ȴ�ת������

	switch (mux_select)
	{
		case MUX_CPU_TEMP:
	      	temp= ADC0H;					//���ֽ�
		    ams=ADC0L;						  //���ֽ�
			temp=temp<<8;
			temp_l=temp+ams;

			temp_l=temp_l*3.125;
	//		temp_l=(temp_l-603)/2.5;
			temp_l=temp_l/0.24;

			int2str(temp_l, a);
			SendString("T:");
			SendString(a);

			cpu_temperature=temp_l;
			AMX0P = MUX_CPU_VOLTS;		// Select AIN1 for next read  ѡ��AIN1��Ϊ��һ��Ҫ��ȡ����������
			REF0CN    = 0x0F; 
			mux_select = MUX_CPU_VOLTS;
		break;

		case MUX_CPU_VOLTS:
			temp= ADC0H;					   //���ֽ�
		    ams=ADC0L;						  //���ֽ�
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
     temp= ADC0H;						  //���ֽ�
     ams=ADC0L;						  //���ֽ�
	 temp=temp<<8;
	 temp_l=temp+ams;
	 temp_l=temp_l>>4;
   cpu_voltage = temp_l;
   int2str(cpu_voltage, a);
   SendString(a);*/
}
 

 