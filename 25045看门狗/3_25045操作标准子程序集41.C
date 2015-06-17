/*
;程 序 最 后 修 改 时 间  0-4-3 23:43
;软 件 标 题：25045操作标准子程序集41
;软 件 说 明：25045 I2C 串行EEPROM 驱动
;_________________________________________

;原作者:	庞波     
;程序修改人：
;版本号：
;_________________________________________
*/


/*到现在为止所有的问题都已经解决，此版本已经较为完善*/
# include <stdio.h>
# include <reg52.h>
# define uchar unsigned char
# define uint unsigned int
 sbit SO=P1^1;/*25045输出*/
 sbit SI=P1^2;/*25045输入*/
 sbit SCK=P1^3;/*25045时钟*/
 sbit CS=P1^4;/*25045片选*/
 uchar code WREN_INST=0X06;
 /* Write enable latch instruction (WREN)*/
 uchar code WRDI_INST=0X04;
 /* Write disable latch instruction (WRDI)*/
 uchar code WRSR_INST=0X01;
 /* Write status register instruction (WRSR)*/
 uchar code RDSR_INST=0X05;
 /* Read status register instruction (RDSR)*/
 uchar code WRITE_INST=0X02;
 /* Write memory instruction (WRITE)*/
 /*写入25045的先导字,应当为0000A010,其中的A为写入25045的高位地址
 将此WRITE_INST和写入高位地址相或后即为正确的写先导字*/
 uchar code READ_INST=0X03;
 /* Read memory instruction (READ)*/
 /*读出25045的先导字,应当为0000A011,其中的A为读出25045的高位地址
 将此READ_INST和读出高位地址相或后即为正确的读先导字*/
 uint code BYTE_ADDR=0X55;
 /* Memory address for byte mode operations*/
 uchar code BYTE_DATA=0X11;
 /*Data byte for byte write operation*/
 uint  code PAGE_ADDR=0X1F;
 /* Memory address for page mode operations*/
 /*页面写入的其始地址*/
 uchar code PAGE_DATA1=0X22;
 /* 1st data byte for page write operation*/
 uchar code PAGE_DATA2=0X33;
 /* 2nd data byte for page write operation*/
 uchar code PAGE_DATA3=0X44;
 /* 3rd data byte for page write operation*/
 uchar code STATUS_REG=0X20;
 /* Status register,设置DOG时间设置为200毫秒,无写保护*/
 /*这是状态寄存器的值,他的意义在于第5,第4位为WDI1,WDI0代表DOG的时间,00为1.4秒,01为600毫秒,10为200毫秒,00为disabled
 第3位和第2位为BL1,BL0,是写保护设置位,00为无保护,01为保护180-1FF,10为保护100-1FF,11为保护000-1FF.第1位为WEL,
 当他为1时代表已经"写使能"设置了,现在可以写了,只读位.第0位为WIP,当他为1时代表正在进行写操作,是只读*/
 uchar code  MAX_POLL=0x99;
 /* Maximum number of polls*/
 /*最大写过程时间,确定25045的最大的写入过程的时间*/
 uchar code INIT_STATE=0x09;
 /* Initialization value for control ports*/
 uint code SLIC=0x30;
 /* Address location of SLIC*/
 void wren_cmd(void);/*写使能子程序*/
 void wrdi_cmd(void);/*写使能复位*/ 
 void wrsr_cmd(void);/*复位时间位和数据保护位写入状态寄存器*/ 
 uchar rdsr_cmd(void);/*读状态寄存器*/
 void byte_write(uchar aa,uint dd);/*字节写入,aa为写入的数据,dd为写入的地址*/
 uchar byte_read(uint dd);/*字节读出,dd为读出的地址,返回读出的数据*/
 void page_write(uchar aa1,uchar aa2,uchar aa3,uchar aa4,uint dd);/*页写入*/
 void sequ_read(void);/*连续读出*/ 
 void rst_wdog(void);/*DOG复位*/
 void outbyt(uchar aa);/*输出一个字节到25045中，不包括先导字等*/
 uchar inputbyt();/*由25045输入一个字节，不包括先导字等额外的东西*/ 
 void wip_poll(void);/*检查写入过程是否结束*/
 
 
/*25045操作子程序集*/
/*;*******************************************************************************************
*
;* Name: WREN_CMD
;* Description: Set write enable latch
;* Function: This routine sends the command to enable writes to the EEPROM memory array or
;* status register
;* Calls: outbyt
;* Input: None
;* Outputs: None
;* Register Usage: A
;*******************************************************************************************
*/
/*写使能子程序*/
void wren_cmd(void)
{
 uchar aa;
 SCK=0;/* Bring SCK low */
 CS=0;/* Bring /CS low */
 aa=WREN_INST;
 outbyt(aa);/* Send WREN instruction */
 SCK=0;/* Bring SCK low */
 CS=1;/* Bring /CS high */
}

