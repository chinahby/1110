#ifndef _OI_OSINTERFACE_H
#define _OI_OSINTERFACE_H
/**
 @file  
 * This file provides the platform-independent interface for functions for which 
 * implementation is platform-specific.
 *   
 * The functions in this header file define the operating system or hardware 
 * services needed by the BLUEmagic 3.0 protocol stack. The
 * actual implementation of these services is platform-dependent.
 *
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_stddefs.h"
#include "oi_time.h"
#include "oi_status.h"
#include "oi_modules.h"

/** 
 * Terminate execution.
 *
 * @param reason  the reason for termination
 */
void OI_FatalError(OI_STATUS reason);

/** 
 * Log an error
 *
 * @param module - Module the error detected in (see oi_modules.h)
 * @param lineno - Line number of the C file OI_LOG_ERROR called
 */
void OI_LogError(OI_MODULE module, OI_INT lineno);

/** 
 * Read the time from the real time clock.
 *
 * @param now       (OUT) pointer to the buffer to which current time will be returned
 */
void OI_Time_Now(OI_TIME *now);

/** 
 * Cause the current thread to sleep for the specified amount of time.
 *
 * @param milliseconds  the number of milliseconds to sleep
 */
void OI_Sleep(OI_UINT32 milliseconds);


/** 
 * Write a string to stdout.
 *
 * @param str  the string to print
 */
void OI_Print(OI_CHAR const *str);

/**
 * Write an integer to stdout in hex. This macro is intended for selective use
 * when debugging in small memory configurations or other times when it is not
 * possible to use OI_DBGPRINT.
 *
 * @param n  the integer to print
 */

#define OI_Print_Int(n) \
{ \
    static const OI_CHAR _digits[] = "0123456789ABCDEF"; \
    OI_CHAR _buf[9]; \
    OI_CHAR *_str = &_buf[8]; \
    OI_UINT32 _i = n; \
    *_str = 0; \
    do { *(--_str) = _digits[(_i & 0xF)]; _i >>= 4; } while (_i); \
    OI_Print(_str); \
}

/*****************************************************************************/
#endif /* _OI_OSINTERFACE_H */
