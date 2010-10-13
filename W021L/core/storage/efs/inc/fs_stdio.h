/**********************************************************************
 * fs_stdio.h
 * DEPRECATED EFS Stdio implementation.
 *
 * Standard IO library for EFS.
 * Copyright (C) 2004, 2006-2009, Qualcomm, Inc.
 *
 * Most of the calls in this library have a direct replacement on the
 * enhanced version of stdio (fs_estdio.h). The enhanced version has
 * the following advantages:
 * - Detailed documentation of API's.
 * - Thread Safe.
 * - Allows buffered and non buffered I/O.
 * - The buffer can be any size (not only power of two)
 * - Supports Append Mode.
 * - Bug fixes.
 * - More Posix Compliant.
 * - Added fflush function.
 *
 * Don't use the API's defined here in new code, existing code should
 * move to the new API's. All of the API's defined here are deprecated.
 *
 */

/*==========================================================================
 *
 *                          EDIT HISTORY FOR MODULE
 *
 *  This section contains comments describing changes made to the module.
 *  Notice that changes are listed in reverse chronological order.
 *
 *  $Header: //source/qcom/qct/core/storage/efs/main/latest/inc/fs_stdio.h#4 $ $DateTime: 2009/07/06 18:30:05 $ $Author: ebuller $
 *
 * when        who  what, where, why
 * ---------   ---  -------------------------------------------------------
 * 2009-06-12  ebb  Moved rex_xxx calls to fs_osal.h abstraction.
 * 2009-01-26  wek  Added deprecation comments.
 * 2008-05-23   rp  Introducing /inc folder as per new SCM.
 * 2007-06-09   rp  Doxygen comments markup.
 * 2007-03-01   rp  Removed efs_fget_rbk_file_nbr() and efs_fget_rbk_file_fd().
 * 2007-01-31   rp  Add efs_fget_rbk_file_nbr() and efs_fget_rbk_file_fd().
 * 2006-11-02   rp  Add efs_stdio_init().
 * 2006-10-14   sh  Add efs_fpending().  And comments.
 * 2006-06-09  dlb  Add efs_fdopen().
 * 2006-05-30  dlb  Add efs_fileno().
 * 2004-02-10   gr  Added support for asynchronous writes.
 * 2003-01-25  dlb  Implement with basic write buffering.
 *
 ==========================================================================*/

#ifndef __FS_STDIO_H__
#define __FS_STDIO_H__

#include "fs_sys_types.h"
#include "fs_fcntl.h"
#include "fs_osal.h"

/**
 * @defgroup Deprecated stream_io Stream based EFS i/o
 * @ingroup efs
 *
 * DEPRECATED DEPRECATED DEPRECATED DEPRECATED DEPRECATED DEPRECATED DEPRECATED
 *
 * All of the statements defined here are deprecated, please refer to
 * fs_estdio.h for replacements.
 */


/** DEPRECATED, use EFS_EFILE instead. */
typedef struct _efs_file_io EFS_FILE;

/** DEPRECATED */
void efs_stdio_init (void);

/** DEPRECATED, replacement call is  efs_efopen(). */
EFS_FILE *efs_fopen (const char *path, const char *mode);

/** DEPRECATED, replacement call is efs_efdopen(). */
EFS_FILE *efs_fdopen (int filedes, const char *mode);

/** DEPRECATED, replacement call is efs_efread(). */
fs_size_t efs_fread (void *ptr, fs_size_t size, fs_size_t nitems,
  EFS_FILE *stream);

/** DEPRECATED replacement call is efs_efwrite(). */
fs_size_t efs_fwrite (void *ptr, fs_size_t size, fs_size_t nitems,
  EFS_FILE *stream);

/** DEPRECATED replacement call is efs_efclose(). */
int efs_fclose (EFS_FILE *stream);

/** DEPRECATED replacement call is efs_efseek(). */
int efs_fseek (EFS_FILE *stream, long offset, int whence);

/** DEPRECATED replacement call is efs_eftell(). */
long efs_ftell (EFS_FILE *stream);

/** DEPRECATED replacement call is efs_esetvbuf(). */
int efs_setvbuf (EFS_FILE *stream, char *buf, int mode, fs_size_t size);

/** DEPRECATED replacement call is efs_efileno(). */
int efs_fileno (EFS_FILE *stream);

/** DEPRECATED, If needed the application can keep track of this number. */
fs_size_t efs_fpending (EFS_FILE *stream);

/*  DEPRECATED, This API is deprecated and has no direct replacement on
 * fs_estdio.h */
int efs_set_sync (EFS_FILE *stream);

/*  DEPRECATED, This API is deprecated and has no direct replacement on
 * fs_estdio.h */
int efs_set_async_params (EFS_FILE *stream, fs_os_tcb_type *caller_tcb,
  unsigned long caller_sigs, fs_os_tcb_type *writer_tcb,
  unsigned long writer_sigs);

/*  DEPRECATED, This API is deprecated and has no direct replacement on
 * fs_estdio.h */
void efs_process_async_writes (void);

#endif /* not __FS_STDIO_H__ */
