/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	me_keymacro.h
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/12/31
* Last modify : Amanda Deng 2007-xx-xx     
*
* version 0.1: 
*	define all switch macro in this file
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#ifndef _ME_KEY_MACRO_
#define _ME_KEY_MACRO_

///////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

#include "compile_macro.h"

/******************* about hardware driver control *****************************/
#ifndef V0820_MPW_SV2
#define V0820_MPW_SV2				0x00	
#endif

#ifndef V0820_FPGA
#define V0820_FPGA					0x01	
#endif

#ifndef HARDWARE_V0820_VERSION
#define HARDWARE_V0820_VERSION	V0820_FPGA
#endif

#ifndef ID_FPAG
#define ID_FPAG							0	
#endif

#ifndef ID_VC0824
#define ID_VC0824						1	
#endif

#ifndef ID_VC0878
#define ID_VC0878						2		
#endif

#if((CHIP_ID==ID_VC0824)|(CHIP_ID==ID_FPAG))
/******************* about plugin enable *************************************/
#define	ME_KM_PARSER_3GP_EN		0x01 
#define	ME_KM_PARSER_MP4_EN		0x01
#define	ME_KM_PARSER_AMR_EN		0x01
#define	ME_KM_PARSER_AAC_EN		0x01
#define	ME_KM_PARSER_MP3_EN		0x01
#define	ME_KM_PARSER_AVI_EN		0x00 	//added by dnn 2007/8/21
#define	ME_KM_PARSER_FLAC_EN		0x00
#define	ME_KM_PARSER_RM_EN		0x00
#define	ME_KM_PARSER_WAV_EN		0x01 
#define	ME_KM_PARSER_WMA_EN		0x00 
#define	ME_KM_PARSER_APE_EN		0x00
#define	ME_KM_PARSER_OGG_EN		0x00
#define	ME_KM_PARSER_JPG_EN		0x01
#define	ME_KM_PARSER_MIDI_EN		0x01
#define	ME_KM_PARSER_GIF_EN		0x00
#define	ME_KM_PARSER_PNG_EN		0x00

#define	ME_KM_MUXER_3GP_EN		0x01
#define	ME_KM_MUXER_MP4_EN		0x01
#define	ME_KM_MUXER_JPG_EN		0x00
#define	ME_KM_MUXER_AMR_EN		0x01
#define	ME_KM_MUXER_MP3_EN		0x01//0x00,lily modified
#define	ME_KM_MUXER_WAV_EN		0x00 
#define	ME_KM_MUXER_AAC_EN		0x00

#define	ME_KM_ID3_3GP_EN			0x00
#define	ME_KM_ID3_MP4_EN			0x00
#define	ME_KM_ID3_AMR_EN			0x00
#define	ME_KM_ID3_AAC_EN			0x00
#define	ME_KM_ID3_MP3_EN			0x00
#define	ME_KM_ID3_WAV_EN			0x00 
#define	ME_KM_ID3_WMA_EN			0x00

#define	ME_KM_DECODE_MP4A_EN	0x00 
#define	ME_KM_DECODE_MP4V_EN	0x01
#define	ME_KM_DECODE_S263_EN	0x01
#define	ME_KM_DECODE_SAMR_EN	0x00 
#define	ME_KM_DECODE_AAC_EN		0x00
#define	ME_KM_DECODE_AMR_EN		0x00 
#define	ME_KM_DECODE_MP3_EN		0x00
#define	ME_KM_DECODE_WAV_EN		0x00 
#define	ME_KM_DECODE_APE_EN		0x00
#define	ME_KM_DECODE_OGG_EN		0x00
#define	ME_KM_DECODE_WMA_EN		0x00 
#define	ME_KM_DECODE_FLAC_EN	0x00
#define	ME_KM_DECODE_PCM_EN		0x00
#define	ME_KM_DECODE_JPG_EN		0x01
#define	ME_KM_DECODE_SJPG_EN		0x00
#define	ME_KM_DECODE_RA_EN		0x00
#define	ME_KM_DECODE_RV_EN		0x00
#define	ME_KM_DECODE_MIDI_EN		0x00
#define	ME_KM_DECODE_GIF_EN		0x00
#define	ME_KM_DECODE_PNG_EN		0x00

