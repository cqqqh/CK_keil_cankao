#include <RB100.h>

void main(void)
{
	uint iTemp;
	byte bNum;
	
	Init();
	while(1){
		CS=0;
		if(GetData()){
			EX0=0;
			AddValue(EE_TOTAL,4);
			if(gbAddrValue>99){
				if(gbAddrValue==0XFF){	// ��ַFF,��ʾ�������úͲ�����ѯ //
					AnalyzeCmd();
				}
				else{
					fDisFlash=1;		
					DispOut(gbAddrValue);
					Delay10ms(100);
					fDisFlash=0;
				}
			}
			else{
				fQueryPara=0;	// �ڲ������ù�����,���յ������û��������,���Զ��˳�����״̬ //
				fSetPara=0;
				AnalyzeData();
				gbSerDisp=0;
			}
			INT0=1;	// ��ֹӲ�������ж�, ������������Ϻ��ٴ�  //
			TH0=0;
			TL0=0;
			bNum=0;
			gbKeyHold=0;
			while(1){
				CS=0;
				iTemp=(TH0<<8)+TL0;
				if(iTemp>SYN_MAX+200)	// �ȴ���ʱ 30 �������ҽ��� //
					break;
				CS=1;
				if(INT0)
					continue;
				iTemp=(TH0<<8)+TL0;
				TH0=0;
				TL0=0;				
				if((iTemp>SYN_MIN)&&(iTemp<SYN_MAX))
					;	// Ѱ��ͬ��λ //
				else if((iTemp>SHORT_MIN)&&(iTemp<LONG_MAX))
					;
				else if(++bNum>20)
					break;
				CS=1;
				while(!INT0);
			}
			if((gbAddrValue==0xFF)&&(gbKeyHold>200)){	// ����������Ϊ������ //
				gbRxData|=0x80;
				gaADBuf[2*gbWrite]=gbAddrValue;
				gaADBuf[2*gbWrite+1]=gbRxData;
				if(++gbWrite>=AD_MAX)
					gbWrite=0;				
				gbRxAddrBak=0;
				gbRxDataBak=0;
			}			
			IE0=0;
			EX0=1;
		}
		CS=1;
	}
}

byte AnalyzeData(void)
{
	byte i,j;
	
	for(i=0;i<gbSerNum;i++){
		if(gaSerBuf[i]==gbAddrValue){ //����鵽,��ʾ��������//
			if(gbMode==1){
				for(j=i;j<gbSerNum-1;j++){
					gaSerBuf[j]=gaSerBuf[j+1];
					giSerTime[j]=giSerTime[j+1];
				}
				gbSerNum--;
				fDisFlash=1;	// ��ֹ�ж�T2 ��ˢ����ʾ //
				DispOut(HexToBcd(gbAddrValue));
				DOTH=1;
				DOTL=1;
				PlaySound(SND_DEL,gbVolum);
				DOTH=0;
				DOTL=0;
				fDisFlash=0;			
				return 0;
			}
			else{	// �Զ�ȡ��ģʽ,������,�������ó�ʱ������ //
				giSerTime[i]=0;
				fDisFlash=1;	// ��ֹ�ж�T2 ��ˢ����ʾ //
				DispOut(HexToBcd(gbAddrValue));
				PlaySound(SND_NEW,gbVolum);
				fDisFlash=0;	// ��ֹ�ж�T2 ��ˢ����ʾ //
				return 1;
			}
		}
	}
	if(gbSerNum<SER_MAX){
		gaSerBuf[gbSerNum]=gbAddrValue;
		giSerTime[gbSerNum]=0;
		gbSerNum++;
		fDisFlash=1;	// ��ֹ�ж�T2 ��ˢ����ʾ //
		DispOut(HexToBcd(gbAddrValue));
		PlaySound(SND_NEW,gbVolum);
		fDisFlash=0;	// ��ֹ�ж�T2 ��ˢ����ʾ //
		return 1;
	}
	// �������,���账��, ��˸��ʾ"FU" //
	PlaySound(SND_DEL,gbVolum);
	fDisFlash=1;	// ��ֹ�ж�T2 ��ˢ����ʾ //
	for(j=0;j<4;j++){
		DispOut(0xFF);
		DispLeft(D1_U);
		DispRight(D0_U);
		Delay10ms(30);
		DispClr();
		Delay10ms(30);
	}
	fDisFlash=0;
	return 2;
}

