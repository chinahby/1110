/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	me_rtnflag.h
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

#ifndef _ME_RETURN_FLAG_
#define _ME_RETURN_FLAG_


///////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
///////////////////////////////////////////////////////////////////////////////
#ifndef 	ME_BASE_RTN							
#define	ME_BASE_RTN							0x00000000
#endif

/* success and faile macro */
#define	ME_TRUE								0x00000001
#define 	ME_FALSE								0x00000000

/* success FILE end*/
#define 	ME_FILE_LAST_OF_AUDIO 				0x00000001
#define 	ME_FILE_LAST_OF_VIDEO 				0x00000001
#define 	ME_FILE_END_OF_VIDEO 					0x00000002
#define 	ME_FILE_END_OF_AUDIO 				0x00000002
/* success and faile macro */
#define	ME_SUCCESS								0x00000000
#define 	ME_FAILE								ME_BASE_RTN+0x00000011
#define 	ME_RTN_ERROR							ME_BASE_RTN+0x00000012

/* about MEMORY */
#define	ME_ERR_MALLOC_MEMORY_ERROR			ME_BASE_RTN+0x00000020
#define	ME_ERR_FREE_MEMORY_ERROR			ME_BASE_RTN+0x00000021

/* about FILE */
#define	ME_ERR_INVALID_PARAMETER 			ME_BASE_RTN+0x00000030
#define ME_ERR_MEMORY_LESS_DATA 			ME_BASE_RTN+0x00000031
#define ME_ERR_READ_FILE_ERROR				ME_BASE_RTN+0x00000032
#define ME_ERR_WRITE_FILE_ERROR				ME_BASE_RTN+0x00000033
#define	ME_ERR_FILE_BE_RUINATE				ME_BASE_RTN+0x00000034
#define	ME_ERR_POINT_IS_NULL 				ME_BASE_RTN+0x00000035
#define	ME_ERR_FILE_IS_UNSUPPORT 			ME_BASE_RTN+0x00000036

/* about register control */
#define	ME_ERR_PARSER_IS_NULL 				ME_BASE_RTN+0x00000040
#define 	ME_ERR_DECODER_IS_NULL 				ME_BASE_RTN+0x00000041
#define 	ME_ERR_ENCODER_IS_NULL				ME_BASE_RTN+0x00000042
#define 	ME_ERR_MUXER_IS_NULL					ME_BASE_RTN+0x00000043
#define	ME_ERR_ID3_IS_NULL					ME_BASE_RTN+0x00000044
#define	ME_ERR_POSTPROC_IS_NULL				ME_BASE_RTN+0x00000045

/* about parser interface */
#define	ME_ERR_PARSER_HAVE_NULL_FUNCTION			ME_BASE_RTN+0x00000100
/* about muxer interface */
#define	ME_ERR_MUXER_HAVE_NULL_FUNCTION			ME_BASE_RTN+0x00000120 
/* about decoder interface */
#define	ME_ERR_DECODER_HAVE_NULL_FUNCTION			ME_BASE_RTN+0x00000130
/* about encoder interface */
#define	ME_ERR_ENCODER_HAVE_NULL_FUNCTION			ME_BASE_RTN+0x00000140
/* about id3 parser interface */
#define	ME_ERR_ID3_HAVE_NULL_FUNCTION				ME_BASE_RTN+0x00000150
/* about id3 parser interface */
#define	ME_ERR_POSTPROC_HAVE_NULL_FUNCTION		ME_BASE_RTN+0x00000160

/* about MEDIA ENGINE CONTROL */
#define	ME_ERR_UNSUPPORT_FILETYPE					ME_BASE_RTN+0x00000201
#define	ME_ERR_UNSUPPORT_DECODER					ME_BASE_RTN+0x00000202
#define	ME_ERR_UNSUPPORT_ENCODER					ME_BASE_RTN+0x00000203
#define	ME_ERR_UNSUPPORT_OUTPUT						ME_BASE_RTN+0x00000204

#define	ME_ERR_HAVE_NO_AUDIO						ME_BASE_RTN+0x00000205
#define	ME_ERR_HVAE_NO_VIDEO							ME_BASE_RTN+0x00000206

#define	ME_ERR_UNSUPPORT_POSTPRC					ME_BASE_RTN+0x00000207
/* about mp4_dec module */
#define 	ME_M4V_NO_INPUT								ME_BASE_RTN+0x00001000
#define 	ME_M4V_NOT_SUPPORT							ME_BASE_RTN+0x00001001

#define	VIDEO_SIZE_RUN_OVER							ME_BASE_RTN+0x00002001

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  以下范围之内的返回值由各个实现parser,decoder,muxer,encoder的同事在
  自己的头文件中定义，不能定义自己范围以内的返回值
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/************ about mp4 file lib src return form 0x000010000 . define by deng naili ************/

/************ about aac  file lib src return form 0x000020000 . define by zhang yin  ************/
/************ about amr file lib src return form 0x000030000 . define by zhang yin ************/

/************ about mp3 file lib src return form 0x000040000  ************/

/************ about wma file lib src return form 0x000050000  ************/

/************ about wav file lib src return form 0x000060000  ************/

///////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif
///////////////////////////////////////////////////////////////////////////////

#endif //_ME_RETURN_FLAG_

