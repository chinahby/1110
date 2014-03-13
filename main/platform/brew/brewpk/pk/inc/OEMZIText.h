#ifndef OEMTEXT_H
#define OEMTEXT_H
/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:      OEMText.h
                 
 
SERVICES:  OEM Functions for Text Control support

 
DESCRIPTION: This file provides the data structures and prototypes of functions
that OEMs must provide in order to support the Text Control. This header file
shall be included by OEMTextCtl.c which provides sample implementation for
the functions mentioned here. The AEETextCtl API implementation makes down calls into
the functions mentioned here.


PUBLIC CLASSES:  Not Applicable


INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable


        Copyright ?1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //tsg/UI/main/TSGBREWOEM/inc/OEMText.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/01/08   bw      Build for ZI

===========================================================================*/

/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/

/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/

#include "AEE_OEMComdef.h"
#include "AEE.h"
#include "AEEText.h"
#include "OEMFeatures.h"
#include "OEMConfig.h"
#include "OEMCFGI.h"
#include "zi8api.h"
#include "Word_engine.h"
//#include "zhcnime.brh"


/*-------------------------------------------------------------------
      Type Declarations
      FEATURE DEFINED
-------------------------------------------------------------------*/

// USE ZI input method

#define FEATURE_ZI_INPUT
#ifdef FEATURE_LANGUAGE_MYANMAR
#define FEATURE_MYANMAR_INPUT_MOD     //add by yangdecai 2010-12-23
#endif
#ifdef WIN32  //add by ydc
#define FEATURE_LANG_ENGLISH
#define FEATURE_TOUCHPANEL
//#define FEATURE_LANG_CHINESE
#endif

#define AEE_APPSTISIMAPP_RES_FILE AEE_RES_LANGDIR ZHCNIME_RES_FILE
#define IDD_IME 6001



#ifdef FEATURE_ZI_INPUT

#define FEATURE_ZI_ALPHABETIC

#ifdef FEATURE_ZI_ALPHABETIC

#define FEATURE_ZI_MULTITAP
//#define FEATURE_LANG_HINDI     //add  by yangdecai
//#define FEATURE_LANG_ARABIC
#ifdef FEATURE_LANG_ENGLISH
#define FEATURE_ZI_MT_ENGLISH
#ifndef FEATURE_VERSION_K232_Y105A
#define FEATURE_ZI_RAPID_ENGLISH
#endif
#if defined (FEATURE_ALL_KEY_PAD)|| defined(FEATURE_VERSION_C01)||defined(FEATURE_VERSION_W515V3)
#define FEATURE_ZI_CAP_LOWER_ENGLISH   //add by yangdecai 2010-09-09
#endif

#endif //FEATURE_LANG_ENGLISH

#ifdef FEATURE_LANG_ARABIC
#define FEATURE_ZI_MT_ARABIC
#define FEATURE_ZI_RAPID_ARABIC
#endif //FEATURE_LANG_ARABIC

#ifdef FEATURE_LANG_HEBREW
#define FEATURE_ZI_MT_HEBREW
#define FEATURE_ZI_RAPID_HEBREW
#endif //FEATURE_LANG_HEBREW

#ifdef FEATURE_LANG_HINDI
#define FEATURE_ZI_MT_HINDI
#define FEATURE_ZI_RAPID_HINDI
#endif //FEATURE_LANG_HINDI

#ifdef FEATURE_LANG_THAI
#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM) || defined(FEATURE_VERSION_C01)
#define FEATURE_ZI_MT_THAI
#else
#define FEATURE_ZI_MT_THAI
#define FEATURE_ZI_RAPID_THAI
#endif
#endif //FEATURE_LANG_THAI

#ifdef FEATURE_LANG_SPANISH
#define FEATURE_ZI_MT_SPANISH
#define FEATURE_ZI_RAPID_SPANISH
#endif //FEATURE_LANG_SPANISH

#ifdef FEATURE_LANG_PORTUGUESE
//#define FEATURE_ZI_MT_PORTUGUESE
#define FEATURE_ZI_RAPID_PORTUGUESE
#endif //FEATURE_LANG_PORTUGUESE

#ifdef FEATURE_INPUTMODE_INDONESIAN
#define FEATURE_ZI_MT_INDONESIAN
#define FEATURE_ZI_RAPID_INDONESIAN
#endif //FEATURE_LANG_INDONESIAN

#ifdef FEATURE_LANG_VIETNAMESE
#define FEATURE_ZI_MT_VIETNAMESE
#define FEATURE_ZI_RAPID_VIETNAMESE
#endif //FEATURE_LANG_VIETNAMESE

#ifdef FEATURE_LANG_FRENCH
#define FEATURE_ZI_MT_FRENCH
#define FEATURE_ZI_RAPID_FRENCH
#endif //FEATURE_LANG_FRENCH

#ifdef FEATURE_LANG_JAPANESE
#define FEATURE_ZI_JAPANESE
#endif //FEATURE_LANG_JAPANESE

#ifdef FEATURE_LANG_KOREAN
#define FEATURE_ZI_KOREAN
#endif //FEATURE_LANG_KOREAN

#ifdef FEATURE_LANG_ITALIAN
#define FEATURE_ZI_ITALIAN
#endif //FEATURE_LANG_ITALIAN

#endif //#ifdef FEATURE_ZI_ALPHABETIC

#ifdef FEATURE_LANG_CHINESE
#define FEATURE_ZICORP_STROKE
#define FEATURE_ZICORP_PINYIN
#define FEATURE_ZICORP_EZI_HI
#endif

#ifdef FEATURE_LANG_CHINESE
#define FEATURE_ZI_CHINESE
#define FEATURE_ZI_PINYIN
#define FEATURE_ZI_STROKE
#endif //FEATURE_LANG_CHINESE

#ifdef FEATURE_LANG_TCHINESE
#define FEATURE_ZI_CHINESE
#ifndef AEE_SIMULATOR
#define FEATURE_ZI_ZHUYIN
#endif
#endif //FEATURE_LANG_TCHINESE

#ifdef FEATURE_ZI_MT_ENGLISH
#define FEATURE_3STYLE_MULTITAP
#endif //FEATURE_ZI_MT_ENGLISH

#endif //FEATURE_ZI_INPUT
//===============================================================================
// Input method mode use for input method 
//===============================================================================  
#define TEXT_MODE_NUMBERS          AEE_TM_NUMBERS

//#ifdef FEATURE_ZI_MULTITAP
#define TEXT_MODE_MULTITAP         AEE_TM_LETTERS
//#endif //FEATURE_ZI_MULTITAP

//#ifdef FEATURE_ZI_RAPID_ENGLISH
#define TEXT_MODE_ZI_RAPID_ENGLISH AEE_TM_RAPID 
//#endif //FEATURE_ZI_RAPID_ENGLISH
//#ifdef FEATURE_ZI_CAP_LOWER_ENGLISH  //add by yangdecai   2010-09-09
#define TEXT_MODE_ZI_CAP_LOWER_ENGLISH  AEE_TM_CAPLOWER
//#endif
//#ifdef FEATURE_MYANMAR_INPUT_MOD
#define TEXT_MODE_MYANMAR  AEE_TM_MYANMAR   //add by yangdecai   2010-12-23
//#endif
//#ifdef FEATURE_ZI_PINYIN
#define TEXT_MODE_ZI_PINYIN        AEE_TM_PINYIN 
//#endif //FEATURE_ZI_PINYIN

//#ifdef FEATURE_ZI_STROKE
#define TEXT_MODE_ZI_STROKE        AEE_TM_STROKE 
//#endif //FEATURE_ZI_STROKE

//#ifdef FEATURE_ZI_ZHUYIN
#define TEXT_MODE_ZI_ZHUYIN        AEE_TM_ZHUYIN 
//#endif //FEATURE_ZI_ZHUYIN

//#ifdef FEATURE_ZI_MT_ARABIC
#define TEXT_MODE_ZI_MT_ARABIC     AEE_TM_ARABIC
//#endif //FEATURE_ZI_MT_ARABIC

//#ifdef FEATURE_ZI_RAPID_ARABIC
#define TEXT_MODE_ZI_RAPID_ARABIC  AEE_TM_ARABIC_R
//#endif //FEATURE_ZI_RAPID_ARABIC

//#ifdef FEATURE_ZI_MT_HEBREW
#define TEXT_MODE_ZI_MT_HEBREW     AEE_TM_HEBREW
//#endif //FEATURE_ZI_MT_HEBREW

//#ifdef FEATURE_ZI_RAPID_HEBREW
#define TEXT_MODE_ZI_RAPID_HEBREW  AEE_TM_HEBREW_R
//#endif //FEATURE_ZI_RAPID_HEBREW

//#ifdef FEATURE_ZI_MT_HINDI
#define TEXT_MODE_ZI_MT_HINDI      AEE_TM_HINDI
//#endif //FEATURE_ZI_MT_HINDI

//#ifdef FEATURE_ZI_RAPID_HINDI
#define TEXT_MODE_ZI_RAPID_HINDI   AEE_TM_HINDI_R
//#endif //FEATURE_ZI_RAPID_HINDI

#ifdef FEATURE_ZI_MT_THAI
#define TEXT_MODE_ZI_MT_THAI       AEE_TM_THAI
#endif //FEATURE_ZI_MT_THAI

#ifdef FEATURE_ZI_RAPID_THAI
#define TEXT_MODE_ZI_RAPID_THAI    AEE_TM_THAI_R
#endif //FEATURE_ZI_RAPID_THAI

#ifdef FEATURE_ZI_MT_SPANISH
#define TEXT_MODE_ZI_MT_SPANISH    AEE_TM_SPANISH_LA
#endif //FEATURE_ZI_MT_SPANISH

#ifdef FEATURE_ZI_RAPID_SPANISH
#define TEXT_MODE_ZI_RAPID_SPANISH AEE_TM_SPANISH_LA_R
#endif //FEATURE_ZI_RAPID_SPANISH

#ifdef FEATURE_ZI_MT_PORTUGUESE
#define TEXT_MODE_ZI_MT_PORTUGUESE AEE_TM_PORTUGUESE_EU
#endif //FEATURE_ZI_MT_PORTUGUESE

#ifdef FEATURE_ZI_RAPID_PORTUGUESE
#define TEXT_MODE_ZI_RAPID_PORTUGUESE AEE_TM_PORTUGUESE_EU_R
#endif //FEATURE_ZI_RAPID_PORTUGUESE

#ifdef FEATURE_ZI_MT_INDONESIAN
#define TEXT_MODE_ZI_MT_INDONESIAN    AEE_TM_INDONESIAN
#endif //FEATURE_ZI_MT_INDONESIAN

#ifdef FEATURE_ZI_RAPID_INDONESIAN
#define TEXT_MODE_ZI_RAPID_INDONESIAN AEE_TM_INDONESIAN_R
#endif //FEATURE_ZI_RAPID_INDONESIAN

#ifdef FEATURE_ZI_MT_VIETNAMESE
#define TEXT_MODE_ZI_MT_VIETNAMESE    AEE_TM_VIETNAMESE
#endif //FEATURE_ZI_MT_VIETNAMESE

#ifdef FEATURE_ZI_RAPID_VIETNAMESE
#define TEXT_MODE_ZI_RAPID_VIETNAMESE AEE_TM_VIETNAMESE_R
#endif //FEATURE_ZI_RAPID_VIETNAMESE

#ifdef FEATURE_ZI_MT_FRENCH
#define TEXT_MODE_ZI_MT_FRENCH        AEE_TM_FRENCH_CA
#endif //FEATURE_ZI_MT_FRENCH

#ifdef FEATURE_ZI_RAPID_FRENCH
#define TEXT_MODE_ZI_RAPID_FRENCH     AEE_TM_FRENCH_CA_R
#endif //FEATURE_ZI_RAPID_FRENCH

#ifdef FEATURE_ZI_JAPANESE
#define TEXT_MODE_ZI_JAPANESE         AEE_TM_KANJI
#endif //FEATURE_ZI_JAPANESE

#ifdef FEATURE_ZI_KOREAN
#define TEXT_MODE_ZI_KOREAN           AEE_TM_HANGUL
#endif //FEATURE_ZI_KOREAN

#ifdef FEATURE_ZI_ITALIAN
#define TEXT_MODE_ZI_ITALIAN          AEE_TM_ITALIAN
#endif //FEATURE_ZI_ITALIAN


//#ifdef WIN32 

#define AVK_F0  AVK_SOFT1

#define AVK_F1  AVK_SOFT2

#define AVK_F2  AVK_SOFT3

#define AVK_F3  AVK_SOFT4

#define  MAX_HZ_PAGE_SIZE       20      // 每页最多显示8
#define  MAX_PY_PAGE_SIZE       15       // 每页显示的最多5个拼音

void             OEM_TextDrawIMEDlg(OEMCONTEXT hTextField);

