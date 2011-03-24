/* ezitype.h 
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
#ifndef _EZITYPE_H
#define _EZITYPE_H 

    typedef struct _ZI8ETPARAM
    {
        ZI8UCHAR  Language;
        ZI8UCHAR  SubLanguage;
        PZI8WCHAR pElements;
        ZI8UCHAR  ElementCount;
        ZI8USHORT SizeOfCandidatesBuff;
        ZI8UCHAR  MaxCandidates;
        ZI8WCHAR  FirstCandidate;
        ZI8UCHAR  MaxErrors;
        PZI8WCHAR pCurrentWord;
        ZI8UCHAR  WordCharacterCount;
        ZI8UCHAR  Count;
        PZI8WCHAR pCandidates;
        ZI8BOOL   IsCorrected;
    } ZI8ETPARAM;
    typedef struct _ZI8ETPARAM ZI8RAMPOINTER PZI8ETPARAM;


    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8ETinitialize
                                              ( void
                                                ZI8_GBL_PARM1 );

    ZI8FUNCTION ZI8_IMPORT_C ZI8BOOL Zi8ETinitializeDynamic
                                              ( PZI8VOID pLanguageTable
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8USHORT Zi8ETgetVersion
                                              ( void
                                                ZI8_GBL_PARM1 );

    ZI8FUNCTION ZI8_IMPORT_C ZI8USHORT Zi8ETgetBuildID
                                              ( void
                                                ZI8_GBL_PARM1 );

    ZI8FUNCTION ZI8_IMPORT_C ZI8WCHAR Zi8ETgetOEMID
                                              ( void
                                                ZI8_GBL_PARM1 );

    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8ETgetDataVersion
                                              ( ZI8UCHAR lang 
                                                ZI8_GBL_PARM );

    ZI8FUNCTION ZI8_IMPORT_C ZI8UCHAR Zi8ETgetCandidates
                                              ( PZI8ETPARAM pETParam 
                                                ZI8_GBL_PARM );
#endif /* #ifndef _EZITYPE_H */

