/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	media_typedef.h 
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/12/31
* Last modify : Amanda Deng 2007-xx-xx     
*
* version 0.1: 
*	define all our data type 
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#ifndef _MEDIA_TYPE_DEF_
#define _MEDIA_TYPE_DEF_

///////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
	typedef __int64 				MP_S64;
	typedef unsigned __int64 		MP_U64;
	typedef __int64 				MR_S64;
	typedef unsigned __int64 		MR_U64;
#else
	typedef long long 				MP_S64;
	typedef unsigned long long 		MP_U64;
	typedef long long 				MR_S64;
	typedef unsigned long long 		MR_U64;
#endif
	
typedef int 					MP_S32;
typedef short 				MP_S16;
typedef char 					MP_S08;
typedef unsigned int 			MP_U32;
typedef unsigned short 		MP_U16;
typedef unsigned char 			MP_U08;
typedef unsigned short* 		MP_PU16;
typedef unsigned int* 			MP_PU32;
typedef short* 				MP_PS16;
typedef int* 					MP_PS32;		
	
typedef const unsigned char 	MP_CU08;	
typedef const char 		 	MP_CS08;	
typedef unsigned char* 	 	MP_PU08;	
typedef char* 			 	MP_PS08;
typedef const unsigned char* 	MP_PCU08;	
typedef const char* 			MP_PCS08;	

typedef void 					MP_VOID;
typedef void* 				MP_PVOID;
typedef char					MP_CHAR;

typedef volatile unsigned int 	MP_V32;
typedef volatile unsigned char 	MP_V08;

typedef int 					MR_S32;
typedef short 				MR_S16;
typedef char 					MR_S08;

typedef unsigned int 			MR_U32;
typedef unsigned short 		MR_U16;
typedef unsigned char 			MR_U08;

typedef unsigned short* 		MR_PU16;
typedef unsigned int* 			MR_PU32;
typedef short* 				MR_PS16;
typedef int* 					MR_PS32;	
	
typedef const unsigned char 	MR_CU08;	
typedef const char 		 	MR_CS08;	
typedef unsigned char* 	 	MR_PU08;	
typedef char* 			 	MR_PS08;
typedef const unsigned char* 	MR_PCU08;	
typedef const char* 			MR_PCS08;	

typedef void 					MR_VOID;
typedef void* 				MR_PVOID;
typedef char					MR_CHAR;

typedef volatile unsigned int 	MR_V32;
typedef volatile unsigned char 	MR_V08;

#ifndef BOOL
#define BOOL int
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

///////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif
///////////////////////////////////////////////////////////////////////////////

#endif //_MEDIA_TYPE_DEF_

