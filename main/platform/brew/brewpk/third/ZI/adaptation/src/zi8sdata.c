/*****************************************************************************
* COPYRIGHT ZI AND SUBJECT TO CONFIDENTIALITY RESTRICTIONS                   *
*                                                                            *
* This file is the confidential and proprietary property of Zi Corporation   *
* of Canada, Inc. ("Zi") or one of its affiliates.                           *
**************************************************************************** */
/* zi8sdata.c
// This module contains a static language table that references
// The compiled version of data. This file is referenced from
// Zi8Initialize. 
//
// THIS FILE MUST BE MODIFIED TO PROVIDE THE DESIRED CONFIGURATION OF
// LANGUAGES. REFERENCES FROM THIS MODULE WILL CAUSE THE APPROPRIATE 
// DATASETS TO BE LINKED. 
*/
#include "zi8api.h" 

extern ZI8UCHAR ZI8ROMDATA Zi8DatENPRC[];
#if defined(__MMI_ZI_SM_CHINESE__)
extern ZI8UCHAR ZI8ROMDATA Zi8DatZH[];
#endif
#if defined(FEATURE_ZI_MT_HINDI)
extern ZI8UCHAR ZI8ROMDATA Zi8DatHI[];
#endif
#if defined(FEATURE_ZI_MT_ARABIC)
extern ZI8UCHAR ZI8ROMDATA Zi8DatAR[];
#endif
#if defined(FEATURE_ZI_MT_HEBREW)
extern ZI8UCHAR ZI8ROMDATA Zi8DatHR[];
#endif
#if defined(FEATURE_ZI_MT_THAI)
extern ZI8UCHAR ZI8ROMDATA Zi8DatTH[];
#endif
#if defined(FEATURE_ZI_MT_SPANISH)
extern ZI8UCHAR ZI8ROMDATA Zi8DatESSA[];
#endif
#if defined(FEATURE_ZI_MT_FRENCH)
extern ZI8UCHAR ZI8ROMDATA Zi8DatFREU[];
#endif

ZI8_LANGUAGE_ENTRY ZI8ROMDATA Zi8StaticLanguageTable[] =
{
    {ZI8_LANG_EN,(void ZI8ROMPOINTER)&Zi8DatENPRC[0]},
#if defined(__MMI_ZI_SM_CHINESE__)
    {ZI8_LANG_ZH,(void ZI8ROMPOINTER)&Zi8DatZH[0]},
#endif
#if defined(FEATURE_ZI_MT_HINDI)
	{ZI8_LANG_HI,(void ZI8ROMPOINTER)&Zi8DatHI[0]},
#endif
#if defined(FEATURE_ZI_MT_ARABIC)
	{ZI8_LANG_AR,(void ZI8ROMPOINTER)&Zi8DatAR[0]},
#endif
#if defined(FEATURE_ZI_MT_HEBREW)
	{ZI8_LANG_HR,(void ZI8ROMPOINTER)&Zi8DatHR[0]},
#endif
#if defined(FEATURE_ZI_MT_THAI)
	{ZI8_LANG_TH,(void ZI8ROMPOINTER)&Zi8DatTH[0]},
#endif
#if defined(FEATURE_ZI_MT_SPANISH)
    {ZI8_LANG_ESSA,(void ZI8ROMPOINTER)&Zi8DatESSA[0]},
#endif
#if defined(FEATURE_ZI_MT_FRENCH)
	{ZI8_LANG_FR,(void ZI8ROMPOINTER)&Zi8DatFREU[0]},
#endif
    {0,0}
};

