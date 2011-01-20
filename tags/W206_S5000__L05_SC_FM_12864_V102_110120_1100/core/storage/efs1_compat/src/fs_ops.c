/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   E M B E D D E D   F I L E   S Y S T E M


GENERAL DESCRIPTION
  File System "Operations Level" routines.

Copyright (c) 1999, 2000, 2001 by QUALCOMM Incorporated. All Rights Reserved.
Copyright (c) 2002, 2003       by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/core/storage/efs1_compat/main/latest/src/fs_ops.c#1 $ $DateTime: 2008/06/03 18:36:07 $ $Author: richardp $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06-23-07   jkl     Featurized for EFS2.
06-17-03   pg      Increased the threshold on clean block count in 
                   fs_ops_space_erased().
04-15-03   pg      Changed more ERR msg to MSG_HIGH.
04-14-03   pg      Changed some ERR msg to MSG_HIGH.
11-26-02   pg      Added checking to see if there are still files in the directory
                   before removing it.
                   Renamed fsi_ops_open_readwrite() to fs_ops_open_readwrite().  
                   And make it as a global function.
11-06-02   pg      Fixed boundary checking on directory names as they are created.
05-31-02   dlb     Fix off-by-one problem in seek.
02-27-02   dlb     Removed unused variable.
12-05-01   dlb     Added fs_opened_file_size().
09-10-01   dlb     Add fs_statfs() call.
01-17-00   pg      Modified an inaccurate return status in fsi_ops_open_create().
11-09-00   pg      Initial version for common archive.
08-29-00   pg      Changed some misleading ERR msgs to MSG_HIGH.
08-22-00   dlb     Use FEATURE_EFS_FLASH.
07-27-00   pg      Initialized requested task in fs_ops_write_tlog before 
                   calling fs_ops_write.
07-25-00   pg      Added support to reserve some blocks for NV-writes along 
                   with tlog operations.
07-24-00   pg      Reserve some blocks for tlog operations only.
04-25-00   pg      Modified fs_ops_nametest() to fixed the problem that 
                   fs_test() could not properly distinguish a file from a 
                   directory.
03-24-00   pg      Added fs_ops_enum_prev() to support fs_enum_prev().
02-15-00   rp      In fs_ops_remove_file() added error return if trying
                   to remove a system file ($USER_DIRS, $SYS_RMT). 
02-14-00   pg      Added support for remote links in fs_ops_enum_next().
02-07-00   kjm     Fixed problem in fs_ops_enum_next for FS_ENUM_DIRECTORIES
                   where the data_ptr parameter was being used instead of
                   the iterator_ptr parameter leading to memory faults.
02-02-00   rp      Updated fsi_ops_convert_dirname() to properly handle a 
                   given dirname that may optionally be terminated with the
                   directory char.
02-01-00   rp      Updated fsi_ops_convert_dirname() to properly handle the
                   root directory.
01-31-00   pg      Added fs_ops_enum_init() and fs_ops_enum_next() to 
                   support fs_enum_init() and fs_enum_next().
01-21-00   pg      Added code to fs_ops_nametest() to support testing the
                   existence of a directory.
                   Added fs_ops_rename_file() function.
                   Added fsi_ops_write_tlog() function.
                   Changed interface of fs_ops_remove_tlog():
                   Eliminated one of the arguments: fs_handle_type handle.
01-20-00   rp      In fs_ops_get_file_count() and fs_ops_get_dir_count()
                   fixed improper check for root directory from
                   dirname_internal.length to 
                   strlen(inparm->parms.num_dirs.dirname).   
12-28-99   rp      Add fix for bug when accessing the root firectory
                   in fs_ops_get_file_count(), fs_ops_get_dir_count(),
                   and fs_ops_list_files().
12-16-99   rp      Added fs_ops_remove_remote_links function. 
11-12-99   rp      Removed all code under "#ifdef FS_USE_RAM" as there is
                   now an actual RAM driver.  
11-01-99   ls      Fixed multiple-opens of revertable files bug.
10-28-99   ls      Backward-merged Version 3.0 fixes.
09-22-99   jc      Added ifdef FEATURE_EFS to whole file.
07-02-99   ls      Fixed tlog index rollover bug.
06-22-99   ls      Changed transaction logging to support user revertable
                   files across multiple garbage collections.
06-08-99   ls      Add general system-permanent file creation capability.
05-24-99   ls      Added support for SYS_PERMANENT attribute.
05-20-99   ls      Added fmemset operation.
04-27-99   ls      Reduced reported space available by user-defined offset.
04-27-99   ls      Added fs_waiters functionality.
04-25-99   ls      Added space_erased function.
04-22-99   ls      Removed enclosing ifdef.
04-21-99   ls      Version 2.2 release.
04-03-99   ls      Changed interface per Version 2.1 specification.
03-10-99   kjm     Replaced itoa calls with sprintf calls.
03-04-99   ls      Initial version.


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"

#ifdef FEATURE_EFS

#include <string.h>
#include <memory.h>
#include <stdio.h>
#include "err.h"
#include "clk.h"
#include "task.h"
#include "fsi.h"
#ifndef FEATURE_EFS2
#include "fs_alloc.h"
#include "fs_dev.h"
#include "fs_dir.h"
#include "fs_udir.h"
#endif
#include "fs_ops.h"

#ifdef FEATURE_EFS_FLASH


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

/* Buffers for manipulating file names and file path names */
static fsi_filename_type          path;
static fsi_filename_type          local_fname;

/* The Table of Open Files */
static fsi_open_table_type        open_table;

/* The Reservations Ledger */
static fsi_rsvr_ledger_type       rsvr_ledger;

/* Current highest transaction log file name suffix */
static word                       tlog_count;

/* Buffer for constructing a transaction log file */
static byte                       tlog_file_buffer[FS_MAX_TLOG_SIZE];

/* Buffer for constructing a transaction log file name */
static fsi_filename_type          tlog_next_fname;
static char                       *tlog_full_name;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                      REX Timers and Queues                              */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#endif /* FEATURE_EFS_FLASH */
/* FS task command queue */
extern q_type                     fs_cmd_q;

/* Number of commands waiting on the EFS task command queue */
static int                        fs_waiters = 0;
#ifdef FEATURE_EFS_FLASH

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                            Local Data                                   */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                         Local Functions                                 */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                            Debug Data                                   */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FS_FEATURE_UNIT_TEST
#error code not present
#endif /* FS_FEATURE_UNIT_TEST */



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                               Macros                                    */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                              Functions                                  */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



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
)
{
  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif
  
  INTLOCK();
  fs_waiters++;
  INTFREE();

  return;
} /* fs_ops_increment_fs_waiters */


#endif /* FEATURE_EFS_FLASH */


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
)
{
  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif

  INTLOCK();
  fs_waiters--;
  INTFREE();

  return;
} /* fs_ops_decrement_fs_waiters */




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
)
{
  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif

  return (fs_waiters>0);
} /* fs_ops_fs_waiters */


#ifdef FEATURE_EFS_FLASH


/***************************************************************************
 *                                                                         *
 *  FSI_OPS_GET_TIMESTAMP                                                  *
 *                                                                         *
 *  Get an absolute timestamp from the system clock.  Note that if system  *
 *  time is unknown, zero will be used.                                    *
 *                                                                         *
 ***************************************************************************/
static fsi_file_date_type  fsi_ops_get_timestamp
(
  void
)
{
  #define  FSI_ONE_YEAR  31536000L /* one year in seconds (leaps ignored) */
  dword                      systime;
  
  systime = clk_read_secs();
  if (systime < FSI_ONE_YEAR) 
  {
    /* system time currently unavailable, use zero */
    systime = (dword)0;
  }

  return (fsi_file_date_type)systime;
} /* fsi_ops_get_timestamp */




/***************************************************************************
 *                                                                         *
 *  FSI_OPS_OPENT_ALLOCATE_ENTRY                                           *
 *                                                                         *
 *  Find and return the index of the first free entry of the Open Files    *
 *  Table and set the "in-use" flag of the entry to TRUE.  If no free      *
 *  entry is found, log an error and return -1.                            *
 *                                                                         *
 ***************************************************************************/
static int  fsi_ops_opent_allocate_entry
(
  void
)
{
  int                        i;
  
  /* Reserve slot 0 */
  for (i=1; i<FSI_MAX_OPEN_TABLE_ENTRIES; i++)
  {
    if (!open_table.entry[i].in_use)
    {
      open_table.entry[i].in_use     = TRUE;
      open_table.entry[i].mdir_index = 0;
      break; 
    }
  }

  if (i == FSI_MAX_OPEN_TABLE_ENTRIES)
  {
    ERR("No free open table entries",0,0,0);
    i = -1;
  }

  return i;
} /* fsi_ops_opent_allocate_entry */




/***************************************************************************
 *                                                                         *
 *  FSI_OPS_OPENT_FREE_ENTRY                                               *
 *                                                                         *
 *  Make the specified entry of the Open Files Table available for         *
 *  allocation again (i.e., set the "in-use" flag to FALSE).               *
 *                                                                         *
 ***************************************************************************/
static fs_status_type  fsi_ops_opent_free_entry
(
  word                         index
)
{
  if (index < FSI_MAX_OPEN_TABLE_ENTRIES)
  {
    open_table.entry[index].in_use     = FALSE;
    open_table.entry[index].mdir_index = 0;
    return FS_OKAY_S;
  }
  else
  {
    ERR("Invalid Open Table index",0,0,0);
    return FS_PARAMETER_ERROR_S;
  }
} /* fsi_ops_opent_free_entry */




/***************************************************************************
 *                                                                         *
 *  FSI_OPS_CONVERT_FILENAME                                               *
 *                                                                         *
 *  Convert the input "fs_filename_type" external file name into the       *
 *  equivalent "fsi_filename_type" internal file name.                     *
 *                                                                         *
 ***************************************************************************/
static fs_status_type  fsi_ops_convert_filename
(
  fs_filename_type             *input_fname,
  fsi_filename_type            *output_fname
)
{         
  size_t                       length;
           
  /* Verify filename is under maximum length (add one for the final NULL) */
  length = strlen(*input_fname) + 1;
  if (length > FS_FILENAME_MAX_LENGTH)
  {
    ERR("File name too long",0,0,0);
    return FS_BAD_FILE_NAME_S;
  }    
  else
  {
    /* Convert filename to internal format */
    memcpy((byte *)output_fname->name,(byte *)*input_fname,length);
    output_fname->length = (byte)length;
    return FS_OKAY_S;
  }
} /* fsi_ops_convert_filename */




/***************************************************************************
 *                                                                         *
 *  FSI_OPS_CONVERT_DIRNAME                                                *
 *                                                                         *
 *  Convert the input "fs_filename_type" external directory name into the  *
 *  equivalent "fsi_filename_type" internal directory name.                *
 *                                                                         *
 ***************************************************************************/
static fs_status_type  fsi_ops_convert_dirname
(
  fs_filename_type             *input_dname,
  fsi_filename_type            *output_dname
)
{
  size_t                       length;

  /* Verify that directory name is under maximum length (add 3: one  */
  /* byte for the length field, one byte for the directory separator */
  /* character, and one byte for the terminating NULL)               */
  length = strlen(*input_dname);
  /* Input dirname may optionally be terminated with the directory char */
  if (length > 0)
  {
    if (*(*input_dname+length-1) == FS_DIRECTORY_CHAR)
    {
      length--;
    }
  }
  if ((length+3) > FS_FILENAME_MAX_LENGTH)
  {
    ERR("Directory name too long",0,0,0);
    return FS_BAD_FILE_NAME_S;
  }    
  else if (length == 0)
  {
    /* NULL is a special case: return NULL internal format name */
    output_dname->length  = 0;
    output_dname->name[0] = 0x00;
    return FS_OKAY_S;
  }
  else
  {
    /* Convert directory name to internal format */
    memcpy((byte *)output_dname->name,(byte *)*input_dname,length);
    /* Add directory terminator */
    output_dname->name[length]   = FS_DIRECTORY_CHAR;
    output_dname->name[length+1] = 0x00;
    output_dname->length         = (length+2);
    return FS_OKAY_S;
  }
} /* fsi_ops_convert_dirname */




/***************************************************************************
 *                                                                         *
 *  FSI_OPS_VALIDATE_HANDLE                                                *
 *                                                                         *
 *  Validate the input file handle and check that the file is open.        *
 *                                                                         *
 ***************************************************************************/
static fs_status_type  fsi_ops_validate_handle
(
  fs_handle_type               handle
)
{
  /* Validate handle and check that file is currently open */
  if (handle >= FSI_MAX_OPEN_TABLE_ENTRIES)
  {
    ERR("Invalid file handle specified",0,0,0);
    return FS_BAD_FILE_HANDLE_S;
  }
  if (!open_table.entry[handle].in_use)
  {
    ERR("File not open",0,0,0);
    return FS_FILE_NOT_OPEN_S;
  }
  return FS_OKAY_S;

} /* fsi_ops_validate_handle */




/***************************************************************************
 *                                                                         *
 *  FSI_OPS_FILE_IS_OPEN                                                   *
 *                                                                         *
 *  Return TRUE if the specified file is currently open, FALSE otherwise.  *
 *  If the function return value is TRUE, the return parameter *tlog_index *
 *  will contain the tlog file index of the checkpoint file associated     *
 *  with this file (if there is one), and the return parameter *mode will  *
 *  contain the "highest" access mode the file is open under.  The access  *
 *  modes, ranked from highest to lowest, are: FSI_MODE_UPDATE,            *
 *  FSI_MODE_APPEND, and FSI_MODE_READONLY (note: FSI_MODE_CLOSED is       *
 *  implied by a FALSE function return value).                             *
 *                                                                         *
 ***************************************************************************/