#define	ME_KM_ENCODE_MP4V_EN	0x01
#define	ME_KM_ENCODE_S263_EN	0x01
#define	ME_KM_ENCODE_JPG_EN		0x01
#define	ME_KM_ENCODE_SAMR_EN	0x00
#define	ME_KM_ENCODE_AMR_EN		0x00
#define	ME_KM_ENCODE_MP3_EN		0x00
#define	ME_KM_ENCODE_MP4A_EN	0x00 
#define	ME_KM_ENCODE_AAC_EN		0x00

#define	ME_AUDIO_POSTPROC_ENABLE	0x00
#define	ME_KM_POSTPRC_3D_EN			0x00
#define	ME_KM_POSTPRC_EQ_EN			0x00
#define	ME_KM_POSTPRC_SRC_EN			0x00
#define	ME_KM_POSTPRC_MIR_EN			0x00
#define 	ME_KM_POSTPRC_SLOW_EN		0x00

#define	ME_VIDEO_POSTPROC_ENABLE	0x00
#define	ME_AUDIO_PREPROC_ENABLE		0x00
#define	ME_KM_PREPRC_ONC_EN		       0x00
#define	ME_KM_PREPRC_AGC_EN		       0x00
#define	ME_VIDEO_PREPROC_ENABLE		0x00
#endif

#if(CHIP_ID==ID_VC0878)
/******************* about plugin enable *************************************/
#define	ME_KM_PARSER_3GP_EN		0x01 
#define	ME_KM_PARSER_MP4_EN		0x01
#define	ME_KM_PARSER_JPG_EN		0x01
#define	ME_KM_PARSER_AMR_EN		0x01
#define	ME_KM_PARSER_AAC_EN		0x01
#define	ME_KM_PARSER_MP3_EN		0x01
#define	ME_KM_PARSER_AVI_EN		0x00	//added by dnn 2007/8/21
#define	ME_KM_PARSER_FLAC_EN		0x00
#define	ME_KM_PARSER_RM_EN		0x00
#define	ME_KM_PARSER_WAV_EN		0x01
#define	ME_KM_PARSER_WMA_EN		0x00 
#define	ME_KM_PARSER_APE_EN		0x00
#define	ME_KM_PARSER_OGG_EN		0x00
#define	ME_KM_PARSER_MIDI_EN		0x01
#define	ME_KM_PARSER_GIF_EN		0x00
#define	ME_KM_PARSER_PNG_EN		0x00

#define	ME_KM_MUXER_3GP_EN		0x01
#define	ME_KM_MUXER_MP4_EN		0x01
#define	ME_KM_MUXER_JPG_EN		0x01
#define	ME_KM_MUXER_AMR_EN		0x01
#define	ME_KM_MUXER_MP3_EN		0x01//0x00,lily modified
#define	ME_KM_MUXER_WAV_EN		0x00 
#define	ME_KM_MUXER_AAC_EN		0x00

#define	ME_KM_ID3_3GP_EN			0x00
#define	ME_KM_ID3_MP4_EN			0x00
#define	ME_KM_ID3_AMR_EN			0x00
#define	ME_KM_ID3_AAC_EN			0x00
#define	ME_KM_ID3_MP3_EN			0x01  //modify by yjz 2009/2/13
#define	ME_KM_ID3_WAV_EN			0x00
#define	ME_KM_ID3_WMA_EN			0x00

#define	ME_KM_DECODE_MP4A_EN	0x00
#define	ME_KM_DECODE_MP4V_EN	0x01
#define	ME_KM_DECODE_S263_EN	0x01
#define	ME_KM_DECODE_SAMR_EN	0x00
#define	ME_KM_DECODE_JPG_EN		0x01
#define	ME_KM_DECODE_SJPG_EN		0x00
#define	ME_KM_DECODE_AAC_EN		0x00
#define	ME_KM_DECODE_AMR_EN		0x00
#define	ME_KM_DECODE_MP3_EN		0x00
#define	ME_KM_DECODE_WAV_EN		0x00
#define	ME_KM_DECODE_APE_EN		0x00
#define	ME_KM_DECODE_OGG_EN		0x00
#define	ME_KM_DECODE_WMA_EN		0x00 
#define	ME_KM_DECODE_FLAC_EN	0x00
#define	ME_KM_DECODE_PCM_EN		0x00
#define	ME_KM_DECODE_RA_EN		0x00
#define	ME_KM_DECODE_RV_EN		0x00
#define	ME_KM_DECODE_MIDI_EN		0x00
#define	ME_KM_DECODE_GIF_EN		0x00
#define	ME_KM_DECODE_PNG_EN		0x00