//#endif
//===============================================================================
// Input method mode use for display input method name
//===============================================================================  
enum 
{
    OEM_MODE_NUMBERS = 0,   // 鏁板瓧 

#ifdef FEATURE_ZI_MT_ENGLISH
    OEM_MODE_ZI_MT_ENGLISH 
    ,OEM_MODE_ZI_MT_ENGLISH_LOW
    ,OEM_MODE_ZI_MT_ENGLISH_UP
    #ifdef FEATURE_VERSION_C01
    ,OEM_MODE_ZI_MT_CAP_LOWER
    #endif
#endif //FEATURE_ZI_MT_ENGLISH

#ifdef FEATURE_ZI_RAPID_ENGLISH
    ,OEM_MODE_ZI_RAPID_ENGLISH
    ,OEM_MODE_ZI_RAPID_ENGLISH_LOW
    ,OEM_MODE_ZI_RAPID_ENGLISH_UP    
#endif //FEATURE_ZI_RAPID_ENGLISH

#ifdef FEATURE_ZI_CAP_LOWER_ENGLISH   //add by yangdecai
	,OEM_MODE_ZI_CAP_LOWER_ENGLISH
#endif
#ifdef FEATURE_MYANMAR_INPUT_MOD
	,OEM_MODE_MYANMAR,
#endif
#ifdef FEATURE_ZI_PINYIN
    ,OEM_MODE_ZI_PINYIN
#endif //FEATURE_ZI_PINYIN

#ifdef FEATURE_ZI_STROKE
    ,OEM_MODE_ZI_STROKE
#endif //FEATURE_ZI_STROKE

#ifdef FEATURE_ZI_ZHUYIN
    ,OEM_MODE_ZI_ZHUYIN
#endif //FEATURE_ZI_ZHUYIN

#ifdef FEATURE_ZI_MT_ARABIC
    ,OEM_MODE_ZI_MT_ARABIC
#endif //FEATURE_ZI_MT_ARABIC

#ifdef FEATURE_ZI_RAPID_ARABIC
    ,OEM_MODE_ZI_RAPID_ARABIC
#endif //FEATURE_ZI_RAPID_ARABIC

#ifdef FEATURE_ZI_MT_HEBREW
    ,OEM_MODE_ZI_MT_HEBREW
#endif //FEATURE_ZI_MT_HEBREW

#ifdef FEATURE_ZI_RAPID_HEBREW
    ,OEM_MODE_ZI_RAPID_HEBREW
#endif //FEATURE_ZI_RAPID_HEBREW

#ifdef FEATURE_ZI_MT_HINDI
    ,OEM_MODE_ZI_MT_HINDI
#endif //FEATURE_ZI_MT_HINDI

#ifdef FEATURE_ZI_RAPID_HINDI
    ,OEM_MODE_ZI_RAPID_HINDI
#endif //FEATURE_ZI_RAPID_HINDI

#ifdef FEATURE_ZI_MT_THAI
    ,OEM_MODE_ZI_MT_THAI
#endif //FEATURE_ZI_MT_THAI

#ifdef FEATURE_ZI_RAPID_THAI
    ,OEM_MODE_ZI_RAPID_THAI
#endif //FEATURE_ZI_RAPID_THAI

#ifdef FEATURE_ZI_MT_SPANISH
    ,OEM_MODE_ZI_MT_SPANISH
    ,OEM_MODE_ZI_MT_SPANISH_LOW  
    ,OEM_MODE_ZI_MT_SPANISH_UP
#endif //FEATURE_ZI_MT_SPANISH

#ifdef FEATURE_ZI_RAPID_SPANISH
    ,OEM_MODE_ZI_RAPID_SPANISH
    ,OEM_MODE_ZI_RAPID_SPANISH_LOW  
    ,OEM_MODE_ZI_RAPID_SPANISH_UP
#endif //FEATURE_ZI_RAPID_SPANISH

#ifdef FEATURE_ZI_MT_PORTUGUESE
    ,OEM_MODE_ZI_MT_PORTUGUESE
#endif //FEATURE_ZI_MT_PORTUGUESE

#ifdef FEATURE_ZI_RAPID_PORTUGUESE
    ,OEM_MODE_ZI_RAPID_PORTUGUESE
#endif //FEATURE_ZI_RAPID_PORTUGUESE

#ifdef FEATURE_ZI_MT_INDONESIAN
    ,OEM_MODE_ZI_MT_INDONESIAN
#endif //FEATURE_ZI_MT_INDONESIAN

#ifdef FEATURE_ZI_RAPID_INDONESIAN
    ,OEM_MODE_ZI_RAPID_INDONESIAN
#endif //FEATURE_ZI_RAPID_INDONESIAN

#ifdef FEATURE_ZI_MT_VIETNAMESE
    ,OEM_MODE_ZI_MT_VIETNAMESE
#endif //FEATURE_ZI_MT_VIETNAMESE

#ifdef FEATURE_ZI_RAPID_VIETNAMESE
    ,OEM_MODE_ZI_RAPID_VIETNAMESE
#endif //FEATURE_ZI_RAPID_VIETNAMESE

#ifdef FEATURE_ZI_MT_FRENCH
    ,OEM_MODE_ZI_MT_FRENCH
    ,OEM_MODE_ZI_MT_FRENCH_LOW  
    ,OEM_MODE_ZI_MT_FRENCH_UP
#endif //FEATURE_ZI_MT_FRENCH

#ifdef FEATURE_ZI_RAPID_FRENCH
    ,OEM_MODE_ZI_RAPID_FRENCH
    ,OEM_MODE_ZI_RAPID_FRENCH_LOW  
    ,OEM_MODE_ZI_RAPID_FRENCH_UP
#endif //FEATURE_ZI_RAPID_FRENCH

#ifdef FEATURE_ZI_JAPANESE
    ,OEM_MODE_ZI_JAPANESE
#endif //FEATURE_ZI_JAPANESE

#ifdef FEATURE_ZI_KOREAN
    ,OEM_MODE_ZI_KOREAN
#endif //FEATURE_ZI_KOREAN

#ifdef FEATURE_ZI_ITALIAN
    ,OEM_MODE_ZI_ITALIAN
#endif //FEATURE_ZI_ITALIAN
};

//===============================================================================
// Input method title
//=============================================================================== 
//Maximum length of the Mode string. This is used in the Menu.    
#define MAX_MODE_STR                (16)

#define _ZI_NUMBER_MODES             (1)

#ifdef FEATURE_ZI_MT_ENGLISH
#define _ZI_MULTITAP_MODES             (1)
#else // #ifdef FEATURE_ZI_MT_ENGLISH
#define _ZI_MULTITAP_MODES             (0)
#endif //#ifdef FEATURE_ZI_MT_ENGLISH

#ifdef	FEATURE_ZI_CAP_LOWER_ENGLISH   //add by yangdecai 2010-09-09
#define _ZI_CAPLOWER_MODES             (1)
#else
#define _ZI_CAPLOWER_MODES             (0)
#endif

#ifdef FEATURE_ZI_RAPID_ENGLISH
#define _ZI_RAPID_ENGLISH_MODE             (1)
#else // #ifdef FEATURE_ZI_RAPID_ENGLISH
#define _ZI_RAPID_ENGLISH_MODE             (0)
#endif //#ifdef FEATURE_ZI_RAPID_ENGLISH

#ifdef FEATURE_ZI_PINYIN
#define _ZI_PINYIN_MODE             (1)
#else // #ifdef FEATURE_ZI_PINYIN
#define _ZI_PINYIN_MODE             (0)
#endif //FEATURE_ZI_PINYIN
#ifdef FEATURE_MYANMAR_INPUT_MOD
#define _ZI_MYANMAR_MODE             (1)
#else // #ifdef FEATURE_ZI_PINYIN
#define _ZI_MYANMAR_MODE             (0)
#endif //FEATURE_ZI_PINYIN
#ifdef FEATURE_ZI_STROKE
#define _ZI_STROKE_MODE             (1)
#else // #ifdef FEATURE_ZI_STROKE
#define _ZI_STROKE_MODE             (0)
#endif //FEATURE_ZI_STROKE

#ifdef FEATURE_ZI_ZHUYIN
#define _ZI_ZHUYIN_MODE             (1)
#else // #ifdef FEATURE_ZI_ZHUYIN
#define _ZI_ZHUYIN_MODE             (0)
#endif //FEATURE_ZI_ZHUYIN

#ifdef FEATURE_ZI_MT_ARABIC
#define _ZI_MT_ARABIC_MODE             (1)
#else // #ifdef FEATURE_ZI_MT_ARABIC
#define _ZI_MT_ARABIC_MODE             (0)
#endif //FEATURE_ZI_MT_ARABIC

#ifdef FEATURE_ZI_RAPID_ARABIC
#define _ZI_RAPID_ARABIC_MODE             (1)
#else // #ifdef FEATURE_ZI_MT_ARABIC
#define _ZI_RAPID_ARABIC_MODE             (0)
#endif //FEATURE_ZI_RAPID_ARABIC

#ifdef FEATURE_ZI_MT_HEBREW
#define _ZI_MT_HEBREW_MODE             (1)
#else // #ifdef FEATURE_ZI_MT_HEBREW
#define _ZI_MT_HEBREW_MODE             (0)
#endif //FEATURE_ZI_MT_HEBREW

#ifdef FEATURE_ZI_RAPID_HEBREW
#define _ZI_RAPID_HEBREW_MODE             (1)
#else // #ifdef FEATURE_ZI_RAPID_HEBREW
#define _ZI_RAPID_HEBREW_MODE             (0)
#endif //FEATURE_ZI_RAPID_HEBREW

#ifdef FEATURE_ZI_MT_HINDI
#define _ZI_MT_HINDI_MODE             (1)
#else // #ifdef FEATURE_ZI_MT_HINDI
#define _ZI_MT_HINDI_MODE             (0)
#endif //FEATURE_ZI_MT_HINDI

#ifdef FEATURE_ZI_RAPID_HINDI
#define _ZI_RAPID_HINDI_MODE             (1)
#else // #ifdef FEATURE_ZI_RAPID_HINDI
#define _ZI_RAPID_HINDI_MODE             (0)
#endif //FEATURE_ZI_RAPID_HINDI

#ifdef FEATURE_ZI_MT_THAI
#define _ZI_MT_THAI_MODE             (1)
#else // #ifdef FEATURE_ZI_MT_THAI
#define _ZI_MT_THAI_MODE             (0)
#endif //FEATURE_ZI_MT_THAI

#ifdef FEATURE_ZI_RAPID_THAI
#define _ZI_RAPID_THAI_MODE             (1)
#else // #ifdef FEATURE_ZI_RAPID_THAI
#define _ZI_RAPID_THAI_MODE             (0)
#endif //FEATURE_ZI_RAPID_THAI

#ifdef FEATURE_ZI_MT_SPANISH
#define _ZI_MT_SPANISH_MODE             (1)
#else // #ifdef FEATURE_ZI_MT_SPANISH
#define _ZI_MT_SPANISH_MODE             (0)
#endif //FEATURE_ZI_MT_SPANISH

#ifdef FEATURE_ZI_RAPID_SPANISH
#define _ZI_RAPID_SPANISH_MODE             (1)
#else // #ifdef FEATURE_ZI_RAPID_SPANISH
#define _ZI_RAPID_SPANISH_MODE             (0)
#endif //FEATURE_ZI_RAPID_SPANISH

#ifdef FEATURE_ZI_MT_PORTUGUESE
#define _ZI_MT_PORTUGUESE_MODE             (1)
#else // #ifdef FEATURE_ZI_MT_PORTUGUESE
#define _ZI_MT_PORTUGUESE_MODE             (0)
#endif //FEATURE_ZI_MT_PORTUGUESE

#ifdef FEATURE_ZI_RAPID_PORTUGUESE
#define _ZI_RAPID_PORTUGUESE_MODE             (1)
#else // #ifdef FEATURE_ZI_MT_PORTUGUESE
#define _ZI_RAPID_PORTUGUESE_MODE             (0)
#endif //FEATURE_ZI_RAPID_PORTUGUESE

#ifdef FEATURE_ZI_MT_INDONESIAN
#define _ZI_MT_INDONESIAN_MODE             (1)
#else // #ifdef FEATURE_ZI_MT_INDONESIAN
#define _ZI_MT_INDONESIAN_MODE             (0)
#endif //FEATURE_ZI_MT_INDONESIAN

#ifdef FEATURE_ZI_RAPID_INDONESIAN
#define _ZI_RAPID_INDONESIAN_MODE             (1)
#else // #ifdef FEATURE_ZI_MT_INDONESIAN
#define _ZI_RAPID_INDONESIAN_MODE             (0)
#endif //FEATURE_ZI_RAPID_INDONESIAN

#ifdef FEATURE_ZI_MT_VIETNAMESE
#define _ZI_MT_VIETNAMESE_MODE             (1)
#else // #ifdef FEATURE_ZI_MT_VIETNAMESE
#define _ZI_MT_VIETNAMESE_MODE             (0)
#endif //FEATURE_ZI_MT_VIETNAMESE

#ifdef FEATURE_ZI_RAPID_VIETNAMESE
#define _ZI_RAPID_VIETNAMESE_MODE             (1)
#else // #ifdef FEATURE_ZI_MT_VIETNAMESE
#define _ZI_RAPID_VIETNAMESE_MODE             (0)
#endif //FEATURE_ZI_RAPID_VIETNAMESE

#ifdef FEATURE_ZI_MT_FRENCH
#define _ZI_MT_FRENCH_MODE             (1)
#else // #ifdef FEATURE_ZI_MT_FRENCH
#define _ZI_MT_FRENCH_MODE             (0)
#endif //FEATURE_ZI_MT_FRENCH

