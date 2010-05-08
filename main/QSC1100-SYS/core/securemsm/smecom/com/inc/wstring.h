#ifndef WSTRING_H
#define WSTRING_H
/*============================================================================
wstring

ABSTRACT
   The definition for a wide string.

DESCRIPTION
   This service will provide basic operations on a wide string.


Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
=========================================================================== */

/*=========================================================================
                              Edit History

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/wstring.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
09/14/06    sm     Add wstrcmp()
08/30/06    ws     Add more utility functions
01/12/06    ws     Initial Revision

=========================================================================== */
#include <stdlib.h>
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16             wchar;

int wstrlen( const wchar *s);

wchar *wstrcpy(wchar *pDest, const wchar *pSrc);

int wstrlcpy(wchar* pwszDst, const wchar* cpwszSrc, size_t nDestSize);

int wstrlcat(wchar* pcDst, const wchar* pszSrc, size_t nDestSize);

wchar * strtowstr(const char *pszIn, wchar *pDest, int nSize);

char * wstrtostr(const wchar *cpwszIn, char *pDest, int nSize);

boolean floattowstr(double v, wchar * psz, int nSize);

wchar *wstrchr(const wchar *pText, wchar ch);

int wstrcmp(const wchar *s1,  const wchar *s2);

#ifdef __cplusplus
}
#endif

#endif