#define	ME_KM_ENCODE_MP4V_EN	0x01
#define	ME_KM_ENCODE_S263_EN	0x01
#define	ME_KM_ENCODE_JPG_EN		0x01
#define	ME_KM_ENCODE_SAMR_EN	0x00
#define	ME_KM_ENCODE_MP3_EN		0x00
#define	ME_KM_ENCODE_AMR_EN		0x00
#define	ME_KM_ENCODE_MP4A_EN	0x00
#define	ME_KM_ENCODE_AAC_EN		0x00

#define	ME_AUDIO_POSTPROC_ENABLE	0x00  //modified by yjz
#define	ME_KM_POSTPRC_3D_EN			0x00
#define	ME_KM_POSTPRC_EQ_EN			0x00
#define	ME_KM_POSTPRC_SRC_EN			0x00  
#define	ME_KM_POSTPRC_MIR_EN			0x00
#define 	ME_KM_POSTPRC_SLOW_EN		0x00

#define	ME_VIDEO_POSTPROC_ENABLE	0x00
#define	ME_AUDIO_PREPROC_ENABLE		0x00
#define	ME_KM_PREPRC_ONC_EN		       0x00
#define	ME_KM_PREPRC_AGC_EN		       0x00
#define	ME_VIDEO_PREPROC_ENABLE		0x00
#endif

/******************* about debug info output *********************************/
#define 	ME_KM_PRINT 				0x01	/* output information */
#define	ME_KM_FUNNAME			0x00	/* output function's name */

/******************* about stack enable or not *******************************/
#define	ME_KM_OBJ_STACK			0x01	/* 1->object is used as global variables, 0->malloc */

/******************************* about PROPERTY TYPE define*******************************************/
#ifndef VIM_MEDIA_FILE_PROPERTY_TITLE
#define VIM_MEDIA_FILE_PROPERTY_TITLE 			"title"		// title property    is a string, 0<=length(sTitle)<=256
#endif

#ifndef VIM_MEDIA_FILE_PROPERTY_ARSTIST
#define VIM_MEDIA_FILE_PROPERTY_ARSTIST 		"artist"		// artist property  is a string, 0<=length(sArtist)<=256
#endif

#ifndef VIM_MEDIA_FILE_PROPERTY_ALBUM
#define VIM_MEDIA_FILE_PROPERTY_ALBUM 		"album" 		// album property is a string, 0<=length(sAlbum)<=256  
#endif

#ifndef VIM_MEDIA_FILE_PROPERTY_YEAR
#define VIM_MEDIA_FILE_PROPERTY_YEAR 			"year"		// year property   is a string, 0<=length(sYear)<=256
#endif

#ifndef VIM_MEDIA_FILE_PROPERTY_COMMENT
#define VIM_MEDIA_FILE_PROPERTY_COMMENT 		"comment"	// comment property is a string, 0<=length(sComment)<=256
#endif

#ifndef VIM_MEDIA_FILE_PROPERTY_GENRE
#define VIM_MEDIA_FILE_PROPERTY_GENRE 		"genre"		// genre property  is a string, 0<=length(sGenre)<=256
#endif

#ifndef VIM_MEDIA_FILE_PROPERTY_TRACK
#define VIM_MEDIA_FILE_PROPERTY_TRACK 		"track"  		// track property  is a unsigned char, 0<=track<=255 
#endif

#define MAX_VIDEO_FRAMECNT_IN_BUFFER		30

#ifdef __cplusplus
}
#endif
///////////////////////////////////////////////////////////////////////////////

#endif //_ME_KEY_MACRO_