static boolean  fsi_ops_file_is_open
(
  word                         dindex,
  word                         *tlog_index,
  fsi_access_mode_type         *access_mode
)
{
  int                          i;
  boolean                      found            = FALSE;
  fsi_access_mode_type         temp_access_mode = FSI_MODE_CLOSED;


  /* Search Open Files Table for matching dindex */
  for (i=0; i<FSI_MAX_OPEN_TABLE_ENTRIES; i++)
  {
    if (open_table.entry[i].in_use)
    {
      if (open_table.entry[i].mdir_index == dindex)
      {
        *tlog_index = fs_dir_get_tlog_index(dindex);
        if (open_table.entry[i].fmode == FSI_MODE_UPDATE)
        {
          temp_access_mode = FSI_MODE_UPDATE;
        }
        else if (open_table.entry[i].fmode == FSI_MODE_APPEND)
        {
          if (temp_access_mode != FSI_MODE_UPDATE)
          {
            temp_access_mode = FSI_MODE_APPEND;
          }
        }
        else if (open_table.entry[i].fmode == FSI_MODE_READONLY)
        {
          if ((temp_access_mode != FSI_MODE_UPDATE) &&
              (temp_access_mode != FSI_MODE_APPEND))
          {
            temp_access_mode = FSI_MODE_READONLY;
          }
        }   
        found = TRUE;
      }
    }
  }

  *access_mode = temp_access_mode;
  return found;
} /* fsi_ops_file_is_open */




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
)
{
  fs_status_type          status;
  int                     i;

  /* Initialize device level */
  fs_dev_init();

  /* Initialize Open Files Table */
  for (i=0; i<FSI_MAX_OPEN_TABLE_ENTRIES; i++)
  {
    open_table.entry[i].in_use     = FALSE;
    open_table.entry[i].mdir_index = 0;
  }

  /* Initialize Reservations Ledger */
  for (i=0; i<FS_MAX_RESERVATIONS; i++)
  {
    rsvr_ledger.entry[i].conf_code.code  = FS_NULL_CONF_CODE;
    rsvr_ledger.entry[i].conf_code.count = (dword)0;
  }

  /* Initialize transaction log count */
  tlog_count = FSI_TLOG_COUNT_INITIAL;

  /* Create master file directory */
  status = fs_dir_create();
  if (status != FS_OKAY_S)
  {
    ERR_FATAL("Master Directory creation failed",0,0,0);
  }
  return status;

} /* fs_ops_init */


#endif /* FEATURE_EFS_FLASH */



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
)
{
  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #else

    /* Set the request status to busy. Initialize and link the command  */
    /* onto the FS command queue, set a signal to the FS task and exit. */

    cmd_ptr->status = FS_BUSY_S;
    cmd_ptr->rsp_msg->open.status = FS_BUSY_S;
    (void) q_link(cmd_ptr, &cmd_ptr->link);
    q_put(&fs_cmd_q, &cmd_ptr->link);

    fs_waiters++; 
    (void) rex_set_sigs(&fs_tcb, FS_CMD_Q_SIG);
  #endif
  
  return;
} /* fs_ops_cmd */


#ifdef FEATURE_EFS_FLASH


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
)
{
  word                      dindex;
  byte                      filename_length;
  fs_status_type            status;


  /* Validate handle and check that file is currently open */
  status = fsi_ops_validate_handle(handle);
  if (status != FS_OKAY_S)
  {
    /* File is not open */
    *file_ptr_position = FSI_START_OF_FILE_POSITION;
    *num_blks          = 0;
    return status;
  }

  /* Get number of file system blocks the file currently occupies */
  dindex = open_table.entry[handle].mdir_index;
  *num_blks = fs_dir_get_numblks(dindex);

  /* Get file's bad powerdown clean-up option */
  *cleanup_option = fs_dir_get_cleanup_option(dindex);

  /* Get file's current file pointer position */
  *file_ptr_position = open_table.entry[handle].fp;

  /* Compute offset of start of user data in first file block */
  filename_length = fs_dir_get_filename_length(fs_dir_get_adrs(dindex));
  *data_start_offset = FSI_FILE_HEADER_FIXED_SIZE + filename_length + 1;

  return FS_OKAY_S;
} /* fs_ops_get_file_info */




/* <EJECT> */
/*===========================================================================

FUNCTION FSI_OPS_WRITE_TLOG

DESCRIPTION
  Write a transaction log entry to a $SYS_TLOG file for the file identified
  by the input dindex.  $SYS_TLOG files are named $SYS_TLOG01, $SYS_TLOG02,
  etc.  The next index is selected automatically.

DEPENDENCIES
  None.

RETURN VALUE
  Status of operation.

SIDE EFFECTS
  None.

===========================================================================*/
static fs_status_type  fsi_ops_write_tlog
(
  fsi_tlog_type               type_of_tlog,  /* Which kind of tlog file */
  word                        dindex,        /* File's directory index */
  fsi_filename_type           *old_name,     /* For file rename only */
  fsi_filename_type           *new_name,     /* For file rename only */
  word                        *tlog_index    /* Returned log file suffix */
)
{
  char                        suffix_string[18];
  word                        tlog_dindex;
  fsi_owner_id_type           owner_id;
  word                        num_blks;
  word                        fbl_list_size;
  word                        file_length;
  dword                       tlog_write_length;
  fs_status_type              status;
  char                        *suffix = suffix_string;


  /* Determine name of next $SYS_TLOG file */
  tlog_full_name = (char *)tlog_file_buffer;
  *tlog_index = tlog_count;
  (void)sprintf(suffix_string, "%d", tlog_count);
  suffix = suffix_string;
  memcpy(tlog_full_name,
         (char *)FS_TLOG_FILENAME_PREFIX,
         (size_t)FS_TLOG_FILENAME_PREFIX_LEN);
  memcpy((char *)(tlog_full_name + FS_TLOG_FILENAME_PREFIX_LEN),
         (char *)suffix,
         (size_t)6);

  /* Convert filename to internal format */
  (void)fsi_ops_convert_filename(
            (fs_filename_type *)&tlog_full_name,&tlog_next_fname);

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif


  /* Create the system transaction log file */
  status = fs_dir_create_file(
               &tlog_next_fname,
               FS_FA_UNRESTRICTED,
               (fs_file_date_type)0,
               FS_OC_CLOSE,
               FS_OB_ALLOW,
               (dword)0,
               (dword)0,
               FSI_FILE_TYPE_TLOG,
               &tlog_dindex);
  if (status != FS_OKAY_S)
  {
    ERR("Couldn't create transaction log file",0,0,0);
  }
  else
  { 
    tlog_count++;
    if (tlog_count >= FSI_TLOG_COUNT_ROLLOVER)
    {
      tlog_count = FSI_TLOG_COUNT_INITIAL;
    }

    /* Begin loading buffer: type_of_tlog field is always first */
    memcpy((byte *)(tlog_file_buffer + FSI_TLOG_OFFSET_TLOG_TYPE),
           (byte *)&type_of_tlog,
           (size_t)FSI_TLOG_TYPE_SIZE);
  
    /* Remaining contents depends on what type of tlog to write */
    if (type_of_tlog == FSI_TLOG_TYPE_RENAME)
    {
      /* File is being renamed: save TLOG file's total length followed */
      /* by the old and new file names                                 */
      file_length = FSI_TLOG_TYPE_SIZE + sizeof(word) +
                    old_name->length + new_name->length + 2;
      memcpy((byte *)(tlog_file_buffer + FSI_TLOG_OFFSET_FNAME),
             (byte *)old_name,
             (size_t)(old_name->length + 1));
      memcpy((byte *)(tlog_file_buffer + FSI_TLOG_OFFSET_FNAME +  
                      old_name->length + 1),
             (byte *)new_name,
             (size_t)new_name->length+1);
    }
    else
    {
      /* Get owner id of the file being operated on */
      owner_id = fs_dir_get_owner_id(dindex);

      /* Load the file's owner ID into the buffer */
      memcpy((byte *)(tlog_file_buffer + FSI_TLOG_OFFSET_OWNER_ID),
             (byte *)&owner_id,
             (size_t)FSI_OWNER_ID_SIZE);
      file_length = FSI_TLOG_OFFSET_OWNER_ID + FSI_OWNER_ID_SIZE;

      /* If this tlog file is the "restore" type, also write the  */
      /* current number of file blocks and the version numbers of */
      /* the blocks to the log file                               */
      if (type_of_tlog == FSI_TLOG_TYPE_RESTORE)
      {
        /* Get number of blocks currently in the file */
        num_blks = fs_dir_get_numblks(dindex);

        memcpy((byte *)(tlog_file_buffer + FSI_TLOG_OFFSET_NUMBLKS),
               (byte *)&num_blks,
               (size_t)sizeof(word));

        /* Write block version number data from the file's current */
        /* fbl list to the log file (no need to save the garbage   */
        /* blocks list)                                            */
        fbl_list_size = FS_MAX_TLOG_SIZE - FSI_TLOG_OFFSET_VERSIONS - 1;
        status = fs_dir_get_fbl_list(
                     dindex,
                     (byte *)(tlog_file_buffer+FSI_TLOG_OFFSET_VERSIONS),
                     &fbl_list_size);
        if (status != FS_OKAY_S)
        {
          ERR_FATAL("Couldn't get fbl list",0,0,0);
        }

        /* Save TLOG file length */
        file_length = FSI_TLOG_OFFSET_VERSIONS + fbl_list_size;
      }
    }

    /* Put total length of TLOG file into buffer */
    memcpy((byte *)(tlog_file_buffer + FSI_TLOG_OFFSET_LENGTH),
           (byte *)&file_length,
           (size_t)sizeof(word));
    tlog_write_length = (dword)file_length;

    /* Write to file from buffer */
    status = fs_dir_write_to_file(
                 tlog_dindex,
                 FSI_START_OF_FILE_POSITION,
                 FALSE,
                 (byte *)tlog_file_buffer,
                 &tlog_write_length,
				 FSI_OP_REQ_BY_OTHERS);
    if (status != FS_OKAY_S)
    {
      ERR("Couldn't write transaction log",0,0,0);
    }
    
    if (type_of_tlog == FSI_TLOG_TYPE_RESTORE)
    {
      /* Save checkpointed file blocks in file's sblkptr list */
      fs_dir_set_sblkptr(dindex);
    }
  }
  return status;
} /* fsi_ops_write_tlog */




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
  fs_handle_type        handle,         /* Handle of file to write log for */
  fsi_tlog_type         type_of_tlog,   /* Which kind of tlog file */
  word                  *tlog_index     /* Returned log file suffix */
)
{
  word                  dindex;
  char                  suffix_string[18];
  fs_handle_type        sys_handle;
  fs_op_request_type    op_req;
  fsi_owner_id_type     owner_id;
  word                  num_blks;
  word                  tlog_size;
  fs_status_type        status  = FS_OKAY_S;
  char                  *suffix = suffix_string;


  /* Get file's Master Directory table index */
  dindex = open_table.entry[handle].mdir_index;

  /* Get owner id of the file being operated on */
  owner_id = fs_dir_get_owner_id(dindex);

  /* Get number of blocks currently in the file */
  num_blks = fs_dir_get_numblks(dindex);

  /* Determine name of next $SYS_TLOG file */
  tlog_full_name = (char *)tlog_file_buffer;
  *tlog_index = tlog_count;
  (void)sprintf(suffix_string, "%d", tlog_count);
  suffix = suffix_string;
  memcpy(tlog_full_name,
         (char *)FS_TLOG_FILENAME_PREFIX,
         (size_t)FS_TLOG_FILENAME_PREFIX_LEN);
  memcpy((char *)(tlog_full_name + FS_TLOG_FILENAME_PREFIX_LEN),
         (char *)suffix,
         (size_t)6);

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif
  
  /* Create the system transaction log file */
  op_req.op                                        = FS_OPEN_OP;
  op_req.parms.open.access_kind                    = FS_OA_CREATE;
  op_req.parms.open.option.create.filename         = tlog_full_name;
  op_req.parms.open.option.create.cleanup_option   = FS_OC_CLOSE;
  op_req.parms.open.option.create.buffering_option = FS_OB_ALLOW;
  op_req.parms.open.option.create.attribute_mask   = FS_FA_UNRESTRICTED;
  op_req.parms.open.option.create.file_type        = FSI_FILE_TYPE_TLOG;
  sys_handle = fs_ops_open(&op_req);
  if (sys_handle == FS_NULL_HANDLE)
  {
    ERR("Couldn't open transaction log file",0,0,0);
    status = FS_FAIL_S;
  }
  else
  {
    tlog_count++;
    if (tlog_count >= FSI_TLOG_COUNT_ROLLOVER)
    {
      tlog_count = FSI_TLOG_COUNT_INITIAL;
    }

    /* Write the tlog type and file's owner ID to the log file */
    memcpy((byte *)tlog_file_buffer,
           (byte *)&type_of_tlog,
           (size_t)FSI_TLOG_TYPE_SIZE);
    memcpy((byte *)(tlog_file_buffer + FSI_TLOG_TYPE_SIZE),
           (byte *)&owner_id,
           (size_t)FSI_OWNER_ID_SIZE);

    /* If this tlog file is the "restore" type, also write the  */
    /* current number of file blocks and the version numbers of */
    /* the blocks to the log file                               */
    if (type_of_tlog == FSI_TLOG_TYPE_RESTORE)
    {
      memcpy((byte *)(tlog_file_buffer+FSI_TLOG_TYPE_SIZE+FSI_OWNER_ID_SIZE),
             (byte *)&num_blks,
             (size_t)sizeof(word));

      /* Write block version number data from the file's current fbl    */
      /* list to the log file (no need to save the garbage blocks list) */
      tlog_size = FS_MAX_TLOG_SIZE - (FSI_OWNER_ID_SIZE + sizeof(word));
      status = fs_dir_get_fbl_list(
                   dindex,
                   (byte *)(tlog_file_buffer + FSI_TLOG_TYPE_SIZE + 
                            FSI_OWNER_ID_SIZE + sizeof(word)),
                   &tlog_size);
      if (status != FS_OKAY_S)
      {
        ERR("Couldn't get fbl list",0,0,0);
      }
      tlog_size += FSI_TLOG_TYPE_SIZE + FSI_OWNER_ID_SIZE + sizeof(word);
    }
    else
    {
      tlog_size = FSI_TLOG_TYPE_SIZE + FSI_OWNER_ID_SIZE;
    }

    if (status == FS_OKAY_S)
    {
      /* Write out file */
      op_req.op                     = FS_WRITE_OP;
      op_req.req_task               = FSI_OP_REQ_BY_OTHERS;
      op_req.parms.write.fhandle    = sys_handle;
      op_req.parms.write.buf        = (byte *)tlog_file_buffer;
      op_req.parms.write.length     = (dword)tlog_size;
      op_req.parms.write.ccode.code = FS_NULL_CONF_CODE;
      status = fs_ops_write(&op_req);
      if (status != FS_OKAY_S)
      {
        ERR("Couldn't write transaction log",0,0,0);
      }
    }
    
    /* Close file */
    op_req.op                  = FS_CLOSE_OP;
    op_req.parms.close.fhandle = sys_handle;
    if (fs_ops_close(&op_req) != FS_OKAY_S)
    {
      ERR_FATAL("Couldn't close transaction log",0,0,0);
    }

    /* Save checkpointed file blocks in file's sblkptr list */
    fs_dir_set_sblkptr(dindex);
  
    /* Set pointer to this transaction log in master file directory */
    fs_dir_set_tlog_index(dindex,*tlog_index);
  }
  return status;
} /* fs_ops_write_tlog */




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
)
{
  fs_status_type            status;
  dword                     tlog_size = (dword)FS_MAX_TLOG_SIZE;

  
  /* Read file directly into local buffer */
  status = fs_dir_read_from_file(dindex,
                                 FSI_START_OF_FILE_POSITION,
                                 tlog_file_buffer, 
                                 &tlog_size);
  return status;
} /* fs_ops_read_tlog */




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
)
{
  return *((fsi_tlog_type *)tlog_file_buffer);
} /* fs_ops_get_tlog_type_id */




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
)
{
  return *((fsi_owner_id_type *)(tlog_file_buffer + FSI_TLOG_TYPE_SIZE));
} /* fs_ops_get_tlog_owner_id */




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
)
{
  byte                      *numblks_adrs;

  numblks_adrs = tlog_file_buffer + FSI_TLOG_TYPE_SIZE + FSI_OWNER_ID_SIZE;
  return *((word *)numblks_adrs);
} /* fs_ops_get_tlog_numblks */




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
)
{
  byte                      *version_adrs;

  version_adrs = tlog_file_buffer + FSI_TLOG_TYPE_SIZE + FSI_OWNER_ID_SIZE +
                 sizeof(word) + (blk_number*sizeof(fsi_blk_version_type));
  return *((fsi_blk_version_type *)version_adrs);
} /* fs_ops_get_tlog_version */




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
//  fs_handle_type            handle,     /* Handle of file to write log for */
  word                      tlog_index  /* Returned log file suffix */
)
{
  char                      suffix_string[18];
  fs_status_type            status  = FS_OKAY_S;
  char                      *suffix = suffix_string;


//  /* Get file's Master Directory table index */
//  dindex = open_table.entry[handle].mdir_index;
  tlog_full_name = (char *)tlog_file_buffer;

  /* Determine name of the specific $SYS_TLOG file */
  (void)sprintf(suffix_string, "%d", tlog_index); 
  suffix = suffix_string;
  memcpy((byte *)tlog_full_name,
         (byte *)FS_TLOG_FILENAME_PREFIX,
         (size_t)FS_TLOG_FILENAME_PREFIX_LEN);
  memcpy((byte *)(tlog_full_name + FS_TLOG_FILENAME_PREFIX_LEN),
         (byte *)suffix,
         (size_t)6);

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif

  /* Remove the system file */
  status = fs_ops_remove_file(tlog_full_name);
  if (status != FS_OKAY_S)
  {
    ERR("Couldn't remove transaction log file",0,0,0);
  }
  return status;

} /* fs_ops_remove_tlog */




