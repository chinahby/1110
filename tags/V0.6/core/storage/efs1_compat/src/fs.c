/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             D E P R E C A T E D
                  E M B E D D E D   F I L E   S Y S T E M   1

GENERAL DESCRIPTION

  This header file and the EFS1 API's listed here are DEPRECATED.
  Please use EFS2 API's instead, EFS2 API's are listed fs_public.h. These APIs 
  can also be referred from the published document 80-VG203-1.
  Some of the benefits of using EFS2 APIs are:
- Runs in caller’s context, therefore the priority is maintained when a read,
  write or other operations is requested.
- Many of the calls are POSIX compliant, others are similar to the standard.
- The calls have lower latency because there is no handler task.
- The EFS2 calls are more efficient.
- The EFS2 have a broader scope. Therefore, it's possible for an application
  to list all the directory contents (files and directories) in one pass.
- Error status is better defined, and generalized over all the calls.
- Path limitations have been lifted.
- UTF-8 Unicode paths are supported.
- Smaller memory footprint than EFS1.

Copyright (c) 1999-2003,2005-2006,2008 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/core/storage/efs1_compat/main/latest/src/fs.c#4 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08-27-08   wek     Added deprecation comments.
03-14-08   sh      Address Klocwork concerns.
11-10-06   sh      Revert EFS1 Sync/Async to #30, until further testing.
11-06-06   sh      Handle EFS1 callbacks in caller's context.
10-30-06   sh      Blocked EFS1 asynchronous call support.
09-22-06   sh      Added the implied "int" type to cur_rsvr_id.
09-19-05   sh      Correction to allow FEATURE_EFS_ACCESS_METHODS undef.
08-08-05   sch     Passed file name in fs_statfs
11-25-03    gr     Filled out stubbed fs_space_used function.
10-24-03   pg      Added support for FS_FORMAT_OP to format MMC card.
06-23-03   jkl     Featurized for EFS2.
09-27-02    gr     Added code to disable the Dog for the calling task while
                   waiting for operations to complete.
09-21-02   dlb     Reservations now have format expected by NV.
09-19-02    gr     Added stubs for functions defined under FEATURE_EFS_FLASH.
09-17-02   dlb     Add initialize for EFS2.
08-13-02   jqi     Updated with task operation interface to support multimode,
                   GSM and WCDMA.
05-13-02   dlb     Fix problem with enum iterator and AM.
04-02-02   pg      Use FEATURE_TASK_MODE_CONTROL instead of T_MSM5200 to 
                   support the change from mc to tmc.
12-05-01   dlb     Added fs_opened_file_size().
10-15-01   dlb     Fixed a race in fsi_perform_subcmd().
09-10-01   dlb     Add fs_statfs() call.
08-07-01   dlb     Fix incorrect length returned from fs_seek_read.
07-11-01   dlb     Implement fs_rename() and fs_remove_remote_links() with
                   access methods enabled.
05-22-01   pg      Added support for T_MSM5200 (SIMBA).
04-02-01   dlb     Added RPC Support for MSP1000.
01-29-01   dlb     Bring in EFF_FATAL fix on file table full.
12-11-00   dlb     Correctly handle fs_enum_next called after finish.
11-09-00   pg      Initial version for common archive.
08-22-00   dlb     Handle bad handles better in access methods.
                   Added support for FEATURE_EFS_FLASH.
07-25-00   pg      Added support to reserve some file system space for NV 
                   writes.
05-12-00   dlb     Added FEATURE_EFS_ACCESS_METHODS and code.
05-12-00   dlb     Added fs_enum_finish().
04-25-00   pg      Fixed the problem that fs_test() could not properly
                   distinguish a file from a directory:
                   Replaced function fs_test() with fs_nametest().
03-24-00   pg      Added fs_enum_prev() function and support for it.
03-16-00   pg      Added fs_seek_read(), fs_seek_write() functions and 
                   supports for them.
01-31-00   pg      Added fs_enum_init(), fs_enum_next() functions and
                   supports for them.
01-21-00   pg      Added function fs_rename().
                   Added support for rename operation in fsi_perform_subcmd().
01-05-00   rp      Added include of fs_dir.h for fs_dir_remove_remote_files().
12-28-99   rp      Added code to fsi_queue_cmd() to raise the rex priority
                   of any request coming from the nv task.  
12-17-99   rp      In fsi_task_init added call to fs_dir_remove_remote_files
                   under FS_FEATURE_AUTO_REMOVE_REMOTE_LINKS. 
12-16-99   rp      Added support for fs_remove_remote_links function. 
10-28-99   ls      Backward-merged Version 3.0 fixes.
09-22-99   jc      Added ifdef FEATURE_EFS to whole file.
06-28-99   ls      Fixed status reporting error.
06-22-99   ls      Fixed early cmd buffer release and incorrect returned 
                   status, also fixed reserved_write conf code handling.
05-20-99   ls      Added fmemset operation.
04-29-99   ls      Move ops_init to after rex_def_timers call.
04-29-99   ls      Clear rex write completion signal.
04-28-99   ls      Added more checks for task waiters.
04-25-99   ls      Added space_erased function.
04-23-99   ls      Changed scheduling of garbage collection.
04-22-99   ls      Removed enclosing ifdef.
04-21-99   ls      Version 2.2 release.
04-03-99   ls      Changed interface per Version 2.1 specification.
03-04-99   ls      Initial version.


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "task.h"
    
#ifdef FEATURE_EFS
#include "msg.h"
#include "err.h"
#include "dog.h"
#include "memory.h"
#include "mc.h"
#include "fsi.h"
#ifndef FEATURE_EFS2
#include "fs_alloc.h"
#include "fs_dir.h"
#endif
#include "fs_ops.h"

#ifdef FEATURE_EFS_ACCESS_METHODS
#include "fs_am.h"
#endif /* FEATURE_EFS_ACCESS_METHODS */


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

Deprecated  constants, macros, types, variables and other items, please refer
to fs_public.h for interface with the Embedded File System 2.

===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                         DATA DEFINITIONS                                */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                      REX Timers and Queues                              */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

rex_timer_type         fs_rpt_timer;     /* FS watchdog report timer. */
rex_timer_type         fs_write_timer;   /* FS write timer. */
q_type                 fs_cmd_q;         /* FS task command queue. */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                         Local Functions                                 */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_EFS_ACCESS_METHODS
static void  fsi_queue_cmd
(
  fs_cmd_type           *cmd_ptr,
  fs_handle_type         am_handle
);
#else /* not FEATURE_EFS_ACCESS_METHODS */
static void  fsi_queue_cmd
(
  fs_cmd_type           *cmd_ptr
);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
 
static int  fsi_allocate_cmd_buffer
(
  void
);

/*static*/ void  fsi_free_cmd_buffer
(
  int                   i
);

/*static*/ void  fsi_queue_subop
(
  fs_cmd_type           *cmd_ptr
);

#ifdef FEATURE_EFS_FLASH
static int  fsi_allocate_subcmd_buffer
(
  void
);
#endif /* FEATURE_EFS_FLASH */

#ifdef FEATURE_EFS_FLASH
static void  fsi_free_subcmd_buffer
(
  int                   i
);
#endif /* FEATURE_EFS_FLASH */

void  fsi_task_init
( 
  void
);

extern int efs_initialize (void);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                           Local Data                                    */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Command packet buffers */
static fs_cmd_type             fs_cmd_buffers[FSI_MAX_CMD_PACKETS];

/* The internal command queue (also known as the "sub-command" queue) */
static fsi_subcmd_queue_type   fsi_internal_cmd_queue;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/




#ifdef FS_FEATURE_UNIT_TEST
#error code not present
#endif /* FS_FEATURE_UNIT_TEST */





/*===========================================================================

FUNCTION FS_OPEN

DESCRIPTION
  This API is deprecated, please use efs_open() instead. For more information 
  refer to fs_public.h
  
===========================================================================*/