void AnalyzeCmd(void)
{
	PlaySound(SND_KEY,1);
	if(gbDataValue==0x82){	// ����С��,��Ϊ����ѡ�� //
		if(fQueryPara){
			fQueryPara=0;
		}
		else if(!fSetPara){
			fQueryPara=1;	// ��ѯ���� //
			DispOut(0xC0);
			gbFunc=0;
		}
	}
	else if(gbDataValue==0x86){	//������ͬʱ����//
		if(fSetPara){
			fSetPara=0;
			P0=0;
			P2=0;
		}
		else if(!fQueryPara){
			fSetPara=1;
			DispLeft(0x01);	// ��ʾ 1. //
			gbFunc=0;
			DOTH=1;
			gbParaData=xReadByte(EE_VOL);
			DispRight(gbParaData);				
		}						
	}
	else if(fQueryPara){
		if(gbDataValue==0x02){
			DispLeft(0x0C);	// ��ʾ C //
			if(++gbFunc>=MAX_FUNC)
				gbFunc=0;
			DispRight(gbFunc);
		}
		else if(gbDataValue==0x04){ // �����Ϊȷ�ϼ� //
			if(gbFunc==0){
				DispVer();
			}
			else if(gbFunc==1){
				DispTotal(EE_RESET,3);
			}
			else if(gbFunc==2){
				DispTotal(EE_TOTAL,4);
			}
			DispLeft(0x0C);	// ��ʾ C //
			if(++gbFunc>=MAX_FUNC)
				gbFunc=0;
			DispRight(gbFunc);	
		}
	}
	else if(fSetPara){
		if(gbDataValue==0x02){
			if(++gbFunc>=MAX_SETPARA)
				gbFunc=0;
			DispLeft(gbFunc+1);
			DOTH=1;
			if(gbFunc==0){
				gbParaData=xReadByte(EE_VOL);
			}
			else if(gbFunc==1){
				gbParaData=xReadByte(EE_MODE);
			}
			else if(gbFunc==2){
				gbParaData=xReadByte(EE_CANCEL);
			}
			else if(gbFunc==3){
				gbParaData=xReadByte(EE_TONE);
			}			
			DispRight(gbParaData);					
		}
		else if(gbDataValue==0x04){ // �����Ϊȷ�ϼ� //
			if(gbFunc==0){
				if(++gbParaData>3)
					gbParaData=0;
				DispRight(gbParaData);		
				Delay10ms(50);
				xWriteByte(EE_VOL,gbParaData);
				gbVolum=gbParaData;
				PlaySound(SND_NEW,gbVolum);
			}
			else if(gbFunc==1){
				if(++gbParaData>1)
					gbParaData=0;
				xWriteByte(EE_MODE,gbParaData);
				gbMode=gbParaData;
			}
			else if(gbFunc==2){
				if(++gbParaData>9)
					gbParaData=0;
				xWriteByte(EE_CANCEL,gbParaData);
				gbCancel=gbParaData;
			}
			else if(gbFunc==3){
				if(++gbParaData>=TONE_MAX)
					gbParaData=0;
				DispRight(gbParaData);
				Delay10ms(50);
				xWriteByte(EE_TONE,gbParaData);
				gbTone=gbParaData;
				PlaySound(SND_NEW,gbVolum);
			}
			DispRight(gbParaData);			
		}
	}
}
					
void Delay10ms(byte n)
{
	gbC10ms=n;
	f10ms=0;
	while(1){
		CS=0;
		if(f10ms)
			break;
		CS=1;
	}
	CS=1;
}

byte HexToBcd(byte bHex)
{
	return ((bHex/10)<<4)+(bHex%10);
}

byte PbcdToByte(byte bHex)
{
	return (bHex>>4)*10+(bHex&0x0F);
}