/*;*******************************************************************************************
*
;* Name: WRDI_CMD
;* Description: Reset write enable latch
;* Function: This routine sends the command to disable writes to the EEPROM memory array or
;* status register
;* Calls: outbyt
;* Input: None
;* Outputs: None
;* Register Usage: A
;*******************************************************************************************
*/
/*写使能复位子程序*/
void wrdi_cmd(void)
{
 uchar aa;
 SCK=0;/* Bring SCK low */
 CS=0;/* Bring /CS low */
 aa=WRDI_INST;
 outbyt(aa);/* Send WRDI instruction */
 SCK=0;/* Bring SCK low */
 CS=1;/* Bring /CS high */
}


/*;*******************************************************************************************
*
;* Name: WRSR_CMD
;* Description: Write Status Register
;* Function: This routine sends the command to write the WD0, WD1, BP0 and BP0 EEPROM
;* bits in the status register
;* Calls: outbyt, wip_poll
;* Input: None
;* Outputs: None
;* Register Usage: A
;*******************************************************************************************
*/
/*写状态寄存器子程序*/
void wrsr_cmd(void)
{
 uchar aa;
 SCK=0;/* Bring SCK low */
 CS=0;/* Bring /CS low */
 aa=WRSR_INST;
 outbyt(aa) ;/* Send WRSR instruction */
 aa=STATUS_REG;
 outbyt(aa);/* Send status register */
 SCK=0;/* Bring SCK low */
 CS=1;/* Bring /CS high */
 wip_poll();/* Poll for completion of write cycle */
}





/*;*******************************************************************************************
*
;* Name: RDSR_CMD
;* Description: Read Status Register
;* Function: This routine sends the command to read the status register
;* Calls: outbyt, inputbyt
;* Input: None
;* Outputs: A = status registerXicor Application Note AN21
;* Register Usage: A
;*******************************************************************************************
*/
/*读状态寄存器,读出的数据放入到aa中*/
uchar rdsr_cmd (void)
{
 uchar aa;
 SCK=0;
 CS=0;
 aa=RDSR_INST;
 outbyt(aa);
 aa=inputbyt();
 SCK=0;
 CS=1;
 return aa;
}







/*;*******************************************************************************************
*
;* Name: BYTE_WRITE
;* Description: Single Byte Write
;* Function: This routine sends the command to write a single byte to the EEPROM memory
array
;* Calls: outbyt, wip_poll
;* Input: None
;* Outputs: None
;* Register Usage: A, B
;*******************************************************************************************
*/
/*字节写入,aa为写入的数据,dd为写入的地址,对于25045而言为000-1FF*/
void byte_write(aa,dd)
uchar aa;
uint dd;
{
 SCK=0;
 CS=0;
 outbyt((((uchar)(dd-0XFF))<<3)|WRITE_INST);/* Send WRITE instruction including MSB of address */
 /*将高位地址左移3位与写入先导字相或,得到正确的先导字写入25045*/
 outbyt((uchar)(dd));
 /*输出低位地址到25045*/
 outbyt(aa);
 /*写入数据到25045的对应单元*/
 SCK=0;
 CS=1;
 wip_poll();
 /*检测是否写完*/
}



/*;*******************************************************************************************
*
;* Name: BYTE_READ
;* Description: Single Byte Read
;* Function: This routine sends the command to read a single byte from the EEPROM memory
array
;* Calls: outbyt, inputbyt
;* Input: None
;* Outputs: A = read byte
;* Register Usage: A, BXicor Application Note AN21
;*******************************************************************************************
*/
/*字节读出,其中dd为读出的地址,返回的值为读出的数据*/
uchar byte_read(dd)
uint dd;
{
 uchar cc;
 SCK=0;
 CS=0;
 outbyt((((uchar)(dd-0XFF))<<3)|READ_INST);/* Send READ_INST instruction including MSB of address */
 /*将高位地址左移3位与读出先导字相或,得到正确的先导字写入25045*/
 outbyt((uchar)(dd));
 /*输出低位地址到25045*/
 cc=inputbyt();/*得到读出的数据*/
 SCK=0;
 CS=1;
 return cc;
}




/*;*******************************************************************************************
*
;* Name: PAGE_WRITE
;* Description: Page Write
;* Function: This routine sends the command to write three consecutive bytes to the EEPROM
;* memory array using page mode
;* Calls: outbyt, wip_poll
;* Input: None
;* Outputs: None
;* Register Usage: A, B
;*******************************************************************************************
*/
/*页面写入,其中aa1,aa2,aa3,aa4为需要写入的4个数据(最大也就只能一次写入4个字,dd为写入的首地址*/
void page_write(aa1,aa2,aa3,aa4,dd)
uchar aa1,aa2,aa3,aa4;
uint dd;
{
 SCK=0;
 CS=0;
 outbyt((((uchar)(dd-0XFF))<<3)|WRITE_INST);/* Send WRITE instruction including MSB of address */
 /*将高位地址左移3位与写入先导字相或,得到正确的先导字写入25045*/
 outbyt((uchar)(dd));
 /*写入低位地址到25045*/
 outbyt(aa1);
 /*写入数据1到25045的对应单元*/
 outbyt(aa2);
 /*写入数据2到25045的对应单元*/
 outbyt(aa3);
 /*写入数据3到25045的对应单元*/
 outbyt(aa4);
 /*写入数据4到25045的对应单元*/
 SCK=0;
 CS=1;
 wip_poll();
}