#ifdef FEATURE_ZI_RAPID_FRENCH
#define _ZI_RAPID_FRENCH_MODE             (1)
#else // #ifdef FEATURE_ZI_RAPID_FRENCH
#define _ZI_RAPID_FRENCH_MODE             (0)
#endif //FEATURE_ZI_RAPID_FRENCH

#ifdef FEATURE_ZI_JAPANESE
#define _ZI_JAPANESE_MODE             (1)
#else // #ifdef FEATURE_ZI_JAPANESE
#define _ZI_JAPANESE_MODE             (0)
#endif //FEATURE_ZI_JAPANESE

#ifdef FEATURE_ZI_KOREAN
#define _ZI_KOREAN_MODE             (1)
#else // #ifdef FEATURE_ZI_JAPANESE
#define _ZI_KOREAN_MODE             (0)
#endif //FEATURE_ZI_KOREAN

#ifdef FEATURE_ZI_ITALIAN
#define _ZI_ITALIAN_MODE             (1)
#else // #ifdef FEATURE_ZI_ITALIAN
#define _ZI_ITALIAN_MODE             (0)
#endif //FEATURE_ZI_ITALIAN

#define NUM_OF_MODES            ( _ZI_NUMBER_MODES \
                                + _ZI_MULTITAP_MODES \
                                + _ZI_RAPID_ENGLISH_MODE \
                                + _ZI_MYANMAR_MODE \
                                + _ZI_PINYIN_MODE \
                                + _ZI_STROKE_MODE \
                                + _ZI_ZHUYIN_MODE \
                                + _ZI_MT_ARABIC_MODE \
                                + _ZI_RAPID_ARABIC_MODE \
                                + _ZI_MT_HEBREW_MODE \
                                + _ZI_RAPID_HEBREW_MODE \
                                + _ZI_MT_HINDI_MODE \
                                + _ZI_RAPID_HINDI_MODE \
                                + _ZI_MT_THAI_MODE \
                                + _ZI_RAPID_THAI_MODE \
                                + _ZI_MT_SPANISH_MODE \
                                + _ZI_RAPID_SPANISH_MODE \
                                + _ZI_MT_PORTUGUESE_MODE \
                                + _ZI_RAPID_PORTUGUESE_MODE \
                                + _ZI_MT_INDONESIAN_MODE \
                                + _ZI_RAPID_INDONESIAN_MODE \
                                + _ZI_MT_VIETNAMESE_MODE \
                                + _ZI_RAPID_VIETNAMESE_MODE \
                                + _ZI_MT_FRENCH_MODE \
                                + _ZI_RAPID_FRENCH_MODE \
                                + _ZI_JAPANESE_MODE \
                                + _ZI_KOREAN_MODE \
                                + _ZI_ITALIAN_MODE  \
                                + _ZI_CAPLOWER_MODES  \
                                )

//wangliang modify!  2010-09-01
//#if defined(FEATURE_PROJECT_SMART) || defined(FEATURE_PROJECT_M8)
#if defined (FEATURE_ZI_TEXT)
static const AECHAR englishTitle[NUM_OF_MODES][MAX_MODE_STR+1] =
{
   // Lint complains because all the fields aren't explicitly initialized
   //lint -save -e785
   {'1','2','3',0}
   
#ifdef FEATURE_ZI_MT_ENGLISH   
   ,{'A','B','C',0}
#endif //FEATURE_ZI_MT_ENGLISH

#ifdef FEATURE_ZI_RAPID_ENGLISH
   ,{'a', 'b','c',0}
#endif //FEATURE_ZI_RAPID_ENGLISH

#ifdef FEATURE_ZI_CAP_LOWER_ENGLISH   //add by yangdecai
   ,{'A','b','c',0}
#endif
#ifdef FEATURE_MYANMAR_INPUT_MOD      //add by yangdecai 2010-1223
   ,{'M','Y','A','N','M','A','R',0}
#endif
#ifdef FEATURE_ZI_PINYIN
   ,{'T', '9','P', 'I', 'N', 'Y', 'I', 'N', 0}
#endif //FEATURE_ZI_PINYIN

#ifdef FEATURE_ZI_STROKE
   ,{'T', '9','S', 'T', 'R', 'O', 'K', 'E', 0}
#endif //FEATURE_ZI_STROKE

#ifdef FEATURE_ZI_ZHUYIN
   ,{'T', '9','Z', 'H', 'U', 'Y', 'I', 'N', 0}
#endif //FEATURE_ZI_ZHUYIN

#ifdef FEATURE_ZI_MT_ARABIC
   ,{'T', '9', 'M', 'T', 'A', 'R', 'A', 'B','C',0}
#endif //FEATURE_ZI_MT_ARABIC

#ifdef FEATURE_ZI_RAPID_ARABIC
   ,{'T', '9', 'E', 'Z', 'A', 'R', 'A', 'B', 'I', 'C',0}
#endif //FEATURE_ZI_RAPID_ARABIC

#ifdef FEATURE_ZI_MT_HEBREW
   ,{'T', '9', 'M', 'T', 'H', 'E', 'B', 'R', 'E', 'W',0}
#endif //FEATURE_ZI_MT_HEBREW

#ifdef FEATURE_ZI_RAPID_HEBREW
   ,{'T', '9','E', 'Z', 'H', 'E', 'B', 'R', 'E', 'W',0}
#endif //FEATURE_ZI_RAPID_HEBREW

#ifdef FEATURE_ZI_MT_HINDI
   ,{'T', '9','M', 'T', 'H', 'I', 'N', 'D', 'I', 0}
#endif //FEATURE_ZI_MT_HINDI

#ifdef FEATURE_ZI_RAPID_HINDI
   ,{'T', '9', 'E', 'Z', 'H', 'I', 'N', 'D', 'I', 0}
#endif //FEATURE_ZI_RAPID_HINDI

#ifdef FEATURE_ZI_MT_THAI
   ,{'T', '9','M', 'T', 'T', 'H', 'A', 'I', 0}
#endif //FEATURE_ZI_MT_THAI

#ifdef FEATURE_ZI_RAPID_THAI
   ,{'T', '9','E', 'Z', 'T', 'H', 'A', 'I', 0}
#endif //FEATURE_ZI_RAPID_THAI

#ifdef FEATURE_ZI_MT_SPANISH
   ,{'T', '9','M', 'T', 'S', 'P', 'A', 'N', 'I', 'S', 'H',0}
#endif //FEATURE_ZI_MT_SPANISH

#ifdef FEATURE_ZI_RAPID_SPANISH
   ,{'T', '9','E', 'Z', 'S', 'P', 'A', 'N', 'I', 'S', 'H',0}
#endif //FEATURE_ZI_RAPID_SPANISH

#ifdef FEATURE_ZI_MT_PORTUGUESE
   ,{'T', '9','M', 'T', 'P', 'O', 'R', 'T', 'U', 'G', 'U','E', 'S', 'E',0}
#endif //FEATURE_ZI_MT_PORTUGUESE

#ifdef FEATURE_ZI_RAPID_PORTUGUESE
   ,{'T', '9','E', 'Z', 'P', 'O', 'R', 'T', 'U', 'G', 'U','E', 'S', 'E',0}
#endif //FEATURE_ZI_RAPID_PORTUGUESE

#ifdef FEATURE_ZI_MT_INDONESIAN
   ,{'T', '9','M', 'T', 'I', 'N', 'D', 'O', 'N', 'E', 'S','I', 'A', 'N',0}
#endif //FEATURE_ZI_MT_INDONESIAN

#ifdef FEATURE_ZI_RAPID_INDONESIAN
   ,{'T', '9','E', 'Z', 'I', 'N', 'D', 'O', 'N', 'E', 'S','I', 'A', 'N',0}
#endif //FEATURE_ZI_RAPID_INDONESIAN

#ifdef FEATURE_ZI_MT_VIETNAMESE
   ,{'T', '9','M', 'T', 'N', 'I', 'E', 'T', 'N', 'A', 'M','E', 'S', 'E',0}
#endif //FEATURE_ZI_MT_VIETNAMESE

#ifdef FEATURE_ZI_RAPID_VIETNAMESE
   ,{'T', '9','E', 'Z', 'N', 'I', 'E', 'T', 'N', 'A', 'M','E', 'S', 'E',0}
#endif //FEATURE_ZI_RAPID_VIETNAMESE

#ifdef FEATURE_ZI_MT_FRENCH
   ,{'T', '9','M', 'T', 'F', 'R', 'E', 'N', 'C', 'H',0}
#endif //FEATURE_ZI_MT_FRENCH

#ifdef FEATURE_ZI_RAPID_FRENCH
   ,{'T', '9','E', 'Z', 'F', 'R', 'E', 'N', 'C', 'H',0}
#endif //FEATURE_ZI_RAPID_FRENCH

#ifdef FEATURE_ZI_JAPANESE
   ,{'J', 'A', 'P', 'A', 'N', 'E', 'S', 'E',0}
#endif //FEATURE_ZI_JAPANESE

#ifdef FEATURE_ZI_KOREAN
   ,{'K', 'O', 'R', 'E', 'A', 'N', 0}
#endif //FEATURE_ZI_KOREAN

#ifdef FEATURE_ZI_ITALIAN
   ,{'I', 'T', 'A', 'L', 'I', 'A',  'N',0}
#endif //FEATURE_ZI_ITALIAN
};
#else
static const AECHAR englishTitle[NUM_OF_MODES][MAX_MODE_STR+1] =
{
   // Lint complains because all the fields aren't explicitly initialized
   //lint -save -e785
   {'1','2','3',0}
   
#ifdef FEATURE_ZI_MT_ENGLISH   
   ,{'A','B','C',0}
#endif //FEATURE_ZI_MT_ENGLISH
   
#ifdef FEATURE_ZI_RAPID_ENGLISH
   ,{'a', 'b','c', 0}
#endif //FEATURE_ZI_RAPID_ENGLISH

#ifdef FEATURE_ZI_CAP_LOWER_ENGLISH   //add by yangdecai
   ,{'A','b','c',0}
#endif
#ifdef FEATURE_MYANMAR_INPUT_MOD      //add by yangdecai 2010-1223
   ,{'M','Y','A','N','M','A','R',0}
#endif
#ifdef FEATURE_ZI_PINYIN
   ,{'T', '9','P', 'I', 'N', 'Y', 'I', 'N', 0}
#endif //FEATURE_ZI_PINYIN

#ifdef FEATURE_ZI_STROKE
   ,{'T', '9','S', 'T', 'R', 'O', 'K', 'E', 0}
#endif //FEATURE_ZI_STROKE

#ifdef FEATURE_ZI_ZHUYIN
   ,{'T', '9','Z', 'H', 'U', 'Y', 'I', 'N', 0}
#endif //FEATURE_ZI_ZHUYIN

#ifdef FEATURE_ZI_MT_ARABIC
   ,{'T', '9', 'M', 'T', 'A', 'R', 'A', 'B', 'I', 'C',0}
#endif //FEATURE_ZI_MT_ARABIC

#ifdef FEATURE_ZI_RAPID_ARABIC
   ,{'T', '9', 'E', 'Z', 'A', 'R', 'A', 'B', 'I', 'C',0}
#endif //FEATURE_ZI_RAPID_ARABIC

#ifdef FEATURE_ZI_MT_HEBREW
   ,{'T', '9', 'M', 'T', 'H', 'E', 'B', 'R', 'E', 'W',0}
#endif //FEATURE_ZI_MT_HEBREW

#ifdef FEATURE_ZI_RAPID_HEBREW
   ,{'T', '9','E', 'Z', 'H', 'E', 'B', 'R', 'E', 'W',0}
#endif //FEATURE_ZI_RAPID_HEBREW

#ifdef FEATURE_ZI_MT_HINDI
   ,{'T', '9','M', 'T', 'H', 'I', 'N', 'D', 'I', 0}
#endif //FEATURE_ZI_MT_HINDI

#ifdef FEATURE_ZI_RAPID_HINDI
   ,{'T', '9', 'E', 'Z', 'H', 'I', 'N', 'D', 'I', 0}
#endif //FEATURE_ZI_RAPID_HINDI

#ifdef FEATURE_ZI_MT_THAI
   ,{'T', '9','M', 'T', 'T', 'H', 'A', 'I', 0}
#endif //FEATURE_ZI_MT_THAI

#ifdef FEATURE_ZI_RAPID_THAI
   ,{'T', '9','E', 'Z', 'T', 'H', 'A', 'I', 0}
#endif //FEATURE_ZI_RAPID_THAI

#ifdef FEATURE_ZI_MT_SPANISH
   ,{'T', '9','M', 'T', 'S', 'P', 'A', 'N', 'I', 'S', 'H',0}
#endif //FEATURE_ZI_MT_SPANISH

#ifdef FEATURE_ZI_RAPID_SPANISH
   ,{'T', '9','E', 'Z', 'S', 'P', 'A', 'N', 'I', 'S', 'H',0}
#endif //FEATURE_ZI_RAPID_SPANISH

#ifdef FEATURE_ZI_MT_PORTUGUESE
   ,{'T', '9','M', 'T', 'P', 'O', 'R', 'T', 'U', 'G', 'U','E', 'S', 'E',0}
#endif //FEATURE_ZI_MT_PORTUGUESE

#ifdef FEATURE_ZI_RAPID_PORTUGUESE
   ,{'T', '9','E', 'Z', 'P', 'O', 'R', 'T', 'U', 'G', 'U','E', 'S', 'E',0}
#endif //FEATURE_ZI_RAPID_PORTUGUESE

#ifdef FEATURE_ZI_MT_INDONESIAN
   ,{'T', '9','M', 'T', 'I', 'N', 'D', 'O', 'N', 'E', 'S','I', 'A', 'N',0}
#endif //FEATURE_ZI_MT_INDONESIAN

#ifdef FEATURE_ZI_RAPID_INDONESIAN
   ,{'T', '9','E', 'Z', 'I', 'N', 'D', 'O', 'N', 'E', 'S','I', 'A', 'N',0}
#endif //FEATURE_ZI_RAPID_INDONESIAN

#ifdef FEATURE_ZI_MT_VIETNAMESE
   ,{'T', '9','M', 'T', 'N', 'I', 'E', 'T', 'N', 'A', 'M','E', 'S', 'E',0}
#endif //FEATURE_ZI_MT_VIETNAMESE

#ifdef FEATURE_ZI_RAPID_VIETNAMESE
   ,{'T', '9','E', 'Z', 'N', 'I', 'E', 'T', 'N', 'A', 'M','E', 'S', 'E',0}
#endif //FEATURE_ZI_RAPID_VIETNAMESE

#ifdef FEATURE_ZI_MT_FRENCH
   ,{'T', '9','M', 'T', 'F', 'R', 'E', 'N', 'C', 'H',0}
#endif //FEATURE_ZI_MT_FRENCH

#ifdef FEATURE_ZI_RAPID_FRENCH
   ,{'T', '9','E', 'Z', 'F', 'R', 'E', 'N', 'C', 'H',0}
#endif //FEATURE_ZI_RAPID_FRENCH

#ifdef FEATURE_ZI_JAPANESE
   ,{'T', '9','J', 'A', 'P', 'A', 'N', 'E', 'S', 'E',0}
#endif //FEATURE_ZI_JAPANESE

#ifdef FEATURE_ZI_KOREAN
   ,{'T', '9','K', 'O', 'R', 'E', 'A', 'N', 0}
#endif //FEATURE_ZI_KOREAN

#ifdef FEATURE_ZI_ITALIAN
   ,{'T', '9','I', 'T', 'A', 'L', 'I', 'A',  'N',0}
#endif //FEATURE_ZI_ITALIAN
};
#endif