// Ӳ����·�Կ��߷���,�����ش��� //
/*

_|-|_________|-|___|---|_

-|_|---------|_|---|___|---|_



*/
void IntEx0(void) interrupt 0 using 1
{
	uint iTemp;
	bit fPulse,fValid;
	byte bTemp;
	
	INT0=1;
	gbRxAddrBak=0;
	gbRxDataBak=0;
	fRxStart=0;
	fPulse=1;		// 1 ��ʾ INT0=0, ��Ӧ�ı��������Ϊ 1 //
	fValid=0;
	while(!INT0){CS=0;CS=1;};
	TL0=0;
	TH0=0;
	TR0=1;
	TMOD=0x19;  	//��ʱ��1 ��ʱ��0�����ڷ�ʽ1(ȫ16λ��ʱ��), T0 ͨ��INT0���� //
	gbPtRx=0;
	giRxBuf[0]=gbPtRx;
	while(1){
		CS=0;
		iTemp=(TH0<<8)+TL0;
		if(iTemp>PULSE_TIME_OUT)	// �ȴ���ʱ 20 �������ҽ��� //
			break;
		CS=1;
		INT0=1;
		if(INT0)	// ���INT0=1,���ڼ��� //
			continue;
		// INT0=0, ֹͣ����,ȡ������ //
		iTemp=(TH0<<8)+TL0;
		TL0=0;
		TH0=0;
		if(fValid)				// ����Ѿ�����ɹ�,�򲻱��ٴ���,�ȴ��ͻ��ɿ����� //
			continue;
		giRxBuf[0]=gbPtRx;
		giRxBuf[gbPtRx++]=iTemp;
		if(gbPtRx>=MAX_RX)
			gbPtRx=1;
		// ��������͵�ƽʱ��, ���ڼ��ͬ��λ, ͬ��λ���ȴ�ԼΪ 11 ms //
		if((iTemp>SYN_MIN)&&(iTemp<SYN_MAX)){	// Ѱ��ͬ��λ //
			fRxStart=1;
			giRxBuf[0]=1;
			giRxBuf[1]=iTemp;
			gbPtRx=2;
			gbRxData=0;
			gbRxAddr=0;
			gbRxPos=0;
		}
		else if(fRxStart){
			if(((iTemp>SHORT_MIN)&&(iTemp<SHORT_MAX))||((iTemp>LONG_MIN)&&(iTemp<LONG_MAX))){
				if((gbPtRx%2)==0){
					if((giRxBuf[gbPtRx-1]<PULSE_WIDE)&&(iTemp<PULSE_WIDE)){
						bTemp=1;
					}
					else if((giRxBuf[gbPtRx-1]>PULSE_WIDE)&&(iTemp>PULSE_WIDE)){
						bTemp=0;
					}
					else{
						fRxStart=0; //�յ���������,�˳��ж�,�ȴ��´�ͬ�� //
						break;
					}
					if(gbRxPos<8){
						if(bTemp==1)
							gbRxAddr+=acEnCode1[gbRxPos];
					}
					else{
						if(bTemp==1)
							gbRxData+=acEnCode1[gbRxPos-8];
					}
					if(++gbRxPos>=12){
						gbRxPos=0;
						fRxStart=0;
						// ���ؼ��������ͬ,����̰�������Ӧ,ʵ�ʲ���,���1������,����1000�δ�δ���ֹ����� //
//						if((gbRxAddrBak==gbRxAddr)&&(gbRxDataBak==gbRxData)){//						if((gbRxAddrBak==gbRxAddr)&&(gbRxDataBak==gbRxData)){
						fValid=1;
//						}
//						else{
						gbRxAddrBak=gbRxAddr;
						gbRxDataBak=gbRxData;
//						}
						gbKeyHold=0;
						INT0=0;		// ��ֹӲ�������ж�, ������������Ϻ��ٴ�  //
						IE0=0;	// ���жϱ�־ //
						EX0=0;						
						break;
					}
				}
			}
			else{
				fRxStart=0;
				break;	//�յ���������,�˳��ж�,�ȴ��´�ͬ�� //
			}
		}
		while(!INT0){CS=0;CS=1;}
	}
	fRxStart=0;
	if(fValid&&(gbRxAddrBak>0)){	// ������ĵ�ַ�����ݴ��� gaADBuf[] //
		gaADBuf[2*gbWrite]=gbRxAddrBak;
		if(gbKeyHold>200){	// ����������Ϊ������ //
			gbRxDataBak|=0x80;
		}
		gaADBuf[2*gbWrite+1]=gbRxDataBak;
		if(++gbWrite>=AD_MAX)
			gbWrite=0;		
		gbRxAddrBak=0;
		gbRxDataBak=0;
	}
	
}