/* <EJECT> */
/*===========================================================================

FUNCTION FSI_OPS_OPEN_CREATE

DESCRIPTION
  Set the block header fields and the file header data in the input
  generic block.

DEPENDENCIES
  None.

RETURN VALUE
  FS_NULL_HANDLE  - if operation failed
  file handle     - if operation worked

SIDE EFFECTS
  None.

===========================================================================*/
static fs_handle_type  fsi_ops_open_create
(
  fs_op_request_type        *inparm,
  fsi_filename_type         *filename
)
{
  boolean                   found;
  fsi_current_pointer_type  found_pos;
  word                      dindex;
  int                       dindex_int;
  int                       oindex;
  fsi_file_date_type        file_creation_date;
  fs_handle_type            handle = FS_NULL_HANDLE;


  /* Remote files can only be linked-to, not created */
  if (inparm->parms.open.option.create.attribute_mask == FS_FA_REMOTE)
  {
    ERR("Cannot create REMOTE files",0,0,0);
    inparm->status = FS_ILLEGAL_OPERATION_S;
    return FS_NULL_HANDLE;
  }

  /* Set internal file type to system-permanent if attribute so specifies */
  if (inparm->parms.open.option.create.attribute_mask == 
      FS_FA_SYS_PERMANENT)
  {
    inparm->parms.open.option.create.file_type = FSI_FILE_TYPE_SYS_PERM;
  }

  /* Split off directory path from file name */
  fs_udir_split_filename(filename,&path,&local_fname);

  /* Verify that file name doesn't contain illegal characters */
  if (strchr((char *)local_fname.name,FS_DIRECTORY_CHAR) != NULL)
  {
    ERR("File name contains directory separator character",0,0,0);
    inparm->status = FS_BAD_FILE_NAME_S;
  }
  else
  {
    /* If path is non-NULL, verify that the path to the file exists */
    found = FALSE;
    if (path.length != 0)
    {
      dindex_int = fs_udir_find_udir_dindex();
      dindex     = (word)dindex_int;
      fs_udir_search_user_dirs(dindex,&path,&found,&found_pos);
    }

    if (found || (path.length == 0))
    {
      /* Create new user file, leave open for UPDATE access unless */
      /* file is a remote file                                     */
      oindex = fsi_ops_opent_allocate_entry();
      if (oindex < 0)
      {
        /* Out of Open Files Table entries */
        inparm->status = FS_OPEN_TABLE_FULL_S;
        return FS_NULL_HANDLE;
      }
 
      file_creation_date = fsi_ops_get_timestamp();
      inparm->status = 
          fs_dir_create_file(
              filename,
              inparm->parms.open.option.create.attribute_mask,
              file_creation_date,
              inparm->parms.open.option.create.cleanup_option,
              inparm->parms.open.option.create.buffering_option,
              (dword)0,
              (dword)0,
              inparm->parms.open.option.create.file_type,
              &dindex);
      if (inparm->status == FS_OKAY_S)
      {
        open_table.entry[oindex].mdir_index = dindex;
  
        /* Set current file pointer position to start of file */
        open_table.entry[oindex].fp = FSI_START_OF_FILE_POSITION;
  
        /* Set file's original size */
        open_table.entry[oindex].orig_size = (dword)0;
  
        /* Set current file access mode to UPDATE */
        open_table.entry[oindex].fmode = FSI_MODE_UPDATE;
        
        /* Set return file handle */
        handle = (fs_handle_type)oindex; 
      }
      else
      {
        /* Close file by freeing the open files table entry */
        (void)fsi_ops_opent_free_entry((word)oindex);
        return FS_NULL_HANDLE;
      }
    }
    else
    {
      inparm->status = FS_NONEXISTENT_DIR_S;
    }
  }

  return handle;
} /* fsi_ops_open_create */




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
)
{
  fsi_attribute_mask_type  attribute_mask;
  dword                    fsize;
  int                      oindex;
  word                     tlog_index;
  fs_cleanup_option_type   cleanup_option;
  fsi_tlog_type            type_of_tlog = FSI_TLOG_TYPE_NONE;


  attribute_mask = fs_dir_get_attribute_mask(dindex);

  if ((attribute_mask == FS_FA_READONLY) ||
      (attribute_mask == FS_FA_REMOTE))
  {
    /* File's attribute mask prohibits update access */
    ERR("Attribute mask prohibits operation",0,0,0);
    *status = FS_ILLEGAL_OPERATION_S;
    return FS_NULL_HANDLE;
  }

  /* Allocate an entry from the Open Files Table */
  oindex = fsi_ops_opent_allocate_entry();
  if (oindex < 0)
  {
    /* Out of Open Files Table entries */
    *status = FS_OPEN_TABLE_FULL_S;
    return FS_NULL_HANDLE;
  }

  /* Set Master File Directory pointer */
  open_table.entry[oindex].mdir_index = dindex;
  
  /* Set current file access mode to APPEND */
  open_table.entry[oindex].fmode = FSI_MODE_APPEND;

  /* Set current file pointer position to current end of file */
  fsize = fs_dir_get_fsize(dindex);
  open_table.entry[oindex].fp = fsize;

  /* Set file's original size (i.e., size at open) */
  open_table.entry[oindex].orig_size = fsize;

  /* Write a transaction log if the file reversion option is set */
  cleanup_option = fs_dir_get_cleanup_option(dindex);
  switch (cleanup_option)
  {
    case FS_OC_DISCARD:
      type_of_tlog = FSI_TLOG_TYPE_DELETE;
      break;
    case FS_OC_TRUNCATE:
      type_of_tlog = FSI_TLOG_TYPE_TRUNCATE;
      break;
    case FS_OC_REVERT:
      type_of_tlog = FSI_TLOG_TYPE_RESTORE;
      break;
  }

  if (type_of_tlog != FSI_TLOG_TYPE_NONE)
  {
    /* Don't write a new tlog if there's already one for this file */
    /* (only happens if multiple opens for update occur)           */
    if (!fs_dir_file_checkpointed(dindex))
    {
      *status = fs_ops_write_tlog((fs_handle_type)oindex,
                                  type_of_tlog,
                                  &tlog_index);
      if (*status != FS_OKAY_S)
      {
        /* No room for transaction log, file cannot be opened. */
        /* Close file by freeing the open files table entry.   */
        /* Also remove file's sblkptr list.                    */
        (void)fs_ops_remove_tlog(/*(fs_handle_type)oindex,*/tlog_index);
        fs_dir_invalidate_sblkptr((word)oindex);

        (void)fsi_ops_opent_free_entry((word)oindex);
        *status = FS_SPACE_EXHAUSTED_S;
        return FS_NULL_HANDLE;
      }
      /* Set pointer to this transaction log in master file directory */
      fs_dir_set_tlog_index(dindex,tlog_index);
    }
  }

  /* Return file handle */
  return (fs_handle_type)oindex;
} /* fs_ops_open_append */




/* <EJECT> */
/*===========================================================================

FUNCTION FSI_OPS_OPEN_READONLY

DESCRIPTION
  Open file for readonly access.  Sets the current file pointer position
  and file access mode in this file's master directory entry.

DEPENDENCIES
  None.

RETURN VALUE
  FS_NULL_HANDLE  - if operation failed
  file handle     - if operation worked

SIDE EFFECTS
  None.

===========================================================================*/
static fs_handle_type  fsi_ops_open_readonly
(
  word                     dindex,
  fs_status_type           *status
)
{
  int                      oindex;


  /* Allocate an entry from the Open Files Table */
  oindex = fsi_ops_opent_allocate_entry();
  if (oindex < 0)
  {
    /* Out of Open Files Table entries */
    *status = FS_OPEN_TABLE_FULL_S;
    return FS_NULL_HANDLE;
  }

  /* Set Master File Directory pointer */
  open_table.entry[oindex].mdir_index = dindex;
  
  /* Set current file access mode to READONLY */
  open_table.entry[oindex].fmode = FSI_MODE_READONLY;

  /* Set current file pointer position to start of file */
  open_table.entry[oindex].fp = FSI_START_OF_FILE_POSITION;

  /* Set return status */
  *status = FS_OKAY_S;

  /* Return file handle */
  return (fs_handle_type)oindex;

} /* fsi_ops_open_readonly */




/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_OPEN_READWRITE

