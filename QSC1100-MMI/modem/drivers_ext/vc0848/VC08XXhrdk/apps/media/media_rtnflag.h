/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	media_rtnflag.h
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/12/31
* Last modify : Amanda Deng 2007-xx-xx     
*
* version 0.1: 
*	define all error information return by media engine interface
*
* NOTE:
* 		because return value maybe a part of one project, so need define base value, if need add the code to
*		any project only modify the base value.
*		our internal return must in 0x0000-----0xffff
*		base value will >= 0x00010000
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#ifndef _MEDIA_RETURN_FLAG_
#define _MEDIA_RETURN_FLAG_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RET_PLY_BASE
#define RET_PLY_BASE							0x00030000
#endif

#define	MP_BASE_RTN							RET_PLY_BASE
/* success and faile macro */
#define	MP_TRUE								0x00000001
#define 	MP_FALSE								0x00000000
/* success and faile macro */
#define	MP_SUCCESS								0x00000000
#define 	MP_FAILE								MP_BASE_RTN+0x00000001
#define 	MP_ERROR								MP_BASE_RTN+0x00000002
#define 	MP_PARAMETER_ERROR					MP_BASE_RTN+0x00000003
#define 	MP_POINT_IS_NULL						MP_BASE_RTN+0x00000004
#define	MP_ERR_FILE_HANDLE_ERROR				MP_BASE_RTN+0x00000005
#define	MP_ERR_FILE_STATUS_ERROR				MP_BASE_RTN+0x00000006

/* about FILE SYSTEM*/
#define	MP_ERR_FS_OPEN_FILE_ERROR			MP_BASE_RTN+0x00000010
#define	MP_ERR_FS_CLOSE_FILE_ERROR			MP_BASE_RTN+0x00000011

/* about MEMORY */
#define	MP_ERR_MALLOC_MEMORY_ERROR			MP_BASE_RTN+0x00000020
#define	MP_ERR_FREE_MEMORY_ERROR			MP_BASE_RTN+0x00000021

/* about PROPERTY */
#define	MP_ERR_PROPERTY_SIZE_ERROR			MP_BASE_RTN+0x00000030

#ifdef __cplusplus
}
#endif

#endif //_MP_RETURN_FLAG_

