/*by xzp21st 2009.5 Email:tyter1223@163.com*/

#include "c8051f340.h"
#include "spi.h"
#include "uart.h"
#include "sd.h"
#include "mcu_init.h"
#include "diskio.h"
#include "ff.h"

BYTE buff[1024]; /*working buffer*/

DWORD get_fattime (void)
{
	DWORD tmr = 0;
	return tmr;
}

/*测试函数*/
void test_readSingleBlock(unsigned long);
void test_writeSingleBlock(unsigned long);
void test_readMultipleBlock(unsigned long, unsigned char);
void test_writeMultipleBlock(unsigned long, unsigned char);
void test_FileWR();

/*------------------------------------------------------------------------------------*/
/*main*/
int main(void)
{
	PCA0MD &= ~0x40; /*关内部看门狗*/

	Init_Devices();

	CLS;/*windows超级终端清屏*/

	transmitString("***********************************\r\n");
	transmitString(" xzp21st's microSD Card Testing..\r\n");
	transmitString("***********************************\r\n");
	
	if(disk_initialize(0))
		transmitString("\r\nSD card init fail!\r\n");
	else
	{
		transmitString("\r\nSD card init success!\r\n");
		
		/*单块读写测试*/
		test_readSingleBlock(0);
		test_writeSingleBlock(10000);
		
		/*多块读写测试*/
		test_readMultipleBlock(0, 2);
		test_writeMultipleBlock(10000, 2);
		
		/*文件读写测试*/
		test_FileWR();
	}

	while(1)
	{	
		transmitString("|/-\\\r");
		delay(1000);
		transmitString("/-\\|\r");
		delay(1000);
		transmitString("-\\|/\r");
		delay(1000);
		transmitString("\\|/-\r");
		delay(1000);
		transmitString("|/-\\\r");
		delay(1000);
	}

	return 0;
}

/*------------------------------------------------------------------------------------*/
/*for test*/

/*测试单块读操作*/
void test_readSingleBlock(unsigned long block_addr)
{
	if(disk_read(0, buff, block_addr, 1)) /*读第block_addr块*/
		transmitString("\r\nRead single block operation fail!!!\r\n");
	else
	{
		transmitString("\r\nRead single block operation success~~~\r\n");
		transmitString("\r\nPress any key to display the data~~~\r\n");
		receiveByte();
		CLS;
		displayData(buff, 512);
	}
}

/*测试单块写操作*/
void test_writeSingleBlock(unsigned long block_addr)
{
	int i = 0;
	for(i=0; i<512; i++)
	{
		buff[i] = i+2;
	}
	
	if(disk_write(0, buff, block_addr, 1)) /*向第block_addr块写数据*/
		transmitString("\r\nWrite single block operation fail!!!\r\n");
	else /*写成功*/
	{	
		transmitString("\r\nWrite single block operation success~~~\r\n");
		test_readSingleBlock(block_addr);
	}
}

/*测试多块读操作*/
void test_readMultipleBlock(unsigned long block_addr, unsigned char count)
{
	if(disk_read(0, buff, block_addr, count)) /*读从第num块开始的两个块*/
		transmitString("\r\nRead multiple blocks operation fail!!!\r\n");
	else /*读多块成功*/
	{
		transmitString("\r\nRead multiple blocks operation success~~~\r\n");
		transmitString("\r\nPress any key to display the data~~~\r\n");
		receiveByte();
		CLS;
		displayData(buff, 1024);
	}
}

/*测试多块写操作*/
void test_writeMultipleBlock(unsigned long block_addr, unsigned char count)
{
	int i = 0;
	for(i=0; i<1024; i++)
	{
		buff[i] = i+1;
	}
	
	if(disk_write(0, buff, block_addr, count))
		transmitString("\r\nWrite multiple blocks operation fail!!!\r\n");
	else /*写多块成功*/
	{	
		transmitString("\r\nWrite multiple blocks operation success~~~\r\n");
		test_readMultipleBlock(block_addr, count);
	}
}

/*测试文件读写操作,文件大小为512字节*/
void test_FileWR()
{
	FATFS fs;       /*Work area (file system object) for logical drive*/
    FIL file;      /*file objects*/
	UINT bw, br;    /*File R/W count*/


	int i = 0;
	for(i=0; i<512; i++)
	{
		buff[i] = i+1;
	}	
	/*Register a work area for logical drive 0*/
    f_mount(0, &fs);

    /*Create file*/
    if(f_open(&file, "test.dat", FA_CREATE_ALWAYS | FA_WRITE | FA_READ))
		transmitString("\r\nFile creat fail!\r\n");
	else
	{	
		transmitString("\r\nFile creat success!\r\n");

		if(f_write(&file, buff, 512, &bw))
			transmitString("\r\nFile write fail!\r\n");
		else
		{	
			transmitString("\r\nFile write success!\r\n");
			transmitString("\r\nPress any key to read the file!\r\n");
			receiveByte();
			if(f_read(&file, buff, 512, &br))
				transmitString("\r\nFile read fail!\r\n");
			else
			{	
				transmitString("\r\nFile read success!\r\n");
				transmitString("\r\nPress any key to display the data!\r\n");
				receiveByte();
				displayData(buff, 512);
			}
		}
		/*Close all files*/
   	 	f_close(&file);
	}
	
    /*Unregister a work area before discard it*/
    f_mount(0, 0);
}