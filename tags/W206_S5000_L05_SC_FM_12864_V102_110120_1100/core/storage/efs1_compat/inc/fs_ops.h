#ifndef FS_OPS_H
#define FS_OPS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   E M B E D D E D   F I L E   S Y S T E M


GENERAL DESCRIPTION
  Header file for File System "Operations Level" routines.

Copyright (c) 1999 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/fs_ops.h_v   1.2   05 Dec 2001 16:21:00   davidb  $
$Header: //source/qcom/qct/core/storage/efs1_compat/main/latest/inc/fs_ops.h#1 $ $DateTime: 2008/06/03 18:36:07 $ $Author: richardp $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12-06-02   pg      Added fs_ops_open_readwrite().  
12-05-01   dlb     Added fs_opened_file_size().
09-10-01   dlb     Add fs_statfs() call.
11-09-00   pg      Initial version for common archive.
08-22-00   dlb     Removed ^Z from end of file.
03-24-00   pg      Added fs_ops_enum_prev() to support fs_enum_prev().
01-31-00   pg      Added fs_ops_enum_init() and fs_ops_enum_next() to 
                   support fs_enum_init() and fs_enum_next().
01-21-00   pg      Added fs_ops_rename_file() function.
                   Changed interface of fs_ops_remove_tlog():
                   Eliminated one of the arguments: fs_handle_type handle.
12-16-99   rp      Added fs_ops_remove_remote_links function. 
11-01-99   ls      Changed dindex to be word type universally.
06-22-99   ls      Changed transaction logging to support user revertable
                   files across multiple garbage collections.
05-20-99   ls      Added fmemset operation.
04-25-99   ls      Added space_erased function.
04-22-99   ls      Removed enclosing ifdef.
04-21-99   ls      Version 2.2 release.
04-03-99   ls      Changed interface per Version 2.1 specification.
03-04-99   ls      Initial version.


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "fsi.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                         DATA DEFINITIONS                                */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                               Functions                                 */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_INIT

DESCRIPTION
  Initialize all file system module and invokes creation of the master
  file directory and the sector status table.

DEPENDENCIES
  None.

RETURN VALUE
  FS_OKAY_S       - if initialization succeeded
  FS_FAIL_S       - if initialization failed

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_init
(
  void
);



/*===========================================================================

FUNCTION FS_OPS_CMD

DESCRIPTION
  This function takes an already filled out fs_cmd_type and places it on 
  the fs command queue.  The function returns to the caller after setting 
  the status to busy indicaton.  This does _not_ mean that the command 
  has been executed yet, just that it is waiting to be executed.

DEPENDENCIES
  The FS task must have been started up already.  All of the items in
  the fs_cmd_block must be already set.

RETURN VALUE
  None directly.  The status variable of the fs_cmd_type will be updated
  to reflect the current status of this command as it is processed.

SIDE EFFECTS
  The fs_cmd_type is placed on the FS command queue.  It must not be
  modified until the command has been processed.

===========================================================================*/

