#ifndef STR_UTIL_H
#define STR_UTIL_H
/*============================================================================
str_util

ABSTRACT
   string utilities.

DESCRIPTION
   This service will provide basic operations on a wide string.


Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
=========================================================================== */

/*=========================================================================
                              EDIT HISTORY FOR FILE
                            
  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/str_util.h#3 $
  $DateTime: 2008/04/03 11:42:01 $
  $Author: sladha $  

when       who     what, where, why
--------   ---     --------------------------------------------------------
08/30/06    ws     Add more utility functions
07/20/06   jay     Added a function to do file name encoding
04/20/06    rv     Added 'stricmp' for target
03/08/06    gr     Added 'strnicmp' for target
02/27/06    rv     Added functions strdup & strrev for target only
01/12/06    ws     Initial Revision

=========================================================================== */


#include <string.h>
#include <stdio.h>
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PLATFORM_LTK
#error code not present
#endif /* PLATFORM_LTK */

size_t strutil_b64fs_encode( uint8*   src_ptr, 
                             size_t   src_len,
                             char*    des_ptr );

char *IxStrdup(const char *pString);

int IxStricmp (const char* s1, const char* s2);

char *IxStrrev(const char *pString);

int IxStrcmp(const char *pString1, const char *pString2);

int IxStrnicmp (const char* s1, const char* s2, size_t n);

size_t IxStrlcat(char* pcDst, const char* pszSrc, size_t nDestSize);

size_t IxStrlcpy(char* pcDst, const char* pszSrc, size_t nDestSize);

char* IxStristr(char* pString, const char* pSubString);

#ifdef __cplusplus
}
#endif

#endif /* STR_UTIL_H */

