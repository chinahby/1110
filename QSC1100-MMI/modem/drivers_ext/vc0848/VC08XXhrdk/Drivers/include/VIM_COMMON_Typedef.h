/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_COMMON_Typedef.h
 * [Description]        : 公共文件，
 *                       	    一些公共定义的部分。
 *				
 * [Author]                 : angela	
 * [Date Of Creation]  : 2007-03-10 16:01
 * [Platform]               : 平台无关
 * [Note]                 : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        			Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2007-03-10  		angela      		 Created
 *******************************************************************************/
/*******************************************************************************
 * This source code has been made available to you by VIMICRO on an
 * AS-IS basis. Anyone receiving this source code is licensed under VIMICRO
 * copyrights to use it in any way he or she deems fit, including copying it,
 * modifying it, compiling it, and redistributing it either with or without
 * modifications. Any person who transfers this source code or any derivative
 * work must include the VIMICRO copyright notice and this paragraph in
 * the transferred software.
 *******************************************************************************/

#ifndef _VIM_5XRDK_COMMON_TYPEDEF_H_
#define _VIM_5XRDK_COMMON_TYPEDEF_H_

#ifdef __cplusplus
extern "C" {
#endif




#define TSTINT(flg, type) (flg & (0x1 << type)) 


#ifndef SINT8
#define SINT8 signed char
#endif

//unsigned 8-bit integer
#ifndef UINT8
#define UINT8 unsigned char
#endif

//signed 16-bit integer
#ifndef SINT16
#define SINT16 short
#endif

//unsigned 16-bit integer
#ifndef UINT16
#define UINT16 unsigned short
#endif

//signed 32-bit integer
#ifndef SINT32
#define SINT32 int
#endif

//unsigned 32-bit integer
#ifndef UINT32
#define UINT32 unsigned int
#endif

#ifndef SINT64
#define SINT64 signed long long
#endif

#ifndef UINT64
#define UINT64 unsigned long long
#endif

#ifndef INT32
#define INT32 int
#endif

#ifndef INT16
#define INT16 short
#endif

#ifndef BOOL
#define BOOL unsigned char
#endif

#ifndef BITDEF
#define BITDEF
enum BITDEFINEenum
{
	BIT0	=	(1 << 0),
	BIT1	=	(1 << 1),
	BIT2	=	(1 << 2),
	BIT3	=	(1 << 3),
	BIT4	=	(1 << 4),
	BIT5	=	(1 << 5),
	BIT6	=	(1 << 6),
	BIT7	=	(1 << 7),
	BIT8	=	(1 << 8),
	BIT9	=	(1 << 9),
	BIT10	=	(1 << 10),
	BIT11	=	(1 << 11),
	BIT12	=	(1 << 12),
	BIT13	=	(1 << 13),
	BIT14	=	(1 << 14),
	BIT15	=	(1 << 15),
	BIT16	=	(1 << 16),
	BIT17	=	(1 << 17),
	BIT18	=	(1 << 18),
	BIT19	=	(1 << 19),
	BIT20	=	(1 << 20),
	BIT21	=	(1 << 21),
	
	BIT28	=	(1 << 28)

	
};
#endif


#ifndef TRUE
#define TRUE		1
#endif

#ifndef FALSE
#define FALSE	0
#endif


#ifndef NULL
#define  NULL	0
#endif

#ifndef ENABLE
#define ENABLE	1
#endif

#ifndef DISABLE
#define  DISABLE	0
#endif


typedef struct tag_TSize
{	
	UINT16 cx;
	UINT16 cy;
} TSize, *PTSize;

typedef struct tag_TPoint
{	
	UINT16 x;
	UINT16 y;
} TPoint, *PTPoint;

typedef struct tag_TRect
{	
	TPoint ltp;
	TSize  size;
} TRect, *PTRect;

typedef struct tag_TClip
{	
	TPoint ltp;
	TPoint rbp;
} TClip, *PTClip;

typedef struct tag_TReg {
	UINT32 adr;					//register address
	UINT32 val;					//register setting value
	UINT16 wait;				//ms
} TReg, *PTReg;

typedef struct tag_TRegGroup {
	UINT32 len;					/* register list length */
	TReg *regval;				/* registers array point */	
} TRegGroup, *PTRegGroup;

typedef struct tag_TDataBuf
{
	UINT32		Len;			//buffer length 
	UINT32		Pos;			//current data point
	UINT8		*pData;			//data pointer 
} TDataBuf, *PTDataBuf;

#define VIM_MemCpy 	 	memcpy
#define VIM_MemSet 	 	memset

#ifdef __cplusplus
}
#endif

#endif 
