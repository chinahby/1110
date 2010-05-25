/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	me_utils.h 
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/12/31
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#ifndef _ME_UTILS_
#define _ME_UTILS_


///////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
///////////////////////////////////////////////////////////////////////////////
#include "me_comhead.h"
#include "mmd_data_type.h"
#include "mmd_mm.h"
//#define _WIN32

#ifdef _WIN32
#else
extern void   uart_printf(char *fmt,...);
void RM_Malloc_Init(void);
void RM_Malloc_Release(void);
void* RM_Malloc(unsigned int size);
void RM_Free(void* pBuf);
unsigned int RM_Malloc_Free_Size(void);

#endif

#ifndef 	me_printf
#define 	me_printf MMDPrintf
#endif 

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						no use function define for memory. Amanda Deng
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
ME_PS08		me_memcpy(ME_PS08 str_dst, ME_PCS08 str_src, ME_U32 len);
ME_PVOID	me_malloc(ME_U32 size);
ME_PVOID	me_calloc(ME_U32 num, ME_U32 size);
ME_VOID		me_free(ME_PVOID buf);
ME_VOID		me_memset(ME_PU08 buf, ME_U08 val, ME_U32 size);
ME_U32		me_strlen(ME_PU08 str);
ME_U32		me_abs(ME_U32 value);
ME_S32		me_cmp(ME_PU08 str_dst, ME_PU08 str_src, ME_U32 len);
ME_PS08		me_move(ME_PS08 str_dst, ME_PCS08	str_src, ME_U32 len);

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						macro define for debug. Amanda Deng
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
#define 		MEDIA_ENGINE_MALLOC				0
#define 		MEDIA_ENGINE_MESSAGE				1
#define 		MEDIA_ENGINE_ERROR				1
#define 		MEDIA_ENGINE_DEBUG				1
#define 		MEDIA_ENGINE_CHECK				0

#if	MEDIA_ENGINE_MALLOC
#define 		ME_MALLOC_MSG(str)			{me_printf("\nME_MALLOC:	");me_printf str;}
#define 		ME_FREE_MSG(str)			{me_printf("\nME_FREE:	");me_printf str;}
#else
#define 		ME_MALLOC_MSG(str)	
#define 		ME_FREE_MSG(str)	
#endif

#if MEDIA_ENGINE_MESSAGE
#define 		ME_MESSAGE(str)			{me_printf("\nME_MESSAGE:	");me_printf str;}
#else
#define 		ME_MESSAGE(str)
#endif

#if MEDIA_ENGINE_ERROR
#define 		ME_ERROR(str)				{me_printf("\nME_ERROR:	");me_printf str;}
#else
#define 		ME_ERROR(str)
#endif

#define 		ME_SPRINTF		sprintf

#if MEDIA_ENGINE_DEBUG
#define 		ME_DEBUG(str)				{me_printf("\nME_DEBUG:	");me_printf str;}
#else
#define 		ME_DEBUG(str)
#endif

#if MEDIA_ENGINE_CHECK
#define 	ME_ASSERT_WHILE(_X_)			{ if(!(_X_)) \
										{	me_printf("\nME_ASSERT_WHILE at %s, line=%d",__FILE__, __LINE__);\
									  			while(1);}}
#define 	ME_ASSERT_BREAK(_X_)			{ if(!(_X_)) \
									 	{	me_printf("\nME_ASSERT_BREAK at %s, line=%d",__FILE__, __LINE__);\
												break;}}
#define 	ME_ASSERT_RETURN(_X_)			{ if(!(_X_)) \
										{	me_printf("\nME_ASSERT_RETURN at %s, line=%d",__FILE__, __LINE__);\
												return;}}
#define 	ME_ASSERT_PARAM_RETURN(_X_)	{ if(!(_X_)) \
									 	{	me_printf("\nME_ASSERT_PARAM at %s, line=%d",__FILE__, __LINE__);\
									 			return ME_ERR_INVALID_PARAMETER;}}
#else
#define 	ME_ASSERT_WHILE(_X_)			//define this for check parameter
#define 	ME_ASSERT_BREAK(_X_)			//define this for check parameter
#define 	ME_ASSERT_RETURN(_X_)			//define this for check parameter
#define 	ME_ASSERT_PARAM_RETURN(_X_)	//define this for check parameter
#endif

#ifndef _WIN32
/*extern void* SD_Malloc(unsigned int size);
extern void  SD_Free(void* pbuf);*/
#define ME_MALLOC			MMD_Malloc//SD_Malloc
#define ME_FREE				MMD_Free//SD_Free
#else
#define ME_MALLOC			malloc
#define ME_FREE				free
#endif
#define ME_VERSION2_FOURHANDLE	1
#if ME_VERSION2_FOURHANDLE
//#define MAX_FILE_HANDLE_NUM		5
#ifndef _WIN32
//#include "..\..\..\v820hrdk\inc\v8_biu.h"
//extern ME_U32 Vm_WrSram(ME_U32 addr, ME_S08 * pbuf, ME_U32 size);
//extern ME_U32 Vm_RdSram(ME_U32 addr, ME_S08 * pbuf, ME_U32 size);
extern ME_U32 VIM_HAPI_WriteVideoRawData(ME_U32 addr, ME_U08 * pbuf, ME_U32 size);
extern ME_U32 V8_Mul16RdSram(ME_U32 addr, ME_S08 * pbuf, ME_U32 size);
extern ME_U32 MMD_SYS_Malloc(ME_U32 size);
extern ME_U32 MMD_SYS_Free(ME_U32 addr);
extern ME_U32 MMD_SYS_FillMem(ME_U32 addr, ME_U32 size);
extern ME_U32 MMD_SYS_FillMemData(MMD_U32 addr, MMD_U32 size,MMD_U32 data);
ME_U32 me_memset_h2d(ME_U32 addr, ME_U32 size);
#define ME_MEMCPY_H2D		VIM_HAPI_WriteVideoRawData
#define ME_MEMCPY_D2H		V8_Mul16RdSram
#define ME_MALLOC_H2D		MMD_SYS_Malloc
#define ME_FREE_H2D		MMD_SYS_Free
#else
#define ME_MEMCPY_H2D		memcpy
#define ME_MEMCPY_D2H		memcpy
#define ME_MALLOC_H2D		malloc
#define ME_FREE_H2D		free
#endif
#endif

///////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif
///////////////////////////////////////////////////////////////////////////////

#endif //_ME_UTILS_