code byte acMusicNew[]={
	F3C,25,15,
	F1C,25,0,
	255,255,255
};

code byte acMusicNew1[]={
	F3C,35,20,
	F1C,35,0,
	255,255,255
};

code byte acMusicNew2[]={
	F3C,35,20,
	F1C,35,45,

	F3C,35,20,
	F1C,35,0,
	
	255,255,255
};

code byte acMusicDel[]={
	F2C,30,0,
	255,255,255
};
code byte acMusicDel1[]={
	F2C,35,0,
	255,255,255
};

code byte acMusicKey[]={
	F1C,15,0,
	255,255,255
};

code uint acTone0[]={(uint)acMusicNew,(uint)acMusicDel,(uint)acMusicKey};
code uint acTone1[]={(uint)acMusicNew1,(uint)acMusicDel1,(uint)acMusicKey};
code uint acTone2[]={(uint)acMusicNew2,(uint)acMusicDel1,(uint)acMusicKey};
code uint acToneMode[]={acTone0,acTone1,acTone2};

void PlaySound(byte bMode,byte bVol)
{
	if(bVol==0)
		return;
	gbSndIO=bVol;
	gbSndMode=bMode; 
	gbSound=0;
	gbStep=0;	// �������� //
	while(gbStep!=0xff){
		CS=0;
		CS=1;
	}	
}

void IntTime1(void) interrupt 3 using 2
{
	TH1=gbTH1;
	TL1=gbTL1;
	if(gbSndIO==1){
		VOL1=~VOL1;
	}
	else if(gbSndIO==2){
		VOL2=~VOL2;
	}
	else{
		VOL3=~VOL3;
	}
}