DESCRIPTION
  Open file for read-write access.  Sets the current file pointer position
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
)
{
  fsi_attribute_mask_type  attribute_mask;
  int                      oindex;
  word                     tlog_index;
  fs_cleanup_option_type   cleanup_option;
  fsi_tlog_type            type_of_tlog = FSI_TLOG_TYPE_NONE;


  attribute_mask = fs_dir_get_attribute_mask(dindex);

  if ((attribute_mask == FS_FA_PERMANENT)     ||
      (attribute_mask == FS_FA_SYS_PERMANENT) ||
      (attribute_mask == FS_FA_READONLY)      ||
      (attribute_mask == FS_FA_REMOTE))
  {
    /* File's attribute mask prohibits update access */
    ERR("Attribute mask prohibits operation",0,0,0);
    *status = FS_ILLEGAL_OPERATION_S;
    return FS_NULL_HANDLE;
  }

  /* Allocate an entry from the Open Files Table */
  oindex = fsi_ops_opent_allocate_entry();
  if (oindex < 0)
  {
    /* Out of Open Files Table entries */
    *status = FS_OPEN_TABLE_FULL_S;
    return FS_NULL_HANDLE;
  }

  /* Set Master File Directory pointer */
  open_table.entry[oindex].mdir_index = dindex;

  /* Set current file access mode to UPDATE */
  open_table.entry[oindex].fmode = FSI_MODE_UPDATE;

  /* Set current file pointer position to start of file */
  open_table.entry[oindex].fp = FSI_START_OF_FILE_POSITION;

  /* Write a transaction log if the file reversion option is set */
  cleanup_option = fs_dir_get_cleanup_option(dindex);
  switch (cleanup_option)
  {
    case FS_OC_DISCARD:
      type_of_tlog = FSI_TLOG_TYPE_DELETE;
      break;
    case FS_OC_TRUNCATE:
      type_of_tlog = FSI_TLOG_TYPE_TRUNCATE;
      break;
    case FS_OC_REVERT:
      type_of_tlog = FSI_TLOG_TYPE_RESTORE;
      break;
  }

  if (type_of_tlog != FSI_TLOG_TYPE_NONE)
  {
    /* Don't write a new tlog if there's already one for this file */
    /* (only happens if multiple opens for update occur)           */
    if (!fs_dir_file_checkpointed(dindex))
    {
      *status = fs_ops_write_tlog((fs_handle_type)oindex,
                                  type_of_tlog,
                                  &tlog_index);
      if (*status != FS_OKAY_S)
      {
        /* No room for transaction log, file cannot be opened. */
        /* Close file by freeing the open files table entry.   */
        /* Also remove file's sblkptr list.                    */
        (void)fs_ops_remove_tlog(/*(fs_handle_type)oindex,*/tlog_index);
        fs_dir_invalidate_sblkptr((word)oindex);

        (void)fsi_ops_opent_free_entry((word)oindex);
        *status = FS_SPACE_EXHAUSTED_S;
        return FS_NULL_HANDLE;
      }
      /* Set pointer to this transaction log in master file directory */
      fs_dir_set_tlog_index(dindex,tlog_index);
    }
  }
 
  /* Return file handle */
  return (fs_handle_type)oindex;

} /* fs_ops_open_readwrite */




/* <EJECT> */
/*===========================================================================

FUNCTION FSI_OPS_OPEN_TRUNCATE

DESCRIPTION
  Truncate a file from a specified position.  The file's attribute
  field must allow update access.
  
DEPENDENCIES
  None.

RETURN VALUE
  FS_OKAY_S       - if operation worked
  status          - if errors encountered

SIDE EFFECTS
  None.

===========================================================================*/
static fs_handle_type  fsi_ops_open_truncate
(
  word                     dindex,         /* Master File Directory index */
  fs_file_position_type    truncate_position,
  fs_status_type           *status
)
{
  fsi_attribute_mask_type  attribute_mask;
  int                      oindex;
  word                     tlog_index;
  fs_cleanup_option_type   cleanup_option;
  boolean                  wrote_tlog  = FALSE;
  fsi_tlog_type            type_of_tlog = FSI_TLOG_TYPE_NONE;


  attribute_mask = fs_dir_get_attribute_mask(dindex);

  if ((attribute_mask == FS_FA_PERMANENT)     ||
      (attribute_mask == FS_FA_SYS_PERMANENT) ||
      (attribute_mask == FS_FA_READONLY)      ||
      (attribute_mask == FS_FA_REMOTE))
  {
    /* File's attribute mask prohibits file truncation */
    ERR("Attribute mask prohibits operation",0,0,0);
    *status = FS_ILLEGAL_OPERATION_S;
    return FS_NULL_HANDLE;
  }

  /* Is specified truncate position valid?  It must fall within the */
  /* current file extent                                            */
  if (truncate_position > fs_dir_get_fsize(dindex))
  {
    ERR("Invalid truncate position",0,0,0);
    *status = FS_BAD_TRUNCATE_POS_S;
    return FS_NULL_HANDLE;
  }

  /* Allocate an entry from the Open Files Table */
  oindex = (word)fsi_ops_opent_allocate_entry();
  if (oindex < 0)
  {
    /* Out of Open Files Table entries */
    *status = FS_OPEN_TABLE_FULL_S;
    return FS_NULL_HANDLE;
  }

  /* Write a transaction log if the file reversion option is set */
  cleanup_option = fs_dir_get_cleanup_option(dindex);
  switch (cleanup_option)
  {
    case FS_OC_DISCARD:
      type_of_tlog = FSI_TLOG_TYPE_DELETE;
      break;
    case FS_OC_TRUNCATE:
      type_of_tlog = FSI_TLOG_TYPE_TRUNCATE;
      break;
    case FS_OC_REVERT:
      type_of_tlog = FSI_TLOG_TYPE_RESTORE;
      break;
  }

  if (type_of_tlog != FSI_TLOG_TYPE_NONE)
  {
    /* Don't write a new tlog if there's already one for this file */
    /* (only happens if multiple opens for update occur)           */
    if (!fs_dir_file_checkpointed(dindex))
    {
      *status = fs_ops_write_tlog((fs_handle_type)oindex,
                                  type_of_tlog,
                                  &tlog_index);
      if (*status != FS_OKAY_S)
      {
        /* No room for transaction log, file cannot be opened. */
        /* Close file by freeing the open files table entry.   */
        /* Also remove file's sblkptr list.                    */
        (void)fs_ops_remove_tlog(/*(fs_handle_type)oindex,*/tlog_index);
        fs_dir_invalidate_sblkptr((word)oindex);

        (void)fsi_ops_opent_free_entry((word)oindex);
        *status = FS_SPACE_EXHAUSTED_S;
        return FS_NULL_HANDLE;
      }

      wrote_tlog = TRUE;
      /* Set pointer to this transaction log in master file directory */
      fs_dir_set_tlog_index(dindex,tlog_index);
    }
  }
 
  /* Truncate the file */
  *status = fs_dir_truncate_file(dindex,truncate_position);
  if (*status == FS_OKAY_S)
  {
    /* Set Master File Directory pointer */
    open_table.entry[oindex].mdir_index = dindex;
  
    /* Set current file access mode to UPDATE */
    open_table.entry[oindex].fmode = FSI_MODE_UPDATE;
  
    /* Set current file pointer position to start of file */
    open_table.entry[oindex].fp = FSI_START_OF_FILE_POSITION;
  
    /* Set file's original size */
    open_table.entry[oindex].orig_size = (dword)0;
 
    /* Return file handle */
    return (fs_handle_type)oindex;
  }
  else
  {
    /* Truncate failed: close file by freeing the open files table entry */
    (void)fsi_ops_opent_free_entry((word)oindex);

    /* If a tlog file was written, remove it */
    if (wrote_tlog)
    {
      (void)fs_ops_remove_tlog(/*(fs_handle_type)oindex,*/tlog_index);
      fs_dir_invalidate_sblkptr((word)oindex);
    }
    return FS_NULL_HANDLE;
  }

} /* fsi_ops_open_truncate */




/* <EJECT> */
/*===========================================================================

FUNCTION FSI_OPS_OPEN_TEST

DESCRIPTION
  Perform file system test functions.
  
DEPENDENCIES
  None.

RETURN VALUE
  FS_OKAY_S       - if operation worked
  status          - if errors encountered

SIDE EFFECTS
  None.

===========================================================================*/
static fs_handle_type  fsi_ops_open_test
(
  void
)
{
  return FS_NULL_HANDLE;
} /* fsi_ops_open_test */




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
)
{
  word                     dindex;
  int                      dindex_int;
  fsi_filename_type        filename;
  fs_handle_type           f_handle = FS_NULL_HANDLE;
  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif
  

  /* Convert filename to internal format */
  inparm->status = fsi_ops_convert_filename(
                       &inparm->parms.open.option.create.filename,&filename);
  if (inparm->status != FS_OKAY_S)
  {
    return FS_NULL_HANDLE;
  }

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif


  /* Find file name in master file directory */
  dindex_int = fs_dir_find_filename_index(&filename);
  dindex     = (word)dindex_int;

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif


  /* Branch based on requested access_option */
  switch (inparm->parms.open.access_kind)
  {
    case FS_OA_CREATE:
    {
      if (dindex_int < 0)
      {   
        f_handle = fsi_ops_open_create(inparm,&filename);
      }
      else
      {
        /* File already exists */
        MSG_HIGH("Attempted to create existing file",0,0,0);
        inparm->status = FS_FILE_ALREADY_EXISTS_S;
        return FS_NULL_HANDLE;
      }
      break;
    }
    case FS_OA_APPEND:
    {
      if (dindex_int >= 0) 
      {
        f_handle = fs_ops_open_append(dindex,&inparm->status);
      }
      else
      {
        /* File doesn't exist */
        MSG_HIGH("Appending to non-existent file",0,0,0);
        inparm->status = FS_NONEXISTENT_FILE_S;
        return FS_NULL_HANDLE;
      }
      break;
    }

    case FS_OA_READONLY:
    {
      if (dindex_int >= 0) 
      {
        f_handle = fsi_ops_open_readonly(dindex,&inparm->status);
      }
      else
      {
        /* File doesn't exist */
        MSG_HIGH("Reading from non-existent file",0,0,0);
        inparm->status = FS_NONEXISTENT_FILE_S;
        return FS_NULL_HANDLE;
      }
      break;
    }

    case FS_OA_READWRITE:
    {
      if (dindex_int >= 0)
      {
        f_handle = fs_ops_open_readwrite(dindex,&inparm->status);
      }
      else
      {
        /* File doesn't exist */
        MSG_HIGH("Updating non-existent file",0,0,0);
        inparm->status = FS_NONEXISTENT_FILE_S;
        return FS_NULL_HANDLE;
      }
      break;
    }

    case FS_OA_TRUNCATE:
    {
      if (dindex_int >= 0)
      {
        f_handle = fsi_ops_open_truncate(
                       dindex,
                       inparm->parms.open.option.truncate.position,
                       &inparm->status);
      }
      else
      {
        /* File doesn't exist */
        MSG_HIGH("Truncating non-existent file",0,0,0);
        inparm->status = FS_NONEXISTENT_FILE_S;
        return FS_NULL_HANDLE;
      }
      break;
    }

    case FS_OA_TEST:
    {
      /* Reserved for test purposes */
      f_handle = fsi_ops_open_test();
      break;
    }

    default:
    {
      ERR("Specified invalid access type",0,0,0);
      inparm->status = FS_ILLEGAL_OPERATION_S;
      return FS_NULL_HANDLE;
    }
  }

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif

  return  f_handle;
} /* fs_ops_open */




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
)
{
  word                   dindex;
  word                   tlog_index;
  word                   saved_tlog_index;
  boolean                more_handles;
  fs_status_type         status;
  fsi_access_mode_type   access_mode;
  fs_handle_type         handle = inparm->parms.close.fhandle;


  /* Validate handle */
  status = fsi_ops_validate_handle(handle);
  if (status != FS_OKAY_S)
  {
    return status;
  }

  /* Find Master Directory entry corresponding to file handle */
  dindex = open_table.entry[handle].mdir_index;

  /* Verify that file is currently open and get tlog_index (if one) */
  if (!fsi_ops_file_is_open(dindex,&saved_tlog_index,&access_mode))
  {
    ERR("File status consistency check error",0,0,0);
    return FS_FAIL_S;
  }

  /* Close file by freeing the open files table entry */
  status = fsi_ops_opent_free_entry((word)handle);
 
  /* If file had a transaction log associated with it, determine    */
  /* whether it should be removed now.  If there is even one handle */
  /* (for update access) still on the file, leave the tlog file     */
  /* alone; but if this is the last open handle on the file, the    */
  /* tlog file must be removed now.                                 */
  more_handles = fsi_ops_file_is_open(dindex,&tlog_index,&access_mode);
  if (more_handles)
  {
    if (!((access_mode==FSI_MODE_UPDATE) || (access_mode==FSI_MODE_APPEND)))
    {
      /* Only remaining handles are for non-update access */
      more_handles = FALSE;
    }
  }

  if (!more_handles)
  {
    if (saved_tlog_index != FSI_TLOG_INDEX_NULL)
    {
      (void)fs_ops_remove_tlog(/*handle,*/saved_tlog_index);

      /* Also remove file's sblkptr list */
      fs_dir_invalidate_sblkptr(dindex);

      /* And clear the file's tlog_index */
      fs_dir_set_tlog_index(dindex,FSI_TLOG_INDEX_NULL);
    }
  }
 
  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif
  
  return status;
} /* fs_ops_close */




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
)
{
  word                        dindex;
  fsi_access_mode_type        access_mode;
  dword                       read_length;
  fs_status_type              status;
  fs_handle_type              handle = inparm->parms.read.fhandle;


  /* Initialize count of bytes read */
  read_length = inparm->parms.read.length;
  inparm->parms.read.length = (dword)0;

  /* Validate handle and check that file is currently open */
  status = fsi_ops_validate_handle(handle);
  if (status != FS_OKAY_S)
  {
    return status;
  }

  /* Find Master Directory entry corresponding to file handle */
  dindex = open_table.entry[handle].mdir_index;
  
  /* Is file open for read or update access? */
  access_mode = open_table.entry[handle].fmode;
  if ((access_mode != FSI_MODE_UPDATE)  && 
      (access_mode != FSI_MODE_APPEND)  &&
      (access_mode != FSI_MODE_READONLY))
  {
    /* File not open for read or update access, return error */
    MSG_HIGH("File not open for read access",0,0,0);
    return FS_FILE_NOT_OPEN_S;
  }

  /* Read bytes from file */
  if (fs_dir_get_remote_ptr(dindex) == FSI_NULL_REMOTE_PTR)
  {
    status = fs_dir_read_from_file(
                 dindex,
                 open_table.entry[handle].fp,
                 inparm->parms.read.buf,
                 &read_length);
  }
  else
  {
    status = fs_dir_read_from_remote_file(
                 dindex,
                 open_table.entry[handle].fp,
                 inparm->parms.read.buf,
                 &read_length);
  }  

  /* Update file pointer position */
  open_table.entry[handle].fp += read_length;

  /* Set return number of bytes actually read */
  inparm->parms.read.length = read_length;

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif
  
  if (status == FS_EOF_S) 
  {
    status = FS_OKAY_S;
  }
  return status;
} /* fs_ops_read */




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
)
{
  fsi_access_mode_type      access_mode;
  word                      dindex;
  dword                     write_length;
  dword                     additional_space;
  dword                     filesize;
  int                       i;
  fs_status_type            status;
  fs_handle_type            handle  = inparm->parms.write.fhandle;


  /* Initialize count of bytes written */
  write_length = inparm->parms.write.length;
  inparm->parms.write.length = (dword)0;

  /* Validate handle and check that file is currently open */
  status = fsi_ops_validate_handle(handle);
  if (status != FS_OKAY_S)
  {
    return status;
  }

  /* Is file open for update or append access? */
  access_mode = open_table.entry[handle].fmode;
  if ((access_mode != FSI_MODE_UPDATE)  && 
      (access_mode != FSI_MODE_APPEND))
  {
    /* File not open for update or append access, return error */
    MSG_HIGH("File not open for update access",0,0,0);
    return FS_FILE_NOT_OPEN_S;
  }

  /* Find Master Directory entry corresponding to file handle */
  dindex = open_table.entry[handle].mdir_index;

  /* If this is a reserved write, must validate reservation */
  if (inparm->parms.write.ccode.code != FS_NULL_CONF_CODE)
  {
    /* Validate confirmation code */
    i = (int)((inparm->parms.write.ccode.code>>16)&0x000000FF);
    if (((i<0) || (i>=FS_MAX_RESERVATIONS))   ||
        (rsvr_ledger.entry[i].conf_code.code  != 
            inparm->parms.write.ccode.code)   ||
        (rsvr_ledger.entry[i].conf_code.count != 
            inparm->parms.write.ccode.count))
    {
      ERR("Invalid confirmation code",0,0,0);
      return FS_INVALID_CONF_CODE_S;
    }
    else
    {
      /* Verify that enough space was reserved for the write */
      filesize = fs_dir_get_fsize(dindex);
      additional_space = 
          MAX((open_table.entry[handle].fp + write_length),filesize);
      additional_space = 
          (additional_space>filesize ? (additional_space - filesize) : 0);
      if (rsvr_ledger.entry[i].conf_code.count < additional_space)
      {
        ERR("Reserved insufficient space",0,0,0);
        return FS_INVALID_CONF_CODE_S;
      }
    }
  }
 
  /* Write bytes to file */
  status = fs_dir_write_to_file(
               dindex,
               open_table.entry[handle].fp,
               FALSE,
               inparm->parms.write.buf,
               &write_length,
			   inparm->req_task);
 
  /* Update file pointer position (yes, even if status is bad) */
  open_table.entry[handle].fp += write_length;

  /* Set return number of bytes actually written */
  inparm->parms.write.length = write_length;
 
  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif
 
  return status;
} /* fs_ops_write */




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
)
{
  word                   dindex;
  int                    dindex_int;
  dword                  fsize = 0l;
  fsi_filename_type      filename;
  fs_status_type         status;


  /* Convert filename to internal format */
  status = fsi_ops_convert_filename(
               &inparm->parms.file_size.filename,&filename);
  if (status != FS_OKAY_S)
  {
    return status;
  }

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif

  /* Find file name in master file directory */
  dindex_int = fs_dir_find_filename_index(&filename);
  dindex     = (word)dindex_int;

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif

  if (dindex_int >= 0)
  {
    /* File found, get it's current size */
    fsize = fs_dir_get_fsize(dindex);
  }
  else
  {
    MSG_HIGH("File doesn't exist",0,0,0);
    inparm->parms.file_size.length = (dword)0;
    return FS_NONEXISTENT_FILE_S;
  }

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif
  
  /* Set return "length" parameter */
  inparm->parms.file_size.length = fsize;
  
  return FS_OKAY_S;

} /* fs_ops_file_size */



