/* ezicmn.h 
*/
/*****************************************************************************
* COPYRIGHT ZI AND SUBJECT TO CONFIDENTIALITY RESTRICTIONS                   *
*                                                                            *
* This file is the confidential and proprietary property of Zi Corporation   *
* of Canada, Inc. ("Zi") or one of its affiliates.                           *
**************************************************************************** */

/* Note: This file is ANSI C
**
** This file provides the definitions for constants and structures used in the
** eZiText core library.
**
** All of these definitions will generally remain the same regardless
** of platform.
**
*/
#include "zi8cfg.h" 
#include "zi8types.h"


/* prevent multiple inclusion */
#ifndef _EZICMN_H
#define _EZICMN_H 

#define ZI8_MAX_WORDLENGTH             64

#define ZI8_CASE_DEFAULT               0    /* preserve original case */
#define ZI8_CASE_UPPER                 1
#define ZI8_CASE_LOWER                 2
#define ZI8_CASE_TITLE                 3

#define ZI8TRUE                        1
#define ZI8FALSE                       0
#define ZI8NULL                        0

#define ZI8_LANG_GSMDEFAULT            0
#define ZI8_LANG_ZH                    1
#define ZI8_LANG_PY                    2
#define ZI8_LANG_DE                    5
#define ZI8_LANG_SV                    9
#define ZI8_LANG_FI                    10
#define ZI8_LANG_NO                    11
#define ZI8_LANG_DA                    12
#define ZI8_LANG_NL                    13
#define ZI8_LANG_EL                    14
#define ZI8_LANG_TR                    15
#define ZI8_LANG_JP                    16
#define ZI8_LANG_KO                    18
#define ZI8_LANG_AR                    19
#define ZI8_LANG_IN                    20
#define ZI8_LANG_MS                    21
#define ZI8_LANG_PL                    22
#define ZI8_LANG_CS                    23
#define ZI8_LANG_IW                    24
#define ZI8_LANG_VI                    25
#define ZI8_LANG_SK                    26
#define ZI8_LANG_EU                    27
#define ZI8_LANG_BN                    28
#define ZI8_LANG_HR                    29
#define ZI8_LANG_CH                    30
#define ZI8_LANG_ET                    31
#define ZI8_LANG_HI                    32
#define ZI8_LANG_HU                    33
#define ZI8_LANG_LV                    34
#define ZI8_LANG_LT                    35
#define ZI8_LANG_FA                    36
#define ZI8_LANG_PA                    37
#define ZI8_LANG_RO                    38
#define ZI8_LANG_RU                    39
#define ZI8_LANG_SL                    41
#define ZI8_LANG_TH                    42
#define ZI8_LANG_UR                    43
#define ZI8_LANG_UK                    44
#define ZI8_LANG_TL                    45
#define ZI8_LANG_IT                    47
#define ZI8_LANG_IS                    48
#define ZI8_LANG_BG                    49
#define ZI8_LANG_MR                    53
#define ZI8_LANG_SW                    54
#define ZI8_LANG_GU                    55
#define ZI8_LANG_KN                    56
#define ZI8_LANG_TA                    57
#define ZI8_LANG_ENUK                  58
#define ZI8_LANG_ENAM                  59
#define ZI8_LANG_ENPRC                 60
#define ZI8_LANG_ENTW                  61
#define ZI8_LANG_ENHK                  62
#define ZI8_LANG_FREU                  63
#define ZI8_LANG_FRCA                  64
#define ZI8_LANG_ESSA                  65
#define ZI8_LANG_ESEU                  66
#define ZI8_LANG_PTEU                  67
#define ZI8_LANG_PTBZ                  68
#define ZI8_LANG_SRLA                  69
#define ZI8_LANG_SRCY                  70
#define ZI8_LANG_KM                    71
#define ZI8_LANG_TE                    72
#define ZI8_LANG_ML                    73
#define ZI8_LANG_SI                    74
#define ZI8_LANG_AF                    75
#define ZI8_LANG_ST                    76
#define ZI8_LANG_XH                    77
#define ZI8_LANG_ZU                    78
#define ZI8_LANG_SQ                    79
#define ZI8_LANG_MK                    80
#define ZI8_LANG_AZ                    81
#define ZI8_LANG_KK                    82
#define ZI8_LANG_UZ                    83
#define ZI8_LANG_HA                    84
#define ZI8_LANG_IG                    85
#define ZI8_LANG_YO                    86
#define ZI8_LANG_PYP                   119
#define ZI8_LANG_PYS                   120
#define ZI8_LANG_ZYP                   121
#define ZI8_LANG_ZYS                   122
#define ZI8_LANG_SP2                   123
#define ZI8_LANG_PY2                   124
#define ZI8_LANG_ZY2                   125
#define ZI8_LANG_ZHP                   126
#define ZI8_LANG_ARB                   127 
#define ZI8_AUXTABLE_ZH1               128
#define ZI8_AUXTABLE_JP1               129

