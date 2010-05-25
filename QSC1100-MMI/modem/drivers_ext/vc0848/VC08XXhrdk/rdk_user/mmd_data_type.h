/*============================================================================================================*/
/*    All rights reserved, Copyright (C) vimicro LIMITED 2003-2004 */
/*----------------------------------------------------------------------------------------------------------*/
/*! @file    COMMON_typedef.h                                                                                     
    @brief   common typedef file                                                                             
    @author Hideaki MIYASHITA
*/
/*============================================================================================================*/

#ifndef _MMD_DATA_TYPE_H_
#define _MMD_DATA_TYPE_H_
#include "stdio.h"
typedef int                 MMD_S32;
typedef unsigned int                MMD_U32;
typedef short               MMD_S16;
typedef unsigned short              MMD_U16;
typedef char                    MMD_S08;
typedef unsigned char               MMD_U08;
typedef volatile int            MMD_VS32;
typedef volatile unsigned int       MMD_VU32;
typedef volatile short      MMD_VS16;
typedef volatile unsigned short     MMD_VU16;
typedef volatile char       MMD_VS08;
typedef volatile unsigned char      MMD_VU08;
typedef char			MMD_CHAR;

typedef struct {
	unsigned char second;
	unsigned char minute;
	unsigned char hour;
}MMD_TIME;

typedef struct {
	unsigned char day;
	unsigned char month;
	unsigned short year;
}MMD_DATE;

//#define DWORD_ALIGN	__align(32)
/***********************************************************
Uart printf defination
***********************************************************/
#define MMDPrintf	printf
#define MMDSPrintf	sprintf

#endif