/*===========================================================================
 * FUNCTION FS_OPS_OPENED_FILE_SIZE
 *
 * Return the size of the opened file.
===========================================================================*/

fs_status_type fs_ops_opened_file_size (fs_op_request_type  *inparm)
{
  fs_status_type   status;
  word dindex;
  fs_handle_type handle = inparm->parms.opened_file_size.fhandle;

  status = fsi_ops_validate_handle (handle);
  if (status != FS_OKAY_S) {
    return status;
  }

  /* Find master directory entry corresponding to file handle. */
  dindex = open_table.entry[handle].mdir_index;

  inparm->parms.opened_file_size.length =
    fs_dir_get_fsize (dindex);

  return FS_OKAY_S;
}


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
)
{
  word                   dindex;
  fsi_access_mode_type   access_mode;
  dword                  fsize;
  dword                  original_fsize;
  dword                  length;
  fs_status_type         status;
  fs_handle_type         handle = inparm->parms.seek.fhandle;
  fs_seek_origin_type    origin = inparm->parms.seek.origin;
  fs_file_position_type  spos   = inparm->parms.seek.position;


  /* Validate handle and check that file is currently open */
  status = fsi_ops_validate_handle(handle);
  if (status != FS_OKAY_S)
  {
    return status;
  }

  /* Find Master Directory entry corresponding to file handle */
  dindex = open_table.entry[handle].mdir_index;
 
  /* Get current file size */
  fsize = fs_dir_get_fsize(dindex);

  /* Translate specified seek position into absolute file position */
  if (origin == FS_SEEK_CURRENT)
  {
    /* Add current file position to seek position */
    spos += open_table.entry[handle].fp;
  }
  else if (origin == FS_SEEK_EOF)
  {
    /* Add current file size to seek position */
    spos += (fs_file_position_type)fsize;
  }

  /* Verify that file is currently open and that seek position */
  /* is valid (validity depends on access mode)                */
  access_mode = open_table.entry[handle].fmode;
  if (access_mode == FSI_MODE_APPEND)
  {
    /* Append access: prohibit seeking to any point within the */
    /* file's original extent                                  */
    original_fsize = open_table.entry[handle].orig_size;

    if (spos < original_fsize)
    {
      ERR("Invalid SEEK position",0,0,0);
      return FS_BAD_SEEK_POS_S;
    }
  }
  else if (access_mode == FSI_MODE_READONLY)
  {
    /* Read-only access: seek position must fall within        */
    /* existing file extent                                    */
    if (spos > fsize)
    {
      ERR("Invalid SEEK position",0,0,0);
      return FS_BAD_SEEK_POS_S;
    }
  }
  else if (access_mode != FSI_MODE_UPDATE)
  {
    /* File is not currently open, return error */
    MSG_HIGH("File not open",0,0,0);
    return FS_FILE_NOT_OPEN_S;
  }


  /* Is seek position beyond current end of file? */
  if (spos > fsize)
  {
    /* Set file pointer position to current end of file position */
    open_table.entry[handle].fp = (fsi_current_pointer_type)fsize;

    /* Write erased bytes up to seek position */
    length = spos - fsize;
    status = fs_dir_write_blanks_to_file(
                 dindex,open_table.entry[handle].fp,&length);
 
    /* Update file pointer position (yes, even if status is bad) */
    open_table.entry[handle].fp += length;
    if (status != FS_OKAY_S) 
    {
      return status;
    }
  }

  /* Set file pointer position to new seek position */
  open_table.entry[handle].fp = (fsi_current_pointer_type)spos;

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif
 
  return  FS_OKAY_S;
} /* fs_ops_seek */




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
)
{
  fs_status_type            status;
  fs_handle_type            handle = inparm->parms.tell.fhandle;
   

  /* Validate handle and check that file is currently open */
  status = fsi_ops_validate_handle(handle);
  if (status != FS_OKAY_S)
  {
    return status;
  }

  /* Set return variable containing current file pointer position */
  inparm->parms.tell.position = open_table.entry[handle].fp;
  
  return  FS_OKAY_S;
} /* fs_ops_tell */




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
)
{
  word                      dindex;
  fsi_attribute_mask_type   attribute_mask;
  fsi_access_mode_type      access_mode;
  fs_status_type            status;
  fs_handle_type            handle = inparm->parms.tell.fhandle;
   

  /* Validate handle and check that file is currently open */
  status = fsi_ops_validate_handle(handle);
  if (status != FS_OKAY_S)
  {
    return status;
  }

  access_mode = open_table.entry[handle].fmode;
  if (access_mode != FSI_MODE_UPDATE)
  {
    /* File not open for update (truncate) access, return error */
    MSG_HIGH("File not open for update access",0,0,0);
    return FS_FILE_NOT_OPEN_S;
  }

  /* Find Master Directory entry corresponding to file handle */
  dindex = open_table.entry[handle].mdir_index;
   
  attribute_mask = fs_dir_get_attribute_mask(dindex);
  if ((attribute_mask == FS_FA_PERMANENT)     ||
      (attribute_mask == FS_FA_SYS_PERMANENT) ||
      (attribute_mask == FS_FA_READONLY)      ||
      (attribute_mask == FS_FA_REMOTE))
  {
    /* File's attribute mask prohibits file truncation */
    ERR("Attribute mask prohibits operation",0,0,0);
    return FS_ILLEGAL_OPERATION_S;
  }

  /* Is specified truncate position valid?  It must fall within the */
  /* current file extent                                            */
  if (inparm->parms.truncate.position > fs_dir_get_fsize(dindex))
  {
    ERR("Invalid truncate position",0,0,0);
    return FS_BAD_TRUNCATE_POS_S;
  }

  /* Set current file pointer position to specified truncate position  */
  open_table.entry[handle].fp = inparm->parms.truncate.position;

  /* Truncate the file */
  status = fs_dir_truncate_file(dindex,open_table.entry[handle].fp);

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif
   
  return status;

} /* fs_ops_truncate */




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
)
{
  word                   dindex;
  fsi_access_mode_type   access_mode;
  dword                  fsize;
  dword                  original_fsize;
  dword                  length;
  fs_status_type         status;
  fs_handle_type         handle   = inparm->parms.fmemset.fhandle;
  fs_file_position_type  end_pos  = inparm->parms.fmemset.position;
  byte                   set_char = inparm->parms.fmemset.fill_value;


  /* Validate handle and check that file is currently open */
  status = fsi_ops_validate_handle(handle);
  if (status != FS_OKAY_S)
  {
    return status;
  }

  /* Find Master Directory entry corresponding to file handle */
  dindex = open_table.entry[handle].mdir_index;
 
  /* Get current file size */
  fsize = fs_dir_get_fsize(dindex);

  /* Verify that file is currently open and that seek position */
  /* is valid (validity depends on access mode)                */
  access_mode = open_table.entry[handle].fmode;
  if (access_mode == FSI_MODE_APPEND)
  {
    /* Append access: prohibit seeking to any point within the */
    /* file's original extent                                  */
    original_fsize = open_table.entry[handle].orig_size;

    if (end_pos < original_fsize)
    {
      ERR("Invalid fmemset file position",0,0,0);
      return FS_ILLEGAL_OPERATION_S;
    }
  }
  else if (access_mode != FSI_MODE_UPDATE)
  {
    /* File is not currently open in correct access mode, return error */
    MSG_HIGH("File not open for update access",0,0,0);
    return FS_FILE_NOT_OPEN_S;
  }

  /* Write the specified byte value from current file pointer position */
  /* up to specified end position */
  length = end_pos - open_table.entry[handle].fp;
  status = fs_dir_fmemset(dindex,
                          set_char,
                          open_table.entry[handle].fp,
                          &length);
  /* Update file pointer position (yes, even if status is bad) */
  open_table.entry[handle].fp += length;
  if (status != FS_OKAY_S) 
  {
    return status;
  }

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif
 
  return  FS_OKAY_S;
} /* fs_ops_fmemset */




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
)
{
  word                      dindex;
  int                       dindex_int;
  fsi_filename_type         dirname_internal;
  boolean                   found;
  fsi_current_pointer_type  found_pos;
  fs_status_type            status;


  /* Find master file directory entry for the system file $USER_DIRS */
  dindex_int = fs_udir_find_udir_dindex();
  dindex     = (word)dindex_int;

  /* Convert directory name to internal format */
  status = fsi_ops_convert_dirname(
               &inparm->parms.mkdir.dirname,&dirname_internal);

  if (status != FS_OKAY_S)
  {
    return status;
  }
  
  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif
    
  /* If a matching directory name already exists, return error */
  fs_udir_search_user_dirs(dindex,&dirname_internal,&found,&found_pos);
  if (found)
  {
    MSG_HIGH("Directory already exists",0,0,0);
    return FS_DIR_ALREADY_EXISTS_S;
  }
  else
  {
    /* Add this new directory name to end of user directory file */
    status = fs_udir_add_user_dirs(dindex,&dirname_internal);
  }
  return status;

} /* fs_ops_mkdir */
  
  


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
)
{
  word                      dindex;
  int                       dindex_int;
  fsi_filename_type         dirname_internal;
  boolean                   found;
  fsi_current_pointer_type  found_pos;
  word                      num_files;
  fs_status_type            status;


  /* Find master file directory entry for the system file $USER_DIRS */
  dindex_int = fs_udir_find_udir_dindex();
  dindex     = (word)dindex_int;

  /* Convert directory name to internal format */
  status = fsi_ops_convert_dirname(
               &inparm->parms.rmdir.dirname,&dirname_internal);

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif

  /* If a matching directory name doesn't exist, return error */
  fs_udir_search_user_dirs(dindex,&dirname_internal,&found,&found_pos);
  if (!found)
  {
    MSG_HIGH("Directory doesn't exist",0,0,0);
    status = FS_NONEXISTENT_DIR_S;
  }
  else
  {
    /* Verify that the directory is empty */
    num_files = fs_udir_get_ndirs(&dirname_internal);
    if (num_files != 0)
    {
      MSG_HIGH("Directory not empty",0,0,0);
      status = FS_DIR_NOT_EMPTY_S;
    }
    else
    {
      num_files = fs_dir_get_nfiles(&dirname_internal);
      if (num_files != 0) 
      {
        MSG_HIGH("Directory not empty",0,0,0);
        status = FS_DIR_NOT_EMPTY_S;
      }
      else 
      {
      
        /* Remove the specified directory name from the user directory */
        /* file and compress remaining entries                         */
        status = fs_udir_remove_user_dirs(dindex,
                                          &dirname_internal,
                                          found_pos);
      }
    }
  }
  return status;

} /* fs_ops_rmdir */