// 10ms��ʱ�ж�, 9.998ms //
void IntTime2(void) interrupt 5 using 3
{
	byte bTemp,i,j;
	
	TF2=0;	// ����������� //
	TH2=L10MSH;
	TL2=L10MSL;
	if(gbC10ms==0)
		f10ms=1;
	else if(--gbC10ms==0)
		f10ms=1;
	if(gbKeyHold<255)
		gbKeyHold++;
	if(gbStep==0){
		bTemp= *( (byte code *)(*(uint code *)(acToneMode[gbTone]+2*gbSndMode)) + 3*gbSound);
//	 	if(gbSndMode==SND_NEW){
//	 		if(gbTone==0)
//				bTemp=acMusicNew[3*gbSound];
//			else
//				bTemp=acMusicNew1[3*gbSound];
//		}
//		else if(gbSndMode==SND_DEL)
//			bTemp=acMusicDel[3*gbSound];
//		else
//			bTemp=acMusicKey[3*gbSound];
		if(bTemp==0XFF){	//��ֹ��
			TR1=0;
			ET1=0;
			TXD=0;
			PT2=1;
			//EX0=1; ����ʱ,EX0�Ѿ�Ϊ0,���ܿ��� //
			gbStep=0xff;
			return;		
		}
		bTemp--;
		gbTH1=acTone[bTemp][0];
		gbTL1=acTone[bTemp][1];
		gbStep=1;
		gbToneDelay= *( (byte code *)(*(uint code *)(acToneMode[gbTone]+2*gbSndMode)) + 3*gbSound + 1);
		gbToneStop= *( (byte code *)(*(uint code *)(acToneMode[gbTone]+2*gbSndMode)) + 3*gbSound + 2);
//		if(gbSndMode==SND_NEW){
//			if(gbTone==0){
//				gbToneDelay=acMusicNew[3*gbSound+1];
//				gbToneStop=acMusicNew[3*gbSound+2];
//			}
//			else{
//				gbToneDelay=acMusicNew1[3*gbSound+1];
//				gbToneStop=acMusicNew1[3*gbSound+2];			
//			}
//		}
//		else if(gbSndMode==SND_DEL){
//			gbToneDelay=acMusicDel[3*gbSound+1];
//			gbToneStop=acMusicDel[3*gbSound+2];
//		}
//		else{
//			gbToneDelay=acMusicKey[3*gbSound+1];
//			gbToneStop=acMusicKey[3*gbSound+2];			
//		}
		PT1=1;	// ���� T1 ��ʱ�����ȼ���� //
		PT2=0;
		TR1=1;
		ET1=1;
		//EX0=0;	����ʱ,EX0�Ѿ�Ϊ0 //
		TF1=1;
		
		return;
	}
	else if(gbStep==1){
		if(--gbToneDelay==0){
			if(gbToneStop==0){
				gbStep=0;
				gbSound++;
			}
			else{
				gbStep=2;
			}	
			PT2=1;
			ET1=0;
			VOL1=0;
			VOL2=0;
			VOL3=0;		
		}
		return;
	}
	else if(gbStep==2){//ֹͣ�׶�
		if(--gbToneStop==0){
			gbStep=0;
			gbSound++;
			PT2=1;
		}
		return;
	}

	if(fQueryPara||fSetPara)
		return;
	if(++gbSecond>100){
		gbSecond=0;
		// �������������������1��,��ÿ��1��������ʾ,һ����Ϻ�ͣ1��,�ܶ���ʼ //
		if(gbSerNum>0){	
			if(!fDisFlash){
				if(gbSerDisp>=gbSerNum){
					P0=0;
					P2=0;
				}
				else{					
					bTemp=gaSerBuf[gbSerDisp];
					P2=acHigh[bTemp/10];
					P0=acLow[bTemp%10];
				}
				if(++gbSerDisp>gbSerNum){
					gbSerDisp=0;
				}				
				if(gbSerNum==1)
					gbSerDisp=0;
			}
			for(i=0;i<gbSerNum;i++){
				if(++giSerTime[i]>=acTimeOut[gbCancel]){
					giSerTime[i]=0;
					for(j=i;j<gbSerNum-1;j++){
						gaSerBuf[j]=gaSerBuf[j+1];		
						giSerTime[j]=giSerTime[j+1];		
					}
					gbSerNum--;
				}
			}		
		}
		else if(!fDisFlash){
			P0=0;
			P2=0;
		}
	}	

}

/******************************************************************************
*
*	������: AddValue()
*	��  ��: ͳ��ֵ��1
*	��  ��: ��
*	��  ��: ��
*
******************************************************************************/
void AddValue(byte bAddr,byte bDigit)
{
	byte aBuf[6];
	byte i,bTemp;
	
	if(bDigit>6)
		return;
	//�ȶ���ͳ��ֵ, 0x99,0x99,0x99 //
	for(i=0;i<bDigit;i++){
		aBuf[i]=xReadByte(i+bAddr);
	}
	for(i=0;i<bDigit;i++){
		bTemp=PbcdToByte(aBuf[bDigit-i-1])+1;
		if(bTemp<100){
			bTemp=HexToBcd(bTemp);
			xWriteByte(bAddr+bDigit-i-1,bTemp);
			return;
		}
		xWriteByte(bAddr+bDigit-i-1,0);
	}
}

void DispTotal(byte bAddr,byte bDigit)
{
	byte aBuf[6];
	byte i;
	
	if(bDigit>6)
		return;
	if(bAddr==EE_RESET)
		i=0xC1;
	else 
		i=0xC2;
	DispOut(i);
	Delay10ms(70);
	DispClr();
	Delay10ms(40);
	
	//�ȶ���ͳ��ֵ, 0x99,0x99,0x99 //
	for(i=0;i<bDigit;i++){
		aBuf[i]=xReadByte(i+bAddr);
		DispOut(aBuf[i]);
		Delay10ms(70);
		DispClr();
		Delay10ms(40);
	}
}

