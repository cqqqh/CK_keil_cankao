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
				if(gbAddrValue==0XFF){	// 地址FF,表示功能设置和参数查询 //
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
				fQueryPara=0;	// 在参数设置过程中,接收到任意用户请求服务,则自动退出设置状态 //
				fSetPara=0;
				AnalyzeData();
				gbSerDisp=0;
			}
			INT0=1;	// 禁止硬件产生中断, 在主程序处理完毕后再打开  //
			TH0=0;
			TL0=0;
			bNum=0;
			gbKeyHold=0;
			while(1){
				CS=0;
				iTemp=(TH0<<8)+TL0;
				if(iTemp>SYN_MAX+200)	// 等待超时 30 毫秒左右结束 //
					break;
				CS=1;
				if(INT0)
					continue;
				iTemp=(TH0<<8)+TL0;
				TH0=0;
				TL0=0;				
				if((iTemp>SYN_MIN)&&(iTemp<SYN_MAX))
					;	// 寻找同步位 //
				else if((iTemp>SHORT_MIN)&&(iTemp<LONG_MAX))
					;
				else if(++bNum>20)
					break;
				CS=1;
				while(!INT0);
			}
			if((gbAddrValue==0xFF)&&(gbKeyHold>200)){	// 超过两秒视为长按键 //
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
		if(gaSerBuf[i]==gbAddrValue){ //如果查到,表示结束服务//
			if(gbMode==1){
				for(j=i;j<gbSerNum-1;j++){
					gaSerBuf[j]=gaSerBuf[j+1];
					giSerTime[j]=giSerTime[j+1];
				}
				gbSerNum--;
				fDisFlash=1;	// 禁止中断T2 中刷新显示 //
				DispOut(HexToBcd(gbAddrValue));
				DOTH=1;
				DOTL=1;
				PlaySound(SND_DEL,gbVolum);
				DOTH=0;
				DOTL=0;
				fDisFlash=0;			
				return 0;
			}
			else{	// 自动取消模式,按键后,重新设置超时计数器 //
				giSerTime[i]=0;
				fDisFlash=1;	// 禁止中断T2 中刷新显示 //
				DispOut(HexToBcd(gbAddrValue));
				PlaySound(SND_NEW,gbVolum);
				fDisFlash=0;	// 禁止中断T2 中刷新显示 //
				return 1;
			}
		}
	}
	if(gbSerNum<SER_MAX){
		gaSerBuf[gbSerNum]=gbAddrValue;
		giSerTime[gbSerNum]=0;
		gbSerNum++;
		fDisFlash=1;	// 禁止中断T2 中刷新显示 //
		DispOut(HexToBcd(gbAddrValue));
		PlaySound(SND_NEW,gbVolum);
		fDisFlash=0;	// 禁止中断T2 中刷新显示 //
		return 1;
	}
	// 如果满了,则不予处理, 闪烁显示"FU" //
	PlaySound(SND_DEL,gbVolum);
	fDisFlash=1;	// 禁止中断T2 中刷新显示 //
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
	if(gbDataValue==0x82){	// 长按小键,作为功能选择 //
		if(fQueryPara){
			fQueryPara=0;
		}
		else if(!fSetPara){
			fQueryPara=1;	// 查询参数 //
			DispOut(0xC0);
			gbFunc=0;
		}
	}
	else if(gbDataValue==0x86){	//两个键同时按下//
		if(fSetPara){
			fSetPara=0;
			P0=0;
			P2=0;
		}
		else if(!fQueryPara){
			fSetPara=1;
			DispLeft(0x01);	// 显示 1. //
			gbFunc=0;
			DOTH=1;
			gbParaData=xReadByte(EE_VOL);
			DispRight(gbParaData);				
		}						
	}
	else if(fQueryPara){
		if(gbDataValue==0x02){
			DispLeft(0x0C);	// 显示 C //
			if(++gbFunc>=MAX_FUNC)
				gbFunc=0;
			DispRight(gbFunc);
		}
		else if(gbDataValue==0x04){ // 大键作为确认键 //
			if(gbFunc==0){
				DispVer();
			}
			else if(gbFunc==1){
				DispTotal(EE_RESET,3);
			}
			else if(gbFunc==2){
				DispTotal(EE_TOTAL,4);
			}
			DispLeft(0x0C);	// 显示 C //
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
		else if(gbDataValue==0x04){ // 大键作为确认键 //
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

// 硬件电路对口线反相,上跳沿触发 //
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
	fPulse=1;		// 1 表示 INT0=0, 对应的编码输出脚为 1 //
	fValid=0;
	while(!INT0){CS=0;CS=1;};
	TL0=0;
	TH0=0;
	TR0=1;
	TMOD=0x19;  	//定时器1 定时器0工作在方式1(全16位定时器), T0 通过INT0控制 //
	gbPtRx=0;
	giRxBuf[0]=gbPtRx;
	while(1){
		CS=0;
		iTemp=(TH0<<8)+TL0;
		if(iTemp>PULSE_TIME_OUT)	// 等待超时 20 毫秒左右结束 //
			break;
		CS=1;
		INT0=1;
		if(INT0)	// 如果INT0=1,正在计数 //
			continue;
		// INT0=0, 停止计数,取脉冲宽度 //
		iTemp=(TH0<<8)+TL0;
		TL0=0;
		TH0=0;
		if(fValid)				// 如果已经解码成功,则不必再处理,等待客户松开按键 //
			continue;
		giRxBuf[0]=gbPtRx;
		giRxBuf[gbPtRx++]=iTemp;
		if(gbPtRx>=MAX_RX)
			gbPtRx=1;
		// 检测低脉冲低电平时间, 用于检测同步位, 同步位长度大约为 11 ms //
		if((iTemp>SYN_MIN)&&(iTemp<SYN_MAX)){	// 寻找同步位 //
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
						fRxStart=0; //收到错误脉冲,退出中断,等待下次同步 //
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
						// 不必检测两次相同,否则短按键不响应,实际测试,检测1次足以,测试1000次从未出现过误码 //
//						if((gbRxAddrBak==gbRxAddr)&&(gbRxDataBak==gbRxData)){//						if((gbRxAddrBak==gbRxAddr)&&(gbRxDataBak==gbRxData)){
						fValid=1;
//						}
//						else{
						gbRxAddrBak=gbRxAddr;
						gbRxDataBak=gbRxData;
//						}
						gbKeyHold=0;
						INT0=0;		// 禁止硬件产生中断, 在主程序处理完毕后再打开  //
						IE0=0;	// 清中断标志 //
						EX0=0;						
						break;
					}
				}
			}
			else{
				fRxStart=0;
				break;	//收到错误脉冲,退出中断,等待下次同步 //
			}
		}
		while(!INT0){CS=0;CS=1;}
	}
	fRxStart=0;
	if(fValid&&(gbRxAddrBak>0)){	// 将解码的地址和数据存入 gaADBuf[] //
		gaADBuf[2*gbWrite]=gbRxAddrBak;
		if(gbKeyHold>200){	// 超过两秒视为长按键 //
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
	gbStep=0;	// 启动发送 //
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

// 10ms定时中断, 9.998ms //
void IntTime2(void) interrupt 5 using 3
{
	byte bTemp,i,j;
	
	TF2=0;	// 必须软件清零 //
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
		if(bTemp==0XFF){	//终止符
			TR1=0;
			ET1=0;
			TXD=0;
			PT2=1;
			//EX0=1; 发声时,EX0已经为0,不能开启 //
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
		PT1=1;	// 设置 T1 定时器优先级最高 //
		PT2=0;
		TR1=1;
		ET1=1;
		//EX0=0;	发声时,EX0已经为0 //
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
	else if(gbStep==2){//停止阶段
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
		// 如果请求服务的人数多余1个,则每隔1秒轮流显示,一轮完毕后停1秒,周而复始 //
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
*	函数名: AddValue()
*	功  能: 统计值加1
*	输  入: 无
*	输  出: 无
*
******************************************************************************/
void AddValue(byte bAddr,byte bDigit)
{
	byte aBuf[6];
	byte i,bTemp;
	
	if(bDigit>6)
		return;
	//先读出统计值, 0x99,0x99,0x99 //
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
	
	//先读出统计值, 0x99,0x99,0x99 //
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
	
	
	//先读出统计值, 0x99,0x99,0x99 //
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
	
	xWriteStatusReg();	// 设置看门狗开启 //
	for(j=0;j<3;j++){	//因为统计参数较重要, 因此比较3次,只有3次均不相同才初始化EEPROM //
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
	TMOD=0x19;      //定时器1 定时器0工作在方式1(全16位定时器)	      									/* time0 mode 1 */
	InitIO();	
	DispClr();
	InitEEProm();
	
	gbVolum=xReadByte(EE_VOL);
	gbMode=xReadByte(EE_MODE);
	gbCancel=xReadByte(EE_CANCEL);
	gbTone=xReadByte(EE_TONE);
	
	AddValue(EE_RESET,3);	//每复位一次加1//
//	DispVer();	//显示版本号更新日期//

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
	
	fDisFlash=1;	// 禁止每秒显示数字 //
	Delay10ms(50);
//	PlaySound(SND_DEL,1);
	DispTotal(EE_RESET,3);	//显示复位次数//
	Delay10ms(50);
//	PlaySound(SND_DEL,1);
	DispTotal(EE_TOTAL,4);	//显示接收到有效按键次数//
	fDisFlash=0;	// 必须开放 //
	EX0=1;	
}


/******************************************************************************
*
*	函数名: GetData()
*	功  能: 读取接收到的数据
*	输  入: 无
*	输  出: 无
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
*	显示部分
*
******************************************************************************/

/******************************************************************************
*
*	函数名: DispOut()
*	功  能: 显示两位数字
*	输  入: 无
*	输  出: 无
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
*	函数名: DispClr()
*	功  能: 清屏幕
*	输  入: 无
*	输  出: 无
*
******************************************************************************/
void DispClr(void)
{
	P2=0x00;
	P0=0x00;
}


/******************************************************************************
*
*	x25325 相关部分
*
******************************************************************************/

/******************************************************************************
*
*	函数名: xShiftOut()
*	功  能: 发8bit数据
*	输  入: 无
*	输  出: 无
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
*	函数名: xShiftIn()
*	功  能: 接收8bit数据
*	输  入: 无
*	输  出: 返回接收到的数据
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
*	函数名: xWriteEnable()
*	功  能: 打开写保护
*	输  入: 无
*	输  出: 无
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
*	函数名: xWait()
*	功  能: 判忙
*	输  入: 无
*	输  出: 无
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
*	函数名: xWriteStatusReg()
*	功  能: 写状态寄存器,设置看门狗周期
*	输  入: 无
*	输  出: 无
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
*	函数名: xReadStatusReg()
*	功  能: 读状态寄存器
*	输  入: 无
*	输  出: 返回状态寄存器的值
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
*	函数名: xWriteByte()
*	功  能: 写一字节数据
*	输  入: addr: 地址, value:待写的数据
*	输  出: 无
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
*	函数名: xWriteByte()
*	功  能: 读一字节数据
*	输  入: addr: 地址
*	输  出: 读出的数据
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