/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  ALGNStdLib.h

SERVICES:  Misalign aware OEM C Standard Library Interface

GENERAL DESCRIPTION:
	Definitions for OEM C Standard Library Calls

        Copyright © 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

#ifndef ALGNSTDLIB_H
#define ALGNSTDLIB_H

#include "nmdef.h"

#if   defined(WIN32) && !defined(_WIN32_WCE) && defined(_M_IX86)

#include <stdlib.h>

#define  DECL_ALIGNED_STDLIBFUNC(ftype, pstdlibfunc, argproto, arglist) \
__inline ftype __aligned_##pstdlibfunc argproto                         \
{                                                                       \
   ftype          rtn;                                                  \
                                                                        \
   NO_MISALIGN_BEGIN;                                                   \
   rtn = pstdlibfunc arglist;                                           \
   NO_MISALIGN_END;                                                     \
   return rtn;                                                          \
}

DECL_ALIGNED_STDLIBFUNC(void *         , memmove, (void * dest, const void * src, size_t size)  , (dest, src, size)  )
DECL_ALIGNED_STDLIBFUNC(void *         , memcpy , (void * dest, const void * src, size_t size)  , (dest, src, size)  )
DECL_ALIGNED_STDLIBFUNC(int            , memcmp , (const void * a, const void * b, size_t size) , (a, b, size)       )
DECL_ALIGNED_STDLIBFUNC(void *         , memset , (void * dest, int ch, size_t size)            , (dest, ch, size)   )
DECL_ALIGNED_STDLIBFUNC(char *         , strcat , (char * dest, const char * src)               , (dest, src)        )
DECL_ALIGNED_STDLIBFUNC(char *         , strcpy , (char * dest, const char * src)               , (dest, src)        )
DECL_ALIGNED_STDLIBFUNC(char *         , strncpy, (char * dest, const char * src, size_t count) , (dest, src, count) )
DECL_ALIGNED_STDLIBFUNC(void *         , memchr , (const void  * buf, int c, size_t count)      , (buf, c, count)    )
DECL_ALIGNED_STDLIBFUNC(char *         , strchr , (const char * s1, int ch)                     , (s1, ch)           )
DECL_ALIGNED_STDLIBFUNC(int            , strcmp , (const char * s1, const char * s2)            , (s1, s2)           )
DECL_ALIGNED_STDLIBFUNC(int            , strlen , (const char * s1)                             , (s1)               )
DECL_ALIGNED_STDLIBFUNC(int            , strncmp, (const char * a, const char * b, size_t count), (a, b, count)      )
DECL_ALIGNED_STDLIBFUNC(char *         , strrchr, (const char * s1, int ch)                     , (s1, ch)           )
DECL_ALIGNED_STDLIBFUNC(char *         , strstr , (const char * h, const char * n)              , (h, n)             )
DECL_ALIGNED_STDLIBFUNC(unsigned long  , strtoul, (const char * s1, char ** s2, int n)          , (s1, s2, n)        )

#define aligned_memmove(dest,src,size)    __aligned_memmove((dest),(src),(size))
#define aligned_memcpy(dest,src,size)     __aligned_memcpy((dest),(src),(size))
#define aligned_memcmp(a,b,size)          __aligned_memcmp((a),(b),(size))
#define aligned_memset(dest, ch, size)    __aligned_memset((dest), (ch), (size))
#define aligned_memchr(s, c, size)        __aligned_memchr((s),(c),(size))
#define aligned_strcat(dest, src)         __aligned_strcat((dest),(src))
#define aligned_strcpy(dest, src)         __aligned_strcpy((dest), (src))
#define aligned_strncpy(dest,src,count)   __aligned_strncpy((dest),(src),(count))
#define aligned_strchr(s1,ch)             __aligned_strchr((s1),(ch))
#define aligned_strcmp(s1,s2)             __aligned_strcmp((s1),(s2))
#define aligned_strlen(s1)                __aligned_strlen((s1))
#define aligned_strncmp(a,b,count)        __aligned_strncmp((a),(b),(count))
#define aligned_strrchr(s1,ch)            __aligned_strrchr((s1),(ch))
#define aligned_strstr(h,n)               __aligned_strstr((h),(n))
#define aligned_strtoul(s1,s2,n)          __aligned_strtoul((s1),(s2),(n))

#else

#define aligned_memmove(dest,src,size)    memmove((dest),(src),(size))
#define aligned_memcpy(dest,src,size)     memcpy((dest),(src),(size))
#define aligned_memcmp(a,b,size)          memcmp((a),(b),(size))
#define aligned_memset(dest, ch, size)    memset((dest), (ch), (size))
#define aligned_memchr(s, c, size)        memchr((s),(c),(size))
#define aligned_strcat(dest,src)          strcat((dest),(src))
#define aligned_strcpy(dest, src)         strcpy((dest), (src))
#define aligned_strncpy(dest,src,count)   strncpy((dest),(src),(count))
#define aligned_strchr(s1,ch)             strchr((s1),(ch))
#define aligned_strcmp(s1,s2)             strcmp((s1),(s2))
#define aligned_strlen(s1)                strlen((s1))
#define aligned_strncmp(a,b,count)        strncmp((a),(b),(count))
#define aligned_strrchr(s1,ch)            strrchr((s1),(ch))
#define aligned_strstr(h,n)               strstr((h),(n))
#define aligned_strtoul(s1,s2,n)          strtoul((s1),(s2),(n))

#endif	// defined(WIN32) && !defined(_WIN32_WCE) && defined(_M_IX86)

#endif /* ALGNSTDLIB_H */
