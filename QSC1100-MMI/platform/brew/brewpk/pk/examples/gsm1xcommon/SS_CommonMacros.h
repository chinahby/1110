#ifndef __SS_COMMON_MACROS_H__
#define __SS_COMMON_MACROS_H__

/*=============================================================================
FILE: SS_CommonMacros.h

SERVICES: 
    Definition of common macros for GSM1X SS project.


                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
=============================================================================*/

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

// assert is defined as follows:
// for AEE_SIMULATOR, use window's assert.
// for other builds (static, dynamic), do nothing.

#if defined BREW_STATIC_APP
    //#undef ARR_SIZE /* avoid double definition of this macro. */
    #define assert(x) { if (!(x)) { DBGPRINTF("Assert!"); } }
    #define ASSERT_NOT_REACHABLE { DBGPRINTF("Assert!"); }

#elif defined AEE_SIMULATOR
    #include "assert.h"
    #define ASSERT_NOT_REACHABLE assert(FALSE);
    #define PARAM_NOT_REF(param) /* suppress warning for unused parameter. */

#else	// dynamic build
    #define PARAM_NOT_REF(param) /* suppress warning for unused parameter. */

    #define assert(x) { if (!(x)) { DBGPRINTF("Assert!"); } }
    #define ASSERT_NOT_REACHABLE { DBGPRINTF("Assert!"); }

#endif

#endif // end __SS_COMMON_MACROS_H__
