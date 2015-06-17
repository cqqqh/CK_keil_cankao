#include<reg52.h>
#include<absacc.h>
#include<string.h>
#include<intrins.h>

#define uint unsigned int
#define byte unsigned char

#define NOP			_nop_()
#define TRUE		1
#define FALSE		0

#define L10MSH  	 0xF4
#define L10MSL  	 0x70

// ���߶��� //

// X5045���ƿ��� //
sbit CS=P1^0;
sbit SCK=P1^1;
sbit SI=P1^2;
sbit SO=P1^3;

// ����,δ�� //
sbit KEY1=P1^7;
sbit KEY2=P1^6;

sbit DOTH=P2^0;
sbit DOTL=P0^7;

// �������� //
#define VOL3	T1
#define VOL2	WR
#define VOL1	RD

// ����оƬ���� //
#define MUSIC_ST	T0 // δ�� //

//��ʾ���� P2 �� ,��λ, BIT0 ΪС����, //
#define D1_0	0xFC
#define D1_1	0x60
#define D1_2	0xDA
#define D1_3	0xF2
#define D1_4	0x66
#define D1_5	0xB6
#define D1_6	0xBE
#define D1_7	0xE0
#define D1_8	0xFE
#define D1_9	0xF6
#define D1_A	0xEE
#define D1_B	0x3E
#define D1_C	0x9C
#define D1_D	0x7A
#define D1_E	0x9E
#define D1_F	0x8E
#define D1_G	0xBC
#define D1_H	0x6E
#define D1_U	0X7E

//��ʾ���� P1 �� ,��λ, BIT7 ΪС����, //
#define D0_0	0x3F
#define D0_1	0x06
#define D0_2	0x5B
#define D0_3	0x4F
#define D0_4	0x66
#define D0_5	0x6D
#define D0_6	0x7D
#define D0_7	0x07
#define D0_8	0x7F
#define D0_9	0x6F
#define D0_A	0x77
#define D0_B	0x7C
#define D0_C	0x39
#define D0_D	0x5E
#define D0_E	0x79
#define D0_F	0x71
#define D0_G	0x3D
#define D0_H	0x76
#define D0_U	0X7E

#define MAX_RX 50
uint idata giRxBuf[MAX_RX];	// ����������,����һ��������������Ҫ48����Ԫ //
byte gbPtRx;	// ������������ָ�� //

byte gbC10ms; // 10ms ������,�����ۼƲ������ź� //
byte gbSecond;
byte gbRead;
byte gbWrite;
byte bDataValue;
#define AD_MAX	2		// ���յ������ݻ�����,ǰ�ֽ�Ϊ��ַ,���ֽ�Ϊ����(4bit) //
byte gaADBuf[2*AD_MAX];

byte gbAddrValue;
byte gbDataValue;
byte gbRxPos;
byte gbRxData;
byte gbRxAddr;
byte gbRxDataBak;	// ����,���ڱȽ�����������ͬ�ĵ�ַ������,��Ϊ��ӳ��δ�� //
byte gbRxAddrBak;

byte gbSound;	// �����ж�����,����ָʾ��һ���� //
byte gbToneDelay,gbToneStop; // ���ʹ���ʱ���ֹͣʱ�� //
byte gbStep;	// ������� //
byte gbTH1,gbTL1;	// ���涨ʱ1��װ��ֵ //

#define M4_7M	0

// ������ʶ������, ��� 3.579M����, PT2262 �񵴵���ȡ4.7M  ��������3.35us //
#ifdef M4_7M
	#define PULSE_WIDE	200			// ������ͳ�����ֽ�� //
	#define SHORT_MIN	80			// ���������� //
	#define SHORT_MAX	150			// ���������� //
	#define LONG_MIN	300			// ���������� //
	#define LONG_MAX	420			// ���������� //
	#define SYN_MIN 	3400		// ͬ��֡������� //
	#define SYN_MAX 	4000		// ͬ��֡������� //
	#define PULSE_TIME_OUT	8000	// ��ʱʱ�� 4500 x 3.35 = 15.075 ms, ˫������ʱ4500 ���ײ������� //
#else
	// ������ʶ������, ��� 3.579M����, PT2262 �񵴵���ȡ1.5M  ��������3.35us //
	#define PULSE_WIDE	70			// ������ͳ�����ֽ�� //
	#define SHORT_MIN	30			// ���������� //
	#define SHORT_MAX	50			// ���������� //
	#define LONG_MIN	100			// ���������� //
	#define LONG_MAX	140			// ���������� //
	#define SYN_MIN 	800		// ͬ��֡������� //
	#define SYN_MAX 	1650		// ͬ��֡������� //
	#define PULSE_TIME_OUT	2000	// ��ʱʱ�� 4500 x 3.35 = 15.075 ms, ˫������ʱ4500 ���ײ������� //
#endif