/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_NAMETEST

DESCRIPTION
  Test for the existence of a directory or file with the specified name. 
  Return TRUE if it already exists and FALSE otherwise.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE      - if directory of file name was found
  FALSE     - otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean  fs_ops_nametest
(
  fs_op_request_type        *inparm
)
{
  boolean                   found;
  fsi_current_pointer_type  found_pos;
  fsi_filename_type         filename_internal;
  fs_status_type            status;
  word                      dindex;
  int                       dindex_int;

  if (inparm->parms.nametest.type != FS_TEST_DIR) 
  {
    /* Convert filename to internal format */
    status = fsi_ops_convert_filename(
                 &inparm->parms.nametest.filename,&filename_internal);
    if (status != FS_OKAY_S)
    {
      return FALSE;
    }
    if (fs_dir_find_filename_index(&filename_internal) < 0) 
    {
      /* No file exists with this name */
      if (inparm->parms.nametest.type == FS_TEST_FILE) {
        return FALSE;
      }
      else {
        /* Nametest a file or a directory, check direcotories */
        status = fsi_ops_convert_dirname(
                     &inparm->parms.nametest.filename,&filename_internal);
        if (status != FS_OKAY_S)
        {
          return FALSE;
        }

        dindex_int = fs_udir_find_udir_dindex();
        dindex     = (word)dindex_int;
        fs_udir_search_user_dirs(dindex,&filename_internal,&found,&found_pos);
        if (!found)
        {
          return FALSE;
        }
      }
    }
      return TRUE;
  }
  else
  {
    /* Nametest a directory */
    /* Convert dirname to internal format */
    status = fsi_ops_convert_dirname(
                 &inparm->parms.nametest.filename,&filename_internal);
    if (status != FS_OKAY_S)
    {
      return FALSE;
    }
     
    dindex_int = fs_udir_find_udir_dindex();
    dindex     = (word)dindex_int;
    fs_udir_search_user_dirs(dindex,&filename_internal,&found,&found_pos);
    if (!found)
    {
      return FALSE;
    }
    else return TRUE;
  }
  
} /* fs_ops_nametest */




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
)
{
  fsi_filename_type         filename;
  word                      dindex;
  int                       dindex_int;
  word                      tlog_index;
  fsi_access_mode_type      access_mode;
  fs_status_type            status;
 

  /* Convert filename to internal format */
  status = fsi_ops_convert_filename(
               &inparm->parms.set_attribs.filename,&filename);
  if (status != FS_OKAY_S)
  {
    return status;
  }

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif

  /* Find file name in master file directory */
  dindex_int = fs_dir_find_filename_index(&filename);
  dindex     = (word)dindex_int;

  if (dindex_int >= 0)
  {
    /* File must must be closed to change it's attributes */
    if (fsi_ops_file_is_open(dindex,&tlog_index,&access_mode))
    {
      MSG_HIGH("Cannot change an open file's attributes",0,0,0);
      status = FS_FILE_OPEN_S;
    }
    else
    {
      /* Set file's new attributes */
      status = fs_dir_set_file_attribs(
                   dindex,
                   inparm->parms.set_attribs.attributes,
                   inparm->parms.set_attribs.creation_date,
                   inparm->parms.set_attribs.buffering_option,
                   inparm->parms.set_attribs.cleanup_option);
    }
  }
  else
  {
    MSG_HIGH("File not found",0,0,0);
    status = FS_NONEXISTENT_FILE_S;
  }
  return status;
} /* fs_ops_set_attributes */




/* <EJECT> */
/*===========================================================================

FUNCTION FS_OPS_GET_ATTRIBUTES

DESCRIPTION
  Return the file attributes of a specified file.

  Input parameter definition:

    fs_op_request_type     *inparm
  
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
)
{
  fsi_filename_type         filename;
  fs_handle_type            fhandle;
  word                      dindex;
  int                       dindex_int;
  fs_status_type            status;
  

  /* If input file handle is NULL, then use file name; otherwise, */
  /* use the file handle                                          */
  fhandle = inparm->parms.get_attribs.fhandle;
  if (fhandle == FS_NULL_HANDLE)
  {
    /* Convert filename to internal format */
    status = fsi_ops_convert_filename(
                 &inparm->parms.get_attribs.filename,&filename);
    if (status != FS_OKAY_S)
    {
      return status;
    }

    /* Find file name in master file directory */
    dindex_int = fs_dir_find_filename_index(&filename);
    dindex     = (word)dindex_int;

    #ifdef FS_FEATURE_UNIT_TEST
#error code not present
    #endif
  }
  else
  {
    /* Get master file directory index from open files table */
    dindex     = open_table.entry[fhandle].mdir_index;
    dindex_int = (int)dindex;

    #ifdef FS_FEATURE_UNIT_TEST
#error code not present
    #endif
  }

  if (dindex_int >= 0)
  {
    /* File found, get it's current attributes */
    status = fs_dir_get_file_attribs(
                 dindex,
                 &inparm->parms.get_attribs.attributes,
                 &inparm->parms.get_attribs.creation_date,
                 &inparm->parms.get_attribs.buffering_option,
                 &inparm->parms.get_attribs.cleanup_option);
  }
  else
  {
    MSG_HIGH("File not found",0,0,0);
    status = FS_NONEXISTENT_FILE_S;
  }
  return status;
} /* fs_ops_get_attributes */



   
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
)
{
  word                      dindex;
  int                       dindex_int;
  fsi_filename_type         dirname_internal;
  boolean                   found;
  fsi_current_pointer_type  found_pos;
  fs_status_type            status;


  /* Find master file directory entry for the system file $USER_DIRS */
  dindex_int = fs_udir_find_udir_dindex();
  dindex     = (word)dindex_int;
 
  /* Convert directory name to internal format */
  status = fsi_ops_convert_dirname(
               &inparm->parms.num_files.dirname,&dirname_internal);

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif
  
  if (status != FS_OKAY_S) return status;
  
  if (strlen(inparm->parms.num_files.dirname)) {
    /* Find matching directory name */
    fs_udir_search_user_dirs(dindex,&dirname_internal,&found,&found_pos);
  }
  else {
    /* The root directory always exists */
    found = TRUE;
  }
  if (!found)
  {
    MSG_HIGH("Directory doesn't exist",0,0,0);
    inparm->parms.num_files.num_files = 0;
    status = FS_NONEXISTENT_DIR_S;
  }
  else
  {
    /* Get number of files in the directory */
    inparm->parms.num_files.num_files = 
        fs_dir_get_nfiles(&dirname_internal);
    status = FS_OKAY_S;
  }
  return status;

} /* fs_ops_get_file_count */



   
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
  Status        -- if user directory does not exist
  
SIDE EFFECTS
  None.