void DispVer(void)
{
	byte i;
	
	
	//�ȶ���ͳ��ֵ, 0x99,0x99,0x99 //
	for(i=0;i<4;i++){
		DispOut(acVer[i]);
		Delay10ms(70);
		DispClr();
		Delay10ms(40);
	}

}

void InitEEProm(void)
{
	byte aBuf[7];
	byte i,j;
	
	xWriteStatusReg();	// ���ÿ��Ź����� //
	for(j=0;j<3;j++){	//��Ϊͳ�Ʋ�������Ҫ, ��˱Ƚ�3��,ֻ��3�ξ�����ͬ�ų�ʼ��EEPROM //
		for(i=0;i<7;i++){
			aBuf[i]=xReadByte(EE_FLAG+i);
			if(aBuf[i]!=acFirstRun[i])
				break;
		}
		if(i>=7)
			break;
	}
	if(j>=3){
		for(i=0;i<4;i++){
			xWriteByte(EE_RESET+i,0);
			xWriteByte(EE_TOTAL+i,0);
		}
		xWriteByte(EE_VOL,1);
		xWriteByte(EE_MODE,1);
		xWriteByte(EE_CANCEL,8);
		for(i=0;i<7;i++){
			xWriteByte(EE_FLAG+i,acFirstRun[i]);
		}	
	}
}

void InitIO(void)
{
	CS=1;
	SCK=0;
	SI=0;
	SO=0;
	
	KEY1=0;
	KEY2=0;
	
	T1=0;
	WR=0;
	RD=0;
	RXD=1;
	VOL1=0;
	VOL2=0;
	VOL3=0;
}
	
void Init(void)
{
	
	EA=0;
	TMOD=0x19;      //��ʱ��1 ��ʱ��0�����ڷ�ʽ1(ȫ16λ��ʱ��)	      									/* time0 mode 1 */
	InitIO();	
	DispClr();
	InitEEProm();
	
	gbVolum=xReadByte(EE_VOL);
	gbMode=xReadByte(EE_MODE);
	gbCancel=xReadByte(EE_CANCEL);
	gbTone=xReadByte(EE_TONE);
	
	AddValue(EE_RESET,3);	//ÿ��λһ�μ�1//
//	DispVer();	//��ʾ�汾�Ÿ�������//

	gbStep=0xff;	
	gbSerNum=0;

	gbPtRx=0;
	fRxStart=0;
	gbWrite=0;
	gbRead=0;
	IP=0;	
//	EX0=1;
	EX1=0;
	IT1=1;
	IT0=1;
	ET0=0;
	TR0=1;
	PT2=1;
	TR2=1;
	ET2=1;
	EA=1;
	fSetPara=0;
	fQueryPara=0;
	VOL1=0;
	VOL2=0;
	VOL3=0;
	
	fDisFlash=1;	// ��ֹÿ����ʾ���� //
	Delay10ms(50);
//	PlaySound(SND_DEL,1);
	DispTotal(EE_RESET,3);	//��ʾ��λ����//
	Delay10ms(50);
//	PlaySound(SND_DEL,1);
	DispTotal(EE_TOTAL,4);	//��ʾ���յ���Ч��������//
	fDisFlash=0;	// ���뿪�� //
	EX0=1;	
}


/******************************************************************************
*
*	������: GetData()
*	��  ��: ��ȡ���յ�������
*	��  ��: ��
*	��  ��: ��
*
******************************************************************************/
byte GetData(void)
{	
	if(gbRead!=gbWrite){
		gbAddrValue=gaADBuf[2*gbRead];
		gbDataValue=gaADBuf[2*gbRead+1];		
		if(++gbRead>=AD_MAX)
			gbRead=0;
		return TRUE;
	}
	else
		return FALSE;
}


/******************************************************************************
*
*	��ʾ����
*
******************************************************************************/

/******************************************************************************
*
*	������: DispOut()
*	��  ��: ��ʾ��λ����
*	��  ��: ��
*	��  ��: ��
*
******************************************************************************/
void DispOut(byte bIn)
{
	P2=acHigh[bIn>>4];
	P0=acLow[bIn&0x0F];
}

void DispLeft(byte bIn)
{
	P2=acHigh[bIn];
}