// 2004-09-24 ���Լ�¼ //
/*

0x01:
3660 - 3775
364 - 379, ��ֵ:366
121 - 140

0x88:
3757 3885,
358		370
114		128

0x06
3845 	3847		3850
372		372		373
120 	121


0xFF:
3752		3753
364			366
121			120

// ���ĵ���,1.5M //

1246  121, 40

*/


// x25045����Ĵ������� //
#define WREN_INST    0x06	//Write enable latch instruction (WREN)
#define WRDI_INST    0x04	//Write disable latch instruction (WRDI)
#define WRSR_INST    0x01	//Write status register instruction (WRSR)
#define RDSR_INST    0x05	//Read status register instruction (RDSR)
#define WRITE_INST   0x02	//Write memory instruction (WRITE)
#define READ_INST    0x03	//Read memory instruction (READ)
#define STATUS_REG   0x10	//Status register, 600ms dog
#define MAX_POLL     0x99	//Maximum number of polls

// EEPROM �洢��ַ���� //
#define EE_FLAG		0x00		// ��־�� //
#define EE_RESET	0x10		// ��λ�����ۼ�,3�ֽ� PBCD //
#define EE_TOTAL	0x14		// ���հ��������ۼ�,4�ֽ� PBCD, ���֧�� 9999,9999 //
#define EE_VOL		0x20		// ��ʾ���������� 0:�ر�, 1:��С, 3:��� //
#define EE_MODE		0x21		// ����ģʽѡ�� 0:�Զ�ȡ�� 1:�ֶ�ȡ�� //
#define EE_CANCEL	0x22		// �Զ�ȡ��ʱ��,�������� //
	/*
	0. 10s
	1. 20s
	2. 30s
	3. 60s
	4. 120s		2min
	5. 180s		3min
	6. 300s		5min
	7. 600s		10min
	8. 1200s	20min
	9. 3600s	60min
	*/

#define EE_TONE		0x23		// ���� //
#define MAX_FUNC 	3		// ������ѯ��Ŀ����� //
#define MAX_SETPARA 	4		// �����޸���Ŀ����� //

#define TONE_MAX	2

#define SER_MAX		16		// �ܹ�ͬʱ����ķ����������� //

byte gbFunc;		// ��Ŀ��� //
byte gbParaData;	// ����ֵ //

byte gbKeyHold;		// ��������ʱ�������,�����ж��Ƿ񳤰���(����4��) //

byte gbSerNum;				// ��ǰ�������Ŀͻ����� //
byte gaSerBuf[SER_MAX];		// �������Ŀͻ���ַ //
uint giSerTime[SER_MAX];	// �������ĳ���ʱ��, ��λ�� //
byte gbSerDisp;

byte gbVolum;	// �������Ʊ��� //
byte gbMode;	// ������ʽ //
byte gbCancel;	// ��ʱʱ��ѡ�� //
byte gbSndIO;	// ������������������ָʾ //
byte gbTone;	// ���� //

byte gbSndMode;	// �������� //

bit fRxStart;
bit fDisFlash;	// ��ֹ�ж�T2 ��ˢ����ʾ //
bit fSetPara;	// ���ò���״̬ //
bit fQueryPara;	// ��ѯ����״̬ //

bit f10ms;

byte code acEnCode1[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80}; // 8421�������� // 
byte code acFirstRun[]="ZHANGHG";	// �̶�����,�����һ���ϵ�,����Ҫ��ʼ��EEPROM //
byte code acVer[]={0xC0,0x04,0x10,0x09};	// �����������,��Ӧ�汾�� //
uint code acTimeOut[]={10,20,30,60,120,180,300,600,1200,3600};
byte code acHigh[18]={D1_0,D1_1,D1_2,D1_3,D1_4,D1_5,D1_6,D1_7,D1_8,D1_9,D1_A,D1_B,D1_C,D1_D,D1_E,D1_F,D1_G,D1_H};
byte code acLow[18]={D0_0,D0_1,D0_2,D0_3,D0_4,D0_5,D0_6,D0_7,D0_8,D0_9,D0_A,D0_B,D0_C,D0_D,D0_E,D0_F,D0_G,D0_H};


#define SND_NEW	0
#define SND_DEL	1
#define SND_KEY	2

//�����ɲ�ͬƵ�ʵķ���������������Ƶ�ʵĹ�ϵ���һ��ʾ:
//���� 3.579MHz�� �������ڣ� 3.35us,  x = 3.353.000 / f 
//���� Ƶ�ʣ�Hz) ����   ��ֵ 	HEX
//Hz	T0	��ֵ	
//262	1138 64398	FB8E
//294	1014 64522	FC0A
//330	904	64632	FC78
//349	855	64681	FCA9
//392	761	64775	FD07
//440	678	64858	FD5A
//494	604	64932	FDA4