===========================================================================*/
fs_status_type  fs_ops_get_dir_count
(
  fs_op_request_type        *inparm
)
{
  word                      dindex;
  int                       dindex_int;
  fsi_filename_type         dirname_internal;
  boolean                   found;
  fsi_current_pointer_type  found_pos;
  fs_status_type            status;


  /* Find master file directory entry for the system file $USER_DIRS */
  dindex_int = fs_udir_find_udir_dindex();
  dindex     = (word)dindex_int;
 
  /* Convert directory name to internal format */
  status = fsi_ops_convert_dirname(
               &inparm->parms.num_dirs.dirname,&dirname_internal);

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif

  if (status != FS_OKAY_S) return status;

  if (strlen(inparm->parms.num_dirs.dirname)) {
    /* Find matching directory name */
    fs_udir_search_user_dirs(dindex,&dirname_internal,&found,&found_pos);
  }
  else {
    /* The root directory always exists */
    found = TRUE;
  }
  if (!found)
  {
    MSG_HIGH("Directory doesn't exist",0,0,0);
    inparm->parms.num_dirs.num_dirs = 0;
    status = FS_NONEXISTENT_DIR_S;
  }
  else
  {
    inparm->parms.num_dirs.num_dirs = fs_udir_get_ndirs(&dirname_internal);
  }
  return status;

} /* fs_ops_get_dir_count */




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
)
{
  word                      dindex;
  int                       dindex_int;
  fsi_filename_type         dirname_internal;
  size_t                    length;
  boolean                   found;
  fsi_current_pointer_type  found_pos;
  dword                     buf_length = (dword)0;
  fs_status_type            status     = FS_OKAY_S;


  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif
  

  /* Validate input directory name */
  if (inparm->parms.list_dirs.dirname == NULL)
  {
    dirname_internal.length = 0;
  }
  else 
  {
    length = strlen(inparm->parms.list_dirs.dirname);
    if (length == 0)
    {
      dirname_internal.length = 0;
    }
    else
    {
      if ((length+3) > FS_FILENAME_MAX_LENGTH)
      {
        ERR("Directory name too long",0,0,0);
        return FS_BAD_FILE_NAME_S;
      }    
      else
      {
        /* Convert directory name to internal format */
        memcpy((byte *)&dirname_internal.name,
               (byte *)inparm->parms.list_dirs.dirname,
               length);
        /* Add directory terminator */
        dirname_internal.name[length]   = FS_DIRECTORY_CHAR;
        dirname_internal.name[length+1] = 0x00;
        dirname_internal.length = (length+2);
      }
    }
  }

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif
 

  /* Find master file directory entry for the system file $USER_DIRS */
  dindex_int = fs_udir_find_udir_dindex();
  dindex     = (word)dindex_int;
  
  /* Find matching directory name */
  if (dirname_internal.length == 0)
  {
    /* List directories from root */
    found = TRUE;
  }
  else
  {
    fs_udir_search_user_dirs(dindex,&dirname_internal,&found,&found_pos);
    if (!found)
    {
      MSG_HIGH("Directory doesn't exist",0,0,0);
      status = FS_NONEXISTENT_DIR_S;
    }
  }

  if (found)
  {
    buf_length = inparm->parms.list_dirs.length;
    fs_udir_list_user_dirs(dindex,
                           &dirname_internal,
                           inparm->parms.list_dirs.buf,
                           &buf_length);
  }

  /* Set number of bytes of user buffer actually filled */
  inparm->parms.list_dirs.length = buf_length;
  return status;

} /* fs_ops_list_dirs */



   
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
)
{
  fsi_filename_type           dname_internal;

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif


  /* Convert directory name to internal format */
  inparm->status = fsi_ops_convert_dirname(
                       &inparm->parms.enum_init.dirname,&dname_internal);
  if (inparm->status != FS_OKAY_S) return inparm->status;

  /* Initialize control block for this enumeration */
  inparm->parms.enum_init.iterator_ptr->length = dname_internal.length;
  (void)memcpy((byte *)inparm->parms.enum_init.iterator_ptr->rootname,
               (byte *)&dname_internal.name,
               (size_t)dname_internal.length);
  inparm->parms.enum_init.iterator_ptr->enum_kind =
      inparm->parms.enum_init.enum_kind;
  inparm->parms.enum_init.iterator_ptr->fsequence = (dword)0;
  inparm->parms.enum_init.iterator_ptr->remote_file_processed_index = (word)0;
  return FS_OKAY_S;

} /* fs_ops_enum_init */




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
)
{
  fsi_filename_type           dname_internal;
  fsi_filename_type           dname_last;
  word                        length;

    
  /* Determine the next sequential file or subdirectory in the specified */
  /* root directory and return it's file information                     */
  dname_internal.length = inparm->parms.enum_next.iterator_ptr->length;
  if (dname_internal.length > FS_FILENAME_MAX_LENGTH)
  {
    ERR("Invalid enum iterator",0,0,0);
    return FS_INVALID_ITERATOR_S;
  }
  
  if (dname_internal.length == 0)
  {
    dname_internal.name[0] = 0x00;
  }
  else
  {
    (void)memcpy((byte *)dname_internal.name,
                 (byte *)inparm->parms.enum_next.iterator_ptr->rootname,
                 (size_t)dname_internal.length);
    dname_internal.name[dname_internal.length-1] = '\0';
  }

  if (inparm->parms.enum_next.iterator_ptr->enum_kind == FS_ENUM_DIRECTORIES)
  {
    /* Enumerate subdirectories */
    if (inparm->parms.enum_next.iterator_ptr->fsequence == 0) {
      /* First time initialization */
      inparm->parms.enum_next.iterator_ptr->rootname[
        inparm->parms.enum_next.iterator_ptr->length] = '\0';
      length = 0;
      dname_last.length = 0;
      dname_last.name[0] = 0x00;
    }
    else {
      length = strlen((char *) 
                      &(inparm->parms.enum_next.iterator_ptr->rootname[0]));
      (void)memcpy((byte *)&dname_last.name,
                   (byte *)inparm->parms.enum_next.iterator_ptr->rootname,
                   (size_t)length);
      dname_last.name[length]   = FS_DIRECTORY_CHAR;
      dname_last.name[length+1] = 0x00;
      dname_last.length = (length+2);
    }
    inparm->status = fs_udir_enum_next(
                         &dname_internal,
                         &dname_last,
                         &inparm->parms.enum_next.iterator_ptr->fsequence,
                         inparm->parms.enum_next.data_ptr);
    if (inparm->status == FS_OKAY_S) {
      memcpy(inparm->parms.enum_next.iterator_ptr->rootname,
             inparm->parms.enum_next.data_ptr->fullname,
             inparm->parms.enum_next.data_ptr->fullname_length);
      inparm->parms.enum_next.iterator_ptr->rootname[
        inparm->parms.enum_next.data_ptr->fullname_length] = '\0';
    }
  }
  else if (inparm->parms.enum_next.iterator_ptr->enum_kind == FS_ENUM_FILES)
  {
    /* Enumerate files */
    inparm->status = fs_dir_enum_next(
                         &dname_internal,
                         &inparm->parms.enum_next.iterator_ptr->fsequence,
                         &inparm->parms.enum_next.iterator_ptr->remote_file_processed_index,
                         inparm->parms.enum_next.data_ptr);
  }
  else
  {
    ERR("Invalid enum iterator",0,0,0);
    return FS_INVALID_ITERATOR_S;
  }
  return inparm->status;

} /* fs_ops_enum_next */




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
)
{
  fsi_filename_type           dname_internal;
  fsi_filename_type           dname_last;
  word                        length;

    
  /* Determine the next sequential file or subdirectory in the specified */
  /* root directory and return it's file information                     */
  dname_internal.length = inparm->parms.enum_prev.iterator_ptr->length;
  if (dname_internal.length > FS_FILENAME_MAX_LENGTH)
  {
    ERR("Invalid enum iterator",0,0,0);
    return FS_INVALID_ITERATOR_S;
  }
  
  if (dname_internal.length == 0)
  {
    dname_internal.name[0] = 0x00;
  }
  else
  {
    (void)memcpy((byte *)dname_internal.name,
                 (byte *)inparm->parms.enum_prev.iterator_ptr->rootname,
                 (size_t)dname_internal.length);
    dname_internal.name[dname_internal.length-1] = '\0';
  }

  if (inparm->parms.enum_prev.iterator_ptr->enum_kind == FS_ENUM_DIRECTORIES)
  {
    /* Enumerate subdirectories */
    length = strlen((char *) 
                    &(inparm->parms.enum_prev.iterator_ptr->rootname[0]));
    (void)memcpy((byte *)&dname_last.name,
                 (byte *)inparm->parms.enum_prev.iterator_ptr->rootname,
                 (size_t)length);
    dname_last.name[length]   = FS_DIRECTORY_CHAR;
    dname_last.name[length+1] = 0x00;
    dname_last.length = (length+2);
    inparm->status = fs_udir_enum_prev(
                         &dname_internal,
                         &dname_last,
                         &inparm->parms.enum_prev.iterator_ptr->fsequence,
                         inparm->parms.enum_prev.data_ptr);
    if (inparm->status == FS_OKAY_S) {
      memcpy(inparm->parms.enum_prev.iterator_ptr->rootname,
             inparm->parms.enum_prev.data_ptr->fullname,
             inparm->parms.enum_prev.data_ptr->fullname_length);
      inparm->parms.enum_prev.iterator_ptr->rootname[
        inparm->parms.enum_prev.data_ptr->fullname_length] = '\0';
    }
  }
  else if (inparm->parms.enum_prev.iterator_ptr->enum_kind == FS_ENUM_FILES)
  {
    /* Enumerate files */
    inparm->status = fs_dir_enum_prev(
                         &dname_internal,
                         &inparm->parms.enum_prev.iterator_ptr->fsequence,
                         &inparm->parms.enum_prev.iterator_ptr->remote_file_processed_index,
                         inparm->parms.enum_prev.data_ptr);
  }
  else
  {
    ERR("Invalid enum iterator",0,0,0);
    return FS_INVALID_ITERATOR_S;
  }
  return inparm->status;

} /* fs_ops_enum_prev */




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
)
{
  word                      dindex;
  int                       dindex_int;
  fsi_filename_type         dirname_internal;
  size_t                    length;
  boolean                   found;
  fsi_current_pointer_type  found_pos;
  dword                     buf_length = (dword)0;
  fs_status_type            status     = FS_OKAY_S;

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif
  

  /* Validate input directory name */
  if (inparm->parms.list_files.dirname == NULL)
  {
    dirname_internal.length = 0;
  }
  else 
  {
    length = strlen(inparm->parms.list_files.dirname);
    if (length == 0)
    {
      dirname_internal.length = 0;
    }
    else
    {
      if ((length+3) > FS_FILENAME_MAX_LENGTH)
      {
        ERR("Directory name too long",0,0,0);
        return FS_BAD_FILE_NAME_S;
      }    
      else
      {
        /* Convert directory name to internal format */
        memcpy((byte *)&dirname_internal.name, 
               (byte *)inparm->parms.list_files.dirname,
               length);
        /* Add directory terminator */
        dirname_internal.name[length]   = FS_DIRECTORY_CHAR;
        dirname_internal.name[length+1] = 0x00;
        dirname_internal.length = (length+2);
      }
    }
  }

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif
 
  /* Find master file directory entry for the system file $USER_DIRS */
  dindex_int = fs_udir_find_udir_dindex();
  dindex     = (word)dindex_int;

  if (dirname_internal.length) {
    /* Find matching directory name */
    fs_udir_search_user_dirs(dindex,&dirname_internal,&found,&found_pos);
  }
  else {
    /* Root directory always exists */
    found = TRUE;
  }
  if (!found)
  {
    MSG_HIGH("Directory doesn't exist",0,0,0);
    status = FS_NONEXISTENT_DIR_S;
  }
  else
  {
    buf_length = inparm->parms.list_files.length;
    fs_dir_list_files(&dirname_internal,
                      (byte *)inparm->parms.list_files.buf,
                      &buf_length,
                      TRUE);
  }

  /* Set number of bytes of user buffer actually filled */
  inparm->parms.list_files.length = buf_length;
  return status;

} /* fs_ops_list_files */




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
)
{
  fsi_filename_type         filename;
  word                      dindex;
  int                       dindex_int;
  fs_status_type            status;
  boolean                   found;
  fsi_current_pointer_type  found_pos;
 
  
  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif


  /* Convert filename to internal format */
  status = fsi_ops_convert_filename(
               &inparm->parms.remote_link.filename,&filename);
  if (status != FS_OKAY_S)
  {
    return status;
  }

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif


  /* Check that file name isn't already in master file directory */
  dindex_int = fs_dir_find_filename_index(&filename);
  if (dindex_int >= 0)
  {
    MSG_HIGH("Remote file name already exists",0,0,0);
    return FS_FILE_ALREADY_EXISTS_S;
  }
  dindex = (word)dindex_int;

  /* Split off directory path from file name */
  fs_udir_split_filename(&filename,&path,&local_fname);

  /* Verify that file name doesn't contain illegal characters */
  if (strchr((char *)local_fname.name,FS_DIRECTORY_CHAR) != NULL)
  {
    ERR("File name contains directory separator character",0,0,0);
    return FS_BAD_FILE_NAME_S;
  }

  /* If path is non-NULL, verify that the path to the file exists */
  found = FALSE;
  if (path.length != 0)
  {
    dindex_int = fs_udir_find_udir_dindex();
    dindex     = (word)dindex_int;
    fs_udir_search_user_dirs(dindex,&path,&found,&found_pos);
  }

  if (found || (path.length == 0))
  {
    /* Create a link to the new remote file */
    status = fs_dir_create_file(
                 &filename,
                 FS_FA_REMOTE,
                 ((fs_file_date_type)0),
                 FS_OC_CLOSE,
                 FS_OB_ALLOW,
                 inparm->parms.remote_link.base_address,
                 inparm->parms.remote_link.length,
                 FSI_FILE_TYPE_USER,
                 &dindex);
  }

  return status;
} /* fs_ops_remote_link */




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
)
{
  fs_status_type              status;

  /* Remove all remote file link entries and delete the remote links */
  /* system file                                                     */
  status = fs_dir_remove_remote_files();

  return status;
} /* fs_ops_remove_remote_links */




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
)
{
  word                      dindex;
  int                       dindex_int;
  fsi_filename_type         filename_internal;
  fsi_attribute_mask_type   attribute_mask;
  fsi_access_mode_type      access_mode;
  word                      tlog_index;
  fs_status_type            status;

  /* Can not remove $USER_DIRS or $SYS_RMT */
  if ((strcmp(filename, "$USER_DIRS") == 0) ||
      (strcmp(filename, "$SYS_RMT") == 0))
  {
     ERR("Can not remove system file",0,0,0);
     return FS_ILLEGAL_OPERATION_S;
  }

  /* Convert filename to internal format */
  status = fsi_ops_convert_filename(&filename,&filename_internal);
  if (status != FS_OKAY_S)
  {
    return status;
  }

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif

  /* Find file name in master file directory */
  dindex_int = fs_dir_find_filename_index(&filename_internal);
  dindex     = (word)dindex_int;

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif

  if (dindex_int >= 0)
  {
    /* File must not have attribute PERMANENT and must not */
    /* currently be open                                   */
    attribute_mask = fs_dir_get_attribute_mask(dindex);
    if ((attribute_mask == FS_FA_PERMANENT)     ||
        (attribute_mask == FS_FA_SYS_PERMANENT) ||
        (attribute_mask == FS_FA_READONLY)      ||
        (attribute_mask == FS_FA_REMOTE))
    {
      /* File's attribute mask prohibits truncation or removal */
      ERR("Attribute mask prohibits operation",0,0,0);
      return FS_ILLEGAL_OPERATION_S;
    }
    if (fsi_ops_file_is_open(dindex,&tlog_index,&access_mode))
    {
      /* File is currently open */
      ERR("Cannot truncate or remove open file",0,0,0);
      return FS_ILLEGAL_OPERATION_S;
    }

    /* Remove the file */
    status = fs_dir_remove_file(dindex);
  }
  else
  {
    MSG_HIGH("File doesn't exist",0,0,0);
    status = FS_NONEXISTENT_FILE_S;
  }

  return status;
} /* fs_ops_remove_file */




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
)
{
  word                        dindex;
  int                         dindex_int;
  fs_op_request_type          op;
  fs_handle_type              handle;
  fsi_access_mode_type        access_mode;
  static fsi_filename_type    old_filename_internal;
  static fsi_filename_type    new_filename_internal;
  dword                       space_needed;
  word                        tlog_index;
  fsi_attribute_mask_type     attribute_mask;
  fs_status_type              status;


  /* Convert both filenames to internal format */
  status = fsi_ops_convert_filename(&old_filename,&old_filename_internal);
  if (status != FS_OKAY_S) return status;
  status = fsi_ops_convert_filename(&new_filename,&new_filename_internal);
  if (status != FS_OKAY_S) return status;

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif

  /* The new file name must not already be used */
  dindex_int = fs_dir_find_filename_index(&new_filename_internal);

  if (dindex_int >= 0)
  {
    ERR("File name in use",0,0,0);
    return FS_FILE_ALREADY_EXISTS_S;
  }   

  /* Find old file name in master file directory */
  dindex_int = fs_dir_find_filename_index(&old_filename_internal);
  dindex     = (word)dindex_int;

  #ifdef FS_FEATURE_UNIT_TEST
#error code not present
  #endif

  if (dindex_int >= 0)
  {
    /* File must not be PERMANENT or REMOTE and must not */
    /* currently be open                                 */
    attribute_mask = fs_dir_get_attribute_mask(dindex);
    if ((attribute_mask == FS_FA_PERMANENT)     ||
        (attribute_mask == FS_FA_SYS_PERMANENT) ||
        (attribute_mask == FS_FA_REMOTE))
    {
      /* File's attribute mask prohibits rename */
      ERR("Attribute mask prohibits operation",0,0,0);
      return FS_ILLEGAL_OPERATION_S;
    }
    if (fsi_ops_file_is_open(dindex,&tlog_index,&access_mode))
    {
      /* File is currently open */
      ERR("Cannot rename open file",0,0,0);
      return FS_ILLEGAL_OPERATION_S;
    }

    /* Verify that enough available file system space exists to do  */
    /* the rename.  If both file names are the same length, only    */
    /* one block is required; if the names have different lengths,  */
    /* however, enough space must be available to temporarily store */
    /* a copy of the entire file.                                   */
    if (old_filename_internal.length == new_filename_internal.length)
    {
      space_needed = (dword)FSI_BLK_DATA_SIZE;
      if (space_needed > fs_ops_space_available())
      {
        return FS_SPACE_EXHAUSTED_S;
      }

      /* Rename the file by writing only a new file header block.     */
      /* Since at most one block can be affected, it isn't necessary  */
      /* to write a transaction log first (the natural block checksum */
      /* mechanism will insure operation integrity).                  */
      status = fs_dir_rename_file(
                   &old_filename_internal,&new_filename_internal);
    }
    else
    {
      /* Old and new file names have different lengths, must copy file */
      space_needed = fs_dir_get_fsize(dindex);
      if (space_needed > fs_ops_space_available())
      {
        return FS_SPACE_EXHAUSTED_S;
      }

      /* To insure a safe rename operation, write a transaction log */
      /* before doing the physical updates                          */
      status = fsi_ops_write_tlog(FSI_TLOG_TYPE_RENAME,
                                  dindex,
                                  &old_filename_internal,
                                  &new_filename_internal,
                                  &tlog_index);
      if (status != FS_OKAY_S)
      {
        /* No room for transaction log, file cannot currently be renamed */
        status = FS_SPACE_EXHAUSTED_S;
      }
      else
      {
        /* Get attributes of existing file */
        status = fs_dir_get_file_attribs(
                     dindex,
                     &op.parms.open.option.create.attribute_mask,
                     &op.parms.open.option.create.creation_date,
                     &op.parms.open.option.create.buffering_option,
                     &op.parms.open.option.create.cleanup_option);
        if (status != FS_OKAY_S)
        {
          ERR_FATAL("Couldn't get file attributes",0,0,0);
        }

        /* Create new file */
        op.op                                 = FS_OPEN_OP;
        op.parms.open.access_kind             = FS_OA_CREATE;
        op.parms.open.option.create.filename  = new_filename;
        op.parms.open.option.create.file_type = FSI_FILE_TYPE_USER;
        handle = fsi_ops_open_create(&op,&new_filename_internal);
 
        if (handle == FS_NULL_HANDLE)
        {
          ERR("File rename failed",0,0,0);
          status = FS_FAIL_S;
        }
        else
        {    
          /* Rename the file by copying */
          status = fs_dir_copy_file(
                       &old_filename_internal,
                       (fs_file_position_type)0,
                       &new_filename_internal,
                       (fs_file_position_type)0);
          if (status == FS_OKAY_S)
          {
            /* Close the new file */
            op.parms.close.fhandle = handle;
            if (fs_ops_close(&op) != FS_OKAY_S)
            {
              ERR_FATAL("Couldn't close copied file",0,0,0);
            }
          
            /* Delete the old file */
            status = fs_ops_remove_file(old_filename);
            if (status != FS_OKAY_S)
            {
              ERR_FATAL("Couldn't remove old file",0,0,0);
            }
          }
        }
      }
      /* Remove the transaction log */
      (void)fs_ops_remove_tlog(tlog_index);

      /* Also remove file's sblkptr list */
      fs_dir_invalidate_sblkptr(dindex);
      fs_dir_set_tlog_index(dindex,FSI_TLOG_INDEX_NULL);
    }
  }
  else
  {
    ERR("File doesn't exist",0,0,0);
    status = FS_NONEXISTENT_FILE_S;
  }

  return status;
} /* fs_ops_rename_file */




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
  boolean               for_revertable_file,
  fs_rsvr_request_type  rsvr_request
)
{
  int                   i;
  word                  isave;
  word                  rsvd_blk_count;
  word                  free_blk_count;
  word                  num_blks_to_reserve;
  fs_conf_code_type     conf_code = {FS_NULL_CONF_CODE,(dword)0};


  /* Disable interrupts and save PSW */
  INTLOCK_SAV(isave);

  /* Check if request is for a revertable file */
  if (for_revertable_file)
  {
    /* This reservation is intended for a revertable file, so reserve   */
    /* addional space for the file's transaction log                    */
    num_blks_to_reserve = 
        FS_NUM_BLKS(rsvr_request.revertable_file_rsvr.amount) +
        FS_NUM_BLKS(rsvr_request.revertable_file_rsvr.fsize /
                    (FSI_BLK_DATA_SIZE / FSI_FBL_NODE_DATA_SIZE));
  }
  else
  {
    /* This reservation is intended for a normal (i.e., non-revertable) */
    /* file, so only need to reserve the requested amount of space      */
    num_blks_to_reserve = 
        FS_NUM_BLKS(rsvr_request.normal_file_rsvr.amount);
  }

  /* Reservation must be for a non-zero amount of space */
  if (num_blks_to_reserve != 0)
  {
    /* Get number of blocks currently free and currently already reserved */
    free_blk_count = fs_alloc_get_freeblk_count();
    rsvd_blk_count = fs_alloc_get_rsvdblk_count();

    /* Always reserve free blocks to do safe block updates */
    if (free_blk_count > (FS_GC_CLEAN_RSVD + FSI_TLOG_RSVD))
    {
      free_blk_count -= (FS_GC_CLEAN_RSVD + FSI_TLOG_RSVD);

      /* Reduce free block count by the amount of already reserved space */
      if (free_blk_count > rsvd_blk_count)
      {
        free_blk_count -= rsvd_blk_count;
        
        if (free_blk_count >= num_blks_to_reserve)
        {
          /* Enough space exists to satisfy the request: generate a */
          /* confirmation code and place a copy in the reservations */
          /* ledger at the first unused entry in the ledger         */
          for (i=0; i<FS_MAX_RESERVATIONS; i++)
          {
            if (rsvr_ledger.entry[i].conf_code.code == FS_NULL_CONF_CODE)
            {
              /* Reserve the space */
              fs_alloc_set_rsvdblk_count(rsvd_blk_count+num_blks_to_reserve);

              /* Generate confirmation code */
              conf_code.count = num_blks_to_reserve * FSI_BLK_DATA_SIZE;
              conf_code.code  = (dword)(((dword)i<<16) |
                                (dword)(conf_code.count&0x0000FFFF));

              /* Save copy of confirmation code in reservations ledger */
              rsvr_ledger.entry[i].conf_code.code  = conf_code.code;
              rsvr_ledger.entry[i].conf_code.count = conf_code.count;
              break;
            }
          }
        }
      }
    } 
  }
  INTFREE_SAV(isave);

  return conf_code;
} /* fs_ops_reserve_space */





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
  fs_conf_code_type     conf_code
)
{
  int                   i;
  word                  isave;
  word                  blk_count;
  word                  conf_blk_count;


  /* Validate confirmation code */
  i = (int)((conf_code.code>>16)&0x000000FF);
  if ( ((i<0) || (i>=FS_MAX_RESERVATIONS)) ||
       (rsvr_ledger.entry[i].conf_code.code  != conf_code.code) ||
       (rsvr_ledger.entry[i].conf_code.count != conf_code.count))
  {
    ERR("Invalid confirmation code",0,0,0);
    return FS_INVALID_CONF_CODE_S;
  }

  /* Disable interrupts and save PSW */
  INTLOCK_SAV(isave);

  /* Adjust number of blocks currently reserved */
  conf_blk_count = FS_NUM_BLKS(conf_code.count);
  blk_count = fs_alloc_get_rsvdblk_count() - conf_blk_count;
  fs_alloc_set_rsvdblk_count(blk_count);
  
  /* Clear the confirmation code in the reservations ledger */
  rsvr_ledger.entry[i].conf_code.code  = FS_NULL_CONF_CODE;
  rsvr_ledger.entry[i].conf_code.count = (dword)0;

  INTFREE_SAV(isave);

  return FS_OKAY_S;
} /* fs_ops_cancel_reservation */





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
  fs_conf_code_type     conf_code
)
{
  int                   i;


  /* Validate confirmation code */
  i = (int)((conf_code.code>>16)&0x000000FF);
  if ( ((i>=0) && (i<FS_MAX_RESERVATIONS)) &&
       (rsvr_ledger.entry[i].conf_code.code  == conf_code.code) &&
       (rsvr_ledger.entry[i].conf_code.count == conf_code.count))
  {
    return FALSE;  /* Reservation is not Null */
  }
  else
  {
    return TRUE;   /* Reservation is Null (i.e., invalid) */
  }
} /* fs_ops_reservation_is_null */





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
)
{
  int                   free_blk_count;

  free_blk_count = (int)fs_alloc_get_freeblk_count();

  /* Subtract number of blocks reserved to guarantee safe updates */
  free_blk_count -= (FS_GC_CLEAN_RSVD + FSI_TLOG_RSVD);
  
  /* Subtract number of reserved blocks */
  free_blk_count -= fs_alloc_get_rsvdblk_count();

  /* Subtract constant user-defined offset amount */
  free_blk_count -= FS_SPACE_REPORT_OFFSET;

  return ((free_blk_count > 0) ? 
             ((dword)free_blk_count * (dword)FSI_BLK_DATA_SIZE) :
             (dword)0);

} /* fs_ops_space_available */





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
)
{
  word                  erased_blk_count;
  word                  rsvd_blk_count;

  erased_blk_count = fs_alloc_get_cleanblk_count();

  /* Adjust count to reflect saved blocks for garbage collection */
  if (erased_blk_count > (2*fs_dir_get_largest_sector()+(2*FS_GC_CLEAN_RSVD)))
  {
    /* Subtract number of blocks reserved to guarantee safe updates */
    erased_blk_count -= (2*fs_dir_get_largest_sector()+(2*FS_GC_CLEAN_RSVD)
                         +(2*FSI_TLOG_RSVD));

    /* Subtract number of reserved blocks */
    rsvd_blk_count = fs_alloc_get_rsvdblk_count();
    if (erased_blk_count > rsvd_blk_count)
    {
      erased_blk_count -= rsvd_blk_count;
      return ((dword)erased_blk_count * (dword)FSI_BLK_DATA_SIZE);
    }
  }
  return (dword)0;

} /* fs_ops_space_erased */





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
)
{
  int                   blk_count;

  blk_count = (int)fs_alloc_get_totalblk_count() - 
              (fs_alloc_get_freeblk_count() + FS_GC_CLEAN_RSVD + FSI_TLOG_RSVD);
  if (blk_count < 0) {
    return (dword)0;
  }
  return ((dword)blk_count * (dword)FSI_BLK_DATA_SIZE);

} /* fs_ops_space_used */





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
)
{
  word                  blk_count;

  blk_count = fs_alloc_get_rsvdblk_count();
  return ((dword)blk_count * (dword)FSI_BLK_DATA_SIZE);

} /* fs_ops_space_reserved */