/* keep these IDs for V6 MMI */
#define ZI8_LANG_EN                    3
#define ZI8_LANG_FR                    4
#define ZI8_LANG_ES                    6
#define ZI8_LANG_PT                    8
#define ZI8_LANG_SR                    40

#define ZI8_SUBLANG_DEFAULT            7

#define ZI8_SO_DEFAULT                 0
#define ZI8_SO_EXACT_ROM_DATA          1
#define ZI8_SO_EXACT_PD_DATA           2
#define ZI8_SO_EXACT_UWD_DATA          3
#define ZI8_SO_EXACT_OEM_DATA          4
#define ZI8_SO_OTHERS_ROM_DATA         5
#define ZI8_SO_OTHERS_PD_DATA          6
#define ZI8_SO_OTHERS_UWD_DATA         7
#define ZI8_SO_OTHERS_OEM_DATA         8
#define ZI8_SO_NATURAL_ROM_DATA        9
#define ZI8_SO_MAX_SIZE                (ZI8_SO_NATURAL_ROM_DATA+1)

/* ONE KEY PRESS: KEYS MAPPING */
#define ZI8_ONEKEY_KEY1                (ZI8_BASE_LATIN_KEYS+1)
#define ZI8_ONEKEY_KEY2                (ZI8_BASE_LATIN_KEYS+2)
#define ZI8_ONEKEY_KEY3                (ZI8_BASE_LATIN_KEYS+3)
#define ZI8_ONEKEY_KEY4                (ZI8_BASE_LATIN_KEYS+4)
#define ZI8_ONEKEY_KEY5                (ZI8_BASE_LATIN_KEYS+5)
#define ZI8_ONEKEY_KEY6                (ZI8_BASE_LATIN_KEYS+6)
#define ZI8_ONEKEY_KEY7                (ZI8_BASE_LATIN_KEYS+7)
#define ZI8_ONEKEY_KEY8                (ZI8_BASE_LATIN_KEYS+8)
#define ZI8_ONEKEY_KEY9                (ZI8_BASE_LATIN_KEYS+9)
#define ZI8_ONEKEY_KEY0                (ZI8_BASE_LATIN_KEYS+10)
#define ZI8_ONEKEY_KEY11               (ZI8_BASE_LATIN_KEYS+11)
#define ZI8_ONEKEY_KEY12               (ZI8_BASE_LATIN_KEYS+12)
#define ZI8_ONEKEY_KEY13               (ZI8_BASE_LATIN_KEYS+13)
#define ZI8_ONEKEY_KEY14               (ZI8_BASE_LATIN_KEYS+14)
#define ZI8_ONEKEY_KEY15               (ZI8_BASE_LATIN_KEYS+15)
#define ZI8_ONEKEY_KEY16               (ZI8_BASE_LATIN_KEYS+16)
#define ZI8_ONEKEY_KEY17               (ZI8_BASE_LATIN_KEYS+17)
#define ZI8_ONEKEY_KEY18               (ZI8_BASE_LATIN_KEYS+18)
#define ZI8_ONEKEY_KEY19               (ZI8_BASE_LATIN_KEYS+19)
#define ZI8_ONEKEY_KEY20               (ZI8_BASE_LATIN_KEYS+20)
#define ZI8_ONEKEY_KEY21               (ZI8_BASE_LATIN_KEYS+21)
#define ZI8_ONEKEY_KEY22               (ZI8_BASE_LATIN_KEYS+22)
#define ZI8_ONEKEY_KEY23               (ZI8_BASE_LATIN_KEYS+23)
#define ZI8_ONEKEY_KEY24               (ZI8_BASE_LATIN_KEYS+24)
#define ZI8_ONEKEY_KEY25               (ZI8_BASE_LATIN_KEYS+25)
#define ZI8_ONEKEY_KEY26               (ZI8_BASE_LATIN_KEYS+26)
#define ZI8_ONEKEY_KEY27               (ZI8_BASE_LATIN_KEYS+27)
#define ZI8_ONEKEY_KEY28               (ZI8_BASE_LATIN_KEYS+28)
#define ZI8_ONEKEY_KEY29               (ZI8_BASE_LATIN_KEYS+29)
#define ZI8_ONEKEY_KEY30               (ZI8_BASE_LATIN_KEYS+30)
#define ZI8_ONEKEY_KEY31               (ZI8_BASE_LATIN_KEYS+31)
#define ZI8_ONEKEY_KEY32               (ZI8_BASE_LATIN_KEYS+32)