//===============================================================================
// Show symbol list
//===============================================================================

/* This list of symbols needs to come out of a resource instead.
*/

#ifdef FEATURE_LANG_THAI
static const AECHAR sszSymbolListTH[] =
{
	0xE30,0xE31,0xE32,0xE33,0xE34,0xE35,
	0xE36,0xE37,0xE38,0xE39,0xE40,0xE41,
	0xE42,0xE43,0xE44,0xE4C,0xE47,0xE48,
	0xE49,0xE4A,0xE4B,0xE2F,0xE46,0
		
};
#endif


static const AECHAR sszSymbolList[] =
{
#if defined FEATURE_CARRIER_CHINA_TELCOM
    0x002e, 0x002d, 0x002c, 0x002b, 0x002a, 0x0029, 
    0x0028, 0x0027, 0x0026, 0x0025, 0x0024, 0x0023, 
    0x0022, 0x0021, 0x002f, 0x003a, 0x003b, 0x0020, 
    0x003c, 0x003d, 0x003e, 0x003f, 0x0040, 0x005b, 
    0x005c, 0x005d, 0x005e, 0x005f, 0x0060, 0x007b, 
    0x007c, 0x007d, 0x007e, 0x00d7, 0x00f7, 0x00a3, 
    0x00a5, 0x00b1, 0x00f7, 0x2018, 0x2019, 0x201c, 
    0x201d, 0x2014, 0x2026, 0x3002, 0x300a, 0x300b, 
    0xff01, 0xff08, 0xff09, 0xff0c, 0xff1a, 0xff1b, 
    0xff1f, 0xff3e, 0xff5c, 0xff5e, 0xffe5, 0x00a7,       
#else
    ',','.','@','\'','(',')','_','!',' ','?',
    '*','#','%','\"',':','+','<','>','=',
    '/','&','-','$',';',
#endif
#if (!defined FEATURE_CARRIER_TELEFONICA_VENEZUELA) \
        &&(!defined FEATURE_PERU_VERSION) \
        &&(!defined FEATURE_CARRIER_CHINA_TELCOM)
       '\\','~','{','}','[',']','^',
#endif /* FEATURE_CARRIER_TELEFONICA_VENEZUELA */
#if defined(FEATURE_LANG_PORTUGUESE) ||defined(FEATURE_LANG_SPANISH) ||defined(FEATURE_INPUTMODE_INDONESIAN)
       0x00A3,0x00A5, 0x00BF,0x00A1, 
#endif
//#ifdef FEATURE_LANG_ARABIC
#if defined(FEATURE_LANG_ARABIC) ||defined(FEATURE_LANG_FRENCH)
       0x061f,0x060c,0x061b,//0x20AC,0x00A3,
       //0x00A5,0x00A4,0x00BF,0x00A1,0x00A7,
#endif /*FEATURE_LANG_ARABIC*/
   '\n',  // add the newline on 090204
   0
};
#if defined FEATURE_CARRIER_CHINA_TELCOM
// 1.En/Ab inputmethod: the symbol is English symbol
static const AECHAR sszSymbolListEn[] = 
{
    0x002e, 0x002d, 0x002c, 0x002b, 0x002a, 0x0029, 
    0x0028, 0x0027, 0x0026, 0x0025, 0x0024, 0x0023, 
    0x0022, 0x0021, 0x002f, 0x003a, 0x003b, 0x0020, 
    0x003c, 0x003d, 0x003e, 0x003f, 0x0040, 0x005b, 
    0x005c, 0x005d, 0x005e, 0x005f, 0x0060, 0x007b, 
    0x007c, 0x007d, 0x007e,
    0
};
//  2.Pin/Stroke inputmethod: the symbol is Chinese symbol
static const AECHAR sszSymbolListCn[] = 
{
    0xff0c, 0x3001, 0x3002, 0xff1a, 0xff1b, 0xff01,
    0xff1f, 0x2014, 0x2026, 0x201c, 0x201d, 0x2018,
    0x2019, 0x3008, 0x3009, 0x300a, 0x300b, 0xff08,
    0xff09, 0x300c, 0x300d, 0x300e, 0x300f, 0x3010,
    0x3011, 0xff5e, 0x2500, 0x2502, 0x250c, 0x2510,
    0x2514, 0x2518, 0x251c, 0x2524, 0x2534, 0x253c,
    0x25cb, 0x25ce, 0x25cf, 0xff07, 0x203b, 0x00b1,
    0x00a7, 0x2190, 0x2191, 0x2192, 0x2193, 0x2195,
    0x3014, 0x3015, 0xff20, 0xff02, 0xff03, 0xff04,
    0xff05, 0xff06, 0xfe3f, 0xfe5d, 0xfe5e, 0xff0a,
    0xff0b, 0xff3f, 0xff1c, 0xff1d, 0xff1e, 0xff0e,
    0xff0d, 0xff0f, 0xff3b, 0xff3c, 0xff3d, 0xff3e,
    0xff40, 0xff5b, 0xff5c, 0xff5d, 0xffe0, 0xffe1,
    0xffe2, 0xffe3, 0xffe4, 0xffe5, 0x00a5, 0x00d7,
    0x00f7, 0x2571, 0x2572,
    0
};
#endif // FEATURE_CARRIER_CHINA_TELCOM

//===============================================================================
// Show face symbol list
//===============================================================================
static const AEETextInputModeInfo sBREWTextModes_en[] =
{
   {AEE_TM_SYMBOLS,{'S','y','m','b','o','l','s',0}},
   {AEE_TM_NET_SYMBOL,{'N','e','t',0}},
   {AEE_TM_FACE_SYMBOL,{'F','a','c','e',0}},
   {AEE_TM_RESERVE_LAST, {0}}
};

static const AEETextInputModeInfo sBREWTextModes_cn[] =
{
   {AEE_TM_SYMBOLS,{0x7b26,0x53f7,0}},
   {AEE_TM_RESERVE_LAST, {0}}
};

static const AEETextInputModeInfo sBREWTextModes_net[] =
{
   {AEE_TM_NET_SYMBOL,{'N','e','t',0}},
   {AEE_TM_RESERVE_LAST, {0}}
};

static const AEETextInputModeInfo sBREWTextModes_face[] =
{
   {AEE_TM_FACE_SYMBOL,{'F','a','c','e',0}},
   {AEE_TM_RESERVE_LAST, {0}}
};

#define MAX_FACE_SYMBOL          (20)//缃戠粶绗﹀彿鐨勬�讳釜鏁?

static const AECHAR   sszFaceSymbol0[] =
{
    ':', ')', 0
};

static const AECHAR   sszFaceSymbol1[] =
{
    ':', '-', '(',  0
};

static const AECHAR   sszFaceSymbol2[] =
{
    ':', 'o', 'i', 'c',':',0
};

static const AECHAR   sszFaceSymbol3[] =
{
    ':', '-', 'D', 0
};

static const AECHAR   sszFaceSymbol4[] =
{
    ':', 'o', 0
};

static const AECHAR   sszFaceSymbol5[] =
{
    ';', '-', 'O', 0
};

static const AECHAR   sszFaceSymbol6[] =
{
    ':', '(', 0
};

static const AECHAR   sszFaceSymbol7[] =
{
    '?', '_', '?', 0
};

static const AECHAR   sszFaceSymbol8[] =
{
    ':', '-', 'Q', 0
};

static const AECHAR   sszFaceSymbol9[] =
{
    '<', ':', 'I', 0
};

static const AECHAR   sszFaceSymbol10[] =
{
    'x', '-', '<', 0
};

static const AECHAR   sszFaceSymbol11[] =
{
    '{', '(', ':', 0
};


static const AECHAR   sszFaceSymbol12[] =
{
    ':', '-', '$', 0
};

static const AECHAR   sszFaceSymbol13[] =
{
    '<', '@', '-', '@', '>', 0
};

static const AECHAR   sszFaceSymbol14[] =
{
    '@', '%', '&', '$', '%', '&', 0
};

static const AECHAR   sszFaceSymbol15[] =
{
    '(', ':', '-', '*', 0
};

static const AECHAR   sszFaceSymbol16[] =
{
    '>', ':', '-',  '<', 0
};

static const AECHAR   sszFaceSymbol17[] =
{
    ':', '-', 'x', 0
};

static const AECHAR   sszFaceSymbol18[] =
{
    ',', '_', ')', 0
};

static const AECHAR   sszFaceSymbol19[] =
{
    ';', 'w', 'a', 'v', '1', 0
};

static const AECHAR *const sszFaceSymbolList[MAX_FACE_SYMBOL] =
{
   sszFaceSymbol0, sszFaceSymbol1, sszFaceSymbol2, sszFaceSymbol3,sszFaceSymbol4,
   sszFaceSymbol5, sszFaceSymbol6, sszFaceSymbol7, sszFaceSymbol8,sszFaceSymbol9,
   sszFaceSymbol10, sszFaceSymbol11, sszFaceSymbol12, sszFaceSymbol13,sszFaceSymbol14,   
   sszFaceSymbol15, sszFaceSymbol16, sszFaceSymbol17, sszFaceSymbol18,sszFaceSymbol19  
};

//===============================================================================
// Show net symbol list
//===============================================================================
#if defined(FEATURE_LANG_ENGLISH) && defined(FEATURE_LANG_TCHINESE) && \
    defined(_ZI_NUMBER_MODES)

#define MAX_NET_SYMBOL          (20)//鍙版咕鐗堟湰缃戠粶绗﹀彿鐨勬�讳釜鏁?

static const AECHAR   sszNetSymbol0[] =
{
    '.', 'c', 'o', 'm', 0
};

static const AECHAR   sszNetSymbol1[] =
{
    '.', 'c', 'o', 'm', '.', 't', 'w', 0
};

static const AECHAR   sszNetSymbol2[] =
{
    '.', 'n', 'e', 't', 0
};

static const AECHAR   sszNetSymbol3[] =
{
    '.', 'n', 'e', 't', '.', 't', 'w', 0
};

static const AECHAR   sszNetSymbol4[] =
{
    '.', 'h', 'i', 'n', 'e', 't', '.', 'n', 'e', 't', 0
};

static const AECHAR   sszNetSymbol5[] =
{
    '.', 'g', 'o', 'v', '.', 't', 'w', 0
};

static const AECHAR   sszNetSymbol6[] =
{
    '.', 'g', 'o', 'v', 0
};
static const AECHAR   sszNetSymbol7[] =
{
    '.', 'o', 'r', 'g', 0
};

static const AECHAR   sszNetSymbol8[] =
{
    '.', 'o', 'r', 'g', '.', 't', 'w', 0
};

static const AECHAR   sszNetSymbol9[] =
{
    'h', 't', 't', 'p', ':', '/', '/', 0
};

static const AECHAR   sszNetSymbol10[] =
{
    'h', 't', 't', 'p', ':', '/', '/', 'w', 'w', 'w', '.', 0
};

