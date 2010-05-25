/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	media_keymacro.h
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2007/05/15
* Last modify : Amanda Deng 2007-xx-xx     
*
* version 0.1: 
*	define all switch macro in this file
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#ifndef _VIM_MEDIA_KEY_MACRO_
#define _VIM_MEDIA_KEY_MACRO_

#ifdef __cplusplus
extern "C" {
#endif

#include "media_typedef.h"				//this file add by amanda deng
/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
/* Macro to make a FOURCC out of four characters */
#ifndef VIM_MEDIA_FOURCC
#define VIM_MEDIA_FOURCC( ch0, ch1, ch2, ch3 )				\
		( (MP_U32)(MP_CHAR)(ch0) | ( (MP_U32)(MP_CHAR)(ch1) << 8 ) |	\
		( (MP_U32)(MP_CHAR)(ch2) << 16 ) | ( (MP_U32)(MP_CHAR)(ch3) << 24 ) )
#endif

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						structor and macro define for media system 
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/

 //Only support to open a fixed number of media
#define VIM_MEDIA_MAX_SUPPORT_FILE_NUMBER    1

/*define file type macros for media open used as in parameter*/
#define VIM_MEDIA_FILE_TYPE_MP3 					VIM_MEDIA_FOURCC('m', 'p', '3', 0) 
#define VIM_MEDIA_FILE_TYPE_AMR 					VIM_MEDIA_FOURCC('a', 'm', 'r', 0)
#define VIM_MEDIA_FILE_TYPE_AAC 					VIM_MEDIA_FOURCC('a', 'a', 'c', 0) 
#define VIM_MEDIA_FILE_TYPE_WAV 					VIM_MEDIA_FOURCC('w', 'a', 'v', 0)
#define VIM_MEDIA_FILE_TYPE_WMA 					VIM_MEDIA_FOURCC('w', 'm', 'a', 0)
#define VIM_MEDIA_FILE_TYPE_MIDI 					VIM_MEDIA_FOURCC('m', 'i', 'd', 0)
#define VIM_MEDIA_FILE_TYPE_APE 					VIM_MEDIA_FOURCC('a', 'p', 'e', 0)  
#define VIM_MEDIA_FILE_TYPE_3GPP 					VIM_MEDIA_FOURCC('3', 'g', 'p', 0)
#define VIM_MEDIA_FILE_TYPE_MPG4 					VIM_MEDIA_FOURCC('m', 'p', '4', 0)
#define VIM_MEDIA_FILE_TYPE_AVI 					VIM_MEDIA_FOURCC('a', 'v', 'i', 0)
#define VIM_MEDIA_FILE_TYPE_RM 					VIM_MEDIA_FOURCC('r', 'm', 0, 0)
#define VIM_MEDIA_FILE_TYPE_JPEG 					VIM_MEDIA_FOURCC('j', 'p', 'g', 0)
#define VIM_MEDIA_FILE_TYPE_BMP 					VIM_MEDIA_FOURCC('b', 'm', 'p', 0)
#define VIM_MEDIA_FILE_TYPE_PNG 					VIM_MEDIA_FOURCC('p', 'n', 'g', 0)
#define VIM_MEDIA_FILE_TYPE_GIF 					VIM_MEDIA_FOURCC('g', 'i', 'f', 0)
#define VIM_MEDIA_FILE_TYPE_FLAC 					VIM_MEDIA_FOURCC('f', 'l', 'a', 'c')
#define VIM_MEDIA_FILE_TYPE_UNKNOWN 				VIM_MEDIA_FOURCC('u', 'n', 'k', 'w')

/*define audio type macros for audio property structer used as in or out parameter*/
#define VIM_MEDIA_AUDIO_TYPE_MP3 					VIM_MEDIA_FOURCC('m', 'p', '3', 0) 
#define VIM_MEDIA_AUDIO_TYPE_AMR 					VIM_MEDIA_FOURCC('a', 'm', 'r', 0)
#define VIM_MEDIA_AUDIO_TYPE_AAC 					VIM_MEDIA_FOURCC('a', 'a', 'c', 0)  
#define VIM_MEDIA_AUDIO_TYPE_WAV 				VIM_MEDIA_FOURCC('w', 'a', 'v', 0)
#define VIM_MEDIA_AUDIO_TYPE_WMA 				VIM_MEDIA_FOURCC('w', 'm', 'a', 0)
#define VIM_MEDIA_AUDIO_TYPE_MIDI 				VIM_MEDIA_FOURCC('m', 'i', 'd', 0)
#define VIM_MEDIA_AUDIO_TYPE_APE 					VIM_MEDIA_FOURCC('a', 'p', 'e', 0)   
#define VIM_MEDIA_AUDIO_TYPE_FLAC 				VIM_MEDIA_FOURCC('f', 'l', 'a', 'c')
#define VIM_MEDIA_AUDIO_TYPE_SAMR 				VIM_MEDIA_FOURCC('s', 'a', 'm', 'r')
#define VIM_MEDIA_AUDIO_TYPE_MP4A 				VIM_MEDIA_FOURCC('m', 'p', '4', 'a')
#define VIM_MEDIA_FILE_TYPE_RA					VIM_MEDIA_FOURCC('r', 'a', 0, 0)

/*define audio type macros for audio property structer used as in or out parameter*/
#define VIM_MEDIA_VIDEO_TYPE_S263					VIM_MEDIA_FOURCC('s', '2', '6', '3')
#define VIM_MEDIA_VIDEO_TYPE_MP4V 				VIM_MEDIA_FOURCC('m', 'p', '4', 'v')
#define VIM_MEDIA_VIDEO_TYPE_JPG 					VIM_MEDIA_FOURCC('j', 'p', 'g', 0)

/*define post process type macros for set post process parameter*/
#define VIM_MEDIA_POSTPROCESS_EQ 					VIM_MEDIA_FOURCC('e', 'q', 0, 0) 
#define VIM_MEDIA_POSTPROCESS_3D 					VIM_MEDIA_FOURCC('3', 'd', 0, 0)
#define VIM_MEDIA_POSTPROCESS_SRC				VIM_MEDIA_FOURCC('s', 'r', 'c', 0)
#define VIM_MEDIA_POSTPROCESS_SLOW 				VIM_MEDIA_FOURCC('s', 'l', 'o', 'w') 
#define VIM_MEDIA_PREPROCESS_ONC					VIM_MEDIA_FOURCC('o', 'n', 'c', 0)
#define VIM_MEDIA_PREPROCESS_AGC 					VIM_MEDIA_FOURCC('a', 'g', 'c', 0) 

/*define file property type macros for media get property and set property used as in parameter*/
#define VIM_MEDIA_FILE_PROPERTY_TYPE					"filetype"		// filetype property is a string, 0<=length(sYear)<=4
#define VIM_MEDIA_FILE_PROPERTY_AUDIO_INFOR 			"audioinf"		// audio information property is a structer
#define VIM_MEDIA_FILE_PROPERTY_VIDEO_INFOR 			"videoinf"		// video information property is a structer
#define VIM_MEDIA_FILE_PROPERTY_AUDIO_OUTINFOR 		"audioout"	// audio output property is a structer
#define VIM_MEDIA_FILE_PROPERTY_VIDEO_OUTINFOR 		"videoout"	// video output property is a structer

#define VIM_MEDIA_FILE_PROPERTY_TITLE 					"title"		// title property    is a string, 0<=length(sTitle)<=256
#define VIM_MEDIA_FILE_PROPERTY_ARSTIST 				"artist"		// artist property  is a string, 0<=length(sArtist)<=256
#define VIM_MEDIA_FILE_PROPERTY_ALBUM 				"album" 		// album property is a string, 0<=length(sAlbum)<=256  
#define VIM_MEDIA_FILE_PROPERTY_YEAR 					"year"		// year property   is a string, 0<=length(sYear)<=256
#define VIM_MEDIA_FILE_PROPERTY_COMMENT 				"comment"	// comment property is a string, 0<=length(sComment)<=256
#define VIM_MEDIA_FILE_PROPERTY_GENRE 				"genre"		// genre property  is a string, 0<=length(sGenre)<=256
#define VIM_MEDIA_FILE_PROPERTY_TRACK 				"track"  		// track property  is a unsigned char, 0<=track<=255 

#define VIM_MEDIA_FILE_PROPERTY_AUDIO_INPUTINFOR 	"audioin"		// audio input property is a structer
#define VIM_MEDIA_FILE_PROPERTY_VIDEO_INPUTINFOR 	"videoin"		// video input property is a structer

#define MAX_VIDEO_FRAME_WIDTH					720
#define MAX_VIDEO_FRAME_HEIGHT					576

#ifdef __cplusplus
}
#endif

#endif //_MP_KEY_MACRO_