//524	569	64967	FDC7
//588	507	65029	FE05
//660	452	65084	FE3C
//698	427	65109	FE55
//784	380	65156	FE84
//880	339	65197	FEAD
//988	302	65234	FED2
//1048	285	65251	FEE3
//1176	254	65282	FF02
//1320	226	65310	FF1E
//1396	214	65322	FF2A
//1568	190	65346	FF42
//1760	169	65367	FF57
//1976	151	65385	FF69
//2096	142	65394	FF72
//2352	127	65409	FF81
//2640	113	65423	FF8F
//2792	107	65429	FF95
//3136	95	65441	FFA1
//3520	85	65451	FFAB
//3952	75	65461	FFB5

//���Ľ�������ʱ�ӳ�����ʵ�֡���ʱ�ӳ���ʵ�ֻ�����ʱʱ�䣬����ֵֻ����������������
// �����1/4����������ʱʱ��Ϊ0.16�룬��Ӧ��ʱ�䳣��Ϊ16����10H������3����������ʱʱ��Ϊ1.92�룬��Ӧʱ�䳤��Ϊ192����C0H����

#define F10_H	0xFB
#define F20_H	0xFC
#define F30_H	0xFC
#define F40_H	0xFC
#define F50_H	0xFD
#define F60_H	0xFD
#define F70_H	0xFD
#define F1A_H	0xFD
#define F2A_H	0XFE
#define F3A_H	0xFE
#define F4A_H	0xFE
#define F5A_H	0XFE
#define F6A_H	0xFE
#define F7A_H	0xFE
#define F1B_H	0xFE
#define F2B_H	0XFF
#define F3B_H	0xFF
#define F4B_H	0xFF
#define F5B_H	0XFF
#define F6B_H	0xFF
#define F7B_H	0xFF
#define F1C_H	0xFF
#define F2C_H	0XFF
#define F3C_H	0xFF
#define F4C_H	0xFF
#define F5C_H	0XFF
#define F6C_H	0xFF
#define F7C_H	0xFF

#define F10_L	0x8E
#define F20_L	0x0A
#define F30_L	0x78
#define F40_L	0xA9
#define F50_L	0x07
#define F60_L	0x5A
#define F70_L	0xA4
#define F1A_L	0xC7
#define F2A_L	0X05
#define F3A_L	0x3C
#define F4A_L	0x55
#define F5A_L	0X84
#define F6A_L	0xAD
#define F7A_L	0xD2
#define F1B_L	0xE3
#define F2B_L	0X02
#define F3B_L	0x1E
#define F4B_L	0x2A
#define F5B_L	0X42
#define F6B_L	0x57
#define F7B_L	0x69
#define F1C_L	0x72
#define F2C_L	0X81
#define F3C_L	0x8F
#define F4C_L	0x95
#define F5C_L	0XA1
#define F6C_L	0xAB
#define F7C_L	0xB5

#define F10	1
#define F20	2
#define F30	3
#define F40	4
#define F50	5
#define F60	6
#define F70	7

#define F1A	8
#define F2A	9
#define F3A	10
#define F4A	11
#define F5A	12
#define F6A	13
#define F7A	14

#define F1B	15
#define F2B	16
#define F3B	17
#define F4B	18
#define F5B	19
#define F6B	20
#define F7B	21

#define F1C	22
#define F2C	23
#define F3C	24
#define F4C	25
#define F5C	26
#define F6C	27
#define F7C	28

byte code acTone[][2]={
	F10_H,F10_L,
	F20_H,F20_L,
	F30_H,F30_L,
	F40_H,F40_L,
	F50_H,F50_L,
	F60_H,F60_L,
	F70_H,F70_L,
	
	F1A_H,F1A_L,
	F2A_H,F2A_L,
	F3A_H,F3A_L,
	F4A_H,F4A_L,
	F5A_H,F5A_L,
	F6A_H,F6A_L,
	F7A_H,F7A_L,

	F1B_H,F1B_L,
	F2B_H,F2B_L,
	F3B_H,F3B_L,
	F4B_H,F4B_L,
	F5B_H,F5B_L,
	F6B_H,F6B_L,
	F7B_H,F7B_L,
	
	F1C_H,F1C_L,
	F2C_H,F2C_L,
	F3C_H,F3C_L,
	F4C_H,F4C_L,
	F5C_H,F5C_L,
	F6C_H,F6C_L,
	F7C_H,F7C_L,
};

// �������� //
byte AnalyzeData(void);
byte GetData(void);
byte HexToBcd(byte bHex);
byte Wr24(byte addr,byte value);
byte xReadByte(byte addr);
byte xReadStatusReg(void);
void AddValue(byte bAddr,byte bDigit);
void AnalyzeCmd(void);
void Delay(uint i);
void Delay10ms(byte n);
void DispClr(void);
void DispLeft(byte bIn);
void DispOut(byte bIn);
void DispRight(byte bIn);
void DispTotal(byte bAddr,byte bDigit);
void DispVer(void);
void Init(void);
void PlaySound(byte bMode,byte bVol);
void TestHard(void);
void WatchDog(void);
void xShiftOut(byte ch);
void xWait(void);
void xWriteByte(byte,byte);
void xWriteStatusReg(void);