static const AECHAR   sszNetSymbol11[] =
{
    's', 'i', 'n', 'a', '.', 'c', 'o', 'm', '.', 't', 'w', 0
};

static const AECHAR   sszNetSymbol12[] =
{
    'g', 'o', 'o', 'g', 'l', 'e', '.', 'c', 'o', 'm', '.', 't', 'w', 0
};

static const AECHAR   sszNetSymbol13[] =
{
    'y', 'a', 'h', 'o', 'o', '.', 'c', 'o', 'm', '.', 't', 'w', 0
};
static const AECHAR   sszNetSymbol14[] =
{
    '@', 's', 'i', 'n', 'a', '.', 'c', 'o', 'm', '.', 't', 'w', 0
};

static const AECHAR   sszNetSymbol15[] =
{
    '@', 'g', 'm', 'a', 'i', 'l', '.', 'c', 'o', 'm', 0
};

static const AECHAR   sszNetSymbol16[] =
{
    '@', 'y', 'a', 'h', 'o', 'o', '.', 'c', 'o', 'm', '.', 't', 'w', 0
};

static const AECHAR   sszNetSymbol17[] =
{
    '@', 'h', 'o', 't', 'm', 'a', 'i', 'l', '.', 'c', 'o', 'm', 0
};

static const AECHAR   sszNetSymbol18[] =
{
    '@', 'p', 'c', 'h', 'o', 'm', 'e', '.', 'c', 'o', 'm', '.', 't', 'w', 0
};

static const AECHAR   sszNetSymbol19[] =
{
    '@', 'm', 's', 'n', '.', 'c', 'o', 'm', 0
};

static const AECHAR *const sszNetSymbolList[MAX_NET_SYMBOL] =
{
   sszNetSymbol0, sszNetSymbol1,sszNetSymbol2, sszNetSymbol3, 
   sszNetSymbol4, sszNetSymbol5,sszNetSymbol6, sszNetSymbol7,
   sszNetSymbol8, sszNetSymbol9,sszNetSymbol10, sszNetSymbol11, 
   sszNetSymbol12, sszNetSymbol13,sszNetSymbol14, sszNetSymbol15,
   sszNetSymbol16, sszNetSymbol17,sszNetSymbol18, sszNetSymbol19
};

#elif (defined(FEATURE_CARRIER_THAILAND_CAT) ) || (defined(FEATURE_CARRIER_THAILAND_HUTCH) )

#define MAX_NET_SYMBOL          (5)//缃戠粶绗﹀彿鐨勬�讳釜鏁?

static const AECHAR   sszNetSymbol0[] =
{
    'w', 'w', 'w', '.', 0
};

static const AECHAR   sszNetSymbol1[] =
{
    '.', 'c', 'o', 'm', 0
};

static const AECHAR   sszNetSymbol2[] =
{
    '.', 'c', 'o', '.', 't', 'h', 0
};

static const AECHAR   sszNetSymbol3[] =
{
    '.', 'n', 'e', 't', 0
};

static const AECHAR   sszNetSymbol4[] =
{
    'h', 't', 't', 'p', ':', '/', '/', 0
};

static const AECHAR *const sszNetSymbolList[MAX_NET_SYMBOL] =
{
   sszNetSymbol0, sszNetSymbol1,sszNetSymbol2, sszNetSymbol3, 
   sszNetSymbol4
};



#elif defined(FEATURE_CARRIER_VENEZUELA_MOVILNET)

#define MAX_NET_SYMBOL          (6)//缃戠粶绗﹀彿鐨勬�讳釜鏁?

static const AECHAR   sszNetSymbol0[] =
{
    'w', 'w', 'w', '.', 0
};

static const AECHAR   sszNetSymbol1[] =
{
    '.', 'c', 'o', 'm', 0
};

static const AECHAR   sszNetSymbol2[] =
{
    '.', 'c', 'o', 'm', '.', 'v', 'e', 0
};

static const AECHAR   sszNetSymbol3[] =
{
    '.', 'n', 'e', 't', 0
};

static const AECHAR   sszNetSymbol4[] =
{
    '.', 'n', 'e', 't', '.', 'v', 'e', 0
};

static const AECHAR   sszNetSymbol5[] =
{
    'h', 't', 't', 'p', ':', '/', '/', 0
};

static const AECHAR *const sszNetSymbolList[MAX_NET_SYMBOL] =
{
   sszNetSymbol0, sszNetSymbol1,sszNetSymbol2, sszNetSymbol3, 
   sszNetSymbol4, sszNetSymbol5
};
#elif defined(FEATURE_PREPAID_ISRAEL_HEBREW)

#define MAX_NET_SYMBOL          (6)//缃戠粶绗﹀彿鐨勬�讳釜鏁?

static const AECHAR   sszNetSymbol0[] =
{
    'w', 'w', 'w', '.', 0
};

static const AECHAR   sszNetSymbol1[] =
{
    '.', 'c', 'o', 'm', 0
};

static const AECHAR   sszNetSymbol2[] =
{
    '.', 'c', 'o', '.', 'i', 'l', 0
};

static const AECHAR   sszNetSymbol3[] =
{
    '.', 'n', 'e', 't', 0
};

static const AECHAR   sszNetSymbol4[] =
{
    '.', 'n', 'e', 't', '.', 'i', 'l', 0
};

static const AECHAR   sszNetSymbol5[] =
{
    'h', 't', 't', 'p', ':', '/', '/', 0
};

static const AECHAR *const sszNetSymbolList[MAX_NET_SYMBOL] =
{
   sszNetSymbol0, sszNetSymbol1,sszNetSymbol2, sszNetSymbol3, 
   sszNetSymbol4, sszNetSymbol5
};
#elif defined(FEATURE_CARRIER_SUDAN_SUDATEL)
#define MAX_NET_SYMBOL          (7)//缃戠粶绗﹀彿鐨勬�讳釜鏁?

static const AECHAR   sszNetSymbol0[] =
{
    'w', 'w', 'w', '.', 0
};

static const AECHAR   sszNetSymbol1[] =
{
    '.', 'c', 'o', 'm', 0
};

static const AECHAR   sszNetSymbol2[] =
{
    '.', 'c', 'o', 'm', '.', 's', 'd', 0
};

static const AECHAR   sszNetSymbol3[] =
{
    '.', 'n', 'e', 't', 0
};

static const AECHAR   sszNetSymbol4[] =
{
    '.', 'n', 'e', 't', '.', 's', 'd', 0
};

static const AECHAR   sszNetSymbol5[] =
{
    'h', 't', 't', 'p', ':', '/', '/', 0
};

static const AECHAR   sszNetSymbol6[] =
{
    'c', 'u', 'm', 'a', 'i', 'l', '.', 'c', 'o', 'm', '.', 's', 'd', 0
};

static const AECHAR *const sszNetSymbolList[MAX_NET_SYMBOL] =
{
   sszNetSymbol0, sszNetSymbol1,sszNetSymbol2, sszNetSymbol3, 
   sszNetSymbol4, sszNetSymbol5,sszNetSymbol6
};
#elif defined(FEATURE_CARRIER_INDONESIA)
#define MAX_NET_SYMBOL          (7)//缃戠粶绗﹀彿鐨勬�讳釜鏁?

static const AECHAR   sszNetSymbol0[] =
{
    'w', 'w', 'w', '.', 0
};

static const AECHAR   sszNetSymbol1[] =
{
    '.', 'c', 'o', 'm', 0
};

static const AECHAR   sszNetSymbol2[] =
{
    '.', 'c', 'o', 'm', '.', 'i', 'd', 0
};

static const AECHAR   sszNetSymbol3[] =
{
    '.', 'n', 'e', 't', 0
};

static const AECHAR   sszNetSymbol4[] =
{
    '.', 'n', 'e', 't', '.', 'i', 'd', 0
};

static const AECHAR   sszNetSymbol5[] =
{
    'h', 't', 't', 'p', ':', '/', '/', 0
};

static const AECHAR   sszNetSymbol6[] =
{
    'c', 'u', 'm', 'a', 'i', 'l', '.', 'c', 'o', 'm', '.', 'i', 'd', 0
};

static const AECHAR *const sszNetSymbolList[MAX_NET_SYMBOL] =
{
   sszNetSymbol0, sszNetSymbol1,sszNetSymbol2, sszNetSymbol3, 
   sszNetSymbol4, sszNetSymbol5,sszNetSymbol6
};
#elif defined(FEATURE_CARRIER_ANGOLA_MOVICEL)
#define MAX_NET_SYMBOL          (7)//缃戠粶绗﹀彿鐨勬�讳釜鏁?

static const AECHAR   sszNetSymbol0[] =
{
    'w', 'w', 'w', '.', 0
};

static const AECHAR   sszNetSymbol1[] =
{
    '.', 'c', 'o', 'm', 0
};

static const AECHAR   sszNetSymbol2[] =
{
    '.', 'c', 'o', 'm', '.', 'a', 'o', 0
};

static const AECHAR   sszNetSymbol3[] =
{
    '.', 'n', 'e', 't', 0
};

static const AECHAR   sszNetSymbol4[] =
{
    '.', 'n', 'e', 't', '.', 'a', 'o', 0
};

static const AECHAR   sszNetSymbol5[] =
{
    'h', 't', 't', 'p', ':', '/', '/', 0
};

static const AECHAR   sszNetSymbol6[] =
{
    'c', 'u', 'm', 'a', 'i', 'l', '.', 'c', 'o', 'm', '.', 'a', 'o', 0
};

static const AECHAR *const sszNetSymbolList[MAX_NET_SYMBOL] =
{
   sszNetSymbol0, sszNetSymbol1,sszNetSymbol2, sszNetSymbol3, 
   sszNetSymbol4, sszNetSymbol5,sszNetSymbol6
};
#elif defined(FEATURE_CARRIER_MAROC_WANA)
#define MAX_NET_SYMBOL          (7)//缃戠粶绗﹀彿鐨勬�讳釜鏁?

static const AECHAR   sszNetSymbol0[] =
{
    'w', 'w', 'w', '.', 0
};

static const AECHAR   sszNetSymbol1[] =
{
    '.', 'c', 'o', 'm', 0
};

static const AECHAR   sszNetSymbol2[] =
{
    '.', 'c', 'o', 'm', '.', 'a', 'm', 0
};

static const AECHAR   sszNetSymbol3[] =
{
    '.', 'n', 'e', 't', 0
};

static const AECHAR   sszNetSymbol4[] =
{
    '.', 'n', 'e', 't', '.', 'a', 'm', 0
};

static const AECHAR   sszNetSymbol5[] =
{
    'h', 't', 't', 'p', ':', '/', '/', 0
};

static const AECHAR   sszNetSymbol6[] =
{
    'c', 'u', 'm', 'a', 'i', 'l', '.', 'c', 'o', 'm', '.', 'a', 'm', 0
};

static const AECHAR *const sszNetSymbolList[MAX_NET_SYMBOL] =
{
   sszNetSymbol0, sszNetSymbol1,sszNetSymbol2, sszNetSymbol3, 
   sszNetSymbol4, sszNetSymbol5,sszNetSymbol6
};
#elif defined(FEATURE_CARRIER_CHINA_TELCOM)
#define MAX_NET_SYMBOL          (8)//缃戠粶绗﹀彿鐨勬�讳釜鏁?

static const AECHAR   sszNetSymbol0[] =
{
    'w', 'w', 'w', '.', 0
};

static const AECHAR   sszNetSymbol1[] =
{
    '.', 'c', 'o', 'm', 0
};

static const AECHAR   sszNetSymbol2[] =
{
    '.', 'c', 'o', 'm', '.', 'c', 'n', 0
};

static const AECHAR   sszNetSymbol3[] =
{
    '.', 'n', 'e', 't', 0
};

static const AECHAR   sszNetSymbol4[] =
{
    '.', 'n', 'e', 't', '.', 'c', 'n', 0
};

static const AECHAR   sszNetSymbol5[] =
{
    '.', 'c', 'n', 0
};

static const AECHAR   sszNetSymbol6[] =
{
    'h', 't', 't', 'p', ':', '/', '/', 0
};

static const AECHAR   sszNetSymbol7[] =
{
    'c', 'u', 'm', 'a', 'i', 'l', '.', 'c', 'o', 'm', '.', 'c', 'n', 0
};

static const AECHAR *const sszNetSymbolList[MAX_NET_SYMBOL] =
{
   sszNetSymbol0, sszNetSymbol1,sszNetSymbol2, sszNetSymbol3, 
   sszNetSymbol4, sszNetSymbol5,sszNetSymbol6, sszNetSymbol7,
};

#else

#define MAX_NET_SYMBOL          (7)//缃戠粶绗﹀彿鐨勬�讳釜鏁?

static const AECHAR   sszNetSymbol0[] =
{
    'w', 'w', 'w', '.', 0
};

static const AECHAR   sszNetSymbol1[] =
{
    '.', 'c', 'o', 'm', 0
};

static const AECHAR   sszNetSymbol2[] =
{
    '.', 'c', 'o', 'm', '.', 'c', 'n', 0
};

static const AECHAR   sszNetSymbol3[] =
{
    '.', 'n', 'e', 't', 0
};

static const AECHAR   sszNetSymbol4[] =
{
    '.', 'n', 'e', 't', '.', 'c', 'n', 0
};