void DispRight(byte bIn)
{
	P0=acLow[bIn];
}

/******************************************************************************
*
*	������: DispClr()
*	��  ��: ����Ļ
*	��  ��: ��
*	��  ��: ��
*
******************************************************************************/
void DispClr(void)
{
	P2=0x00;
	P0=0x00;
}


/******************************************************************************
*
*	x25325 ��ز���
*
******************************************************************************/

/******************************************************************************
*
*	������: xShiftOut()
*	��  ��: ��8bit����
*	��  ��: ��
*	��  ��: ��
*
******************************************************************************/
void xShiftOut(byte ch)
{
	byte i;

	for(i=0;i<8;i++){
		SCK=0;
		if(ch&0x80)
			SI=1;
		else
			SI=0;
		SCK=1;
		ch<<=1;
	}
	SI=0;
}

/******************************************************************************
*
*	������: xShiftIn()
*	��  ��: ����8bit����
*	��  ��: ��
*	��  ��: ���ؽ��յ�������
*
******************************************************************************/
byte xShiftIn(void)
{
	byte i,result;
	
	result=0;
	SO=1;
	for(i=0;i<8;i++){
		result<<=1;
		SCK=1;
		SCK=0;
		if(SO)
			result++;		
	}
	return result;	
}

/******************************************************************************
*
*	������: xWriteEnable()
*	��  ��: ��д����
*	��  ��: ��
*	��  ��: ��
*
******************************************************************************/
void xWriteEnable(void)
{
	CS=1;
	SCK=0;
	CS=0;
	xShiftOut(WREN_INST);
	SCK=0;
	CS=1;
}

/******************************************************************************
*
*	������: xWait()
*	��  ��: ��æ
*	��  ��: ��
*	��  ��: ��
*
******************************************************************************/
void xWait(void)
{
	byte i;
	
	for(i=0;i<MAX_POLL;i++){
		if((xReadStatusReg()&0x01)==0)
			break;
	}
}

/******************************************************************************
*
*	������: xWriteStatusReg()
*	��  ��: д״̬�Ĵ���,���ÿ��Ź�����
*	��  ��: ��
*	��  ��: ��
*
******************************************************************************/
void xWriteStatusReg(void)
{
	xWriteEnable();	
	SCK=0;
	CS=0;
	xShiftOut(WRSR_INST);
	xShiftOut(STATUS_REG);
	SCK=0;
	CS=1;	
	xWait();
	xWriteEnable();
}

/******************************************************************************
*
*	������: xReadStatusReg()
*	��  ��: ��״̬�Ĵ���
*	��  ��: ��
*	��  ��: ����״̬�Ĵ�����ֵ
*
******************************************************************************/
byte xReadStatusReg(void)
{
	byte result;
	
	SCK=0;
	CS=0;
	xShiftOut(RDSR_INST);
	result=xShiftIn();
	SCK=0;
	CS=1;	
	SI=0;
	SO=0;
	return result;
}

/******************************************************************************
*
*	������: xWriteByte()
*	��  ��: дһ�ֽ�����
*	��  ��: addr: ��ַ, value:��д������
*	��  ��: ��
*
******************************************************************************/
void xWriteByte(byte addr,byte value)
{
	xWriteEnable();
	SCK=0;
	CS=0;
	xShiftOut(WRITE_INST);
	xShiftOut(addr);
	xShiftOut(value);
	SCK=0;
	CS=1;	
	xWait();
}

/******************************************************************************
*
*	������: xWriteByte()
*	��  ��: ��һ�ֽ�����
*	��  ��: addr: ��ַ
*	��  ��: ����������
*
******************************************************************************/
byte xReadByte(byte addr)
{
	byte result;
	
	SCK=0;
	CS=0;
	xShiftOut(READ_INST);
	xShiftOut(addr);
	result=xShiftIn();
	SCK=0;
	CS=1;	
	SI=0;
	SO=0;
	return result;
}

void IntEx1(void) interrupt 2 using 1
{

}

void IntUart(void) interrupt 4 using 1
{
	
}

void _time0(void) interrupt 1 using 1
{
	
}