#define ZI8_ONEKEY_SEPARATOR           (ZI8_BASE_LATIN_KEYS+33)

#define ZI8_ERROR_PUD                  42
#define ZI8_ERROR_PUD_NO_ERROR         (ZI8_ERROR_PUD+0)
#define ZI8_ERROR_MAX_PUDS_EXCEEDED    (ZI8_ERROR_PUD+1)
#define ZI8_ERROR_BAD_PUD_FORMAT       (ZI8_ERROR_PUD+2)
#define ZI8_ERROR_BAD_PUD_HANDLE       (ZI8_ERROR_PUD+3)
#define ZI8_ERROR_PUD_UNCHANGED        (ZI8_ERROR_PUD+4)
#define ZI8_ERROR_PUD_NOSPACE          (ZI8_ERROR_PUD+5)
#define ZI8_ERROR_DUPLICATE            (ZI8_ERROR_PUD+6)
#define ZI8_ERROR_PUD_CORRUPTED        (ZI8_ERROR_PUD+7)
#define ZI8_ERROR_PUD_NOEXIST          (ZI8_ERROR_PUD+8)
#define ZI8_ERROR_PUD_INVALID_PLACE    (ZI8_ERROR_PUD+9)
#define ZI8_ERROR_PUD_SIZE_TOO_SMALL   (ZI8_ERROR_PUD+10)

/* uwd search options */
#define ZI8_UWD_OPT_DEFAULT            0x00 /* seq. order based on the
                                               learning order */
#define ZI8_UWD_OPT_EXACTMATCH         0x01 /* exact match candidates first
                                               then followed by others */
#define ZI8_UWD_OPT_LATESTMATCH        0x02 /* latest match first followed by
                                               exact match candidates then
                                               followed by others */
/* prefix locking control bits */
#define ZI8_PREFIX_LOCK_ROM            0x01
#define ZI8_PREFIX_LOCK_UWD            0x02
#define ZI8_PREFIX_LOCK_PD             0x04
#define ZI8_PREFIX_LOCK_OEM            0x08
#define ZI8_PREFIX_LOCK_ALL            0x0F


    typedef struct _ZI8_CONVERSION_DATA
    {
        ZI8WCHAR             originalWC;
        ZI8WCHAR             transformedWC;
    } ZI8_CONVERSION_DATA;
    typedef struct _ZI8_CONVERSION_DATA ZI8RAMPOINTER PZI8_CONVERSION_DATA;

    typedef struct _ZI8_CONVERSION_TABLE
    {
        ZI8UCHAR             Language;
        PZI8_CONVERSION_DATA pData;
    } ZI8_CONVERSION_TABLE;
    typedef struct _ZI8_CONVERSION_TABLE ZI8RAMPOINTER PZI8_CONVERSION_TABLE;

    /* This structure is used for the language table reference by
       Zi8InitializeDynamic()*/
    typedef struct _ZI8_LANGUAGE_ENTRY
    {
        ZI8UCHAR           Language;
        PZI8VOID           pTable;
    } ZI8_LANGUAGE_ENTRY;
    typedef struct _ZI8_LANGUAGE_ENTRY ZI8RAMPOINTER PZI8_LANGUAGE_ENTRY;


    typedef ZI8BOOL (*PFZI8GETOEMDATAENTRY)   ( ZI8USHORT usIndex,
                                                PZI8WCHAR pBuffer,
                                                ZI8UCHAR  uMaxBufferSize,
                                                PZI8VOID  pOther );

#ifdef ZI8_NO_GLOBAL_VARIABLES

#define ZI8_GBL_PARM1        ZI8RAMPOINTER pGlobal
#define ZI8_GBL_PARM       , PZI8VOID      pGlobal

    ZI8FUNCTION ZI8_IMPORT_C ZI8USHORT Zi8GetGlobalDataSize
                                              ( void );

#else /* #ifdef ZI8_NO_GLOBAL_VARIABLES */

#define ZI8_GBL_PARM1
#define ZI8_GBL_PARM