static const AECHAR   sszNetSymbol5[] =
{
    'h', 't', 't', 'p', ':', '/', '/', 0
};

static const AECHAR   sszNetSymbol6[] =
{
    'c', 'u', 'm', 'a', 'i', 'l', '.', 'c', 'o', 'm', '.', 'c', 'n', 0
};

static const AECHAR *const sszNetSymbolList[MAX_NET_SYMBOL] =
{
   sszNetSymbol0, sszNetSymbol1,sszNetSymbol2, sszNetSymbol3, 
   sszNetSymbol4, sszNetSymbol5,sszNetSymbol6
};

#endif

#ifdef FEATURE_ZI_MT_ENGLISH
//===============================================================================
// Show multitap english list
//===============================================================================
static const AECHAR sszMultitapShift0[] =
{
   'm','u','l','t','i','t','a','p',0
};

static const AECHAR sszMultitapShift1[] =
{
   'M','u','l','t','i','t','a','p',0
};

static const AECHAR sszMultitapShift2[] =
{
   'M','U','L','T','I','T','A','P',0
};

static const AECHAR* const sszMultitaptapNoShift[3]=
{
   //use English mode name for all other language
   englishTitle[0], englishTitle[0],englishTitle[0]
};

static const AECHAR *const saMultitapShiftNames[3] =
{
   sszMultitapShift0, sszMultitapShift1, sszMultitapShift2
};

static const AECHAR saMultitapStrings_3sdtyle_endsymbol[] =
{
    '.','!','?',0
};

static const AECHAR sszMultitap0TP0[] =
{
   '0','+','T','P','.', 0
};
static const AECHAR sszMultitap0TP1[] =
{
   '0','T','P','.', 0
};

#if defined(FEATURE_LANG_PORTUGUESE)

static const AECHAR sszMultitap1[] =
{
   '.',',','-','?','!',0x0027,'@',':','1',0
};

static const AECHAR sszMultitap2[] =
{
   'a','b','c',0x00E0,0x00E1,0x00E2,0x00E3,0x00E4,0x00E7,'2',
   'A','B','C',0x00C0,0x00C1,0x00C2,0x00C3,0x00C4,0x00C7,0
};

static const AECHAR sszMultitap3[] =
{
   'd','e','f',0x00E9,0x00EA,'3',
   'D','E','F',0x00C9,0x00CA,0
};

static const AECHAR sszMultitap4[] =
{
   'g','h','i',0x00ED,'4','G','H','I',0x00CD,0
};

static const AECHAR sszMultitap5[] =
{
   'j','k','l','5','J','K','L',0
};

static const AECHAR sszMultitap6[] =
{
   'm','n','o',0x00F2,0x00F3,0x00F4,0x00F5,'6', // 0x00F1,0x00F6,
   'M','N','O',0x00D2,0x00D3,0x00D4,0x00D5,0   // 0x00D1,0x00D6,
};

static const AECHAR sszMultitap7[] =
{
   'p','q','r','s','7','P','Q','R','S',0
};

static const AECHAR sszMultitap8[] =
{
   't','u','v',0x00FA,0x00FB,0x00FC,'8',
   'T','U','V',0x00DA,0x00DB,0x00DC,0
};

static const AECHAR sszMultitap9[] =
{
   'w','x','y','z','9','W','X','Y','Z',0
};

static const AECHAR sszMultitap0[] =
{
   ' ','0',0
};

#else

static const AECHAR sszMultitap1[] =
{
   '.','@','-','_','/',':','~','?',',','1',0
};

static const AECHAR sszMultitap2[] =
{
   'a','b','c','2','A','B','C',0
};

static const AECHAR sszMultitap3[] =
{
   'd','e','f','3','D','E','F',0
};

static const AECHAR sszMultitap4[] =
{
   'g','h','i','4','G','H','I',0
};

static const AECHAR sszMultitap5[] =
{
   'j','k','l','5','J','K','L',0
};

static const AECHAR sszMultitap6[] =
{
   'm','n','o','6','M','N','O',0
};

static const AECHAR sszMultitap7[] =
{
   'p','q','r','s','7','P','Q','R','S',0
};

static const AECHAR sszMultitap8[] =
{
   't','u','v','8','T','U','V',0
};

static const AECHAR sszMultitap9[] =
{
   'w','x','y','z','9','W','X','Y','Z',0
};

static const AECHAR sszMultitap0[] =
{
   ' ','0',0
};
#endif

static const AECHAR *const saMultitapStrings[10] =
{
   sszMultitap0, sszMultitap1, sszMultitap2, sszMultitap3, sszMultitap4,
   sszMultitap5, sszMultitap6, sszMultitap7, sszMultitap8, sszMultitap9
};

#endif //FEATURE_ZI_MT_ENGLISH


#ifdef FEATURE_ZI_MT_SPANISH
static const AECHAR esszMultitap1[] =
{
   '.',',','-',0x00BF,'?','!',0x00A1,'@',':','1',0 
};

static const AECHAR esszMultitap2[] =
{
   'a','b','c',0x00E1,0x00E3,0x00E7,'2',
   'A','B','C',0x00C1,0x00C3,0x00C7,0
};

static const AECHAR esszMultitap3[] =
{
   'd','e','f',0x00E9,'3',
   'D','E','F',0x00C9,0
};

static const AECHAR esszMultitap4[] =
{
   'g','h','i',0x00ED,'4','G','H','I',0x00CD,0
};

static const AECHAR esszMultitap5[] =
{
   'j','k','l','5','J','K','L',0
};

static const AECHAR esszMultitap6[] =
{
   'm','n','o',0x00F3,0x00F1,'6',
   'M','N','O',0x00D3,0x00D1,0 
};

static const AECHAR esszMultitap7[] =
{
   'p','q','r','s','7','P','Q','R','S',0
};

static const AECHAR esszMultitap8[] =
{
   't','u','v',0x00FA,0x00FC,'8',
   'T','U','V',0x00DA,0x00DC,0
};

static const AECHAR esszMultitap9[] =
{
   'w','x','y','z','9','W','X','Y','Z',0
};

static const AECHAR esszMultitap0[] =
{
   ' ','0',0
};

static const AECHAR *const esaMultitapStrings[10] =
{
   esszMultitap0, esszMultitap1, esszMultitap2, esszMultitap3, esszMultitap4,
   esszMultitap5, esszMultitap6, esszMultitap7, esszMultitap8, esszMultitap9
};

#endif // FEATURE_ZI_MT_SPANISH

static const ZI8WCHAR   g_ezi_onekeys[MMI_IMC_SYMB_KEY_MAX - MMI_IMC_SYMB_KEY_BASE] =
{
#if defined(__MMI_ZI_CHINESE_PHRASE_LEVEL_INPUTS__)
    ZI8_CODE_SEPARATOR, // MMI_IMC_SYMB_KEY_0
#else
    ZI8_ONEKEY_KEY0, // MMI_IMC_SYMB_KEY_0
#endif
    ZI8_ONEKEY_KEY1, // MMI_IMC_SYMB_KEY_1
    ZI8_ONEKEY_KEY2, // MMI_IMC_SYMB_KEY_2
    ZI8_ONEKEY_KEY3, // MMI_IMC_SYMB_KEY_3
    ZI8_ONEKEY_KEY4, // MMI_IMC_SYMB_KEY_4
    ZI8_ONEKEY_KEY5, // MMI_IMC_SYMB_KEY_5
    ZI8_ONEKEY_KEY6, // MMI_IMC_SYMB_KEY_6
    ZI8_ONEKEY_KEY7, // MMI_IMC_SYMB_KEY_7
    ZI8_ONEKEY_KEY8, // MMI_IMC_SYMB_KEY_8
    ZI8_ONEKEY_KEY9, // MMI_IMC_SYMB_KEY_9
    0, //MMI_IMC_SYMB_KEY_STAR,
    0, //MMI_IMC_SYMB_KEY_POUND,
};

#ifdef FEATURE_ZI_MT_FRENCH
static const AECHAR frszMultitap1[] =
{
   '.',',','-','?','!',0x0027,'@',':','1',0 
};

static const AECHAR frszMultitap2[] =
{
   'a','b','c',0x00E7,0x00E0,0x00E2,0x00E1,0x00E4,'2',
   'A','B','C',0x00C7,0x00C0,0x00C2,0x00C1,0x00C4,0
};

static const AECHAR frszMultitap3[] =
{
   'd','e','f',0x00E9,0x00E8,0x00EA,0x00EB,'3',
   'D','E','F',0x00C9,0x00C8,0x00CA,0x00CB,0
};

static const AECHAR frszMultitap4[] =
{
   'g','h','i',0x00EE,0x00EF,0x00ED,'4',
   'G','H','I',0x00CE,0x00CF,0x00CD,0
};

static const AECHAR frszMultitap5[] =
{
   'j','k','l','5','J','K','L',0
};

static const AECHAR frszMultitap6[] =
{
   'm','n','o',0x00F4,0x00F3,0x00F6,0x00F1,'6',
   'M','N','O',0x00D4,0x00D3,0x00D6,0x00D1,0 
};

static const AECHAR frszMultitap7[] =
{
   'p','q','r','s','7','P','Q','R','S',0
};

static const AECHAR frszMultitap8[] =
{
   't','u','v',0x00F9,0x00FB,0x00FA,0x00FC,'8',
   'T','U','V',0x00D9,0x00DB,0x00DA,0x00DC,0
};

static const AECHAR frszMultitap9[] =
{
   'w','x','y','z','9','W','X','Y','Z',0
};

static const AECHAR frszMultitap0[] =
{
   ' ','0',0
};

static const AECHAR *const fraMultitapStrings[10] =
{
   frszMultitap0, frszMultitap1, frszMultitap2, frszMultitap3, frszMultitap4,
   frszMultitap5, frszMultitap6, frszMultitap7, frszMultitap8, frszMultitap9
};

#endif // FEATURE_ZI_MT_FRENCH

#ifdef FEATURE_ZI_MT_THAI
//===============================================================================
// Show special multitap thai list
//===============================================================================
#ifdef FEATURE_CARRIER_THAILAND_HUTCH   
static const AECHAR sszZIMultitap2SpecialThai[] =
{//0锛屽幓闄や笂鍏冮煶鍜屼笅鍏冮煶 none 0x0e31, 0x0e34, 0x0e35, 0x0e36, 0x0e37, 0x0e38, 0x0e39,
    0x0e01,0x0e02,0x0e04,0x0e06,0x0e07,0x0032,0
};
#endif //FEATURE_CARRIER_THAILAND_HUTCH

static const AECHAR sszZIMultitap0SpecialThai[] =
{//0锛屽幓闄や笂鍏冮煶鍜屼笅鍏冮煶 none 0x0e31, 0x0e34, 0x0e35, 0x0e36, 0x0e37, 0x0e38, 0x0e39,
    0x0030,0
};

static const AECHAR sszZIMultitapPound1SpecialThai[] =
{//#,鍘婚櫎闊虫爣 none 0x0e48, 0x0e49, 0x0e4a, 0x0e4b,
    0x0e46, 0x0e2f, 0x0e3f,0
};

static const AECHAR sszZIMultitapPound2SpecialThai[] =
{//#,鍘婚櫎涓婂厓闊?none 0x0e47,0x0e4c
    0x0e48, 0x0e49, 0x0e4a, 0x0e4b, 0x0e46, 0x0e2f, 0x0e3f,0
};
#endif//FEATURE_ZI_MT_THAI

#ifdef FEATURE_PREPAID_ISRAEL_HEBREW
static const AECHAR sszZIMultitap1SpecialHebrew[] =
{
    0x002e, 0x002c, 0x002d, 0x003f, 0x0021, 0x0027, 0x0022, 0x003a, 0x002b, 0x20aa, 0x002f, 0x0031,0
};
#endif  // FEATURE_PREPAID_ISRAEL_HEBREW

#ifdef FEATURE_CARRIER_CHINA_TELCOM
static const AECHAR sszZIMultitap1SpecialUnicom[] =
{
    0x002E, 0x0040, 0x002D, 0x005F, 0x002F,
    0x003A, 0x007E, 0x003F, 0x002C, 0x0031,0
};
#endif
//===============================================================================
// OTHER DEFINED
//===============================================================================        
#define MULTITAP_TIMEOUT        (1000)  // In milliseconds
#ifdef FEATURE_SUPPORT_ID
#define MAX_INPUTTEXT_SIZE      1537
#else
#define MAX_INPUTTEXT_SIZE      1024
#endif

typedef struct
{
    uint16 wID; // ID of the Text Mode.
    AECHAR pszTitle[MAX_MODE_STR+1]; // Name of the mode. This name is used in the menu.
}AEETextMode;


typedef enum {
   MULTITAP_ALL_SMALL,  // all small letters
   MULTITAP_FIRST_CAP,  // first letter capital
   MULTITAP_ALL_CAPS,   // all letters capital
   MULTITAP_MAX_CAP_STATE
} MultitapCapsState;

typedef enum {
   MULTITAP_AUTOSET_FIRSTCAP,
   MULTITAP_USER_DENY_AUTOSET,
   MULTITAP_MAX_AUTO_STATE
} MultitapAutoSetState;

