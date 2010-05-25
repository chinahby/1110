/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	me_typedef.h 
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

#ifndef _ME_TYPE_DEF_
#define _ME_TYPE_DEF_


///////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32	 //media engine use or testing on windows system

typedef __int64 				ME_S64;
typedef int 					ME_S32;
typedef short					ME_S16;
typedef char 					ME_S08;
typedef __int64 				ME_U64;
typedef unsigned int 			ME_U32;
typedef unsigned short			ME_U16;
typedef unsigned char 			ME_U08;

typedef unsigned short* 		ME_PU16;
typedef unsigned int* 			ME_PU32;
typedef short*				ME_PS16;
typedef int* 					ME_PS32;	

typedef const unsigned char 	ME_CU08;	
typedef const char			ME_CS08;	
typedef unsigned char*		ME_PU08;	
typedef char*				ME_PS08;
typedef const unsigned char* 	ME_PCU08;	
typedef const char* 			ME_PCS08;	

typedef void 					ME_VOID;
typedef void*				ME_PVOID;

#else	//media engine use or testing on ADS(ARM)
	
typedef long long 				ME_S64;
typedef int 					ME_S32;
typedef short					ME_S16;
typedef char 					ME_S08;
typedef unsigned long long 		ME_U64;
typedef unsigned int 			ME_U32;
typedef unsigned short			ME_U16;
typedef unsigned char 			ME_U08;

typedef unsigned short* 		ME_PU16;
typedef unsigned int* 			ME_PU32;
typedef short*				ME_PS16;
typedef int* 					ME_PS32;	

	
typedef const unsigned char 	ME_CU08;	
typedef const char			ME_CS08;	
typedef unsigned char*		ME_PU08;	
typedef char*				ME_PS08;
typedef const unsigned char* 	ME_PCU08;	
typedef const char* 			ME_PCS08;	

typedef void 					ME_VOID;
typedef void*				ME_PVOID;

#endif /* define other  */

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

#endif //_ME_TYPE_DEF_

