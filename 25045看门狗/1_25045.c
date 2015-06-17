#include <reg52.h>
#include <stdio.h>
#include <absacc.h>
sbit SO=P1^4;/*������� */
sbit SI=P1^5;/* ��������*/
sbit SK=P1^3;/* ʱ�� */
sbit CS=P1^7;/*Ƭѡ */
sbit WP=P1^6;/* ����*/

#define WREN 0x06//д����
#define WRDI 0x04//д��ֹ
#define RDSR 0x05//���Ĵ���
#define WRSR 0x01 //д�Ĵ���
#define READ0 0x03//��0ҳ
#define READ1 0x0b//��1ҳ
#define WRITE0 0x02//д0ҳ
#define WRITE1 0x0a//д1ҳ

#define rd XBYTE[0x0000]
#define wr  XBYTE[0x0010]

unsigned char Read8()
{
bit bData;
unsigned char cLoop;
unsigned char cData;
for(cLoop=0;cLoop<8;cLoop++)
{
SK=1;
SK=0;
bData=SO;
cData<<=1;
if(bData)
{
cData|=0x01;
}
}
return cData;
}

void Write8(unsigned char cData)
{
unsigned char cLoop;
for(cLoop=0;cLoop<8;cLoop++)
{
if((cData&0x80)==0)
{
SI=0;
}
else
{
SI=1;
}
SK=0;
SK=1;
cData<<=1;
}
}

unsigned char ReadSR()
{
unsigned char cData;
CS=0;
Write8(RDSR);
cData=Read8();
CS=1;
return cData;
}

unsigned char WriteSR(unsigned char cData)
{
CS=0;
Write8(WREN);
CS=1;
CS=0;
Write8(WRSR);
Write8(cData);
CS=1;
/*wip_poll();*/
}

void Write1B(unsigned char cData,unsigned char cAddress,bit bRegion)
/* д��һ���ֽڣ�cDataΪд�������cAddressΪд���ַ��bRegionΪҳ */
{
while((ReadSR()&0x01)==1)
{
}
CS=0;
Write8(WREN);
CS=1;
CS=0;
if(bRegion==0)
{
Write8(WRITE0);
}
else
{
Write8(WRITE1);
}
Write8(cAddress);
Write8(cData);
SK=0;
CS=1;
/*wip_poll();*/
}

unsigned char Read1B(unsigned char cAddress,bit bRegion)
/* ����һ���ֽڣ�cAddressΪ�����ַ��bRegionΪҳ */
{
unsigned char cData;
while((ReadSR()&0x01)==1)
{
}
CS=0;
if(bRegion==0)
{
Write8(READ0);
}
else
{
Write8(READ1);
}
Write8(cAddress);
cData=Read8();
CS=1;
return cData;
}

/*unsigned char wip_poll(void)
{
unsigned char i;
unsigned char wip;
for(i=0xff;i>=1;i--)
{
wip=ReadSR();
wip&=0x01;
if(wip==0) break;
}
return(wip);
}*/

void main(void)
{
 unsigned char vr,j,add;
 unsigned char xdata *ad_rd;
 unsigned char xdata *ad_wr;
 ad_rd=&rd;
 ad_wr=&wr;
 add=0x00;
 WP=1;

 WriteSR(0x30);//д״̬�Ĵ������忴�Ź�

 CS=0;
 Write8(WREN);
 CS=1;
 vr=ReadSR(); //��״̬�Ĵ���

 /*vr=out;
 Write1B(vr,0x00,0);
 vr=0x00;
 vr=Read1B(0x00,0);
 in=vr;//��0000�������ݵ�0010*/

 for(j=0;j<4;j++)
  {
   vr=*ad_rd;
   Write1B(vr,add,0);
   vr=0x00;
   vr=Read1B(add,0);
   *ad_wr=vr;
   ad_rd++;
   ad_wr++;
   add++;
  }
}