void  fs_ops_cmd
(
  fs_cmd_type  *cmd_ptr                   /* Pointer to FS command packet */
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_OPEN

DESCRIPTION
  Open a specified file.  

  Input record definition:

    fs_filename_type        filename;
    fs_handle_type          f_handle;
    fs_operation_type       op;
    fs_operation_parms_type op_parms;

  Perform the operation specified by 'op' on the file 'filename'
  using the operation parameters in 'op_parms'.

  
DEPENDENCIES
  None.

RETURN VALUE
  FS_NULL_HANDLE  - if operation failed
  file handle     - if operation worked

SIDE EFFECTS
  None.

===========================================================================*/
fs_handle_type  fs_ops_open
(
  fs_op_request_type       *inparm
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_OPEN_APPEND

DESCRIPTION
  Open file for append access.  Sets the current file pointer position
  and file access mode in this file's master directory entry.

DEPENDENCIES
  None.

RETURN VALUE
  FS_NULL_HANDLE  - if operation failed
  file handle     - if operation worked

SIDE EFFECTS
  None.

===========================================================================*/
fs_handle_type  fs_ops_open_append
(
  word                     dindex,
  fs_status_type           *status
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_OPEN_READWRITE

DESCRIPTION
  Open file for read/write access.  Sets the current file pointer position
  and file access mode in this file's master directory entry.

DEPENDENCIES
  None.

RETURN VALUE
  FS_NULL_HANDLE  - if operation failed
  file handle     - if operation worked

SIDE EFFECTS
  None.

===========================================================================*/
fs_handle_type  fs_ops_open_readwrite
(
  word                     dindex,
  fs_status_type           *status
);
 
/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_CLOSE

DESCRIPTION
  Close a specified file.  

  Input record definition:

    fs_filename_type        filename;
    fs_handle_type          f_handle;
    fs_operation_type       op;
    fs_operation_parms_type op_parms;

  Performs the operation specified by 'op' on the file 'filename'
  using the operation parameters in 'op_parms'.

  
DEPENDENCIES
  None.

RETURN VALUE
  fs_status_type enum value

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_close
(
  fs_op_request_type     *inparm
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_READ

DESCRIPTION
  Read from a specified file starting at the current file position.

  Input record definition:

    fs_filename_type        filename;
    fs_handle_type          f_handle;
    fs_operation_type       op;
    fs_operation_parms_type op_parms;

  Performs the operation specified by 'op' on the file 'filename'
  using the operation parameters in 'op_parms'.

  
DEPENDENCIES
  None.

RETURN VALUE
  fs_status_type enum value

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_read
(
  fs_op_request_type          *inparm
);


/* <EJECT> */
/*===========================================================================
    
FUNCTION FS_OPS_GET_FILE_INFO

DESCRIPTION
  Return current file status information needed by upper file system layers.
  
DEPENDENCIES
  File system initialization must have successfully completed.

RETURN VALUES
  status            - input parameter validation status
  access_mode       - open file access mode
  file_ptr_positon  - current file pointer position for the specified handle
  data_start_offset - offset of start of user data within first file block

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_get_file_info
(
  fs_handle_type            handle,
  fsi_current_pointer_type  *file_ptr_position,
  dword                     *data_start_offset,
  word                      *num_blks,
  fs_cleanup_option_type    *cleanup_option
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_WRITE_TLOG

DESCRIPTION
  Write a transaction log entry to a $SYS_TLOG file for the file identified
  by the input handle.  $SYS_TLOG files are named $SYS_TLOG01, $SYS_TLOG02,
  etc.  The next index is selected automatically.
   
DEPENDENCIES
  None.

RETURN VALUE
  Status of operation.

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_write_tlog
(
  fs_handle_type            handle,
  fsi_tlog_type             type_of_tlog,
  word                      *tlog_index
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_READ_TLOG

DESCRIPTION
  Read a transaction log ($SYS_TLOG) file identified by "dindex"
  into the local buffer.  Following this call, the "get_tlog_ . . ." 
  functions can be called to return data from the file stored locally.
   
DEPENDENCIES
  None.

RETURN VALUE
  Status of operation.

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_read_tlog
(
  word                      dindex      /* Index of TLOG file to read */
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_GET_TLOG_TYPE_ID

DESCRIPTION
  Return the type_id field from the local buffer containing the contents
  of a transaction log ($SYS_TLOG) file previously read.

DEPENDENCIES
  Function "fs_ops_read_tlog" must be called before this function can
  be used.

RETURN VALUE
  Which type of transaction log file is present in the local buffer.

SIDE EFFECTS
  None.

===========================================================================*/
fsi_tlog_type  fs_ops_get_tlog_type_id
(
  void
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_GET_TLOG_OWNER_ID

DESCRIPTION
  Return the owner_id field from the local buffer containing the contents
  of a transaction log ($SYS_TLOG) file previously read.

DEPENDENCIES
  Function "fs_ops_read_tlog" must be called before this function can
  be used.

RETURN VALUE
  The owner id of the checkpointed file.

SIDE EFFECTS
  None.

===========================================================================*/
fsi_owner_id_type  fs_ops_get_tlog_owner_id
(
  void
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_GET_TLOG_NUMBLKS

DESCRIPTION
  Return the numblks field from the local buffer containing the contents
  of a transaction log ($SYS_TLOG) file previously read.

DEPENDENCIES
  Function "fs_ops_read_tlog" must be called before this function can
  be used.

RETURN VALUE
  The number of blocks in the checkpointed file.

SIDE EFFECTS
  None.

===========================================================================*/
word  fs_ops_get_tlog_numblks
(
  void
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_GET_TLOG_VERSION

DESCRIPTION
  Return the block version number field corresponding to the specified
  block sequence number from the local buffer containing the contents 
  of a transaction log ($SYS_TLOG) file previously read.

DEPENDENCIES
  Function "fs_ops_read_tlog" must be called before this function can
  be used.

RETURN VALUE
  The version of the checkpointed block at the specified block number.

SIDE EFFECTS
  None.

===========================================================================*/
fsi_blk_version_type  fs_ops_get_tlog_version
(
  word                      blk_number
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_REMOVE_TLOG

DESCRIPTION
  Remove a transaction log file.  $SYS_TLOG files are named $SYS_TLOG01, 
  $SYS_TLOG02, etc.  The specific file to delete is specified by the caller.
   
DEPENDENCIES
  None.

RETURN VALUE
  Status of operation.

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_remove_tlog
(
//  fs_handle_type            handle,
  word                      tlog_index
);


/* <EJECT> */
/*===========================================================================
    
FUNCTION FS_OPS_WRITE

DESCRIPTION
  Write to a specified file starting at the current file position.

  Input record definition:

    fs_filename_type        filename;
    fs_handle_type          f_handle;
    fs_operation_type       op;
    fs_operation_parms_type op_parms;

  Performs the operation specified by 'op' on the file 'filename'
  using the operation parameters in 'op_parms'.

  
DEPENDENCIES
  None.

RETURN VALUE
  fs_status_type enum value

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_write
(
  fs_op_request_type        *inparm
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_FILE_SIZE

DESCRIPTION
  Return the current size (in bytes) of the specified file.

  Input parameter definition:

    fs_filename_type   *filename;
  
DEPENDENCIES
  The caller must insure that the file already exists.

RETURN VALUE
  Current file size in bytes.  If the file does not exist, return value
  is undefined.

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_file_size
(
  fs_op_request_type     *inparm
);

/*===========================================================================

FUNCTION FS_OPS_OPENED_FILE_SIZE

DESCRIPTION
  Return the current size (in bytes) of the specified file.

  Input parameter definition:

    fs_handle_type   *fhandle;
  
DEPENDENCIES
  The file must be opened.

RETURN VALUE
  Size of opened file in bytes.

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_opened_file_size
(
  fs_op_request_type     *inparm
);

/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_SEEK

DESCRIPTION
  Set the current file position to the specified position.

  Input record definition:

    fs_filename_type        filename;
    fs_handle_type          f_handle;
    fs_operation_type       op;
    fs_operation_parms_type op_parms;

  Performs the operation specified by 'op' on the file 'filename'
  using the operation parameters in 'op_parms'.

  
DEPENDENCIES
  None.

RETURN VALUE
  fs_status_type enum value

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_seek
(
  fs_op_request_type     *inparm
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_TELL

DESCRIPTION
  Return the current file position of the specified file.

  Input record definition:

    fs_filename_type        filename;
    fs_handle_type          f_handle;
    fs_operation_type       op;
    fs_operation_parms_type op_parms;

  Performs the operation specified by 'op' on the file 'filename'
  using the operation parameters in 'op_parms'.

  
DEPENDENCIES
  None.

RETURN VALUE
  fs_status_type enum value

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_tell
(
  fs_op_request_type        *inparm
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_TRUNCATE

DESCRIPTION
  Truncate an open file from a specified position.

  Input record definition:

    fs_filename_type        filename;
    fs_handle_type          f_handle;
    fs_operation_type       op;
    fs_operation_parms_type op_parms;

  Performs the operation specified by 'op' on the file 'filename'
  using the operation parameters in 'op_parms'.

  
DEPENDENCIES
  None.

RETURN VALUE
  fs_status_type enum value

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_truncate
(
  fs_op_request_type        *inparm
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_FMEMSET

DESCRIPTION
  Set the file contents to the specified character value from the
  current file position for the specified number of bytes (it is
  allowed for this to be beyond the current end of file).

DEPENDENCIES
  None.

RETURN VALUE
  fs_status_type enum value

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_fmemset
(
  fs_op_request_type     *inparm
);
 

/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_MKDIR

DESCRIPTION
  Create a new user file directory with the specified name.

  Input parameter definition:

    fs_filename_type   *dirname;
  
DEPENDENCIES
  None.

RETURN VALUE
  Status of operation.

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_mkdir
(
  fs_op_request_type        *inparm
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_RMDIR

DESCRIPTION
  Remove the specified (empty) user file directory.

  Input parameter definition:

    fs_filename_type   *dirname;
  
DEPENDENCIES
  None.

RETURN VALUE
  Status of operation.

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_rmdir
(
  fs_op_request_type        *inparm
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_NAMETEST

DESCRIPTION
  Test for the existence of a file with the specified file name.  Return 
  TRUE if the file already exists and FALSE otherwise.

DEPENDENCIES
  None.

RETURN VALUE
  FS_NULL_HANDLE  - if operation failed
  file handle     - if operation worked

SIDE EFFECTS
  None.

===========================================================================*/
boolean  fs_ops_nametest
(
  fs_op_request_type        *inparm
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_SET_ATTRIBUTES

DESCRIPTION
  Set the file attributes of a specified file.

  Input parameter definition:

    fs_filename_type   *filename;
  
DEPENDENCIES
  None.

RETURN VALUE
  Status of operation.

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_set_attributes
(
  fs_op_request_type        *inparm
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_GET_ATTRIBUTES

DESCRIPTION
  Return the file attributes of a specified file.

  Input parameter definition:

    fs_filename_type   *filename;
  
DEPENDENCIES
  None.

RETURN VALUE
  Status of operation.

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_get_attributes
(
  fs_op_request_type        *inparm
);

   
/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_GET_FILE_COUNT

DESCRIPTION
  Get the number of files in the specified user directory.

  Input parameter definition:

    None.
  
DEPENDENCIES
  None.

RETURN VALUE
  File count    -- if operation worked;
  -1            -- if user directory does not exist
  
SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_get_file_count
(
  fs_op_request_type        *inparm
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_GET_DIR_COUNT

DESCRIPTION
  Get the number of subdirectories in the specified user directory.

  Input parameter definition:

    None.
  
DEPENDENCIES
  None.

RETURN VALUE
  File count    -- if operation worked;
  -1            -- if user directory does not exist
  
SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_get_dir_count
(
  fs_op_request_type        *inparm
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_LIST_DIRS

DESCRIPTION
  List all user file directories starting at the specified directory.

  Input parameter definition:

    None.
  
DEPENDENCIES
  None.

RETURN VALUE
  Status of operation.

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_list_dirs
(
  fs_op_request_type        *inparm
);

   
/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_ENUM_INIT

DESCRIPTION
  Initialize a file or directory enumeration control object (allocated
  by the client) for subsequent use with the "fs_enum_next" operation.

DEPENDENCIES
  None.

RETURN VALUE
  Status of operation.

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_enum_init
(
  fs_op_request_type          *inparm
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_ENUM_NEXT

DESCRIPTION
  Using the file or directory enumeration control object referenced
  by the iterator pointer, return file information for the next file
  or directory within the specified root directory.

DEPENDENCIES
  None.

RETURN VALUE
  Status of operation.

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_enum_next
(
  fs_op_request_type          *inparm
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_ENUM_PREV

DESCRIPTION
  Using the file or directory enumeration control object referenced
  by the iterator pointer, return file information for the prev file
  or directory within the specified root directory.

DEPENDENCIES
  None.

RETURN VALUE
  Status of operation.

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_enum_prev
(
  fs_op_request_type          *inparm
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_LIST_FILES

DESCRIPTION
  List all files in a specified user directory.

  Input parameter definition:

    fs_filename_type   *dirname;
  
DEPENDENCIES
  None.

RETURN VALUE
  Status of operation.

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_list_files
(
  fs_op_request_type        *inparm
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_REMOTE_LINK

DESCRIPTION
  Create a link to a remote file.

DEPENDENCIES
  None.

RETURN VALUE
  Status of operation.

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_remote_link
(
  fs_op_request_type        *inparm
);


/*===========================================================================

FUNCTION FS_OPS_REMOVE_REMOTE_LINKS

DESCRIPTION

  Remove all remote file links.

DEPENDENCIES
  The FS task must have been started up already. 

RETURN VALUE
  Status of operation.

SIDE EFFECTS
  None.
  
===========================================================================*/
fs_status_type  fs_ops_remove_remote_links
(
  fs_op_request_type          *inparm
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_REMOVE_FILE

DESCRIPTION
  Remove the specified file from the file system.

  Input parameter definition:

    fs_filename_type   *filename;
  
DEPENDENCIES
  None.

RETURN VALUE
  Status of operation.

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_remove_file
(
  fs_filename_type          filename   /* Name of file to remove blk from */
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_RENAME_FILE

DESCRIPTION
  Rename the file identified by "old_filename" to "new_filename".

  Input parameter definition:

    fs_filename_type   *old_filename  -- name of file to rename
    fs_filename_type   *new_filename  -- new name for file
  
DEPENDENCIES
  None.

RETURN VALUE
  Status of operation.

SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_rename_file
(
  fs_filename_type            old_filename,
  fs_filename_type            new_filename
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_RESERVE_SPACE

DESCRIPTION

  Reserve the amount of file system space needed to guarantee that 
  the specified number of bytes can be written.
  
DEPENDENCIES
  The FS task must have been started up already. 

RETURN VALUE
  If enough file system space exists to satisfy the reservation:
    a confirmation code that can be subsequently used to guarantee
    that a write of less than or equal to the number of reserved 
    bytes will be successful.
  If not enough file system space exists to satisfy the reservation:
    a Null confirmation code.

SIDE EFFECTS
  The amount of file system space available for allocation is immediately
  reduced by the amount needed to satisfy the reservation request.
  
===========================================================================*/
fs_conf_code_type  fs_ops_reserve_space
(
  boolean                   for_revertable_file,
  fs_rsvr_request_type      rsvr_request
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_CANCEL_RESERVATION

DESCRIPTION

  Cancel the specified reservation of file system space.
  
DEPENDENCIES
  The FS task must have been started up already. 

RETURN VALUE
  FS_OKAY_S               -- if reservation was cancelled successfully
  FS_INVALID_CONF_CODE_S  -- if no reservation was found matching the
                             specified confirmation code

SIDE EFFECTS
  The amount of file system space available for allocation is immediately
  increased by the amount released by the cancelled reservation.
  
===========================================================================*/
fs_status_type  fs_ops_cancel_reservation
(
  fs_conf_code_type         conf_code
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_RESERVATION_IS_NULL

DESCRIPTION

  Returns TRUE if the specified confirmation code is Null, FALSE if
  it is valid.
  
DEPENDENCIES
  The FS task must have been started up already. 

RETURN VALUE
  TRUE   -- if the specified confirmation code is Null
  FALSE  -- if the specified confirmation code is valid

SIDE EFFECTS
  None.
  
===========================================================================*/
boolean  fs_ops_reservation_is_null
(
  fs_conf_code_type         conf_code
);
 

/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_SPACE_AVAILABLE

DESCRIPTION

  Return the current amount of available file system space in bytes.
  
DEPENDENCIES
  The FS task must have been started up already. 

RETURN VALUE
  Number of bytes of file system space currently available.

SIDE EFFECTS
  None.
  
===========================================================================*/
dword  fs_ops_space_available
(
  void
);



/*===========================================================================

FUNCTION FS_OPS_SPACE_ERASED

DESCRIPTION

  Return the current amount of erased and available file system space 
  in bytes.
  
DEPENDENCIES
  The FS task must have been started up already. 

RETURN VALUE
  Number of bytes of file system space currently erased and available.

SIDE EFFECTS
  None.
  
===========================================================================*/
dword  fs_ops_space_erased
(
  void
);

 
/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_SPACE_USED

DESCRIPTION

  Return the current amount of "in-use" file system space in bytes.

DEPENDENCIES
  The FS task must have been started up already. 

RETURN VALUE
  Number of bytes of file system space currently in-use.

SIDE EFFECTS
  None.
  
===========================================================================*/
dword  fs_ops_space_used
(
  void
);


/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_SPACE_RESERVED

DESCRIPTION

  Return the current amount of reserved file system space in bytes.

DEPENDENCIES
  The FS task must have been started up already. 

RETURN VALUE
  Number of bytes of file system space currently reserved.

SIDE EFFECTS
  None.
  
===========================================================================*/
dword  fs_ops_space_reserved
(
  void
);

/***********************************************************************
 * FS_OPS_STATFS
 *
 * Perform statfs operation.
 */
fs_status_type
fs_ops_statfs (fs_op_request_type *inparm);

/***************************************************************************
 *                                                                         *
 *  FS_OPS_INCREMENT_FS_WAITERS                                            *
 *                                                                         *
 *  Increment the count of tasks waiting for the EFS task.                 *
 *                                                                         *
 ***************************************************************************/
void  fs_ops_increment_fs_waiters
(
  void
);


/***************************************************************************
 *                                                                         *
 *  FS_OPS_DECREMENT_FS_WAITERS                                            *
 *                                                                         *
 *  Decrement the count of tasks waiting for the EFS task.                 *
 *                                                                         *
 ***************************************************************************/
void  fs_ops_decrement_fs_waiters
(
  void
);


/***************************************************************************
 *                                                                         *
 *  FS_OPS_FS_WAITERS                                                      *
 *                                                                         *
 *  Return TRUE if any task is waiting for the EFS task.                   *
 *                                                                         *
 ***************************************************************************/
boolean  fs_ops_fs_waiters
(
  void
);


/***************************************************************************
 *                                                                         *
 *  FS_OPS_INCREMENT_FS_WAITERS                                            *
 *                                                                         *
 *  Increment the count of tasks waiting for the EFS task.                 *
 *                                                                         *
 ***************************************************************************/
void  fs_ops_increment_fs_waiters
(
  void
);


/***************************************************************************
 *                                                                         *
 *  FS_OPS_DECREMENT_FS_WAITERS                                            *
 *                                                                         *
 *  Decrement the count of tasks waiting for the EFS task.                 *
 *                                                                         *
 ***************************************************************************/
void  fs_ops_decrement_fs_waiters
(
  void
);


/***************************************************************************
 *                                                                         *
 *  FS_OPS_FS_WAITERS                                                      *
 *                                                                         *
 *  Return TRUE if any task is waiting for the EFS task.                   *
 *                                                                         *
 ***************************************************************************/
boolean  fs_ops_fs_waiters
(
  void
);


/* <EJECT> */
#ifdef FEATURE_FS_ALLOW_MASTER_RESET
/*===========================================================================

FUNCTION FS_OPS_MASTER_RESET

DESCRIPTION

  Erase all file system space.  Phone must be rebooted before file system
  can be used again.

DEPENDENCIES
  The FS task must have been started up already. 

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/

void  fs_ops_master_reset
(
  void
);
#endif /* FEATURE_FS_ALLOW_MASTER_RESET */

#endif /* FS_OPS_H */
