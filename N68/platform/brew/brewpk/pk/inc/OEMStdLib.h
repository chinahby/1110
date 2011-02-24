/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  OEMStdLib.h

SERVICES:  OEM C Standard Library Interface

GENERAL DESCRIPTION:
	Definitions for OEM C Standard Library Calls

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

#ifndef OEMSTDLIB_H
#define OEMSTDLIB_H

// <math.h> needs to be here because, the one shipped with MSVC has a conditional
// C++ template in it (for pow) and moving it in "extern 'C'" block causes :-
// error C2894: templates cannot be declared to have 'C' linkage

#include <math.h>

#if defined(__cplusplus)
extern "C" {
#endif

/**************************************************************************/
/*               C STANDARD LIBRARY EXPORTED OPERATIONS                     */
/**************************************************************************/

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>

#define OEM_jmpbuf                   jmp_buf
#define OEM_longjmp(jp,ret)          longjmp((jp),(ret))
#define OEM_setjmp(jp)               setjmp((jp))
#define OEM_DateTime                 struct tm

#include "algnstdlib.h"

#define OEM_memmove(dest,src,size)  aligned_memmove((dest),(src),(size))
#define OEM_memcpy(dest,src,size)   aligned_memcpy((dest),(src),(size))
#define OEM_memcmp(a,b,size)        aligned_memcmp((a),(b),(size))
#define OEM_memset(dest,ch,size)    aligned_memset((dest), (ch), (size))
#define OEM_memchr(s,c,size)        aligned_memchr((s),(c),(size))
#define OEM_strcat(dest,src)        aligned_strcat((dest),(src))
#define OEM_strcpy(dest,src)        aligned_strcpy((dest), (src))
#define OEM_strncpy(dest,src,count) aligned_strncpy((dest),(src),(count))
#define OEM_strchr(s1,ch)           aligned_strchr((s1),(ch))
#define OEM_strcmp(s1,s2)           aligned_strcmp((s1),(s2))
#define OEM_strlen(s1)              aligned_strlen((s1))
#define OEM_strncmp(a,b,count)      aligned_strncmp((a),(b),(count))
#define OEM_strrchr(s1,ch)          aligned_strrchr((s1),(ch))
#define OEM_strstr(h,n)             aligned_strstr((h),(n))
#define OEM_strtoul(s1,s2,n)        aligned_strtoul((s1),(s2),(n))
#define OEM_atof(s)                 atof((s))
#define OEM_atoi(s)                 atoi((s))
#define OEM_qsort(b,n,s,pfn)        qsort((b),(n),(s),(pfn))


#if defined(__cplusplus)
}
#endif


#endif /* OEMSTDLIB_H */
