#ifndef _SD_H_
#define _SD_H_

/*ÃüÁîºê*/
#define CMD0	(0x40+0)	/* GO_IDLE_STATE */
#define CMD1	(0x40+1)	/* SEND_OP_COND */
#define ACMD41	(0xC0+41)	/* SEND_OP_COND */
#define CMD8	(0x40+8)	/* SEND_IF_COND */
#define CMD9	(0x40+9)	/* SEND_CSD */
#define CMD10	(0x40+10)	/* SEND_CID */
#define CMD12	(0x40+12)	/* STOP_TRANSMISSION */
#define ACMD13	(0xC0+13)	/* SD_STATUS */
#define CMD16	(0x40+16)	/* SET_BLOCKLEN */
#define CMD17	(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD18	(0x40+18)	/* READ_MULTIPLE_BLOCK */
#define ACMD23	(0xC0+23)	/* SET_WR_BLK_ERASE_COUNT */
#define CMD24	(0x40+24)	/* WRITE_BLOCK */
#define CMD25	(0x40+25)	/* WRITE_MULTIPLE_BLOCK */
#define CMD32	(0x40+32)	/* ERASE_BLOCK_START_ADDR */
#define CMD33	(0x40+33)	/* ERASE_BLOCK_END_ADDR */
#define CMD38	(0x40+38)	/* ERASE_SELECTED_BLOCKS */
#define CMD55	(0x40+55)	/* APP_CMD */
#define CMD58	(0x40+58)	/* READ_OCR */

#define SD_CS_ASSERT     (CS = 0)
#define SD_CS_DEASSERT   (CS = 1)

/*SD¿¨³õÊ¼»¯*/
unsigned char SD_init(void);

/*ÃüÁî·¢ËÍº¯Êý*/
unsigned char SD_sendCommand(unsigned char cmd, unsigned long arg);

/*¶Áµ¥¿é*/
unsigned char SD_readSingleBlock(unsigned char *buff, unsigned long startBlock);

/*Ð´µ¥¿é*/
unsigned char SD_writeSingleBlock(unsigned char *buff, unsigned long startBlock);

/*¶Á¶à¿é*/
unsigned char SD_readMultipleBlock (unsigned char *buff, unsigned long startBlock, unsigned long totalBlocks);

/*Ð´µ¥¿é*/
unsigned char SD_writeMultipleBlock(unsigned char *buff, unsigned long startBlock, unsigned long totalBlocks);

/*¿é²Á³ý*/
unsigned char SD_erase (unsigned long startBlock, unsigned long totalBlocks);

/*¿é½ÓÊÕ*/
static unsigned char receive_DataBlock(unsigned char *buff, unsigned int btr);

/*¿é·¢ËÍ*/
static unsigned char transmit_DataBlock(const unsigned char *buff, unsigned char token);

#endif
