/*******************************************************************************
;*******************************************************************************
;**                                                                           **
;**                  COPYRIGHT 1997-2005 TEGIC COMMUNICATIONS                 **
;**                                                                           **
;**                TEGIC COMMUNICATIONS PROPRIETARY INFORMATION               **
;**                                                                           **
;**     This software is supplied under the terms of a license agreement      **
;**     or non-disclosure agreement with Tegic Communications and may not     **
;**     be copied or disclosed except in accordance with the terms of that    **
;**     agreement.                                                            **
;**                                                                           **
;**     T9 Export Control Classification Number ECCN: EAR99                   **
;**                                                                           **
;*******************************************************************************
;**                                                                           **
;**     FileName: T9api.h                                                     **
;**                                                                           **
;**  Description: T9 CellPhone API Interface Header File.                     **
;**  Version:	  7.3                                                         **
;*******************************************************************************
;******************************************************************************/

#ifndef T9API_H
#define T9API_H 1

#include "t9.h" /* To get xxt9oem.h, which might set T9ALPHABETIC, et al. */

#if defined(T9EVENTAPI) && !defined(T9ALPHABETIC)
#define T9ALPHABETIC /* Must have T9ALPHABETIC for the event API. */
#endif

#if !defined(T9ALPHABETIC) && !defined(T9CHINESE) && !defined(T9JAPANESE) && \
    !defined(T9KOREAN) && !defined(T9CHINESEPTI)
#define T9ALPHABETIC /* . */
#endif

#ifdef T9ALPHABETIC
#include "t9awapi.h"
#endif

#ifdef T9CHINESE
#include "t9ccapi.h"
#endif

#ifdef T9JAPANESE
#include "t9jwapi.h"
#endif

#ifdef T9KOREAN
#include "t9kcapi.h"
#endif

#ifdef T9EVENTAPI
#include "t9compat.h"
#endif

#ifdef T9CHINESEPTI
#include "t9cpapi.h"
#endif

#endif /* #ifndef T9API_H */



/* ----------------------------------< eof >--------------------------------- */

