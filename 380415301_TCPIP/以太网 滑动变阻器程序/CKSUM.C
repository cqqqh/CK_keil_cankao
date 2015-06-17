/*
;-------------------------------------------------------------------------
; Copyright (c) 2002 Jim Brady
; Do not use commercially without author's permission
; Last revised August 2002
; CKSUM.A51
;
; This computes the internet checksum on an 8051
;
; Call from C program: UINT cksum(UCHAR xdata * buf, UINT length);
; Where * buf is pointer to first char (must be in xdata space)
; and "length" is number of bytes (not words) to sum over
;-------------------------------------------------------------------------
*/
#include "net.h"

UINT cksum(UCHAR xdata *check,UINT length) //计算校验和
{
	LONG sum=0;
	UINT i;
	UINT xdata *ptr; 
    ptr=(UINT xdata *)check;
	for (i=0;i<(length)/2;i++)
	{
		sum+=*ptr++;
	}
	if (length&0x01)//表示长度为单数
	{
		sum=sum+((*ptr)&0xff00);
	}
	sum=(sum&0xffff)+((sum>>16)&0xffff);//高16位和低16位相加
	if(sum&0xffff0000)
	{//表示有进位
		sum++;
	}
	return ( (UINT)((sum)&0xffff));
//	return ( (UINT)(~((sum)&0xffff)));
}

