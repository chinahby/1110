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
extern ZI8UCHAR ZI8ROMDATA Zi8DatZH[];

ZI8_LANGUAGE_ENTRY ZI8ROMDATA Zi8StaticLanguageTable[] =
{
    {ZI8_LANG_EN,(void ZI8ROMPOINTER)&Zi8DatENPRC[0]},
    {ZI8_LANG_ZH,(void ZI8ROMPOINTER)&Zi8DatZH[0]},
    {0,0}
};