#endif /* #else    #ifdef ZI8_NO_GLOBAL_VARIABLES */

    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8GetLastError
                                              ( void ZI8_GBL_PARM1 );
    /* functions for the Used Word Dictionary */
    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8AttachUWDfreqBased
                                              ( PZI8UCHAR pUWD,
                                                ZI8USHORT size
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8AttachUWD
                                              ( PZI8UCHAR pUWD,
                                                ZI8USHORT size
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8DetachUWD
                                              ( ZI8UCHAR UWD_id,
                                                PZI8UCHAR pUWD
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8AddUsedWordW
                                              ( ZI8UCHAR UWD_id,
                                                PZI8WCHAR pWord,
                                                ZI8UCHAR language
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8AddUsedWordW2
                                              ( ZI8UCHAR UWD_id,
                                                PZI8WCHAR pWord,
                                                ZI8UCHAR Language,
                                                ZI8UCHAR SubLanguage
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8VOID Zi8SetDFAthreadCount
                                              ( ZI8UCHAR size
                                                ZI8_GBL_PARM );

    /* For latin: conversion between byte to/from Unicode */
    ZI8FUNCTION ZI8_IMPORT_C ZI8WCHAR Zi8ConvertUC2WC
                                              ( ZI8UCHAR uc,
                                                ZI8UCHAR language
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8ConvertWC2UC
                                              ( ZI8WCHAR wc,
                                                ZI8UCHAR language
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8WCHAR Zi8ConvertWC2Key
                                              ( ZI8WCHAR wc,
                                                ZI8UCHAR language
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8WCHAR Zi8ConvertUC2Key
                                              ( ZI8UCHAR uc,
                                                ZI8UCHAR language
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8USHORT Zi8UTF8ToUCS2
                                              ( PZI8UCHAR UTF8Start,
                                                ZI8USHORT UTF8Size,
                                                PZI8WCHAR UCS2Start,
                                                ZI8USHORT UCS2Size
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8USHORT Zi8UCS2ToUTF8
                                              ( PZI8WCHAR UCS2Start,
                                                ZI8USHORT UCS2Size,
                                                PZI8UCHAR UTF8Start,
                                                ZI8USHORT UTF8Size
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8USHORT Zi8ConvertChars2Ligature
                                              ( ZI8UCHAR language,
                                                PZI8WCHAR awcString,
                                                ZI8USHORT size
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8USHORT Zi8ConvertLigature2Chars
                                              ( ZI8UCHAR language,
                                                PZI8WCHAR awcString,
                                                ZI8USHORT size
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL  Zi8SetLatinSearchOrder
                                              ( PZI8UCHAR pSearchOrderArray,
                                                ZI8UCHAR  uArraySize 
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL  Zi8SetPredictionStartAt
                                              ( ZI8UCHAR uIndex
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C PZI8UCHAR Zi8GetEngineCCFG
                                              ( void );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL  Zi8SetMaxWordLength
                                              ( ZI8UCHAR length 
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8GetMaxWordLength
                                              ( void ZI8_GBL_PARM1 );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL  Zi8SetMaxCountValue
                                              ( ZI8LONG maxCountValue 
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8SetSupportHIligature
                                              ( ZI8BOOL bSupport 
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8SetSupportVIligature
                                              ( ZI8BOOL bSupport 
                                                ZI8_GBL_PARM );

    /* functions for the Personal User Dictionary - common */
    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8AttachPUD
                                              ( PZI8UCHAR pDict,
                                                ZI8USHORT size
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL  Zi8DetachPUD
                                              ( ZI8UCHAR Pud,
                                                PZI8UCHAR pDict
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL  Zi8AddUDWordW
                                              ( ZI8UCHAR Pud,
                                                PZI8WCHAR pWord,
                                                ZI8UCHAR language
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL  Zi8DeleteUDWordW
                                              ( ZI8UCHAR Pud,
                                                PZI8WCHAR pWord,
                                                ZI8UCHAR language
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL  Zi8IsUDWordW
                                              ( ZI8UCHAR Pud,
                                                PZI8WCHAR pWord,
                                                ZI8UCHAR language
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL  Zi8RefUDWordW
                                              ( ZI8UCHAR Pud, 
                                                PZI8WCHAR pWord, 
                                                ZI8UCHAR language 
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL  Zi8RefUDWordW2
                                              ( ZI8UCHAR Pud, 
                                                PZI8WCHAR pWord, 
                                                ZI8UCHAR language, 
                                                ZI8UCHAR secondaryLanguage
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8SetPDremoveOpt
                                              ( ZI8BOOL bAutoRemove
                                                ZI8_GBL_PARM );
    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8SetPDsearchOpt
                                              ( ZI8BOOL bSupportPhrase
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8AttachOEMdata
                                              ( PFZI8GETOEMDATAENTRY pEntry,
                                                ZI8USHORT size,
                                                PZI8VOID  pOther
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8DetachOEMdata
                                              ( ZI8UCHAR OEMdataID
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8SetConversionData
                                              ( PZI8_CONVERSION_TABLE pData
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8SetCaselessSearch
                                              ( ZI8BOOL bValue
                                                ZI8_GBL_PARM );


#endif /* #ifndef _EZICMN_H */
