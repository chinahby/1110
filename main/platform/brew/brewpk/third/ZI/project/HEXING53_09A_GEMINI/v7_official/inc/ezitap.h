/* ezitap.h 
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
#include "ezicmn.h"


/* prevent multiple inclusion */
#ifndef _EZITAP_H
#define _EZITAP_H

/* eZiTap KEY MAPPINGS */
#define ZI8_ONEKEY_TIMEOUT             (ZI8_BASE_LATIN_KEYS+34) 
#define ZI8_ONEKEY_SPACE               (ZI8_BASE_LATIN_KEYS+35) 
#define ZI8_ONEKEY_DONE                (ZI8_BASE_LATIN_KEYS+36) 
#define ZI8_ONEKEY_BS                  (ZI8_BASE_LATIN_KEYS+37) 
#define ZI8_ONEKEY_PREVIOUS            (ZI8_BASE_LATIN_KEYS+38) 
#define ZI8_ONEKEY_NEXT                (ZI8_BASE_LATIN_KEYS+39) 
#define ZI8_ONEKEY_PGUP                (ZI8_BASE_LATIN_KEYS+40) 
#define ZI8_ONEKEY_PGDN                (ZI8_BASE_LATIN_KEYS+41) 

#define ZI8MTC_WORD_FORMED             0
#define ZI8MTC_WORD_PREDICTED          1
#define ZI8MTC_OVERFLOW                2
#define ZI8MTC_ERROR_INPUT             3
#define ZI8MTC_NO_CHANGE               4
#define ZI8MTC_NEXTWORD_PREDICTION     5
#define ZI8MTC_LITERALS_FULL           6

    /* keymap layout structure for eZiTap */
    typedef struct _ZI8LAYOUTINFO
    {
        PZI8WCHAR	pKeyUpper[12];
        PZI8WCHAR	pKeyLower[12];
    } ZI8LAYOUTINFO;
    typedef struct _ZI8LAYOUTINFO ZI8RAMPOINTER PZI8LAYOUTINFO;

    /* I/O structure for eZiTap */
    typedef struct _ZI8MTPARAM
    {
        ZI8UCHAR    Language;
        PZI8WCHAR   pElements;
        ZI8WCHAR    ElementCount;
        PZI8WCHAR   pCandidates;
        ZI8WCHAR    SizeOfCandidatesBuff;
        PZI8WCHAR   pFormedWordW;
        ZI8WCHAR    SizeOfFormedWordWBuff;
        ZI8UCHAR    MaxCandidates;
        ZI8UCHAR    uMinWordLength;
        ZI8BOOL     bPredictOnTimeout;
        ZI8UCHAR    uCaseType;
        ZI8UCHAR    uCandCount;
        ZI8UCHAR    uConfirmedChars;
        ZI8UCHAR    uLastCharIndex;
        ZI8BOOL     bSkipAdd2UWD;
        ZI8UCHAR    InternalData[16];
        ZI8UCHAR    SubLanguage;
        PZI8WCHAR   pCurrentWord;
        ZI8UCHAR    WordCharacterCount;
        ZI8BOOL     bCircularCandidates;
        ZI8UCHAR    uActiveChars;           /* for 1st candidate */
        ZI8UCHAR    uRemainingChars;        /* for 1st candidate */
    } ZI8MTPARAM;
    typedef struct _ZI8MTPARAM ZI8RAMPOINTER PZI8MTPARAM;

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8MTinitialize
                                              ( void
                                                ZI8_GBL_PARM1 );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8MTinitializeDynamic
                                              ( PZI8VOID pLanguageTable
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8MTsetLayout
                                              ( PZI8LAYOUTINFO pLayout
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8USHORT Zi8MTgetVersion
                                              ( void
                                                ZI8_GBL_PARM1 );

    ZI8FUNCTION ZI8_IMPORT_C ZI8USHORT Zi8MTgetBuildID
                                              ( void
                                                ZI8_GBL_PARM1 );

    ZI8FUNCTION ZI8_IMPORT_C ZI8WCHAR Zi8MTgetOEMID
                                              ( void
                                                ZI8_GBL_PARM1 );

    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8MTgetDataVersion
                                              ( ZI8UCHAR lang 
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C PZI8WCHAR Zi8MTgetWordWptr
                                              ( PZI8WCHAR pCandidates,
                                                ZI8USHORT index
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8MTprocess
                                              ( PZI8MTPARAM pGetParam
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8USHORT Zi8MTgetCount4KeyLayout
                                              ( ZI8UCHAR ucLang,
                                                ZI8WCHAR key
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8MTgetKeyLayout
                                              ( ZI8UCHAR ucLang,
                                                ZI8BOOL bUpper,
                                                ZI8WCHAR wcKey,
                                                PZI8WCHAR pCharLayout
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8LONG Zi8MTgetCandidateCount
                                              ( PZI8MTPARAM pMTParam 
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8WCHAR Zi8MTgetCurrentFirstCandidate
                                              ( PZI8MTPARAM pMTParam 
                                                ZI8_GBL_PARM );

#endif /* #ifndef _EZITAP_H */