void  fs_open
(
  const char                 *filename,          /* Name of file to open */
  fs_access_option_type      access_mode,        /* Open mode */
  const fs_open_xparms_type  *open_options,      /* Optional parameters */
  fs_callback_p_type         callback_function,  /* Callback function ptr */
  fs_rsp_msg_type            *open_rsp_msg       /* Response message ptr */
)
{
  int                        i;                  /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type             am_handle;
  const char                *new_fname;  /* Filename to use. */
  boolean                    status;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  status = fs_am_allocate_handle (filename, &am_handle, &new_fname);

  if (!status) {
    open_rsp_msg->open.status = FS_OPEN_TABLE_FULL_S;
    if (callback_function != NULL) {
      (*callback_function) (open_rsp_msg);
    }

    return;    /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                                = FS_OPEN_OP;
  fs_cmd_buffers[i].fs_op.parms.open.access_kind            = access_mode;
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_cmd_buffers[i].fs_op.parms.open.option.create.filename = 
      (fs_filename_type)new_fname;
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fs_cmd_buffers[i].fs_op.parms.open.option.create.filename = 
      (fs_filename_type)filename;
#endif /* not FEATURE_EFS_ACCESS_METHODS */

  if (access_mode == FS_OA_CREATE)
  {
    /* Copy additional file creation parameters */
    fs_cmd_buffers[i].fs_op.parms.open.option.create.cleanup_option   = 
        open_options->create.cleanup_option;
    fs_cmd_buffers[i].fs_op.parms.open.option.create.buffering_option = 
        open_options->create.buffering_option;
    fs_cmd_buffers[i].fs_op.parms.open.option.create.attribute_mask   = 
        open_options->create.attribute_mask;
    fs_cmd_buffers[i].fs_op.parms.open.option.create.file_type        = 
        FSI_FILE_TYPE_USER;
  }
  else if (access_mode == FS_OA_TRUNCATE)
  {
    /* Copy additional file truncation position parameter */
    fs_cmd_buffers[i].fs_op.parms.open.option.truncate.position       =
        open_options->truncate.position;
  }

  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p = callback_function;
  fs_cmd_buffers[i].rsp_msg    = open_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* FEATURE_EFS_ACCESS_METHODS */
  return;

} /* fs_open */





/*===========================================================================

FUNCTION FS_CLOSE

DESCRIPTION
  This API is deprecated, please use efs_close() instead. For more information
  refer to fs_public.h.

===========================================================================*/

void  fs_close
(
  fs_handle_type        handle,            /* Handle of file to close */
  fs_callback_p_type    callback_function, /* Callback function pointer */
  fs_rsp_msg_type       *close_rsp_msg     /* Response message pointer */
)
{
  int                   i;                 /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type        am_handle = handle;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  /* Translate the handle. */
  handle = fs_am_lookup_handle (am_handle);

  if (handle == FS_INVALID_HANDLE) {
    close_rsp_msg->close.status = FS_BAD_FILE_HANDLE_S;
    if (callback_function != NULL) {
      (*callback_function) (close_rsp_msg);
    }

    return;  /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                   = FS_CLOSE_OP;
  fs_cmd_buffers[i].fs_op.parms.close.fhandle  = handle;
  
  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                 = callback_function;
  fs_cmd_buffers[i].rsp_msg                    = close_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;

} /* fs_close */





/*===========================================================================

FUNCTION FS_READ

DESCRIPTION
  This API is deprecated, please use efs_read () instead. For more information
  refer to fs_public.h.
  
===========================================================================*/

void  fs_read
(
  fs_handle_type        handle,            /* Handle of file to read from */
  PACKED void           *buffer,           /* Buffer for read bytes */
  dword                 count,             /* Number of bytes to read */
  fs_callback_p_type    callback_function, /* Callback function pointer */
  fs_rsp_msg_type       *read_rsp_msg      /* Response message pointer */
)
{
  int                   i;                 /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type        am_handle = handle;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  /* Translate the handle. */
  handle = fs_am_lookup_handle (am_handle);

  if (handle == FS_INVALID_HANDLE) {
    read_rsp_msg->close.status = FS_BAD_FILE_HANDLE_S;
    if (callback_function != NULL) {
      (*callback_function) (read_rsp_msg);
    }

    return;  /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                  = FS_READ_OP;
  fs_cmd_buffers[i].fs_op.parms.read.fhandle  = handle;
  fs_cmd_buffers[i].fs_op.parms.read.buf      = (byte *)buffer;
  fs_cmd_buffers[i].fs_op.parms.read.length   = count;
    
  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                = callback_function;
  fs_cmd_buffers[i].rsp_msg                   = read_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;

} /* fs_read */





/*===========================================================================

FUNCTION FS_WRITE

DESCRIPTION
  This API is deprecated, please use efs_write() instead. For more information
  refer to fs_public.h.

===========================================================================*/

void  fs_write
(
  fs_handle_type        handle,            /* Handle of file to write to */
  PACKED const void     *buffer,           /* Buffer with bytes to write */
  dword                 count,             /* Number of bytes to write */
  fs_callback_p_type    callback_function, /* Callback function pointer */
  fs_rsp_msg_type       *write_rsp_msg     /* Response message pointer */
)
{
  int                   i;                 /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type        am_handle = handle;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  /* Translate the handle. */
  handle = fs_am_lookup_handle (am_handle);

  if (handle == FS_INVALID_HANDLE) {
    write_rsp_msg->close.status = FS_BAD_FILE_HANDLE_S;
    if (callback_function != NULL) {
      (*callback_function) (write_rsp_msg);
    }

    return;  /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                     = FS_WRITE_OP;
  fs_cmd_buffers[i].fs_op.parms.write.fhandle    = handle;
  fs_cmd_buffers[i].fs_op.parms.write.buf        = (byte *)buffer;
  fs_cmd_buffers[i].fs_op.parms.write.length     = count;
  fs_cmd_buffers[i].fs_op.parms.write.ccode.code = FS_NULL_CONF_CODE;

  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                   = callback_function;
  fs_cmd_buffers[i].rsp_msg                      = write_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;

} /* fs_write */





/*===========================================================================

FUNCTION FS_RESERVED_WRITE

DESCRIPTION
  This API is deprecated, please use efs_set_reservation() instead. For more
  information refer to fs_public.h. On the new API reservation can be done for
  a given group ID. A directory can be the owner of the group ID and the
  reservation. All the files present in this folder will belong to the group
  ID of the folder and they would be consuming the space of the group ID
  reserved space.
  
===========================================================================*/

void  fs_reserved_write
(
  fs_handle_type        handle,            /* Handle of file to write to */
  PACKED const void     *buffer,           /* Buffer with bytes to write */
  dword                 count,             /* Number of bytes to write */
  fs_conf_code_type     conf_code,         /* Confirmation code */
  fs_callback_p_type    callback_function, /* Callback function pointer */
  fs_rsp_msg_type       *write_rsp_msg     /* Response message pointer */
)
{
  int                   i;                 /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type        am_handle = handle;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  /* Translate the handle. */
  handle = fs_am_lookup_handle (am_handle);

  if (handle == FS_INVALID_HANDLE) {
    write_rsp_msg->close.status = FS_BAD_FILE_HANDLE_S;
    if (callback_function != NULL) {
      (*callback_function) (write_rsp_msg);
    }

    return;  /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                   = FS_RSVD_WRITE_OP;
  fs_cmd_buffers[i].fs_op.parms.write.fhandle  = handle;
  fs_cmd_buffers[i].fs_op.parms.write.buf      = (byte *)buffer;
  fs_cmd_buffers[i].fs_op.parms.write.length   = count;
  fs_cmd_buffers[i].fs_op.parms.write.ccode    = conf_code;

  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                 = callback_function;
  fs_cmd_buffers[i].rsp_msg                    = write_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;

} /* fs_reserved_write */





/*===========================================================================

FUNCTION FS_SET_FILE_ATTRIBUTES


DESCRIPTION
  This API is deprecated. Please refer to fs_public.h for replacement APIs.
  The read/write/exec attributes can be changed using efs_chmod. For file
  buffering use setvbuf() in fs_stdio.h API. All files that use the embedded 
  file system are bad power down recoverable. File date attributes cannot be
  changed in EFS2 APIs.

===========================================================================*/

void  fs_set_file_attributes
(
  const char                *filename,           /* File name */
  fs_attribute_mask_type    attributes,          /* New attributes */
  fs_file_date_type         creation_date,       /* File creation date */
  fs_buffering_option_type  buffering_option,    /* Whether bufferable */
  fs_cleanup_option_type    cleanup_option,      /* Bad powerdown clean-up */
  fs_callback_p_type        callback_function,   /* Callback function ptr */
  fs_rsp_msg_type           *set_attribs_rsp_msg /* Response message ptr */
)
{
  int                       i;                   /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type            am_handle;
  const char               *new_fname;
  boolean                   status;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  status = fs_am_allocate_handle (filename, &am_handle, &new_fname);

  if (!status) {
    set_attribs_rsp_msg->set_attribs.status = FS_OPEN_TABLE_FULL_S;
    if (callback_function != NULL) {
      (*callback_function) (set_attribs_rsp_msg);
    }

    return;    /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                                 = 
      FS_SET_ATTRIBS_OP;
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_cmd_buffers[i].fs_op.parms.set_attribs.filename         = 
      (fs_filename_type)new_fname;
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fs_cmd_buffers[i].fs_op.parms.set_attribs.filename         = 
      (fs_filename_type)filename;
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  fs_cmd_buffers[i].fs_op.parms.set_attribs.attributes       = 
      attributes;
  fs_cmd_buffers[i].fs_op.parms.set_attribs.creation_date    = 
      creation_date;
  fs_cmd_buffers[i].fs_op.parms.set_attribs.buffering_option = 
      buffering_option;
  fs_cmd_buffers[i].fs_op.parms.set_attribs.cleanup_option   = 
      cleanup_option;
  fs_cmd_buffers[i].fs_op.parms.set_attribs.file_type        =
      FSI_FILE_TYPE_USER;

  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p = callback_function;
  fs_cmd_buffers[i].rsp_msg    = set_attribs_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;

} /* fs_set_file_attributes */





/*===========================================================================

FUNCTION FS_GET_FILE_ATTRIBUTES

DESCRIPTION
  This API is deprecated, please use efs_stat(), efs_fstat() and efs_lstat()
  instead. The new APIs are defined and explained in fs_public.h.
  
===========================================================================*/

void  fs_get_file_attributes
(
  const char                *filename,           /* File name */
  fs_callback_p_type        callback_function,   /* Callback function ptr */
  fs_rsp_msg_type           *get_attribs_rsp_msg /* Response message ptr */
)
{
  int                       i;                   /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type            am_handle;
  const char               *new_fname;
  boolean                   status;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  status = fs_am_allocate_handle (filename, &am_handle, &new_fname);

  if (!status) {
    get_attribs_rsp_msg->get_attribs.status = FS_OPEN_TABLE_FULL_S;
    if (callback_function != NULL) {
      (*callback_function) (get_attribs_rsp_msg);
    }

    return;    /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */
                     
  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                         = FS_GET_ATTRIBS_OP;
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_cmd_buffers[i].fs_op.parms.get_attribs.filename = 
      (fs_filename_type)new_fname;
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fs_cmd_buffers[i].fs_op.parms.get_attribs.filename = 
      (fs_filename_type)filename;
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  fs_cmd_buffers[i].fs_op.parms.get_attribs.fhandle  = FS_NULL_HANDLE;

  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                       = callback_function;
  fs_cmd_buffers[i].rsp_msg                          = get_attribs_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;

} /* fs_get_file_attributes */





/*===========================================================================

FUNCTION FS_GET_OPEN_FILE_ATTRIBUTES

DESCRIPTION
  This API is deprecated, please use efs_fstat() instead. For more information
  refer to fs_public.h.
  
===========================================================================*/

void  fs_get_open_file_attributes
(
  fs_handle_type            handle,              /* File handle */
  fs_callback_p_type        callback_function,   /* Callback function ptr */
  fs_rsp_msg_type           *get_attribs_rsp_msg /* Response message ptr */
)
{
  int                       i;                   /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type            am_handle = handle;
#endif /* FEATURE_EFS_ACCESS_METHODS */
                     
#ifdef FEATURE_EFS_ACCESS_METHODS
  handle = fs_am_lookup_handle (am_handle);

  if (handle == FS_INVALID_HANDLE) {
    get_attribs_rsp_msg->close.status = FS_BAD_FILE_HANDLE_S;
    if (callback_function != NULL) {
      (*callback_function) (get_attribs_rsp_msg);
    }

    return;  /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                         = FS_GET_ATTRIBS_OP;
  fs_cmd_buffers[i].fs_op.parms.get_attribs.filename = NULL;
  fs_cmd_buffers[i].fs_op.parms.get_attribs.fhandle  = handle;
  
  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                      = callback_function;
  fs_cmd_buffers[i].rsp_msg                         = get_attribs_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;

} /* fs_get_open_file_attributes */





/*===========================================================================

FUNCTION FS_FILE_SIZE

DESCRIPTION
  This API is deprecated, please use efs_stat(), efs_fstat() or efs_lstat()
  instead. The new APIs are explained in fs_public.h
  
===========================================================================*/

void  fs_file_size
(
  const char                *filename,           /* Name of file */
  fs_callback_p_type        callback_function,   /* Callback function ptr */
  fs_rsp_msg_type           *file_size_rsp_msg   /* Response message ptr */
)
{
  int                       i;                   /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type            am_handle;
  const char               *new_fname;
  boolean                   status;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  status = fs_am_allocate_handle (filename, &am_handle, &new_fname);

  if (!status) {
    file_size_rsp_msg->file_size.status = FS_OPEN_TABLE_FULL_S;
    if (callback_function != NULL) {
      (*callback_function) (file_size_rsp_msg);
    }

    return;    /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                        = FS_FILE_SIZE_OP;
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_cmd_buffers[i].fs_op.parms.file_size.filename  = 
      (fs_filename_type)new_fname;
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fs_cmd_buffers[i].fs_op.parms.file_size.filename  = 
      (fs_filename_type)filename;
#endif /* not FEATURE_EFS_ACCESS_METHODS */

  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                      = callback_function;
  fs_cmd_buffers[i].rsp_msg                         = file_size_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;

} /* fs_file_size */


/*===========================================================================

FUNCTION FS_OPENED_FILE_SIZE

DESCRIPTION
  This API is deprecated, please use efs_fstat() instead. For more information
  refer to fs_public.h

===========================================================================*/

void fs_opened_file_size (
    fs_handle_type      handle,
    fs_callback_p_type  callback_function,
    fs_rsp_msg_type     *file_size_rsp_msg)
{
  int i;
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type        am_handle = handle;
#endif /* not FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  handle = fs_am_lookup_handle (am_handle);

  if (handle == FS_INVALID_HANDLE) {
    file_size_rsp_msg->file_size.status = FS_BAD_FILE_HANDLE_S;
    if (callback_function != NULL) {
      (*callback_function) (file_size_rsp_msg);
    }

    return;   /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  i = fsi_allocate_cmd_buffer ();

  fs_cmd_buffers[i].fs_op.op = FS_OPENED_FILE_SIZE_OP;
  fs_cmd_buffers[i].fs_op.parms.opened_file_size.fhandle = handle;

  fs_cmd_buffers[i].callback_p = callback_function;
  fs_cmd_buffers[i].rsp_msg = file_size_rsp_msg;

#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd (&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd (&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */

  return;
}


/*===========================================================================

FUNCTION FS_SEEK

DESCRIPTION
  This API is deprecated, please use efs_lseek() instead. For more information
  refer to fs_public.h
  
===========================================================================*/

void  fs_seek
(
  fs_handle_type        handle,                /* File handle */
  fs_seek_origin_type   origin,                /* Seek origin */
  fs_file_position_type position,              /* Position to seek to */
  fs_callback_p_type    callback_function,     /* Callback function ptr */
  fs_rsp_msg_type       *seek_rsp_msg          /* Response message ptr */
)
{
  int                   i;                     /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type        am_handle = handle;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  /* Translate the handle. */
  handle = fs_am_lookup_handle (am_handle);

  if (handle == FS_INVALID_HANDLE) {
    seek_rsp_msg->close.status = FS_BAD_FILE_HANDLE_S;
    if (callback_function != NULL) {
      (*callback_function) (seek_rsp_msg);
    }

    return;  /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                   = FS_SEEK_OP;
  fs_cmd_buffers[i].fs_op.parms.seek.fhandle   = handle;
  fs_cmd_buffers[i].fs_op.parms.seek.origin    = origin;
  fs_cmd_buffers[i].fs_op.parms.seek.position  = position;
  
  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                 = callback_function;
  fs_cmd_buffers[i].rsp_msg                    = seek_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;

} /* fs_seek */





/*===========================================================================

FUNCTION FS_SEEK_READ

DESCRIPTION
  This API is deprecated, please use efs_fseek() and efs_read() instead. For
  more information refer to fs_public.h.
  
===========================================================================*/

void  fs_seek_read
(
  fs_handle_type        handle,                /* File handle */
  fs_seek_origin_type   origin,                /* Seek origin */
  fs_file_position_type position,              /* Position to seek to */
  PACKED void           *buffer,               /* Buffer for read bytes */
  dword                 count,                 /* Number of bytes to read */
  fs_callback_p_type    callback_function,     /* Callback function ptr */
  fs_rsp_msg_type       *seek_read_rsp_msg          /* Response message ptr */
)
{
  int                   i;                     /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type        am_handle = handle;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  /* Translate the handle. */
  handle = fs_am_lookup_handle (am_handle);

  if (handle == FS_INVALID_HANDLE) {
    seek_read_rsp_msg->close.status = FS_BAD_FILE_HANDLE_S;
    if (callback_function != NULL) {
      (*callback_function) (seek_read_rsp_msg);
    }

    return;  /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                   = FS_SEEK_READ_OP;
  fs_cmd_buffers[i].fs_op.parms.seek_read.fhandle   = handle;
  fs_cmd_buffers[i].fs_op.parms.seek_read.origin    = origin;
  fs_cmd_buffers[i].fs_op.parms.seek_read.position  = position;
  fs_cmd_buffers[i].fs_op.parms.seek_read.buf      = (byte *)buffer;
  fs_cmd_buffers[i].fs_op.parms.seek_read.length   = count;
  
  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                 = callback_function;
  fs_cmd_buffers[i].rsp_msg                    = seek_read_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;

} /* fs_seek_read */





/*===========================================================================

FUNCTION FS_SEEK_WRITE

DESCRIPTION
  This API is deprecated, please use efs_lseek() and efs_write() instead. For
  more information refer to fs_public.h.
  
===========================================================================*/

void  fs_seek_write
(
  fs_handle_type        handle,                /* File handle */
  fs_seek_origin_type   origin,                /* Seek origin */
  fs_file_position_type position,              /* Position to seek to */
  PACKED const void     *buffer,               /* Buffer with bytes to write */
  dword                 count,                 /* Number of bytes to write */
  fs_callback_p_type    callback_function,     /* Callback function ptr */
  fs_rsp_msg_type       *seek_write_rsp_msg          /* Response message ptr */
)
{
  int                   i;                     /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type        am_handle = handle;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  /* Translate the handle. */
  handle = fs_am_lookup_handle (am_handle);

  if (handle == FS_INVALID_HANDLE) {
    seek_write_rsp_msg->close.status = FS_BAD_FILE_HANDLE_S;
    if (callback_function != NULL) {
      (*callback_function) (seek_write_rsp_msg);
    }

    return;  /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                          = FS_SEEK_WRITE_OP;
  fs_cmd_buffers[i].fs_op.parms.seek_write.fhandle    = handle;
  fs_cmd_buffers[i].fs_op.parms.seek_write.origin     = origin;
  fs_cmd_buffers[i].fs_op.parms.seek_write.position   = position;
  fs_cmd_buffers[i].fs_op.parms.seek_write.buf        = (byte *)buffer;
  fs_cmd_buffers[i].fs_op.parms.seek_write.length     = count;
  
  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                 = callback_function;
  fs_cmd_buffers[i].rsp_msg                    = seek_write_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;

} /* fs_seek_write */



/*===========================================================================

FUNCTION FS_TELL

DESCRIPTION
  This API is deprecated, please use efs_lseek() instead. For more information
  refer to fs_public.h. It can be replaced by calling
  efs_lseek(file_decriptor, 0, SEEK_CUR);
  
===========================================================================*/

void  fs_tell
(
  fs_handle_type        handle,                /* File handle */
  fs_callback_p_type    callback_function,     /* Callback function ptr */
  fs_rsp_msg_type       *tell_rsp_msg          /* Response message ptr */
)
{
  int                   i;                     /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type        am_handle = handle;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  /* Translate the handle. */
  handle = fs_am_lookup_handle (am_handle);

  if (handle == FS_INVALID_HANDLE) {
    tell_rsp_msg->close.status = FS_BAD_FILE_HANDLE_S;
    if (callback_function != NULL) {
      (*callback_function) (tell_rsp_msg);
    }

    return;  /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                   = FS_TELL_OP;
  fs_cmd_buffers[i].fs_op.parms.tell.fhandle   = handle;
  
  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                 = callback_function;
  fs_cmd_buffers[i].rsp_msg                    = tell_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;

} /* fs_tell */





/*===========================================================================

FUNCTION FS_TRUNCATE

DESCRIPTION
  This API is deprecated, please use efs_truncate() and efs_ftruncate() 
  instead. For more information refer to fs_public.h.
  
===========================================================================*/

void  fs_truncate
(
  fs_handle_type        handle,                /* File handle */
  fs_file_position_type truncate_pos,          /* File truncate position */
  fs_callback_p_type    callback_function,     /* Callback function ptr */
  fs_rsp_msg_type       *truncate_rsp_msg      /* Response message ptr */
)
{
  int                   i;                     /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type        am_handle = handle;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  /* Translate the handle. */
  handle = fs_am_lookup_handle (am_handle);

  if (handle == FS_INVALID_HANDLE) {
    truncate_rsp_msg->close.status = FS_BAD_FILE_HANDLE_S;
    if (callback_function != NULL) {
      (*callback_function) (truncate_rsp_msg);
    }

    return;  /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                      = FS_TRUNCATE_OP;
  fs_cmd_buffers[i].fs_op.parms.truncate.fhandle  = handle;
  fs_cmd_buffers[i].fs_op.parms.truncate.position = truncate_pos;
  
  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                    = callback_function;
  fs_cmd_buffers[i].rsp_msg                       = truncate_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;

} /* fs_truncate */





/*===========================================================================

FUNCTION FS_REMOVE

DESCRIPTION
  This API is deprecated, please use efs_unlink() instead. For more information
  refer to fs_public.h
  
===========================================================================*/

void  fs_remove
(
  const char              *filename,           /* Name of file to remove */
  fs_callback_p_type      callback_function,   /* Callback function ptr */
  fs_rsp_msg_type         *rmfile_rsp_msg      /* Response message ptr */
)
{
  int                     i;                   /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type          am_handle;
  const char             *new_fname;
  boolean                 status;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  status = fs_am_allocate_handle (filename, &am_handle, &new_fname);

  if (!status) {
    rmfile_rsp_msg->rmfile.status = FS_OPEN_TABLE_FULL_S;
    if (callback_function != NULL) {
      (*callback_function) (rmfile_rsp_msg);
    }

    return;    /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                     = FS_RMFILE_OP;
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_cmd_buffers[i].fs_op.parms.rmfile.filename  = 
      (fs_filename_type)new_fname;
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fs_cmd_buffers[i].fs_op.parms.rmfile.filename  = 
      (fs_filename_type)filename;
#endif /* not FEATURE_EFS_ACCESS_METHODS */

  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                   = callback_function;
  fs_cmd_buffers[i].rsp_msg                      = rmfile_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;

} /* fs_remove */

 



/*===========================================================================

FUNCTION FS_RENAME

DESCRIPTION
  This API is deprecated, please use efs_rename() instead. For more information
  refer to fs_public.h
  
===========================================================================*/

void  fs_rename
(
  const char                  *old_filename,     /* Current name of file */
  const char                  *new_filename,     /* New name of file */
  fs_callback_p_type          callback_function, /* Callback function ptr */
  fs_rsp_msg_type             *rename_rsp_msg    /* Response message ptr */
)
{
  int                         i;                 /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type              old_am_handle;
  fs_handle_type              new_am_handle;
  const char                 *old_fname;
  const char                 *new_fname;
  boolean status;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  status = fs_am_allocate_handle (old_filename, &old_am_handle, &old_fname);

  if (!status) {
    rename_rsp_msg->rename.status = FS_OPEN_TABLE_FULL_S;
    if (callback_function != NULL) {
      (*callback_function) (rename_rsp_msg);
    }

    return;     /* Error return. */
  }

  status = fs_am_allocate_handle (new_filename, &new_am_handle, &new_fname);
  if (!status) {
    fs_am_free_handle (old_am_handle);
    rename_rsp_msg->rename.status = FS_OPEN_TABLE_FULL_S;
    if (callback_function != NULL) {
      (*callback_function) (rename_rsp_msg);
    }

    return;
  }

  /* Chain these two handles together so that freeing the first will free
   * the second.  This also verifies that both use the same access method. */
  status = fs_am_chain_handles (old_am_handle, new_am_handle);
  if (!status) {
    fs_am_free_handle (old_am_handle);
    fs_am_free_handle (new_am_handle);
    rename_rsp_msg->rename.status = FS_ILLEGAL_OPERATION_S; /* Better idea? */
    if (callback_function != NULL) {
      (*callback_function) (rename_rsp_msg);
    }

    return;
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                        = FS_RENAME_OP;
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_cmd_buffers[i].fs_op.parms.rename.old_filename = 
      (fs_filename_type) old_fname;
  fs_cmd_buffers[i].fs_op.parms.rename.new_filename = 
      (fs_filename_type) new_fname;
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fs_cmd_buffers[i].fs_op.parms.rename.old_filename = 
      (fs_filename_type)old_filename;
  fs_cmd_buffers[i].fs_op.parms.rename.new_filename = 
      (fs_filename_type)new_filename;
#endif /* not FEATURE_EFS_ACCESS_METHODS */
   
  fs_cmd_buffers[i].callback_p                      = callback_function;
  fs_cmd_buffers[i].rsp_msg                         = rename_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd (&fs_cmd_buffers[i], old_am_handle);
#else
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif
  return;

} /* fs_rename */





/*===========================================================================

FUNCTION FS_FMEMSET

DESCRIPTION
  This API is deprecated. For more information refer to fs_public.h. To replace
  this function call the application can use efs_lseek() followed by an 
  efs_write() with the buffer which is memset with the specified character.
  
===========================================================================*/

void  fs_fmemset
(
  fs_handle_type        handle,                /* File handle */
  fs_file_position_type position,              /* Position to write up to */
  byte                  set_char,              /* Character to write */
  fs_callback_p_type    callback_function,     /* Callback function ptr */
  fs_rsp_msg_type       *fmemset_rsp_msg       /* Response message ptr */
)
{
  int                   i;                     /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type        am_handle = handle;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  /* Translate the handle. */
  handle = fs_am_lookup_handle (am_handle);

  if (handle == FS_INVALID_HANDLE) {
    fmemset_rsp_msg->close.status = FS_BAD_FILE_HANDLE_S;
    if (callback_function != NULL) {
      (*callback_function) (fmemset_rsp_msg);
    }

    return;  /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                       = FS_FMEMSET_OP;
  fs_cmd_buffers[i].fs_op.parms.fmemset.fhandle    = handle;
  fs_cmd_buffers[i].fs_op.parms.fmemset.position   = position;
  fs_cmd_buffers[i].fs_op.parms.fmemset.fill_value = set_char;

  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                     = callback_function;
  fs_cmd_buffers[i].rsp_msg                        = fmemset_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;

} /* fs_fmemset */





/*===========================================================================

FUNCTION FS_MKDIR

DESCRIPTION
  This API is deprecated, please use efs_mkdir() instead. For more information
  refer to fs_public.h
  
===========================================================================*/

void  fs_mkdir
(
  const char              *dirname,            /* Name of new directory */
  fs_callback_p_type      callback_function,   /* Callback function ptr */
  fs_rsp_msg_type         *mkdir_rsp_msg       /* Response message ptr */
)
{
  int                     i;                   /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type            am_handle;
  const char               *new_fname;
  boolean                   status;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  status = fs_am_allocate_handle (dirname, &am_handle, &new_fname);

  if (!status) {
    mkdir_rsp_msg->mkdir.status = FS_OPEN_TABLE_FULL_S;
    if (callback_function != NULL) {
      (*callback_function) (mkdir_rsp_msg);
    }

    return;    /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                   = FS_MKDIR_OP;
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_cmd_buffers[i].fs_op.parms.mkdir.dirname  = 
      (fs_filename_type)new_fname;
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fs_cmd_buffers[i].fs_op.parms.mkdir.dirname  = 
      (fs_filename_type)dirname;
#endif /* not FEATURE_EFS_ACCESS_METHODS */

  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                 = callback_function;
  fs_cmd_buffers[i].rsp_msg                    = mkdir_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* FEATURE_EFS_ACCESS_METHODS */
  return;

} /* fs_mkdir */





/*===========================================================================

FUNCTION FS_RMDIR

DESCRIPTION
  This API is deprecated, please use efs_rmdir() instead. For more information
  refer to fs_public.h

===========================================================================*/

void  fs_rmdir
(
  const char              *dirname,            /* Directory to remove */
  fs_callback_p_type      callback_function,   /* Callback function ptr */
  fs_rsp_msg_type         *rmdir_rsp_msg       /* Response message ptr */
)
{
  int                     i;                   /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type            am_handle;
  const char               *new_fname;
  boolean                   status;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  status = fs_am_allocate_handle (dirname, &am_handle, &new_fname);

  if (!status) {
    rmdir_rsp_msg->rmdir.status = FS_OPEN_TABLE_FULL_S;
    if (callback_function != NULL) {
      (*callback_function) (rmdir_rsp_msg);
    }

    return;    /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                   = FS_RMDIR_OP;
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_cmd_buffers[i].fs_op.parms.rmdir.dirname  = 
      (fs_filename_type)new_fname;
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fs_cmd_buffers[i].fs_op.parms.rmdir.dirname  = 
      (fs_filename_type)dirname;
#endif /* not FEATURE_EFS_ACCESS_METHODS */

  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                 = callback_function;
  fs_cmd_buffers[i].rsp_msg                    = rmdir_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;

} /* fs_rmdir */




/*===========================================================================

FUNCTION FS_NAMETEST

DESCRIPTION
  This API is deprecated. Please use efs_stat() to get file information and
  then use the following macros to determine the type:
    S_ISCHR(m)    For a character device file.
    S_ISDIR(m)    For a directory.
    S_ISBLK(m)    For a block device file.
    S_ISREG(m)    For a regular file.
    S_ISLNK(m)    For a symbolic link.
    S_ISSOCK(m)   For a socket.
    S_ISITM(m)    For an item file.
    S_ISSPECIAL(m) indicates if the file is one of special type of files, is
                   ORd combination of S_ISFIFO, S_ISCHR, S_ISBLK, S_ISSOCK.

  For more information refer to fs_public.h

===========================================================================*/

void  fs_nametest
(
  const char              *filename,           /* File name to check for */
  fs_nametest_type        type,                /* Nametest a file or a dir */
  fs_callback_p_type      callback_function,   /* Callback function ptr */
  fs_rsp_msg_type         *test_rsp_msg        /* Response message ptr */
)
{
  int                     i;                   /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type            am_handle;
  const char               *new_fname;
  boolean                   status;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  status = fs_am_allocate_handle (filename, &am_handle, &new_fname);

  if (!status) {
    test_rsp_msg->nametest.status = FS_OPEN_TABLE_FULL_S;
    if (callback_function != NULL) {
      (*callback_function) (test_rsp_msg);
    }

    return;    /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                      = FS_NAMETEST_OP;
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_cmd_buffers[i].fs_op.parms.nametest.filename = 
      (fs_filename_type)new_fname;
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fs_cmd_buffers[i].fs_op.parms.nametest.filename = 
      (fs_filename_type)filename;
#endif /* not FEATURE_EFS_ACCESS_METHODS */

  fs_cmd_buffers[i].fs_op.parms.nametest.type     = type;

  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p = callback_function;
  fs_cmd_buffers[i].rsp_msg    = test_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;

} /* fs_nametest */





/*===========================================================================

FUNCTION FS_NUM_FILES

DESCRIPTION
  This API is deprecated. Please refer to fs_public.h for replacement APIs.
  Use enumeration (efs_opendir, efs_readdir and efs_closedir) to get file/dir
  list, efs_stat() and use macro S_ISREG() to determine the type and count
  appropriately.

===========================================================================*/

void  fs_num_files
(
  const char              *dirname,            /* Root directory name */
  fs_callback_p_type      callback_function,   /* Callback function ptr */
  fs_rsp_msg_type         *num_files_rsp_msg   /* Response message ptr */
)
{
  int                     i;                   /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type            am_handle;
  const char               *new_fname;
  boolean                   status;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  status = fs_am_allocate_handle (dirname, &am_handle, &new_fname);

  if (!status) {
    num_files_rsp_msg->num_files.status = FS_OPEN_TABLE_FULL_S;
    if (callback_function != NULL) {
      (*callback_function) (num_files_rsp_msg);
    }

    return;    /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                      = FS_NUM_FILES_OP;
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_cmd_buffers[i].fs_op.parms.num_files.dirname = 
      (fs_filename_type)new_fname;
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fs_cmd_buffers[i].fs_op.parms.num_files.dirname = 
      (fs_filename_type)dirname;
#endif /* not FEATURE_EFS_ACCESS_METHODS */

  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                    = callback_function;
  fs_cmd_buffers[i].rsp_msg                       = num_files_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;

} /* fs_num_files */





/*===========================================================================

FUNCTION FS_NUM_DIRS

DESCRIPTION
  This API is deprecated. Please refer to fs_public.h for replacement APIs.
  The application can replace this call by using efs_opendir(), efs_readdir(),
  and efs_closedir() to enumerate files and directories. Use efs_stat() and
  macro S_ISDIR() to find out if it is a directory.

===========================================================================*/

void  fs_num_dirs
(
  const char              *dirname,            /* Root directory name */
  fs_callback_p_type      callback_function,   /* Callback function ptr */
  fs_rsp_msg_type         *num_dirs_rsp_msg    /* Response message ptr */
)
{
  int                     i;                   /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type            am_handle;
  const char               *new_fname;
  boolean                   status;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  status = fs_am_allocate_handle (dirname, &am_handle, &new_fname);

  if (!status) {
    num_dirs_rsp_msg->num_dirs.status = FS_OPEN_TABLE_FULL_S;
    if (callback_function != NULL) {
      (*callback_function) (num_dirs_rsp_msg);
    }

    return;    /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                     = FS_NUM_DIRS_OP;
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_cmd_buffers[i].fs_op.parms.num_dirs.dirname = 
      (fs_filename_type)new_fname;
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fs_cmd_buffers[i].fs_op.parms.num_dirs.dirname = 
      (fs_filename_type)dirname;
#endif /* not FEATURE_EFS_ACCESS_METHODS */

  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                   = callback_function;
  fs_cmd_buffers[i].rsp_msg                      = num_dirs_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;

} /* fs_num_dirs */





/*===========================================================================

FUNCTION FS_LIST_FILES

DESCRIPTION
  This API is deprecated. Please refer to fs_public.h for replacement APIs.
  The application can replace this call by using efs_opendir(), efs_readdir(),
  and efs_closedir() to step through files and directories. Use efs_stat() and
  macro S_ISREG() to find out if it is a file.
  
===========================================================================*/

void  fs_list_files
(
  const char              *dirname,            /* Root directory name */
  void                    *buf,                /* Buffer to receive list */
  dword                   buf_length,          /* Length of buffer */
  fs_callback_p_type      callback_function,   /* Callback function ptr */
  fs_rsp_msg_type         *listf_rsp_msg       /* Response message ptr */
)
{
  int                     i;                   /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type            am_handle;
  const char               *new_fname;
  boolean                   status;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  status = fs_am_allocate_handle (dirname, &am_handle, &new_fname);

  if (!status) {
    listf_rsp_msg->list_files.status = FS_OPEN_TABLE_FULL_S;
    if (callback_function != NULL) {
      (*callback_function) (listf_rsp_msg);
    }

    return;    /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                       = FS_LIST_FILES_OP;
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_cmd_buffers[i].fs_op.parms.list_files.dirname = 
      (fs_filename_type)new_fname;
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fs_cmd_buffers[i].fs_op.parms.list_files.dirname = 
      (fs_filename_type)dirname;
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  fs_cmd_buffers[i].fs_op.parms.list_files.buf     = (byte *)buf;
  fs_cmd_buffers[i].fs_op.parms.list_files.length  = buf_length;

#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_cmd_buffers[i].fs_op.parms.list_files.full_length = buf_length;
#endif /* FEATURE_EFS_ACCESS_METHODS */
  
  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                     = callback_function;
  fs_cmd_buffers[i].rsp_msg                        = listf_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;             

} /* fs_list_files */





/*===========================================================================

FUNCTION FS_LIST_DIRS

DESCRIPTION
  This API is deprecated. Please refer to fs_public.h for replacement APIs.
  The application can replace this call by using efs_opendir(), efs_readdir(),
  and efs_closedir() to step through files and directories. Use efs_stat() and
  macro S_ISDIR() to find out if it is a directory.

===========================================================================*/

void  fs_list_dirs
(
  const char              *dirname,            /* Root directory name */
  void                    *buf,                /* Buffer to receive list */
  dword                   buf_length,          /* Length of buffer */
  fs_callback_p_type      callback_function,   /* Callback function ptr */
  fs_rsp_msg_type         *listd_rsp_msg       /* Response message ptr */
)
{
  int                     i;                   /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type            am_handle;
  const char               *new_fname;
  boolean                   status;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  status = fs_am_allocate_handle (dirname, &am_handle, &new_fname);

  if (!status) {
    listd_rsp_msg->list_dirs.status = FS_OPEN_TABLE_FULL_S;
    if (callback_function != NULL) {
      (*callback_function) (listd_rsp_msg);
    }

    return;    /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                      = FS_LIST_DIRS_OP;
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_cmd_buffers[i].fs_op.parms.list_dirs.dirname = 
      (fs_filename_type)new_fname;
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fs_cmd_buffers[i].fs_op.parms.list_dirs.dirname = 
      (fs_filename_type)dirname;
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  fs_cmd_buffers[i].fs_op.parms.list_dirs.buf     = (byte *)buf;
  fs_cmd_buffers[i].fs_op.parms.list_dirs.length  = buf_length;

#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_cmd_buffers[i].fs_op.parms.list_dirs.full_length = buf_length;
#endif /* FEATURE_EFS_ACCESS_METHODS */
  
  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                    = callback_function;
  fs_cmd_buffers[i].rsp_msg                       = listd_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;

} /* fs_list_dirs */





/*===========================================================================

FUNCTION FS_ENUM_INIT

DESCRIPTION
  This API is deprecated, please use efs_opendir() instead. For more 
  information refer to fs_public.h
  
===========================================================================*/

void  fs_enum_init
(
  const char                  *dirname,          /* Root directory */
  fs_enum_type                enum_kind,         /* File or subdir list */
  fs_enum_iterator_type       *iterator_ptr,     /* Ptr to control struct */
  fs_callback_p_type          callback_function, /* Callback function ptr */
  fs_rsp_msg_type             *enum_init_rsp_msg /* Response message ptr */
)
{
  int                         i;                 /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type              am_handle;
  const char                 *new_fname;
  boolean                     status;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  status = fs_am_allocate_handle (dirname, &am_handle, &new_fname);

  if (!status) {
    enum_init_rsp_msg->enum_init.status = FS_OPEN_TABLE_FULL_S;
    if (callback_function != NULL) {
      (*callback_function) (enum_init_rsp_msg);
    }

    return;   /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                           = FS_ENUM_INIT_OP;
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_cmd_buffers[i].fs_op.parms.enum_init.dirname      =
      (fs_filename_type)new_fname;
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fs_cmd_buffers[i].fs_op.parms.enum_init.dirname      =
      (fs_filename_type)dirname;
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  fs_cmd_buffers[i].fs_op.parms.enum_init.enum_kind    = enum_kind;
  fs_cmd_buffers[i].fs_op.parms.enum_init.iterator_ptr = iterator_ptr;

#ifdef FEATURE_EFS_ACCESS_METHODS
  iterator_ptr->am_handle = am_handle;
#endif

  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                         = callback_function;
  fs_cmd_buffers[i].rsp_msg                            = enum_init_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;             

} /* fs_enum_init */





/*===========================================================================

FUNCTION FS_ENUM_NEXT

DESCRIPTION
  This API is deprecated, please use efs_readdir() instead. For more
  information refer to fs_public.h
  
===========================================================================*/

void  fs_enum_next
(
  fs_enum_data_type           *data_ptr,         /* Returned file info */ 
  fs_enum_iterator_type       *iterator_ptr,     /* Ptr to control struct */
  fs_callback_p_type          callback_function, /* Callback function ptr */
  fs_rsp_msg_type             *enum_next_rsp_msg /* Response message ptr */
)
{
  int                         i;                 /* Command buffer index */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                           = FS_ENUM_NEXT_OP;
  fs_cmd_buffers[i].fs_op.parms.enum_next.data_ptr     = data_ptr;
  fs_cmd_buffers[i].fs_op.parms.enum_next.iterator_ptr = iterator_ptr;

  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                         = callback_function;
  fs_cmd_buffers[i].rsp_msg                            = enum_next_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], iterator_ptr->am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;             

} /* fs_enum_next */





/*===========================================================================

FUNCTION FS_ENUM_PREV

DESCRIPTION
  This API is deprecated. Replacement for this function is not available.

===========================================================================*/

void  fs_enum_prev
(
  fs_enum_data_type           *data_ptr,         /* Returned file info */ 
  fs_enum_iterator_type       *iterator_ptr,     /* Ptr to control struct */
  fs_callback_p_type          callback_function, /* Callback function ptr */
  fs_rsp_msg_type             *enum_prev_rsp_msg /* Response message ptr */
)
{
  int                         i;                 /* Command buffer index */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                           = FS_ENUM_PREV_OP;
  fs_cmd_buffers[i].fs_op.parms.enum_prev.data_ptr     = data_ptr;
  fs_cmd_buffers[i].fs_op.parms.enum_prev.iterator_ptr = iterator_ptr;

  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                         = callback_function;
  fs_cmd_buffers[i].rsp_msg                            = enum_prev_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], iterator_ptr->am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;             

} /* fs_enum_prev */





/*===========================================================================

FUNCTION FS_ENUM_FINISH

DESCRIPTION
  This API is deprecated, please use efs_closedir() instead. For more
  information refer to fs_public.h.
  
===========================================================================*/

void  fs_enum_finish
(
  fs_enum_iterator_type       *iterator_ptr,     /* Ptr to control struct */
  fs_callback_p_type          callback_function, /* Callback function ptr */
  fs_rsp_msg_type             *enum_finish_rsp_msg /* Response message ptr */
)
{
  int                         i;                 /* Command buffer index */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                           = FS_ENUM_FINISH_OP;
  fs_cmd_buffers[i].fs_op.parms.enum_finish.iterator_ptr = iterator_ptr;

  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                         = callback_function;
  fs_cmd_buffers[i].rsp_msg                            = enum_finish_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], iterator_ptr->am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;             

} /* fs_enum_finish */





/*===========================================================================

FUNCTION FS_REMOTE_LINK

DESCRIPTION
  This API is deprecated, please use efs_symlink instead. For more information
  refer to fs_public.h.
  
===========================================================================*/

void  fs_remote_link
(
  const char              *filename,           /* Name of remote file */
  dword                   base_address,        /* Address of remote file */
  dword                   length,              /* Size of remote file */
  fs_callback_p_type      callback_function,   /* Callback function ptr */
  fs_rsp_msg_type         *remote_rsp_msg      /* Response message ptr */
)
{
  int                     i;                   /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type          am_handle;
  const char             *new_fname;
  boolean                 status;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  status = fs_am_allocate_handle (filename, &am_handle, &new_fname);

  if (!status) {
    /* FIXME */
    ERR_FATAL ("fs_remove called without sufficient resources",
               0, 0, 0);
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op                             = FS_REMOTE_LINK_OP;
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_cmd_buffers[i].fs_op.parms.remote_link.filename     = 
      (fs_filename_type)new_fname;
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fs_cmd_buffers[i].fs_op.parms.remote_link.filename     = 
      (fs_filename_type)filename;
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  fs_cmd_buffers[i].fs_op.parms.remote_link.base_address = base_address;
  fs_cmd_buffers[i].fs_op.parms.remote_link.length       = length;

  /* Set callback function and response message pointer */
  fs_cmd_buffers[i].callback_p                           = callback_function;
  fs_cmd_buffers[i].rsp_msg                              = remote_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
#else /* not FEATURE_EFS_ACCESS_METHODS */
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif /* not FEATURE_EFS_ACCESS_METHODS */
  return;

} /* fs_remote_link */




/*===========================================================================

FUNCTION FS_REMOVE_REMOTE_LINKS

DESCRIPTION
  This API is deprecated.

===========================================================================*/

void  fs_remove_remote_links
(
  fs_callback_p_type          callback_function, /* Callback function ptr */
  fs_rsp_msg_type             *rmrlink_rsp_msg   /* Response message ptr */
)
{
  int                         i;                 /* Command buffer index */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type              am_handle;
  const char                 *new_fname;
  boolean                     status;
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  status = fs_am_allocate_handle ("", &am_handle, &new_fname);

  if (!status) {
    rmrlink_rsp_msg->rmrlink.status = FS_OPEN_TABLE_FULL_S;
    if (callback_function != NULL) {
      (*callback_function) (rmrlink_rsp_msg);
    }

    return;   /* Error return. */
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer();

  /* Construct the operation request packet */
  fs_cmd_buffers[i].fs_op.op             = FS_RM_REMOTE_LINK_OP;
  
  fs_cmd_buffers[i].callback_p         = callback_function;
  fs_cmd_buffers[i].rsp_msg            = rmrlink_rsp_msg;

  /* Set remainder of operation request packet and queue command */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd (&fs_cmd_buffers[i], am_handle);
#else
  fsi_queue_cmd(&fs_cmd_buffers[i]);
#endif

  return;

} /* fs_remove_remote_links */

/* These calls are only for Flash based EFS. */
#ifdef FEATURE_EFS_FLASH

/*===========================================================================

FUNCTION FS_RESERVE_SPACE

DESCRIPTION
  This API is deprecated, please use efs_set_reservation() instead. For more
  information refer to fs_public.h.On the new API reservation can be done for
  a given group ID. A directory can be the owner of the group ID and the
  reservation. All  the files present in this folder will belong to the group
  ID of the folder and  they would be consuming the space of the group ID
  reserved space.

  
===========================================================================*/

fs_conf_code_type  fs_reserve_space
(
  boolean               for_revertable_file,
  fs_rsvr_request_type  rsvr_request
)
{
  /* Call OPS-level function to handle pre-allocation of requested */
  /* number of bytes                                               */
  return fs_ops_reserve_space(for_revertable_file,rsvr_request);

} /* fs_reserve_space */





/*===========================================================================

FUNCTION FS_CANCEL_RESERVATION

DESCRIPTION
  This API is deprecated, please use efs_set_reservation() giving reservation 
  size of zero. For more information refer to fs_public.h.

===========================================================================*/

fs_status_type  fs_cancel_reservation
(
  fs_conf_code_type     conf_code
)
{
  /* Call OPS-level function to handle pre-allocation of requested */
  /* number of bytes                                               */
  return fs_ops_cancel_reservation(conf_code);

} /* fs_cancel_reservation */





/*===========================================================================

FUNCTION FS_RESERVATION_IS_NULL

DESCRIPTION
  This API is deprecated.

===========================================================================*/

boolean  fs_reservation_is_null
(
  fs_conf_code_type     conf_code
)
{
  /* Call OPS-level function to determine whether specified confirmation */
  /* code is valid                                                       */
  return fs_ops_reservation_is_null(conf_code);

} /* fs_reservation_is_null */





/*===========================================================================

FUNCTION FS_SPACE_AVAILABLE

DESCRIPTION
  This API is deprecated, please use efs_statvfs() instead. For more
  information refer to fs_public.h
  
===========================================================================*/

dword  fs_space_available
(
  void
)
{
  /* Call OPS-level function to obtain byte count of available space */
  return fs_ops_space_available();

} /* fs_space_available */





/*===========================================================================

FUNCTION FS_SPACE_ERASED

DESCRIPTION
  This API is deprecated, please use efs_statvfs() instead. Please refer to
  fs_public.h for more information.

===========================================================================*/

dword  fs_space_erased
(
  void
)
{
  /* Call OPS-level function to obtain byte count of erased space */
  return fs_ops_space_erased();
} /* fs_space_erased */





/*===========================================================================

FUNCTION FS_SPACE_USED

DESCRIPTION
  This API is deprecated, please use efs_statvfs() instead. For more
  information refer to fs_public.h
  
===========================================================================*/

dword  fs_space_used
(
  void
)
{
  /* Call OPS-level function to obtain byte count of available space */
  return fs_ops_space_used();

} /* fs_space_used */





/*===========================================================================

FUNCTION FS_SPACE_RESERVED

DESCRIPTION
  This API is deprecated. To get the space reserved by a group ID use
  efs_get_group_info(). For more information refer to fs_public.h.

===========================================================================*/

dword  fs_space_reserved
(
  void
)
{
  /* Call OPS-level function to obtain byte count of reserved space */
  return fs_ops_space_reserved();

} /* fs_space_reserved */




#ifdef FEATURE_FS_ALLOW_MASTER_RESET

/*===========================================================================

FUNCTION FS_MASTER_RESET

DESCRIPTION
  This API is deprecated, please use efs_reset() instead. For more information
  refer to fs_public.h
  
===========================================================================*/

void  fs_master_reset
(
  void
)
{
  /* Call OPS-level function to obtain byte count of reserved space */
  fs_ops_master_reset();

  ERR_FATAL ("Master reset finished, please reset", 0, 0, 0);

} /* fs_master_reset */
#endif /* FEATURE_FS_ALLOW_MASTER_RESET */


#else

static int cur_rsvr_id = 0;

/*===========================================================================

FUNCTION FS_RESERVE_SPACE

DESCRIPTION
  Return a bogus reservation confirmation code.
  This API is deprecated, please use efs_set_reservation() instead. For more
  information refer to fs_public.h.On the new API reservation can be done for
  a given GID. A directory can be the owner of the GID and the reservation. All
  the files present in this folder will belong to the GID of the folder and
  they would be consuming the space of the GID reserved space.
  
===========================================================================*/

fs_conf_code_type  fs_reserve_space
(
  boolean               for_revertable_file,
  fs_rsvr_request_type  rsvr_request
)
{
  fs_conf_code_type conf_code = {FS_NULL_CONF_CODE,(dword)0};

  /* cur_rsvr_id = (cur_rsvr_id % 0xffff) + 1; */
  cur_rsvr_id = (cur_rsvr_id + 1) & 0xFFFF;
  if (cur_rsvr_id == 0)
    cur_rsvr_id = 1;

  conf_code.count = rsvr_request.revertable_file_rsvr.amount;
  conf_code.code  = (cur_rsvr_id << 16) | (conf_code.count & 0xFFFF);

  return conf_code;
} /* fs_reserve_space */


/*===========================================================================

FUNCTION FS_CANCEL_RESERVATION

DESCRIPTION
  No-op.
  This API is deprecated, please use efs_set_reservation() giving reservation 
  size of zero. For more information refer to fs_public.h.

===========================================================================*/

fs_status_type  fs_cancel_reservation
(
  fs_conf_code_type     conf_code
)
{
  return FS_OKAY_S;

} /* fs_cancel_reservation */


/*===========================================================================

FUNCTION FS_RESERVATION_IS_NULL

DESCRIPTION
  This API is deprecated.
  Returns FALSE always.

===========================================================================*/

boolean  fs_reservation_is_null
(
  fs_conf_code_type     conf_code
)
{
  return FALSE;
} /* fs_reservation_is_null */


/*===========================================================================

FUNCTION FS_SPACE_AVAILABLE

DESCRIPTION
  This API is deprecated, please use efs_statvfs() instead. For more
  information refer to fs_public.h

===========================================================================*/

dword  fs_space_available
(
  void
)
{
  fs_rsp_msg_type statfs_rsp_msg;
  statfs_info     info;

  /* Klocwork wants to see these initialized */
  info.f_bsize = 0;
  info.f_bavail = 0;
  
  fs_statfs ( "/", &info, NULL, &statfs_rsp_msg );
  if ( statfs_rsp_msg.statfs.status == FS_OKAY_S )
    return info.f_bsize * info.f_bavail;
  else
    return 0x100000;
} /* fs_space_available */


/*===========================================================================

FUNCTION FS_SPACE_ERASED

DESCRIPTION
  This API is deprecated, please use efs_statvfs() instead. Please refer to
  fs_public.h for more information.
  Just returns a large bogus number.
  
===========================================================================*/

dword  fs_space_erased
(
  void
)
{
  return 0x100000;
} /* fs_space_erased */


/*===========================================================================

FUNCTION FS_SPACE_USED

DESCRIPTION
  This API is deprecated, please use efs_statvfs() instead. For more
  information refer to fs_public.h

===========================================================================*/

dword  fs_space_used
(
  void
)
{
  fs_rsp_msg_type statfs_rsp_msg;
  statfs_info     info;

  /* Klocworks wants to see these initialized */
  info.f_blocks = 0;
  info.f_bfree = 0;
  info.f_bsize = 0;
  
  fs_statfs ( "/", &info, NULL, &statfs_rsp_msg );
  if ( statfs_rsp_msg.statfs.status == FS_OKAY_S )
    return (info.f_blocks - info.f_bfree) * info.f_bsize;
  else
    return 0x100000;
} /* fs_space_used */


/*===========================================================================

FUNCTION FS_SPACE_RESERVED

DESCRIPTION
  This API is deprecated. To get the space reserved by a groupId use
  efs_get_group_info(). For more information refer to fs_public.h.
  Just returns a large bogus number.

===========================================================================*/

dword  fs_space_reserved
(
  void
)
{
  return 0x100000;
} /* fs_space_reserved */

/*===========================================================================

FUNCTION FS_DEV_GET_BASE_ADDR

DESCRIPTION
  This API is deprecated.
  Just returns a larger number.

===========================================================================*/
dword fs_dev_get_base_addr(void)
{
  return 0x7fffffff;
} /* END fs_dev_get_base_addr */

#endif /* FEATURE_EFS_FLASH */


/*===========================================================================

 FUNCTION FS_STATFS

 DESCRIPTION
  This API is deprecated, please use efs_statvfs() instead. For more
  information refer to fs_public.h.

===========================================================================*/
void
fs_statfs (
    const char         *filename,
    statfs_info        *info,
    fs_callback_p_type  callback_function,
    fs_rsp_msg_type    *statfs_rsp_msg
    )
{
  int i;
  const char *new_filename;
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type am_handle;
  boolean status;
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* The filename is only used to determine the access method to use.  The
   * actual call does not need it. */

#ifdef FEATURE_EFS_ACCESS_METHODS
  status = fs_am_allocate_handle (filename, &am_handle, &new_filename);

  if (!status) {
    statfs_rsp_msg->statfs.status = FS_OPEN_TABLE_FULL_S;
    if (callback_function != NULL) {
      (*callback_function) (statfs_rsp_msg);
    }

    return;
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Get index of an available command packet */
  i = fsi_allocate_cmd_buffer ();

  fs_cmd_buffers[i].fs_op.op = FS_STATFS_OP;
  fs_cmd_buffers[i].callback_p = callback_function;
  fs_cmd_buffers[i].rsp_msg = statfs_rsp_msg;
  fs_cmd_buffers[i].fs_op.parms.statfs.filename =  
                          (fs_filename_type) new_filename;

  fs_cmd_buffers[i].fs_op.parms.statfs.info = info;

  /* Queue command. */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fsi_queue_cmd (&fs_cmd_buffers[i], am_handle);
#else
  fsi_queue_cmd (&fs_cmd_buffers[i]);
#endif /* FEATURE_EFS_ACCESS_METHODS */
}



/*===========================================================================

FUNCTION FS_FORMAT

DESCRIPTION
  This API is deprecated, please use hotplug_format_dev() or hotplug_format()
  For more information refer to fs_hotplug.h.

===========================================================================*/

void  fs_format
(
  const char              *dirname,            /* Root directory name */
  fs_callback_p_type      callback_function,   /* Callback function ptr */
  fs_rsp_msg_type         *format_rsp_msg      /* Response message ptr */
)
{

  int                        i;                  /* Command buffer index */
  #ifdef FEATURE_EFS_ACCESS_METHODS
    fs_handle_type             am_handle;
    const char                 *new_fname;
    boolean                    status;
  #endif /* FEATURE_EFS_ACCESS_METHODS */

  #ifdef FEATURE_EFS_ACCESS_METHODS
    status = fs_am_allocate_handle (dirname, &am_handle, &new_fname);

    if (!status) {
      format_rsp_msg->format.status = FS_OPEN_TABLE_FULL_S;
      if (callback_function != NULL) {
        (*callback_function) (format_rsp_msg);
      }

      return;    /* Error return. */
    }
  #endif /* FEATURE_EFS_ACCESS_METHODS */

    /* Get index of an available command packet */
    i = fsi_allocate_cmd_buffer();

    /* Construct the operation request packet */
    fs_cmd_buffers[i].fs_op.op                                = FS_FORMAT_OP;
  #ifdef FEATURE_EFS_ACCESS_METHODS
    fs_cmd_buffers[i].fs_op.parms.format.dirname = 
        (fs_filename_type)new_fname;
  #else /* not FEATURE_EFS_ACCESS_METHODS */
    fs_cmd_buffers[i].fs_op.parms.format.dirname = 
        (fs_filename_type)dirname;
  #endif /* not FEATURE_EFS_ACCESS_METHODS */

    /* Set callback function and response message pointer */
    fs_cmd_buffers[i].callback_p = callback_function;
    fs_cmd_buffers[i].rsp_msg    = format_rsp_msg;

    /* Set remainder of operation request packet and queue command */
  #ifdef FEATURE_EFS_ACCESS_METHODS
    fsi_queue_cmd(&fs_cmd_buffers[i], am_handle);
  #else /* not FEATURE_EFS_ACCESS_METHODS */
    fsi_queue_cmd(&fs_cmd_buffers[i]);
  #endif /* FEATURE_EFS_ACCESS_METHODS */
    return;

}




#ifdef FEATURE_EFS_FLASH
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/*                                                                         */
/* The following functions are for "internal" use only (i.e., clients      */
/* should not directly call any of the following functions).               */
/*                                                                         */
/*                                                                         */

/*===========================================================================

FUNCTION FSI_HANDLE_CMD_QUEUE

DESCRIPTION
  This API is deprecated.
  This function does not have any meaning in EFS2.

===========================================================================*/

/*static*/ void  fsi_handle_cmd_queue
( 
  void
)
{
  fs_cmd_type        *cmd_ptr;  /* Pointer to cmd received on fs_cmd_q    */

  /* Handle all commands currently on the command queue */
  while ((cmd_ptr = (fs_cmd_type *)q_get(&fs_cmd_q)) != NULL) 
  {
    /* Put operation subcommand on internal queue */
    fsi_queue_subop(cmd_ptr);

    /* Certain subcommands require additional initialization */
    switch (cmd_ptr->fs_op.op) {

      case FS_READ_OP:
        /* Clear length field -- it will contain number of bytes */
        /* read so far                                           */
        cmd_ptr->rsp_msg->read.count = (dword)0;
        break;

      case FS_WRITE_OP:
      case FS_RSVD_WRITE_OP:
        /* Clear length field -- it will contain number of bytes */
        /* written so far                                        */
        cmd_ptr->rsp_msg->write.count = (dword)0;
        break;
        
      case FS_SEEK_READ_OP:
        /* Clear length field -- it will contain number of bytes */
        /* read so far                                           */
        cmd_ptr->rsp_msg->seek_read.count = (dword)0;
        break;
        
      case FS_SEEK_WRITE_OP:
        /* Clear length field -- it will contain number of bytes */
        /* read so far                                           */
        cmd_ptr->rsp_msg->seek_write.count = (dword)0;
        break;

      case FS_RESERVED_OP:
        /* TBD for testing */
        break;
 
      default:
        break;
    } /* switch */
  } /* while */

  return;
} /* fsi_handle_command_queue */
#else /* not FEATURE_EFS_FLASH */
/*===========================================================================

FUNCTION FSI_REJECT_CMD_QUEUE

DESCRIPTION
  This API is deprecated.
  This function does not have any meaning in EFS2.

===========================================================================*/

/*static*/ void  fsi_reject_cmd_queue
( 
  void
)
{
  fs_cmd_type        *cmd_ptr;  /* Pointer to cmd received on fs_cmd_q    */
  rex_tcb_type             *task_ptr;  /* Task to signal upon completion   */
  rex_sigs_type            task_sigs;  /* Signals to set for client task   */

  /* Handle all commands currently on the command queue */
  while ((cmd_ptr = (fs_cmd_type *)q_get(&fs_cmd_q)) != NULL) 
  {
    task_ptr  = cmd_ptr->tcb_ptr;
    task_sigs = cmd_ptr->sigs;

    /* Set the result to failure. */
    cmd_ptr->rsp_msg->open.status = FS_FAIL_S;

#ifdef FEATURE_EFS_ACCESS_METHODS
    fs_am_cleanup_after_command (cmd_ptr);
#endif

    /* Decrement the waiters field. */
    fs_ops_decrement_fs_waiters();

    /* Signal any requesting task, otherwise call the callback. */
    if (task_ptr == NULL) {
      /* Invoke callback. */
      cmd_ptr->callback_p (cmd_ptr->rsp_msg);
    } else {
      (void) rex_set_sigs (task_ptr, task_sigs);
    }
  }
} /* fsi_reject_cmd_queue */
#endif /* not FEATURE_EFS_FLASH */
#ifdef FEATURE_EFS_FLASH




/*===========================================================================

FUNCTION FSI_PERFORM_SUBCMD

DESCRIPTION
  This API is deprecated.
  This function does not have any meaning in EFS2.

===========================================================================*/

/*static*/ boolean  fsi_perform_subcmd
( 
  void
)
{
  word                     subcmd_i;   /* Index of a subcommand            */
  fs_op_request_type       op_req;     /* Operation request packet         */
  rex_tcb_type             *task_ptr;  /* Task to signal upon completion   */
  rex_sigs_type            task_sigs;  /* Signals to set for client task   */
  word                     ncount;     /* Next operation count             */
  int                      i;          /* Loop/index variable              */
  fs_status_type           status;     /* Operation status                 */
  boolean                  last;       /* Whether last subcommand of group */
  fsi_current_pointer_type file_ptr_pos;
  fsi_current_pointer_type eof_pos;
  fs_cleanup_option_type   cleanup_option;
  dword                    data_start_offset;
  word                     num_blks;


  last = TRUE;
  subcmd_i = fsi_internal_cmd_queue.entry[FSI_RSVD_SUBCMD_INDEX].next;

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif

  if (subcmd_i != FSI_NULL_SUBCMD_INDEX)
  {
    i = (int)fsi_internal_cmd_queue.entry[subcmd_i].cmd_queue_index;
    if (fs_cmd_buffers[i].tcb_ptr == &nv_tcb) 
    {
      fs_cmd_buffers[i].fs_op.req_task = FSI_OP_REQ_BY_NV;
    }
    else
    {
      fs_cmd_buffers[i].fs_op.req_task = FSI_OP_REQ_BY_OTHERS;
    }
    switch (fs_cmd_buffers[i].fs_op.op)
    {
      case FS_OPEN_OP:
        /* File OPEN is performed in one subcommand */
        fs_cmd_buffers[i].rsp_msg->open.handle = 
            fs_ops_open(&fs_cmd_buffers[i].fs_op);
        status = fs_cmd_buffers[i].fs_op.status;
        fs_cmd_buffers[i].rsp_msg->open.status = status;
        break;

      case FS_CLOSE_OP:
        /* File CLOSE is performed in one subcommand */
        status = fs_ops_close(&fs_cmd_buffers[i].fs_op);
        fs_cmd_buffers[i].rsp_msg->close.status = status;
        break;
  
      case FS_READ_OP:
        /* File READ is performed in one subcommand */
        status = fs_ops_read(&fs_cmd_buffers[i].fs_op);
        fs_cmd_buffers[i].rsp_msg->read.status = status;
        fs_cmd_buffers[i].rsp_msg->read.count  = 
            fs_cmd_buffers[i].fs_op.parms.read.length;
        break;

      case FS_RSVD_WRITE_OP:
        /* Verify that a non-NULL confirmation code was supplied (the   */
        /* additional checks on the confirmation code will be performed */
        /* by fs_ops, but they are triggered when non-NULL)             */
        if (fs_cmd_buffers[i].fs_op.parms.write.ccode.code == 
            FS_NULL_CONF_CODE)
        {
          /* Client didn't supply a valid confirmation code */
          status = FS_INVALID_CONF_CODE_S;
          fs_cmd_buffers[i].rsp_msg->write.status = status;
          break;
        }
        /* No break here, flow into "write" processing which follows */

      case FS_WRITE_OP:
        /* Get current file information needed to set up the subcommands */
        status = fs_ops_get_file_info(
                     fs_cmd_buffers[i].fs_op.parms.write.fhandle,
                     &file_ptr_pos,
                     &data_start_offset,
                     &num_blks,
                     &cleanup_option);
        if (status == FS_OKAY_S)
        {
          /* File WRITE may require multiple subcommand iterations */
          op_req.op = FS_WRITE_OP;
          op_req.req_task = fs_cmd_buffers[i].fs_op.req_task;
          op_req.parms.write.fhandle = 
              fs_cmd_buffers[i].fs_op.parms.write.fhandle;
          op_req.parms.write.ccode = 
              fs_cmd_buffers[i].fs_op.parms.write.ccode;

          /* Client's input buffer pointer must be offset by the */
          /* number of bytes already written                     */
          op_req.parms.write.buf = 
              fs_cmd_buffers[i].fs_op.parms.write.buf +
              fsi_internal_cmd_queue.entry[subcmd_i].progress;

          /* Only perform one block's worth of the operation */
          ncount = FSI_BLK_DATA_SIZE - 
              ((data_start_offset + file_ptr_pos) % FSI_BLK_DATA_SIZE);
          op_req.parms.write.length = 
              MIN(ncount,fsi_internal_cmd_queue.entry[subcmd_i].count);

          /* Write at most one block of file data */
          status = fs_ops_write(&op_req);

          /* Update "running counts" */
          fsi_internal_cmd_queue.entry[subcmd_i].count -=
              op_req.parms.write.length;
          fsi_internal_cmd_queue.entry[subcmd_i].progress +=
              op_req.parms.write.length;
          fs_cmd_buffers[i].rsp_msg->write.count += 
              op_req.parms.write.length;
          if (fsi_internal_cmd_queue.entry[subcmd_i].count != 0)
          {
            last = FALSE;
          }
          else
          {
            /* Release reservation if this was a reserved write */
            if (fs_cmd_buffers[i].fs_op.op == FS_RSVD_WRITE_OP)
            {
              fs_cmd_buffers[i].rsp_msg->write.status = 
                  fs_ops_cancel_reservation(
                      fs_cmd_buffers[i].fs_op.parms.write.ccode);
              if (status < fs_cmd_buffers[i].rsp_msg->write.status)
              {
                status = fs_cmd_buffers[i].rsp_msg->write.status;
              }
            }
          }
        }
        fs_cmd_buffers[i].rsp_msg->write.status = status;
        break;
         
      case FS_SET_ATTRIBS_OP:
        /* File SET_ATTRIBUTES is performed in one subcommand */
        status = fs_ops_set_attributes(&fs_cmd_buffers[i].fs_op);
        fs_cmd_buffers[i].rsp_msg->set_attribs.status = status;

        break;

      case FS_GET_ATTRIBS_OP:
        /* File GET_ATTRIBUTES is performed in one subcommand */
        status = fs_ops_get_attributes(&fs_cmd_buffers[i].fs_op);
        fs_cmd_buffers[i].rsp_msg->get_attribs.status = status;
        fs_cmd_buffers[i].rsp_msg->get_attribs.attributes =
            fs_cmd_buffers[i].fs_op.parms.get_attribs.attributes;
        fs_cmd_buffers[i].rsp_msg->get_attribs.creation_date =
            fs_cmd_buffers[i].fs_op.parms.get_attribs.creation_date;
        fs_cmd_buffers[i].rsp_msg->get_attribs.buffering_option =
            fs_cmd_buffers[i].fs_op.parms.get_attribs.buffering_option;
        fs_cmd_buffers[i].rsp_msg->get_attribs.cleanup_option =
            fs_cmd_buffers[i].fs_op.parms.get_attribs.cleanup_option;

        break;

      case FS_FILE_SIZE_OP:
        /* FILE_SIZE is performed in one subcommand */
        status = fs_ops_file_size(&fs_cmd_buffers[i].fs_op);
        fs_cmd_buffers[i].rsp_msg->file_size.status = status;
        fs_cmd_buffers[i].rsp_msg->file_size.size   = 
            fs_cmd_buffers[i].fs_op.parms.file_size.length;
        break;

      case FS_OPENED_FILE_SIZE_OP:
        /* OPENED FILE SIZE is performed in one subcommand */
        status = fs_ops_opened_file_size (&fs_cmd_buffers[i].fs_op);
        fs_cmd_buffers[i].rsp_msg->file_size.status = status;
        fs_cmd_buffers[i].rsp_msg->file_size.size =
          fs_cmd_buffers[i].fs_op.parms.file_size.length;
        break;

      case FS_SEEK_OP:
        /* File SEEK is one subcommand */
        status = fs_ops_seek(&fs_cmd_buffers[i].fs_op);
        fs_cmd_buffers[i].rsp_msg->seek.status = status;
        break;

      case FS_SEEK_READ_OP:
        op_req.op = FS_SEEK_OP;
        op_req.parms.seek.fhandle = 
            fs_cmd_buffers[i].fs_op.parms.seek_read.fhandle;
        op_req.parms.seek.origin =
            fs_cmd_buffers[i].fs_op.parms.seek_read.origin;
        op_req.parms.seek.position = 
            fs_cmd_buffers[i].fs_op.parms.seek_read.position;
        status = fs_ops_seek(&op_req);
        if (status != FS_OKAY_S) 
        {
          fs_cmd_buffers[i].rsp_msg->seek_read.status = status;
        }
        else 
        {
          op_req.op = FS_READ_OP;
          op_req.parms.read.fhandle =
              fs_cmd_buffers[i].fs_op.parms.seek_read.fhandle;
          op_req.parms.read.buf =
              fs_cmd_buffers[i].fs_op.parms.seek_read.buf;
          op_req.parms.read.length = 
              fs_cmd_buffers[i].fs_op.parms.seek_read.length;
          status = fs_ops_read(&op_req);
          fs_cmd_buffers[i].rsp_msg->seek_read.status = status;
          fs_cmd_buffers[i].rsp_msg->seek_read.count =
              op_req.parms.read.length;
        }
        break;
      case FS_SEEK_WRITE_OP:
        if (fsi_internal_cmd_queue.entry[subcmd_i].progress == 0) {
          /* The first iteration of possible multiple subcommand
             interations.  Do seek first. */
          op_req.op = FS_SEEK_OP;
          op_req.parms.seek.fhandle = 
              fs_cmd_buffers[i].fs_op.parms.seek_write.fhandle;
          op_req.parms.seek.origin =
              fs_cmd_buffers[i].fs_op.parms.seek_write.origin;
          op_req.parms.seek.position = 
              fs_cmd_buffers[i].fs_op.parms.seek_write.position;
          status = fs_ops_seek(&op_req);
          if (status != FS_OKAY_S) 
          {
            fs_cmd_buffers[i].rsp_msg->seek_write.status = status;
            break;
          }
        }
        status = fs_ops_get_file_info(
                     fs_cmd_buffers[i].fs_op.parms.seek_write.fhandle,
                     &file_ptr_pos,
                     &data_start_offset,
                     &num_blks,
                     &cleanup_option);
        if (status == FS_OKAY_S)
        {
          /* File WRITE may require multiple subcommand iterations */
          op_req.op = FS_WRITE_OP;
          op_req.req_task = fs_cmd_buffers[i].fs_op.req_task;
          op_req.parms.write.fhandle = 
              fs_cmd_buffers[i].fs_op.parms.seek_write.fhandle;
          op_req.parms.write.ccode.code = FS_NULL_CONF_CODE;

          /* Client's input buffer pointer must be offset by the */
          /* number of bytes already written                     */
          op_req.parms.write.buf = 
              fs_cmd_buffers[i].fs_op.parms.seek_write.buf +
              fsi_internal_cmd_queue.entry[subcmd_i].progress;

          /* Only perform one block's worth of the operation */
          ncount = FSI_BLK_DATA_SIZE - 
              ((data_start_offset + file_ptr_pos) % FSI_BLK_DATA_SIZE);
          op_req.parms.write.length = 
              MIN(ncount,fsi_internal_cmd_queue.entry[subcmd_i].count);

          /* Write at most one block of file data */
          status = fs_ops_write(&op_req);

          /* Update "running counts" */
          fsi_internal_cmd_queue.entry[subcmd_i].count -=
              op_req.parms.write.length;
          fsi_internal_cmd_queue.entry[subcmd_i].progress +=
              op_req.parms.write.length;
          fs_cmd_buffers[i].rsp_msg->seek_write.count += 
              op_req.parms.write.length;
          if (fsi_internal_cmd_queue.entry[subcmd_i].count != 0)
          {
            last = FALSE;
          }
        }
        fs_cmd_buffers[i].rsp_msg->seek_write.status = status;
        break;

      case FS_TELL_OP:
        /* File TELL is performed in one subcommand */
        status = fs_ops_tell(&fs_cmd_buffers[i].fs_op);
        fs_cmd_buffers[i].rsp_msg->tell.status   = status;
        fs_cmd_buffers[i].rsp_msg->tell.position = 
            fs_cmd_buffers[i].fs_op.parms.tell.position;
        break;

      case FS_TRUNCATE_OP:
        /* Get current file information needed to set up the subcommand */
        status = fs_ops_get_file_info(
                     fs_cmd_buffers[i].fs_op.parms.truncate.fhandle,
                     &file_ptr_pos,
                     &data_start_offset,
                     &num_blks,
                     &cleanup_option);
        if (status != FS_OKAY_S)
        {
          fs_cmd_buffers[i].rsp_msg->truncate.status = status;
        }
        else
        {
          /* File TRUNCATE may require multiple subcommand iterations;  */
          /* truncate from end of file backwards towards truncate point */
          op_req.op = FS_TRUNCATE_OP;
          op_req.parms.truncate.fhandle = 
              fs_cmd_buffers[i].fs_op.parms.truncate.fhandle;
 
          /* eof_pos is the byte at the beginning of the block containing */
          /* the end of file position                                     */
          eof_pos = ((num_blks == 1) ?
                     FSI_START_OF_FILE_POSITION :
                     (((num_blks-1)*FSI_BLK_DATA_SIZE)-data_start_offset));
         
          #ifdef FS_FEATURE_UNIT_TEST
#error code not present
          #endif

          /* Only perform one block's worth of the operation */
          op_req.parms.truncate.position = 
              MAX(fs_cmd_buffers[i].fs_op.parms.truncate.position,eof_pos);
          status = fs_ops_truncate(&fs_cmd_buffers[i].fs_op);
          fs_cmd_buffers[i].rsp_msg->truncate.status = status;
        }
        break;
 
      case FS_RMFILE_OP:
        /* Remove the file from the file system */
        status = fs_ops_remove_file(
                     fs_cmd_buffers[i].fs_op.parms.rmfile.filename);
        fs_cmd_buffers[i].rsp_msg->rmfile.status = status;
        break;
 
      case FS_RENAME_OP:
        /* Rename the specified file */
        status = fs_ops_rename_file(
                     fs_cmd_buffers[i].fs_op.parms.rename.old_filename,
                     fs_cmd_buffers[i].fs_op.parms.rename.new_filename);
        fs_cmd_buffers[i].rsp_msg->rename.status = status;
        break;

      case FS_FMEMSET_OP:
        /* File contents "set" is one subcommand */
        status = fs_ops_fmemset(&fs_cmd_buffers[i].fs_op);
        fs_cmd_buffers[i].rsp_msg->fmemset.status = status;
        break;

      case FS_MKDIR_OP:
        /* Make Directory is performed in one subcommand */
        status = fs_ops_mkdir(&fs_cmd_buffers[i].fs_op);
        fs_cmd_buffers[i].rsp_msg->mkdir.status = status;
        break;

      case FS_RMDIR_OP:
        /* Remove Directory is performed in one subcommand */
        status = fs_ops_rmdir(&fs_cmd_buffers[i].fs_op);
        fs_cmd_buffers[i].rsp_msg->rmdir.status = status;
        break;

      case FS_NAMETEST_OP:
        /* Test for file existence is performed in one subcommand */
        fs_cmd_buffers[i].rsp_msg->nametest.name_found = 
            fs_ops_nametest(&fs_cmd_buffers[i].fs_op);
        fs_cmd_buffers[i].rsp_msg->nametest.status = status = FS_OKAY_S;
        break;
       
      case FS_NUM_FILES_OP:
        /* NUM_FILES is performed in one subcommand */
        status = fs_ops_get_file_count(&fs_cmd_buffers[i].fs_op);
        fs_cmd_buffers[i].rsp_msg->num_files.status    = status;
        fs_cmd_buffers[i].rsp_msg->num_files.num_files = 
            fs_cmd_buffers[i].fs_op.parms.num_files.num_files;
        break;

      case FS_NUM_DIRS_OP:
        /* NUM_DIRS is performed in one subcommand */
        status = fs_ops_get_dir_count(&fs_cmd_buffers[i].fs_op);
        fs_cmd_buffers[i].rsp_msg->num_dirs.status   = status;
        fs_cmd_buffers[i].rsp_msg->num_dirs.num_dirs = 
            fs_cmd_buffers[i].fs_op.parms.num_dirs.num_dirs;
         break;
 
      case FS_LIST_FILES_OP:
        /* File LIST is performed in one subcommand */
        status = fs_ops_list_files(&fs_cmd_buffers[i].fs_op);
        fs_cmd_buffers[i].rsp_msg->list_files.status = status;
        fs_cmd_buffers[i].rsp_msg->list_files.count  = 
            fs_cmd_buffers[i].fs_op.parms.list_files.length;
#ifdef FEATURE_EFS_ACCESS_METHODS
        if (status == FS_OKAY_S) {
          fs_am_fixup_list (
              (char *) fs_cmd_buffers[i].fs_op.parms.list_files.buf,
              fs_cmd_buffers[i].fs_op.parms.list_files.full_length,
              &fs_cmd_buffers[i].rsp_msg->list_files.count,
              fs_cmd_buffers[i].am_handle);
        }
#endif /* FEATURE_EFS_ACCESS_METHODS */
        break;
 
      case FS_LIST_DIRS_OP:
        /* Directory LIST is performed in one subcommand */
        status = fs_ops_list_dirs(&fs_cmd_buffers[i].fs_op);
        fs_cmd_buffers[i].rsp_msg->list_dirs.status = status;
        fs_cmd_buffers[i].rsp_msg->list_dirs.count  = 
            fs_cmd_buffers[i].fs_op.parms.list_dirs.length;
        break;
     
      case FS_ENUM_INIT_OP:
        /* Initialization of a file or directory enumerator */
        status = fs_ops_enum_init(&fs_cmd_buffers[i].fs_op);
        fs_cmd_buffers[i].rsp_msg->enum_init.status = status;
        break;

      case FS_ENUM_NEXT_OP:
        /* Return information for the next file or directory */
        status = fs_ops_enum_next(&fs_cmd_buffers[i].fs_op);
        fs_cmd_buffers[i].rsp_msg->enum_next.status = status;
#ifdef FEATURE_EFS_ACCESS_METHODS
        if (status == FS_OKAY_S) {
          fs_am_fixup_enum (
              fs_cmd_buffers[i].fs_op.parms.enum_next.data_ptr,
              fs_cmd_buffers[i].fs_op.parms.enum_next.iterator_ptr);
        }
#endif
        break;

      case FS_ENUM_PREV_OP:
        /* Return information for the next file or directory */
        status = fs_ops_enum_prev(&fs_cmd_buffers[i].fs_op);
        fs_cmd_buffers[i].rsp_msg->enum_prev.status = status;
#ifdef FEATURE_EFS_ACCESS_METHODS
        if (status == FS_OKAY_S) {
          fs_am_fixup_enum (
              fs_cmd_buffers[i].fs_op.parms.enum_prev.data_ptr,
              fs_cmd_buffers[i].fs_op.parms.enum_prev.iterator_ptr);
        }
#endif
        break;

      case FS_ENUM_FINISH_OP:
        /* The EFS has nothing to deallocate here. */
        status = FS_OKAY_S;
        fs_cmd_buffers[i].rsp_msg->enum_finish.status = status;
        break;

      case FS_REMOTE_LINK_OP:
        /* Creation of link to remote file is one subcommand */
        status = fs_ops_remote_link(&fs_cmd_buffers[i].fs_op);
        fs_cmd_buffers[i].rsp_msg->remote_link.status = status;
        break;

      case FS_RM_REMOTE_LINK_OP:
        /* Remove of remote links is one subcommand */
        status = fs_ops_remove_remote_links(&fs_cmd_buffers[i].fs_op);
        fs_cmd_buffers[i].rsp_msg->rmrlink.status = status;
        break;

      case FS_RESERVED_OP:
        /* TBD for testing */
        status = FS_OKAY_S;
        break;

      case FS_STATFS_OP:
        status = fs_ops_statfs (&fs_cmd_buffers[i].fs_op);
        fs_cmd_buffers[i].rsp_msg->statfs.status = status;
        break;

      case FS_FORMAT_OP:
        /* The flash can not be formatted. */
        status = FS_ILLEGAL_OPERATION_S;
        fs_cmd_buffers[i].rsp_msg->enum_finish.status = status;
        break;

      default:
        status = FS_UNKNOWN_OPERATION_S;
        fs_cmd_buffers[i].rsp_msg->open.status = status;
        break;
  
    } /* switch */
  
    KICK_WATCHDOG();

    if (status != FS_OKAY_S)
    {
      /* Flush subcommand if failure status received */
      last = TRUE;
    }

    /* Was this the last subcommand in the chain? */
    if (last)
    {
#ifdef FEATURE_EFS_ACCESS_METHODS
      /* Clean up from the operation. */
      fs_am_cleanup_after_command (&fs_cmd_buffers[i]);
#endif

      /* Save the requesting task TCB and signal to set (if any) */
      task_ptr  = fs_cmd_buffers[i].tcb_ptr;
      task_sigs = fs_cmd_buffers[i].sigs;
        
      /* Signal the requesting task or call callback function */
      if (task_ptr == NULL)
      {
        /* Invoke callback function */
        fs_cmd_buffers[i].callback_p(fs_cmd_buffers[i].rsp_msg);
      }
      else
      {
        /* Set specified rex sig */
        (void) rex_set_sigs(task_ptr, task_sigs);
      }

      /* Return buffer to specified queue if requested */
      if (fs_cmd_buffers[i].done_q_ptr != NULL) 
             q_put(fs_cmd_buffers[i].done_q_ptr, &fs_cmd_buffers[i].link);

      /* Return the subcommand buffer to the free list */
      fsi_free_subcmd_buffer(subcmd_i);
   
      /* Return static command buffer to "available" status */
      fsi_free_cmd_buffer(fs_cmd_buffers[i].index);

      fs_ops_decrement_fs_waiters();
    }

#ifdef FEATURE_EFS_FLASH
    /* Run garbage collection now only if conditions require it */
    (void)fs_alloc_gc_eval(FSI_GC_IF_REQ);
  }
#endif /* FEATURE_EFS_FLASH */

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif
 
  return last;
} /* fsi_perform_subcmd */


#endif /* FEATURE_EFS_FLASH */

#ifdef FEATURE_EFS_ACCESS_METHODS
/*===========================================================================
 * FUNCTION FSI_RESPOND_TO_COMMAND
 *
 * DESCRIPTION
 *   This API is deprecated.
 *   This function does not have any meaning in EFS2.
===========================================================================*/
static void
fsi_respond_to_command (fs_cmd_type *cmd_ptr)
{
  rex_tcb_type *task_ptr;
  rex_sigs_type task_sigs;

  task_ptr = cmd_ptr->tcb_ptr;
  task_sigs = cmd_ptr->sigs;

  /* This is only included with access methods, so it is save to call this
   * without ifdefing  */
  fs_am_cleanup_after_command (cmd_ptr);

  fs_ops_decrement_fs_waiters ();

  if (task_ptr == NULL) {
    /* Invoke callback */
    cmd_ptr->callback_p (cmd_ptr->rsp_msg);
  } else {
    (void) rex_set_sigs (task_ptr, task_sigs);
  }
}
#endif



/*===========================================================================

FUNCTION FSI_QUEUE_CMD

DESCRIPTION
  This API is deprecated.
  This function does not have any meaning in EFS2.
  
===========================================================================*/

#ifdef FEATURE_EFS_ACCESS_METHODS
static void  fsi_queue_cmd
(
  fs_cmd_type           *cmd_ptr,
  fs_handle_type         am_handle
)
#else /* not FEATURE_EFS_ACCESS_METHODS */
static void  fsi_queue_cmd
(
  fs_cmd_type           *cmd_ptr
)
#endif /* not FEATURE_EFS_ACCESS_METHODS */
{
  int dog_autokick;
#ifdef FEATURE_EFS_ACCESS_METHODS
  void (*fs_ops_cmd) (fs_cmd_type *cmd_ptr) =
    fs_am_get_ops_cmd (am_handle);

  /* If no command queue function came back, then this is a request from a
   * stale handle.  Set the result to FS_FAIL_S for lack of a more
   * meaningful thing to do. */
  if (fs_ops_cmd == 0) {
    cmd_ptr->rsp_msg->open.status = FS_FAIL_S;

    fsi_respond_to_command (cmd_ptr);

    return;
  }
#endif /* FEATURE_EFS_ACCESS_METHODS */

#ifdef FEATURE_EFS_ACCESS_METHODS
  /* Store the handle in the command block. */
  cmd_ptr->am_handle = am_handle;
#endif /* FEATURE_EFS_ACCESS_METHODS */

  /* Artificially inflate any request coming from the nv task */
  if (rex_self() == &nv_tcb) {
    cmd_ptr->op_priority = 32766; /* Very high priority! */
  }
  else {
    /* Set operation's priority to the calling task's priority */
    cmd_ptr->op_priority  = rex_get_pri();
  }
  /* No buffer to return */
  cmd_ptr->done_q_ptr   = NULL;
  
  /* NULL callback function indicates synchronous invocation */
  if (cmd_ptr->callback_p == NULL)
  {
    /* Synchronous invocation -- clear the signal, issue the command, */
    /* then wait for the response.                                    */
    cmd_ptr->tcb_ptr    = rex_self();          /* Notify back to me */
    cmd_ptr->sigs       = FS_OP_COMPLETE_SIG;  /* With this signal */
    (void) rex_clr_sigs(rex_self(),FS_OP_COMPLETE_SIG);
    fs_ops_cmd(cmd_ptr);

    dog_autokick = rex_autodog_enable(rex_self()->dog_report_val);
    (void) rex_wait(FS_OP_COMPLETE_SIG);
    if (!dog_autokick) {
      rex_autodog_disable();
    }
  }
  else
  {
    /* Asynchronous invocation -- queue command and return now */
    cmd_ptr->tcb_ptr    = NULL;
    fs_ops_cmd(cmd_ptr);
  }
  return;

} /* fsi_queue_cmd */





/*===========================================================================

FUNCTION FSI_ALLOCATE_CMD_BUFFER

DESCRIPTION
  This API is deprecated.
  This function does not have any meaning in EFS2.
  
===========================================================================*/

static int  fsi_allocate_cmd_buffer
(
  void
)
{
  int                   i;

  /* Disable interrupts */
  INTLOCK(); 

  /* Find first unused entry */
  for (i=0; i<FSI_MAX_CMD_PACKETS; i++)
  {
    if (!fs_cmd_buffers[i].in_use)
    {
      fs_cmd_buffers[i].in_use = TRUE;
      break;
    }
  }

  /* Restore interrupts */
  INTFREE();

  if (i >= FSI_MAX_CMD_PACKETS)
  {
    /* No unused buffers found -- bail out */
    ERR_FATAL("Out of command buffers",0,0,0);
  }
  return i;

} /* fsi_allocate_cmd_buffer */





/*===========================================================================

FUNCTION FSI_FREE_CMD_BUFFER
                 
DESCRIPTION
  This API is deprecated.
  This function does not have any meaning in EFS2.
  
===========================================================================*/

/*static*/ void  fsi_free_cmd_buffer
(
  int                   i             /* Index of buffer to free */ 
)
{
  if ((i < 0) || (i >= FSI_MAX_CMD_PACKETS))
  {
    ERR("Command buffer index out of range",0,0,0);
  }
  else
  {
    /* Disable interrupts */
    INTLOCK(); 

    fs_cmd_buffers[i].in_use = FALSE;
           
    /* Restore interrupts */
    INTFREE();
  }
  return;

} /* fsi_free_cmd_buffer */


#ifdef FEATURE_EFS_FLASH



/*===========================================================================

FUNCTION FSI_ALLOCATE_SUBCMD_BUFFER

DESCRIPTION
  This API is deprecated.
  This function does not have any meaning in EFS2.
  
===========================================================================*/

static int  fsi_allocate_subcmd_buffer
(
  void
)
{
  int                   i, nexti;


  /* Get index of buffer at start of free list */
  i = fsi_internal_cmd_queue.entry[FSI_RSVD_SUBCMD_INDEX].prev;

  if (i == FSI_NULL_SUBCMD_INDEX)
  {
    ERR_FATAL("Out of subcommand buffers",0,0,0);
  }

  /* Unlink buffer from free list */
  nexti = fsi_internal_cmd_queue.entry[i].next;
  fsi_internal_cmd_queue.entry[FSI_RSVD_SUBCMD_INDEX].prev = nexti;
  if (nexti != FSI_NULL_SUBCMD_INDEX)
  {
    fsi_internal_cmd_queue.entry[nexti].prev = FSI_RSVD_SUBCMD_INDEX;
  }
  return i;

} /* fsi_allocate_subcmd_buffer */





/*===========================================================================

FUNCTION FSI_FREE_SUBCMD_BUFFER
                 
DESCRIPTION
  This API is deprecated.
  This function does not have any meaning in EFS2.
  
===========================================================================*/

static void  fsi_free_subcmd_buffer
(
  int                   i             /* Index of buffer to free */ 
)
{
  int                   previ, nexti;

  if ((i < 0) || (i >= FSI_MAX_SUBCMDS))
  {
    ERR("Command buffer index out of range",0,0,0);
  }
  else
  {
    /* Unlink from active subcommand queue */
    previ = fsi_internal_cmd_queue.entry[i].prev;
    nexti = fsi_internal_cmd_queue.entry[i].next;

    fsi_internal_cmd_queue.entry[previ].next = nexti;
    if (nexti != FSI_NULL_SUBCMD_INDEX)
    {
      fsi_internal_cmd_queue.entry[nexti].prev = previ;
    }

    /* Link into free list */
    nexti = fsi_internal_cmd_queue.entry[FSI_RSVD_SUBCMD_INDEX].prev;
    fsi_internal_cmd_queue.entry[i].next = nexti;
    fsi_internal_cmd_queue.entry[i].prev = FSI_RSVD_SUBCMD_INDEX;
    fsi_internal_cmd_queue.entry[nexti].prev = i;
    fsi_internal_cmd_queue.entry[FSI_RSVD_SUBCMD_INDEX].prev = i;
  }
  return;

} /* fsi_free_subcmd_buffer */





/*===========================================================================

FUNCTION FSI_GET_SUBCMD_INSERT_INDEX
                 
DESCRIPTION
  This API is deprecated.
  This function does not have any meaning in EFS2.
  
===========================================================================*/

static word  fsi_get_subcmd_insert_index
(
  rex_priority_type     op_prio       /* The subcommand's priority */
)
{
  word                  subcmd_index;
  word                  last_subcmd_index;
  word                  cmd_index;

  /* Set i to start of active subcommand list */
  subcmd_index = fsi_internal_cmd_queue.entry[FSI_RSVD_SUBCMD_INDEX].next;
  last_subcmd_index = subcmd_index;

  while (subcmd_index != FSI_NULL_SUBCMD_INDEX)
  {
    cmd_index = fsi_internal_cmd_queue.entry[subcmd_index].cmd_queue_index;
    if (fs_cmd_buffers[cmd_index].op_priority < op_prio)
    {
      break;
    }
    else
    {
      last_subcmd_index = subcmd_index;
      subcmd_index = fsi_internal_cmd_queue.entry[subcmd_index].next;
    }
  }
  return last_subcmd_index;

} /* fsi_get_subcmd_insert_index */





/*===========================================================================

FUNCTION FSI_SUBCMD_INSERT
                 
DESCRIPTION
  This API is deprecated.
  This function does not have any meaning in EFS2.
  
===========================================================================*/

static void  fsi_subcmd_insert
(
  word                  subcmd_index, /* The subcommand's table index */
  word                  insert_index  /* Index to insert after */ 
)
{
  word                  nexti;

  if (insert_index >= FSI_MAX_SUBCMDS)
  {
    ERR("Subcommand buffer index out of range",0,0,0);
  }
  else
  {
    nexti = fsi_internal_cmd_queue.entry[insert_index].next;
    /* Set the new subcommand's link fields */
    fsi_internal_cmd_queue.entry[subcmd_index].prev = insert_index;
    fsi_internal_cmd_queue.entry[subcmd_index].next = nexti;
 
    /* Insert into active subcommand queue after specified insert_index */
    fsi_internal_cmd_queue.entry[insert_index].next = subcmd_index;
    if (nexti != FSI_NULL_SUBCMD_INDEX)
    {
      fsi_internal_cmd_queue.entry[nexti].prev = subcmd_index;
    }
  }
  return;

} /* fsi_subcmd_insert */





/*===========================================================================

FUNCTION FSI_QUEUE_SUBOP
                 
DESCRIPTION
  This API is deprecated.
  This function does not have any meaning in EFS2.
  
===========================================================================*/

/*static*/ void  fsi_queue_subop
(
  fs_cmd_type              *cmd_ptr   /* Ptr to cmd received on fs_cmd_q */
)
{
  word                     insert_index;
  int                      subcmd_index;


  /* Allocate a subcommand queue entry */
  subcmd_index = fsi_allocate_subcmd_buffer();

  /* Find where the subcommand should be inserted into the internal  */
  /* command queue based on operation priority                       */
  insert_index = fsi_get_subcmd_insert_index(cmd_ptr->op_priority);

  /* Set pointer back to external command queue entry */
  fsi_internal_cmd_queue.entry[subcmd_index].cmd_queue_index = 
      fs_cmd_buffers[cmd_ptr->index].index;

  /* Link this entry into queue at insert index */
  fsi_subcmd_insert(subcmd_index,insert_index);

  /* WRITE subcommands requires additional initialization */
  if ((cmd_ptr->fs_op.op == FS_WRITE_OP) || 
      (cmd_ptr->fs_op.op == FS_RSVD_WRITE_OP))
  {
    /* Initialize subcommand count and progress fields */
    fsi_internal_cmd_queue.entry[subcmd_index].count    = 
        cmd_ptr->fs_op.parms.write.length;
    fsi_internal_cmd_queue.entry[subcmd_index].progress = (dword)0;
  }
  else if (cmd_ptr->fs_op.op == FS_SEEK_WRITE_OP) {
    /* Initialize subcommand count and progress fields */
    fsi_internal_cmd_queue.entry[subcmd_index].count    = 
        cmd_ptr->fs_op.parms.seek_write.length;
    fsi_internal_cmd_queue.entry[subcmd_index].progress = (dword)0;
  }

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif

  return;
} /* fsi_queue_subop */



#endif /* FEATURE_EFS_FLASH */


/*===========================================================================

FUNCTION FSI_TASK_INIT

DESCRIPTION
  This API is deprecated. There is no single task running handling a queue in
  EFS2. EFS2 initialization is done using efs_initialize(). Please refer to
  fs_public.h for more information.

===========================================================================*/

/*static*/ void  fsi_task_init
( 
  void
)
{
#ifdef FEATURE_EFS_FLASH
  fs_status_type     status;    /* Status of operations performed         */
#endif
  int                i;         /* Loop/index variable                    */


  /* Initialize local command buffers to "available" status */
  for (i=0; i<FSI_MAX_CMD_PACKETS; i++)
  {
    fs_cmd_buffers[i].in_use = FALSE;
    fs_cmd_buffers[i].index  = i;
  }

  /* Initialize internal command queue.  Note: zero'th entry is reserved */
  /* for use as the free entries list head and subcommand list head.     */
  for (i=0; i<FSI_MAX_SUBCMDS; i++)
  {
    fsi_internal_cmd_queue.entry[i].prev = (i-1);
    fsi_internal_cmd_queue.entry[i].next = (i+1);
  }
  fsi_internal_cmd_queue.entry[(FSI_MAX_SUBCMDS-1)].next = 
      FSI_NULL_SUBCMD_INDEX;
  /* Entry 0's "prev" field points to start of free list, and "next"  */
  /* points to the top of the active subcommand queue                 */
  fsi_internal_cmd_queue.entry[FSI_RSVD_SUBCMD_INDEX].prev = 1;
  fsi_internal_cmd_queue.entry[FSI_RSVD_SUBCMD_INDEX].next = 
      FSI_NULL_SUBCMD_INDEX;

  /* Initialize the watchdog report timer and FS write timer */
  rex_def_timer(&fs_rpt_timer,   &fs_tcb, FS_RPT_TIMER_SIG);
  rex_def_timer(&fs_write_timer, &fs_tcb, FS_WRITE_TIMER_SIG);

  /* Initialize the FS command queue */
  (void) q_init(&fs_cmd_q);

  /* Kick the watchdog and start task operations */
  KICK_WATCHDOG();

#ifdef FEATURE_EFS_FLASH
  /* Initialize file system and create master file directory */
  status = fs_ops_init();
  if (status != FS_OKAY_S)
  {
    ERR_FATAL("Ops-level initialization failed",0,0,0);
  }

  #ifdef FS_FEATURE_AUTO_REMOVE_REMOTE_LINKS
  /* Clear out the remote links */
  fs_dir_remove_remote_files();
  #endif
#endif /* FEATURE_EFS_FLASH */
  
  /* Wait for start signal */
  task_start (FS_RPT_TIMER_SIG, DOG_FS_RPT, &fs_rpt_timer);
  return;
} /* fsi_task_init */





/*===========================================================================

FUNCTION FS_CMD_IS_WAITING

DESCRIPTION
  This API is deprecated.
  This function does not have any meaning in EFS2.

===========================================================================*/

boolean  fs_cmd_is_waiting
( 
  void
)
{
  return (fs_ops_fs_waiters());
} /* fs_cmd_is_waiting */





/*===========================================================================

FUNCTION FS_TASK

DESCRIPTION
  This API is deprecated.

===========================================================================*/

void  fs_task
( 
  dword              parm       /* from REX - ignored                     */
)
{
  rex_sigs_type      rex_sigs;  /* REX signals word                       */

/*-------------------------------------------------------------------------*/

  MSG_MED("FS task started", 0, 0, 0);

#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_am_init ();
#endif
#ifdef FEATURE_EFS_RPC_HANDLER
#error code not present
#endif
#ifdef FEATURE_EFS2
  (void) efs_initialize ();
#endif
  
  /* Initialize file system task */
  fsi_task_init();

  /* Loop forever handling commands */
  for (;;)   
  {
    /* If there are subcommands on the internal command queue, don't do  */
    /* a REX wait here -- just get current REX signals for this task     */
    if (fsi_internal_cmd_queue.entry[FSI_RSVD_SUBCMD_INDEX].next !=
        FSI_NULL_SUBCMD_INDEX)
    {
      rex_sigs = rex_get_sigs(&fs_tcb);
    }
    else
    {
#ifdef FEATURE_EFS_FLASH
      /* Nothing to do -- before waiting on REX sigs, check if we need   */
      /* to perform some garbage collection                              */
      while (!fs_ops_fs_waiters())
      {
        if (!fs_alloc_gc_eval(FSI_GC_FAVORABLE))
        {
          break;   
        }
        KICK_WATCHDOG();
      }
#endif /* FEATURE_EFS_FLASH */
 
      if (!fs_ops_fs_waiters())
      {
        /* No tasks are calling and all background processing completed, */
        /* so set wait with mask of all possible task input signals      */
        rex_sigs = rex_wait( FS_RPT_TIMER_SIG   | /* Watchdog timer      */
                             FS_CMD_Q_SIG       | /* Command received    */
                             FS_WRITE_TIMER_SIG | /* Device operation    */
                             TASK_STOP_SIG      | /* Stop the FS task    */
                             TASK_OFFLINE_SIG);   /* Go offline          */
      }
      else
      {
        rex_sigs = rex_get_sigs(&fs_tcb);
      }

      /* Kick watchdog and start the watchdog report timer.   */
      /* Note that the watchdog is kicked after every wakeup, */
      /* following input of any signal to the FS task.        */
      KICK_WATCHDOG();
    }


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         COMMAND QUEUE SIGNAL                            */
/*                                                                         */
/*-------------------------------------------------------------------------*/

    if ((rex_sigs & FS_RPT_TIMER_SIG) != 0) {
      MSG_LOW("FS_RPT_TIMER_SIG received", 0, 0, 0);
      (void) rex_clr_sigs(&fs_tcb, FS_RPT_TIMER_SIG);
    }

    /* Check if command queue signal was set; if set then clear signal   */
    /* and proceeed                                                      */
    if ((rex_sigs & FS_CMD_Q_SIG) != 0) {
      MSG_LOW("FS_CMD_Q_SIG received", 0, 0, 0);
      (void) rex_clr_sigs(&fs_tcb, FS_CMD_Q_SIG);

      /* Translate command into subcommands and place on subcommand queue */
#ifdef FEATURE_EFS_FLASH
      fsi_handle_cmd_queue();
#else /* not FEATURE_EFS_FLASH */
      fsi_reject_cmd_queue ();
#endif /* FEATURE_EFS_FLASH */
      KICK_WATCHDOG();
    }

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                       OFFLINE COMMAND SIGNAL                            */
/*                                                                         */
/*-------------------------------------------------------------------------*/

    /* Check if offline command signal was set. If set then clear signal, */
    /* and process task offine procedure.                                 */

    if ((rex_sigs & TASK_OFFLINE_SIG) != 0) {
      MSG_MED("TASK_OFFLINE_SIG received", 0, 0, 0);
      (void) rex_clr_sigs(&fs_tcb, TASK_OFFLINE_SIG);

      task_offline();
    }

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                     POWERDOWN (STOP) COMMAND SIGNAL                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/

    /* Check if powerdown command signal was set. */
    /* If set then clear signal and proceeed.     */

    if ((rex_sigs & TASK_STOP_SIG) != 0) {
      MSG_MED("TASK_STOP_SIG received", 0, 0, 0);
      (void) rex_clr_sigs(&fs_tcb, TASK_STOP_SIG);

      /* Process task stop procedure. */
      task_stop();
    } 

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                     WATCHDOG REPORT TIMER SIGNAL                        */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/* PC debug only.                                                          */

    #ifdef FS_FEATURE_UNIT_TEST
#error code not present
    #endif

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                           PROCESS COMMAND                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/

    if ((rex_sigs & FS_WRITE_TIMER_SIG) != 0) {
      MSG_LOW("FS_WRITE_TIMER_SIG received", 0, 0, 0);
      (void) rex_clr_sigs(&fs_tcb, FS_WRITE_TIMER_SIG);
    }

    /* Execute the subcommand at the front of the subcommand queue */
#ifdef FEATURE_EFS_FLASH
    (void)fsi_perform_subcmd();
#endif /* FEATURE_EFS_FLASH */
    KICK_WATCHDOG();

  } /* for (;;) */

} /* fs_task */

#endif /* FEATURE_EFS */