/***********************************************************************
 * FS_OPS_STATFS
 *
 * Perform statfs operation.
 */
fs_status_type
fs_ops_statfs (fs_op_request_type *inparm)
{
  statfs_info *info = inparm->parms.statfs.info;
  int free_blk_count;

  free_blk_count = (int) fs_alloc_get_freeblk_count ();
  free_blk_count -= (FS_GC_CLEAN_RSVD + FSI_TLOG_RSVD);
  free_blk_count -= fs_alloc_get_rsvdblk_count ();
  free_blk_count -= FS_SPACE_REPORT_OFFSET;

  if (free_blk_count < 0)
    free_blk_count = 0;

  info->f_type = STATFS_TYPE_FLASH;
  info->f_bsize = FSI_BLK_DATA_SIZE;
  info->f_blocks = (dword) fs_alloc_get_totalblk_count ();
  info->f_bfree = free_blk_count;
  info->f_bavail = free_blk_count;
  info->f_files = FS_STATFS_UNKNOWN;
  info->f_ffree = FS_STATFS_UNKNOWN;
  info->f_fsid = FS_STATFS_UNKNOWN;

  /* This isn't quite right since this is the max length of a path.  Flash
   * EFS doesn't have real directories, though. */
  info->f_namelen = FS_FILENAME_MAX_LENGTH_P;

  return FS_OKAY_S;
}


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
)
{
  fs_status_type        status;
  word                  num_sectors;
  word                  i;

  num_sectors = fs_dir_get_num_sectors();
  for (i=0; i<num_sectors; i++)
  {
    /* Erase sector */
    status = fs_dev_erase_start(i);
    /* Poll for completion */
    while (fs_dev_erase_status() == FS_GC_IN_PROGRESS_S) 
    {
      ;
    }
    /* Clear sector status entries for this sector */
    fs_alloc_gc_end(i);
  }
  return;

} /* fs_ops_master_reset */
#endif /* FEATURE_FS_ALLOW_MASTER_RESET */
#endif /* FEATURE_EFS_FLASH */
#endif /* FEATURE_EFS */