/*;*******************************************************************************************
*
;* Name: SEQU_READ
;* Description: Sequential Read
;* Function: This routine sends the command to read three consecutive bytes from the EEPROM
;* memory array using sequential mode
;* Calls: outbyt, inputbyt
;* Input: None
;* Outputs: A = last byte read
;* Register Usage: A, B
;*******************************************************************************************
*/
/*连续读出,由于函数的返回值只能为1个,对于连续读出的数据只能使用指针作为函数的返回值才能做到返回一系列的数组*/
/*sequ_read:*/
unsigned int *page_read(n,dd)
uchar n;/*n是希望读出的数据的个数，n<=11*/ 
unsigned int dd;/*dd是读出数据的首地址*/
{
 uchar i;
 uchar pp[10];
 unsigned int *pt=pp;
 SCK=0;
 CS=0;
 outbyt((((uchar)(dd-0XFF))<<3)|READ_INST);
 for (i=0;i<n;i++)
 {
   pp[i]=inputbyt();
 }
 return (pt);
}
/*调用的方法如下*/
/*unsigned int *p;*/
/*p=page_read(4,100);*/
/*a=*(p)*/  
/*b=*(p+1)*/
/*c=*(p+2)*/
/*d=*(p+3)*/
/*abcd中存放25045中由100地址开始的4个数据*/
 /* Send WRITE */
/*mov DPTR, #PAGE_ADDR ; Set address of 1st byte to be read
clr sck ; Bring SCK low
clr cs ; Bring /CS low
mov A, #READ_INST
mov B, DPH
mov C, B.0
mov ACC.3, C
lcall outbyt ; Send READ instruction with MSB of address
mov A, DPL
lcall outbyt ; Send low order address byte
lcall inputbyt ; Read 1st data byte
lcall inputbyt ; Read 2nd data byte
lcall inputbyt ; Read 3rd data byte
clr sck ; Bring SCK low
setb cs ; Bring /CS high
ret*/




/*;*******************************************************************************************
*
;* Name: RST_WDOG
;* Description: Reset Watchdog Timer
;* Function: This routine resets the watchdog timer without sending a command
;* Calls: None
;* Input: None
;* Outputs: None
;* Register Usage: None
;*******************************************************************************************
*/
/*复位DOG*/
void rst_wdog (void)
{
 CS=0;
 CS=1;
}





/*;*******************************************************************************************
*
;* Name: WIP_POLL
;* Description: Write-In-Progress Polling
;* Function: This routine polls for completion of a nonvolatile write cycle by examining the
;* WIP bit of the status register
;* Calls: rdsr_cmdXicor Application Note AN21
;* Input: None
;* Outputs: None
;* Register Usage: R1, A
;*******************************************************************************************
*/
/*检测写入的过程是否结束*/
void wip_poll(void)
{
 uchar aa;
 uchar idata my_flag;
 for (aa=1;aa>MAX_POLL;aa++)
 {
  my_flag=rdsr_cmd();
  if ((my_flag&&0x01)==0) {aa=MAX_POLL;}/*判断是否WIP=0,即判断是否写入过程已经结束,若结束就跳出,否则继续等待直到达到最大记数值*/
 }
}




/*;*******************************************************************************************
*
;* Name: OUTBYT
;* Description: Sends byte to EEPROM
;* Function: This routine shifts out a byte, starting with the MSB, to the EEPROM
;* Calls: None
;* Input: A = byte to be sent
;* Outputs: None
;* Register Usage: R0, A
;*******************************************************************************************
*/
/*输出一个数据到25045,此数据可能为地址,先导字,写入的数据等*/
void outbyt(aa)
uchar aa;
{
 uchar my_flag1,i;
 for (i=0;i>7;i++)
 {
   my_flag1=aa;
   SCK=0;
   SI=(my_flag1>>i);
   SCK=1;
 }
 SI=0;/*使SI处于确定的状态*/ 
}






/*;*******************************************************************************************
*
;* Name: INPUTBYT
;* Description: Recieves byte from EEPROM
;* Function: This routine recieves a byte, MSB first, from the EEPROM
;* Calls: None
;* Input: None
;* Outputs: A = recieved byte
;* Register Usage: R0, A
;*******************************************************************************************
*/
/*得到一个数据,此数据可能为状态寄存器数据,读出的单元数据等*/
uchar inputbyt(void)
{
 uchar aa,my_flag;
 char i;
 for (i=7;i<0;i--)
 {
   SCK=0;
   my_flag=(uchar)(SO);
   SCK=1;
   aa=(aa||(my_flag<<i));
   my_flag=0x00;
 }
 return aa;
}