typedef enum sFocusType {
   FOCUS_SELECTION,         // focus on Selection Page
   FOCUS_SYLLABLE,          // focus on Syllable
   FOCUS_TEXT,              // focus on Text and no selection page
   FOCUS_TEXT_PREDICTION,   // focus on the Text and display the T9 Selection Page
   FOCUS_MAX

}sFocusType;


typedef struct HzGroupInfoTag          /*根据拼音取得的一组汉字信息*/   
{   
	uint16        beginHzInLib;           /*这组的第一个汉字是LIB中的第几个*/   
	uint16        currentHzIndex;         /*这组的当前一个汉字是LIB中的第几个*/   
	uint16        endHzInLib;             /*这组的最后一个汉字是LIB中的第几个*/    
	uint16        pageNo;                 /*当前这一组是第几页*/   
	uint16        HzIndex;                //汉字的总和；
	ZI8WCHAR      currentSelHz[MAX_HZ_PAGE_SIZE+1];   /*将当前一组汉字全都保存在这里，用于显示*/     
	boolean       bNoNextPage;        /*判断一下后面还有没有汉字了，用来决定按向下箭头是否还要取字*/   
}HzGroupInfo;

typedef struct PyOrSEGroupInfoTag  /* 当前的一组拼音信息*/   
{   
	uint16    firstPosInLib;   /*这组的第一个拼音是LIB中的第几个*/   
	uint16    lastPosInLib;    /*这组的最后一个拼音是LIB中的第几个*/   
	uint16    currentPosInLib; /*当前选中拼音是LIB中的第几个*/     
	/*从ZI库里取出的一组拼音或SE，保存在全局变量里，这样在左右键选下一个拼音或SE的时候就不用再取值了，  
	最多的英文字母是32个吧*/   
	uint16 groupData[MAX_PY_PAGE_SIZE+1][33];   
	boolean  noNextPage;        /*判断一下后面还有没有拼音了，用来显示上下箭头*/   
	uint16   nmaxcountsize;     //拼音组合的总和个数
	uint16   PYpageNo;          //当前这组是第几页
	uint16   bNopageStart;      //第几个换行
}PyOrSEGroupInfo; 

//-------------------------------------------------------------------
// Global Constant Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Data Declarations
//-------------------------------------------------------------------

//===============================================================================
// FUNCTION DECLARATIONS and INLINE FUNCTION DEFINITIONS
//===============================================================================

OEMCONTEXT OEM_TextCreate(const IShell* pIShell, 
                            const IDisplay* pIDisplay,
                            const AEERect *pRect,
                            AEECLSID cls) ;

void OEM_TextDelete(OEMCONTEXT hTextField);


void OEM_TextSet(OEMCONTEXT hTextField, AECHAR *pszText,uint16 size);

AECHAR* OEM_TextGet(OEMCONTEXT hTextField);

void OEM_TextDraw(OEMCONTEXT hTextField);

void OEM_TextUpdate(OEMCONTEXT hTextField);

void OEM_TextGetModeString(OEMCONTEXT hTextField, AECHAR* szBuf, uint16 len) ;

void OEM_TextSetEdit(OEMCONTEXT hTextField, boolean bIsEditable, AEETextInputMode wmode);

AEETextInputMode OEM_TextGetCurrentMode(OEMCONTEXT hTextField) ;

void OEM_TextAddChar(OEMCONTEXT hTextField,AECHAR ch, boolean bOverStrike);
void OEM_TextAddString(OEMCONTEXT hTextField,AECHAR* string) ;

int OEM_TextQueryModes(AEETextMode** ppTextMode);

boolean OEM_TextKeyPress(OEMCONTEXT hTextField,
                         AEEEvent eCode,
                         uint32 dwKeyCode,
                         uint32 dwKeySyms) ;

uint32 OEM_TextGetProperties(OEMCONTEXT hTextField);

void OEM_TextSetProperties(OEMCONTEXT hTextField, uint32 dwProperties);

void OEM_TextGetRect(OEMCONTEXT hTextField, AEERect *pOutRect) ;

void OEM_TextSetRect(OEMCONTEXT hTextField, const AEERect *pInRect);

uint16 OEM_TextGetMaxChars(OEMCONTEXT hTextField) ;

void OEM_TextSetMaxChars(OEMCONTEXT hTextField, uint16 wMaxChars);

void OEM_TextSetSel(OEMCONTEXT hTextField, int iSelStart, int iSelEnd);

void OEM_TextGetSel(OEMCONTEXT hTextField, int *piSelStart, int *piSelEnd) ;

void OEM_TextEnumModesInit(void) ;

//Return true if this is a valid mode. Return false if we already reached end
boolean OEM_TextEnumMode(AEETextMode* pMode) ;

//uint16 OEM_TextQuerySymbols(AECHAR *pszOut, uint16 size) ;

int32 OEM_TextGetCursorPos(OEMCONTEXT hTextField);

void  OEM_TextSetCursorPos(OEMCONTEXT hTextField, int32 nOffset);

int32 OEM_TextGetZIEnd(OEMCONTEXT hTextField);

void OEM_TextSetMultiCaps(OEMCONTEXT hTextField,MultitapCapsState nCaps);

int OEM_TextGetFocusState(OEMCONTEXT hTextField);
int OEM_TextGetKeyBufLen(OEMCONTEXT hTextField);
MultitapCapsState OEM_TextGetMultiCaps(OEMCONTEXT hTextField);

boolean OEM_isFirstCap (OEMCONTEXT hTextField);

int32 OEM_TextGetSelEnd(OEMCONTEXT hTextField) ;
 /*quit the number method in add the contact number and ip number set on 080903*/
int32 OEM_ExitIputMethod(OEMCONTEXT hTextField);
 /* add the code end */
 
void OEM_TextSetBackGround(OEMCONTEXT hTextField, const IImage * pImageBg);
//#ifdef  WIN32   //add by ydc
void             OEM_TextDrawIMEDlg(OEMCONTEXT hTextField);   
boolean          OEM_HandlePenEvt(OEMCONTEXT hTextField,
								  AEEEvent eCode,
								  uint16 wLwrTime,
								  int16 wXPos,
								  int16 wYPos);
boolean          TSIM_ProcPenUp(OEMCONTEXT hTextCtl,int16 wXPos,int16 wYPos);
boolean          TSIM_ProcPenDown(OEMCONTEXT hTextCtl,
								  int16 xpos,
								  int16 ypos);

boolean          OEM_TextShiftStatus(OEMCONTEXT hTextField);
boolean          OEM_TextAltStatus(OEMCONTEXT hTextField);
boolean          OEM_TextCapStatus(OEMCONTEXT hTextField);
int32 OEM_TextGetEnd(OEMCONTEXT hTextField);
#ifdef FEATURE_MYANMAR_INPUT_MOD
void OEM_TextRestart(OEMCONTEXT hTextField);
boolean OEM_TextMyaStar(OEMCONTEXT hTextField);
#endif
uint16 app_ucs2_towupper(uint16 wc);
uint16 app_ucs2_towlower(uint16 wc);
//#endif

