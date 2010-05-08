
/***********************************************************************
 * fs_err.h
 *
 * Error macros for EFS2.
 * Copyright (C) 2002--2006, 2008 Qualcomm, Inc.
 *
 ***********************************************************************/

 /*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_err.h#2 $ $DateTime: 2008/10/29 18:03:15 $ $Author: richardp $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2008-10-17    yog     Moved err.h to be included for all cases.
2006-09-20    dlb     Lint cleanup.
2005-10-12    dlb     Fix ISO C warnings.
2005-01-27    dlb     Allow standalone builds.
2004-04-15    dlb     Update copyright line.
2003-04-22     cr     Add FS_TRACE macro.
2003-01-09     gr     Created.

===========================================================================*/

#ifndef _FS_ERR_H
#define _FS_ERR_H

#include "customer.h"
#include "comdef.h"
#include "err.h"

#include <setjmp.h>

#ifdef FEATURE_EFS_FATAL_ERROR_HANDLER
/* Alloc fatal errors to be "handled".  If this feature is defined, the
 * function handle_efs_err_fatal will be called.  Normally, this function
 * should either do nothing, or call efs_reset_nostop() and then return.
 */
extern void handle_fs_err_fatal (void);
#define _FS_ERR_FATAL_HANDLER   handle_fs_err_fatal ()

#else  /* not FEATURE_EFS_FATAL_ERROR_HANDLER */
#define _FS_ERR_FATAL_HANDLER   do { } while (0)
#endif  /* not FEATURE_EFS_FATAL_ERROR_HANDLER */

#ifdef FS_STANDALONE

#include <stdio.h>

#ifndef NORETURN
  #ifdef __GNUC__
    #define NORETURN   __attribute__ ((noreturn))
  #else
    #define NORETURN
  #endif
#endif

extern void err_fatal_raise (void) NORETURN;

#define FS_MSG_BASE(level,x_fmt,a,b,c) \
  printf ("MSG%s (%s line %d): " x_fmt "\n", level, \
            __FILE__, __LINE__, (a), (b), (c))

#define FS_ERR_FATAL(fmt,a,b,c) \
  do { \
    FS_MSG_BASE ("FATAL", fmt, a, b, c); \
    _FS_ERR_FATAL_HANDLER; \
    err_fatal_raise (); \
  } while (0)

#define FS_ERR(fmt,a,b,c) \
  do { \
    FS_MSG_BASE ("ERR", fmt, a, b, c); \
  } while (0)

#else

#define FS_ERR_FATAL(fmt,a,b,c) \
  do { \
    err_disable_fs_log (); \
    _FS_ERR_FATAL_HANDLER; \
    ERR_FATAL (fmt, a, b, c); \
  } while (0)

#define FS_ERR(fmt,a,b,c) \
  do { \
    err_disable_fs_log (); \
    ERR (fmt, a, b, c); \
    err_enable_fs_log (); \
  } while (0)

#endif /* FS_STANDALONE */

#ifdef EFS2_DEBUG
#error code not present
#else
#define FS_TRACE(a,b,c,d)
#endif /* EFS2_DEBUG */

/* Simplistic exception handling method for EFS calls.  The can be caught
 * to rollback transactions.  If there is not catch, FS_ERR_FATAL is
 * raised. */

/* jmp_buf is an array type of unspecified field types, wrap it in a
 * structure so normal pointers will work.  This can also be extended with
 * other fields, if necessary. */
struct efs_catch_block {
  jmp_buf       jmp;
};

extern struct efs_catch_block *_fs_catch;
extern struct efs_catch_block _fs_static_catch;

/* Catch block for exceptions.  Used like this:
 *
 *   int status;
 *   FS_SIMPLE_TRY (status);
 *   if (status == 0) {
 *     Normal part of code.
 *   } else {
 *     Error handling with raise called with 'status' for arg.
 *   }
 *   FS_SIMPLE_TRY_CLEANUP ();
 */
#define FS_SIMPLE_TRY(_status) \
  do { \
    if (_fs_catch != NULL) \
      FS_ERR_FATAL ("Nested FS_SIMPLE_TRY not allowed", 0, 0, 0); \
    _fs_catch = &_fs_static_catch; \
    (_status) = setjmp (_fs_static_catch.jmp); \
  } while (0)

#define FS_SIMPLE_TRY_CLEANUP() \
  do { \
    _fs_catch = NULL; \
  } while (0)

/* Raise an FS exception.  The first argument is the status to give back to
 * the TRY block.  It must not be zero.  The remaining arguments are as per
 * FS_ERR_FATAL, and will be given to FS_ERR_FATAL is there is no handler.
 */
#define FS_RAISE(status, fmt, a, b, c) \
  do { \
    if (_fs_catch == NULL) \
      FS_ERR_FATAL (fmt, a, b, c); \
    longjmp (_fs_catch->jmp, (status)); \
  } while (0)

/* Return true if an exception will be caught. */
#define FS_HANDLER_PRESENT() (_fs_catch != NULL)

#endif /* End of _FS_ERR_H */