/*========================================================================
  INTERFACES DOCUMENTATION
==========================================================================

OEM_Text Interface

Description:
  This module provides a input text interface for the AEE. The reference
implementation can be modified to add more text input modes for foreign
languges or other input methods.

=======================================================================

Function: OEM_TextCreate()

Description:
   This function creates a dynamic text control object. It uses the given
   rectangle pRect to create the text control. The IShell and IDisplay
   pointers are saved in the newly created context to be used by the
   text control for notification, drawing, etc.

Prototype:
   OEMCONTEXT OEM_TextCreate(const IShell* pIShell,
                                                                  const IDisplay* pIDisplay,
                                                                  const AEERect *pRect,
                                                                  AEECLSID cls)

Parameters:
   pIShell:    const IShell *   Pointer to the IShell interface object

   pIDisplay:  const IDIsplay*  Pointer to the IDisplay interface object

   pRect:      const AEERect*   Pointer to the rectangle specifying the bounds
                                and location of the text control to be created.

   cls:        AEECLSID         Class ID to create. Should be stored to maintain
                                compatibility with older applets.


Return Value:
   If successful, a OEMCONTEXT which can be used as the handle to the
   newly created text control. If failed, returns NULL. If pIShell or
   pIDisplay or pRect is NULL, the function fails.

Comments:
   None

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextDelete()

Description:
   This function deletes a dynamic text control object. The text control
   must have been created successfully using OEM_TextCreate(). This function,
   also frees up memory and other resources (if any) associated with this
   text control.

Prototype:
   void OEM_TextDelete(OEMCONTEXT hTextField)

Parameters:
   htextField:    OEMCONTEXT   Handle for the text control object that needs
                               to be deleted.

Return Value:
   None

Comments:
   If hTextField is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextSet()

Description:
   This function sets the text of a given text control object.
   The text control must have been created successfully using
   OEM_TextCreate(). Once the text has been set, OEM_Textdraw()
   must be called to update the screen with the new text.

Prototype:
   boolean OEM_TextSet(OEMCONTEXT hTextField, const AECHAR *pszText, int nChars)

Parameters:
   htextField:    OEMCONTEXT   Handle for the text control object
   pszText:       AECHAR *     Text string that is to be set into the text control
   nChars:        int          Number of characters to set

Return Value:
   None

Comments:
   If htextField is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextGet()

Description:
   This function gets the text associated with a given text control object.
   It returns a pointer to the text present in the given text control object.

Prototype:
   AECHAR* OEM_TextGet(OEMCONTEXT hTextField)

Parameters:
   htextField:    OEMCONTEXT   Handle for the text control object

Return Value:
   On Success, returns pointer to the Text string in the text control.
   On failure, returns NULL.

Comments:
   If hTextCtl is NULL, the function returns NULL.

Side Effects:
   None

See Also:
   None
==========================================================================

Function: OEM_TextDraw()

Description:
   This function draws the text associated with a given text control object
   on the screen. This function also draws the associated items such as
   Scroll Bar, Border, Cursor etc. if  necessary and if supported.

Prototype:
   void OEM_TextDraw(OEMCONTEXT hTextField)

Parameters:
   htextField:    OEMCONTEXT   Handle for the text control object

Return Value:
   None.

Comments:
When the TP_PASSWORD property is set please display a text buffer of **** in place
of actual characters. You must maintain your original buffer of actual text.
Also when in multitap mode, please allow selection(ie. last character to be shown) while typing
Only * out the last character when it is committed to the text.  If hTextCtl is NULL, the
function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextUpdate()

Description:
   This function draws the text associated with a given text control object
   on the screen if the text control is modified.

Prototype:
   void OEM_TextUpdate(OEMCONTEXT hTextField)

Parameters:
   htextField:    OEMCONTEXT   Handle for the text control object

Return Value:
   None.

Comments:
   If htextField is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextGetModeString()

Description:
   This function returns the wide string corresponding to the current mode of
   the text control specified by hTextField. The possible mode strings are:
   "Multitap", "Numbers" and "Symbols".

Prototype:
   void OEM_TextGetModeString(OEMCONTEXT hTextField, AECHAR* szBuf, uint16 len)

Parameters:
   htextField:    OEMCONTEXT   Handle for the text control object [IN]
   szBuf:         AECHAR *     String corresponding to the mode of the text control [OUT]
   len:           uint16       Length of the mode string buffer [IN]

Return Value:
   None.

Comments:
   If hTextCtl or szBuf is NULL or if len is one or less, the function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextSetEdit()

Description:
   This function informs the text control whenever it goes into or out of focus.
   Typically, when the text control is in focus, the border and cursor are
   activated and when the text control goes out of focus, these items are
   de-activated. This function also informs the text control of the current
   text mode.

Prototype:
   void OEM_TextSetEdit(OEMCONTEXT hTextField,
                        boolean bIsEditable,
                        AEETextInputMode wmode
                       )

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object
   bIsEditable:   boolean           Flag to indicate if the text control
                                    object is in focus (is editable)
   wmode:         AEETextInputMode  Text input mode

Return Value:
   None.

Comments:
   If htextField is NULL, the function simply returns.
   If wmode is out of the valid range, please change the input mode to your
   default mode (reference code: AEE_TM_LETTERS)

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextGetCurrentMode()

Description:
   This function returns the current text mode of the text control specified
   by hTextField.

Prototype:
   AEETextInputMode OEM_TextGetCurrentMode(OEMCONTEXT hTextField)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object

Return Value:
  The current text mode for the text control specified.

Comments:
   None.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextAddChar()

Description:
   This function adds/overwrites a charecter at the current cursor location
   in the specified text control.

Prototype:
   void OEM_TextAddChar(OEMCONTEXT hTextField,AECHAR ch, boolean bOverStrike)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object
   ch:            AECHAR            Wide character to be added to the text control
   bOverStrike:   boolean           Overwrite the text at the cursor location.


Return Value:
   None.

Comments:
   Overstrike is only meaningful if there is an insertion point rather than a
   selection AND the insertion point is not at the very end of the text.  If
   hTextField is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextQueryModes()

Description:
   This function is invoked by the AEE to query the different text modes
   (ZI, MULTITAP, etc) supported by the OEM. This information is used by
   the AEE for two purposes:
   1) To populate the pop-up menu containing selections for the
      different modes supported.
   2) To notify the text control (using OEM_TextSetEdit()) of the mode
      selected by the user.

   The OEM must populate the given data structure and return from this function.


Prototype:
   int OEM_TextQueryModes(AEETextMode** ppTextMode)

Parameters:
   ppTextMode:    AEETextMode**    On return, this contains a valid pointer
                                   to an array of AEE_TextMode containing
                                   information about the different modes
                                   supported by the OEM. The OEM must use the
                                   standard identifier OEM_TEXT_MODE_ SYMBOLS
                                   for symbols mode. Memory for this pointer
                                   must be allocated by the OEM.


Return Value:
   Returns the number of text modes supported by the OEM.

Comments:
   Text  Modes Support:
   Here is a brief description of how text modes are supported.

   ?The AEE platform invokes the OEM function OEM_TextQueryModes() to get
     information on the different text modes supported by the OEM.
   ?The information obtained above is used to populate the menu containing
     selection strings for the different modes.
   ?When the user selects a particular mode, the function OEM_TextSetEdit()
     is invoked and is passed the ID of the mode that has been selected.
     If the user has not changed the mode, the ID is set to OEM_TEXT_MODE_DEFAULT,
     informing the OEM to use the currently selected mode.
   ?The OEM must use the standard ID OEM_TEXT_MODE_SYMBOLS for supporting the
     symbol mode. All other IDs must be based out of OEM_TEXT_MODE_USER.


Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextKeyPress()

Description:
   This function handles the key press events  in a text control.
   Whenever any key is pressed while a text control is active,
   this function is invoked, passing information relating to the
   key that has been pressed. The OEM must handle the key event and
   process it appropriately.

Prototype:
   boolean OEM_TextKeyPress(OEMCONTEXT hTextField,
                         AEEEvent eCode,
                                                                 uint32 dwKeyCode,
                                                                 uint32 dwKeySyms)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object
   eCode:         AEEEvent          Event code for the key event
   dwKeyCode:     uint32            Key code of the key that has been pressed
   dwKeySyms      uint32


Return Value:
  The current text mode for the text control specified.

Comments:
   Ensure to handle key navigation presses on a grapheme boundary.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextDialogEvt()

Description:
   This function handles dialog events of any corresponding dialogs that are
   shown to obtain screen space when inline text editing is not possible due
   to UI constraints.

Prototype:
  boolean OEM_TextDialogEvt(OEMCONTEXT hTextField,
                          AEEEvent eCode,
                                                                  uint16 wDlgID,
                                                                  uint32 dwDlgPtr)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object
   eCode:         AEEEvent          Event code for the dialog event
   wDlgID:        uint32            The dialog ID that sent the event
   dwDlgPtr       uint32            IDialog * for the dialog


Return Value:
  TRUE   Dialog event handled
  FALSE  Dialog event not handled

Comments:
   You must handle only dialog events based on the dialogs created by this layer.

Side Effects:
   Your dialog will have the input focus, reference code sets all events to this layer
   until the dialog is dismissed. This is highly recommended behavior, so events are
   not mixed between dialog and application.

See Also:
   None

==========================================================================

Function: OEM_TextHandlePenEvt()

Description:
   This function will be called ot handle pen events. Please follow the guidelines
   for handling pen events in a cooperative manner to ensure compatibility when
   used in dialogs and applications.

Prototype:
  boolean OEM_TextHandlePenEvt(OEMCONTEXT hTextField,
                           AEEEvent eCode,
                           uint16 wParam,
                           uint32 dwParam);

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object
   eCode:         AEEEvent          Pen Event code
   wParam:        uint16            wParam for EVT_POINTER_* events
   dwParam:       uint32            dwParam for EVT_POINTER_* events


Return Value:
  TRUE   Pen event handled, see documentation for expectations of IControl with pen events
  FALSE  Pen event not handled, see documentation for expectations of IControl with pen events

Comments:
   You must handle the pen events as described in the expectations of Pens with IControl.

Side Effects:
   None.

See Also:
   None

==========================================================================

Function: OEM_TextGetProperties()

Description:
   This function returns the properties of the text control, such as frame type,
   multiline, or rapid entry (such as ZI).

Prototype:
   uint32 OEM_TextGetProperties(OEMCONTEXT hTextField)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object

Return Value:
   Property of the text control, which could be combination of 1 or more
   of the following:


Comments:
Important properties
    TP_MULTILINE :  If set, text control object is multiple line control
    TP_FRAME :      If set, text control object has a frame
    TP_RAPID_MODE : If set, text control object is in rapid mode
    TP_PASSWORD :   If set, text control will display * characters in place of real characters
Safe to ignore:
    TP_NODRAW :     If set, text control object does not draw itself
    TP_NOUPDATE :   If set, text control object does not call IDIPLAY_Update when not needed

When using TP_PASSWORD in multitap mode, please allow selection(ie. last character to be shown) while typing
Only * out the last character when it is committed to the text

If hTextCtl is NULL, the function returns 0 ( zero ).

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextSetProperties()

Description:
   This function sets the properties of the text control, such as frame type,
   multiline, rapid entry (such as ZI), or a combination thereof.

Prototype:
   void OEM_TextSetProperties(OEMCONTEXT hTextField, uint32 dwProperties)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object
   dwProperties   uint32            Properties (TP_FRAME, TP_MULTILINE,
                                    TP_RAPID_MODE, or a combination of these)

Return Value:
   None.

Comments:
Important properties
    TP_MULTILINE :  If set, text control object is multiple line control
    TP_FRAME :      If set, text control object has a frame
    TP_RAPID_MODE : If set, text control object is in rapid mode
    TP_PASSWORD :   If set, text control will display * characters in place of real characters
Safe to ignore:
    TP_NODRAW :     If set, text control object does not draw itself
    TP_NOUPDATE :   If set, text control object does not call IDIPLAY_Update when not needed

When using TP_PASSWORD in multitap mode, please allow selection(ie. last character to be shown) while typing
Only * out the last character when it is committed to the text

If htextField is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextGetRect()

Description:
   This function returns the rectangle correpsonding to the bounds of this
   text control.

Prototype:
   void OEM_TextGetRect(OEMCONTEXT hTextField, AEERect *pOutRect)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object[IN]
   pOuntRect:     AEERect *         Rectangle corresponding to the
                                    bounds of the text control[OUT]

Return Value:
   None.

Comments:
   If hTextCtl or pOutRect is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextSetRect()

Description:
   This function returns the rectangle correpsonding to the bounds of this
   text control.

Prototype:
   void OEM_TextSetRect(OEMCONTEXT hTextField, const AEERect *pInRect)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object
   pInRect:     AEERect *         New bounds for the text control

Return Value:
   None.

Comments:
   If htextField or pInRect is NULL, the function simply returns

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextGetMaxChars()

Description:
   This function gets the maximum number of characters that can be added to
   the specified text control.

Prototype:
   uint16 OEM_TextGetMaxChars(OEMCONTEXT hTextField)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object

Return Value:
   Max. number of characters for the text control specified by hTextField.

Comments:
   if hTextCtl is NULL, the function returns 0 (zero).

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextSetMaxChars()

Description:
   This function sets the maximum number of characters that can be added to
   the specified text control.

Prototype:
   void OEM_TextSetMaxChars(OEMCONTEXT hTextField, uint16 wMaxChars)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object
   wMaxChars:     uint16            New maximum number of chars in this
                                    text control

Return Value:
   None.

Comments:
   If htextField is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextGetSel()

Description:
   This function gets the start and end locations for the selected text.

Prototype:
   void OEM_TextGetSel(OEMCONTEXT hTextField, int *piSelStart, int *piSelEnd)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object [IN]
   piSelStart:    int *             Start location of the text selection [OUT]
   piSelEnd:      int *             Ending location of the text selection [OUT]

Return Value:
   None.

Comments:
   If htextField is NULL, then
     if piSelStart is non-NULL, the first entry is set to zero.
     if piSelEnd is non-NULL, the first entry is set to zero.

   The selection start position must be preserved to what the applet had set to the
   nearest grapheme.
   This means, if you internally represent the selection start as a value less than
   selection end you must return the actual data set in OEM_TextSetSel().
   Examples:
   OEM_TextSetSel(p, 3, 6) -> OEM_TextGetSel() will return *piSelStart = 3, *piSelEnd = 6
   OEM_TextSetSel(p, 6, 3) -> OEM_TextGetSel() will return *piSelStart = 6, *piSelEnd = 3


Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextSetSel()

Description:
   This function sets the start and end locations for text selection.

Prototype:
   void OEM_TextSetSel(OEMCONTEXT hTextField, int iSelStart, int iSelEnd)

Parameters:
   htextField:   OEMCONTEXT       Handle for the text control object
   iSelStart:    int              Start location of the text selection
   iSelEnd:      int              Ending location of the text selection

Return Value:
   None.

Comments:
   If htextField is NULL, the function simply returns.

   This must update the cursor position to the place where iSelEnd is set.
   The selection start position must be preserved to what the applet had set to the
   nearest grapheme.
   This means, if you internally represent the selection start as a value less than
   selection end you must return the actual data set in OEM_TextSetSel().
   Examples:
   OEM_TextSetSel(p, 3, 6) -> OEM_TextGetSel() will return *piSelStart = 3, *piSelEnd = 6
   OEM_TextSetSel(p, 6, 3) -> OEM_TextGetSel() will return *piSelStart = 6, *piSelEnd = 3

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextEnumModesInit()

Description:
   This function initializes the enumeration mode. This function does not
   have an associated handle for the text control object.


Prototype:
   void OEM_TextEnumModesInit(void);

Parameters:
   None.

Return Value:
   None.

Comments:
   None.

Side Effects:
   None

See Also:
   None


==========================================================================

Function: OEM_TextEnumMode()

Description:
   This function gets the next text enumeration mode.


Prototype:
   boolean OEM_TextEnumMode(AEETextMode* pMode)

Parameters:
   pMode:    AEETextMode *     Pointer to the next text mode [IN/OUT]


Return Value:
   TRUE if the next mode is valid.
   FALSE if already at the end of the list.

Comments:
   If pMode is NULL, the function returns FALSE.
   Use the wID member of the AEETextMode structure to enumerate the next mode

Side Effects:
   None

See Also:
   None



==========================================================================

Function: OEM_TextQuerySymbols()

Description:
   This function gets the buffer pszOut with the symbols. The length of
   the buffer is specified by size.


Prototype:
   uint16 OEM_TextQuerySymbols(AECHAR *pszOut, uint16 size)

Parameters:
   pszOut:  AECHAR *     Symbols buffer [OUT]
   size:    uint16       Size of the buffer [IN]

Return Value:
   The number of symbols put into the query buffer. If pszOut is NULL,
   or if size is less than the number of OEM symbols, this function returns
   0.

Comments:
   If pszOut is NULL, the function returns zero. If you do not want the Symbols
   mode to be supported, simply fill the pszOut buffer with an empty string and
   return 0.

Side Effects:
   None

See Also:
   None


==========================================================================

Function: OEM_TextGetCursorPos()

Description:
   This function gets the absolute position of the cursor


Prototype:
   int32 OEM_TextGetCursorPos(OEMCONTEXT hTextField)

Parameters:
   hTextField:    OEMCONTEXT     Handle for the text control object

Return Value:
   The 0 based position of the cursor.
   For example:
   If you have the Text Hi and the cursor is given as |~
~
   |Hi would return 0~
   H|i would return 1~
   Hi| would return 2 ~
*

Comments:
  If you set the character position between an element of a grapheme, the position
  will be updated such that it is in the next available character position after the
  complete grapheme. This means that a call to this will not necessarily match the value
  passed to a previous call to OEM_TextGetCursorPos(). A grapheme is all the characters
  that compose a complete symbol in a language such as Thai.

Side Effects:
   None

See Also:
   None


==========================================================================

Function: OEM_TextSetCursorPos()

Description:
   This function gets the absolute position of the cursor


Prototype:
   int32 OEM_TextSetCursorPos(OEMCONTEXT hTextField, int32 nOffset)

Parameters:
   hTextField:    OEMCONTEXT     Handle for the text control object
   nOffset:       int32          Absolute Offset to move the cursor to

Return Value:
   None.

Comments:
   This function should move the cursor to the 0 based position of the cursor.
   If nOffset is > the length of the text, the cursor should be placed after the text.
   If nOffset is <= 0, the cursor should be placed at the begging of the text
   For example:
   If you have the Text Hi and | represents the cursor:~
~
   nOffset = 0    |Hi~
   nOffset = -1   |Hi~
   nOffset = 1    H|i ~
   nOffset = 2    Hi| ~
   nOffset = 100  Hi| ~
*

  If you set the character position between an element of a grapheme, the position
  must be updated such that it is in the next available character position after the
  complete grapheme. A grapheme is all the characters that compose a complete symbol
  in a language such as Thai.

Side Effects:
   Calling this does not guarentee a matching result in OEM_TextGetCursorPos().

See Also:
   None

==========================================================================

Function: OEM_TextDrawIMEDlg()

Description:
   This function redraws the IME dialog as needed.


Prototype:
   void  OEM_TextDrawIMEDlg(OEMCONTEXT hTextField);

Parameters:
   hTextField:    OEMCONTEXT     Handle for the text control object

Return Value:
   None.

Comments:
   This function should redraw the IME dialog when it is displayed. If one
   is not display this function should return without any operation.

Side Effects:
   None

See Also:
   None

==============================================================
AEEText

See the ITextCTL Interface in the BREW API Reference.

========================================================================*/

#endif    // OEMTEXTCTL_H