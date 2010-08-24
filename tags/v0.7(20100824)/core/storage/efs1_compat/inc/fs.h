#ifndef FS_H
#define FS_H
/*==========================================================================

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

Copyright (c) 1999-2001,2003,2005,2008 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/fs.h_v   1.6   12 Jun 2002 17:43:12   davidb  $
$Header: //source/qcom/qct/core/storage/efs1_compat/main/latest/inc/fs.h#5 $ $DateTime: 2008/09/22 12:51:29 $ $Author: wkimberl $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09-22-08   wek     Fixed end-of-line missing at end of file while compiling.
08-27-08   wek     Added deprecation comments.
09-13-05   sh      Revert fs_handle_type from dword back to word.
10-24-03   pg      Added support for FS_FORMAT_OP to format MMC card.
09-30-03   pg      Added error codes FS_BAD_FORMAT_MEDIA_S and 
                   FS_INVALID_MEDIA_S.
12-05-01   dlb     Added fs_opened_file_size().
09-10-01   dlb     Add fs_statfs() call.
11-09-00   pg      Initial version for common archive.
11-06-00   dlb     Added FS_NO_MEDIA_S, and FS_MEDIA_CHANGED_S.
05-12-00   dlb     Added FEATURE_EFS_ACCESS_METHODS and code.
05-12-00   dlb     Added fs_enum_finish().
04-25-00   pg      Fixed the problem that fs_test() could not properly
                   distinguish a file from a directory:
                   Replaced function fs_test() with fs_nametest(), and
                   defined a MACRO fs_test to keep the old interface.
04-11-00   pg      Added status for exceeding maximum remote links allowed.
03-24-00   pg      Added fs_enum_prev() function and support for it.
03-16-00   pg      Added fs_seek_read(), fs_seek_write() functions and 
                   supports for them.
02-23-00   rp      Add response message for ANY operation.
02-14-00   pg      Changed struct fs_enum_iterator_type -
                   added word remote_file_processed_index -
                   to support fs_enum_next() on remote links.
02-02-00   rp      Moved new status values associted with fs_enum functions
                   to end of fs_status_type so all existing status values
                   remain in sync with diag.
01-31-00   pg      Added fs_enum_init(), fs_enum_next() functions and 
                   supports for them.
01-21-00   pg      Added fs_rename function and support for it.
12-16-99   rp      Added fs_remove_remote_links function. 
08-13-99   rp      Picked up Lee's latest fixes.  
06-22-99   ls      Added status for full Open Files Table.
05-24-99   ls      Added SYS_PERMANENT attribute.
05-24-99   ls      Added system configuration parameters for client use.
05-20-99   ls      Added fmemset support.
04-25-99   ls      Added space_erased function.
04-22-99   ls      Removed enclosing ifdef.
04-21-99   ls      Version 2.2 release.
04-03-99   ls      Changed interface per Version 2.1 specification.
03-04-99   ls      Initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "fs_parms.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

Deprecated  constants, macros, types, variables and other items, please refer 
to fs_public.h for interface with the Embedded File System 2. 

===========================================================================*/


/* Deprecated Operation status values */
/* Do not move these values around as they must be in sysn with diag */
typedef enum {
  FS_OKAY_S,                /* Deprecated                                  */
  FS_FAIL_S,                /* Deprecated                                  */
  FS_BUSY_S,                /* Deprecated                                  */
  FS_FILE_NOT_OPEN_S,       /* Deprecated                                  */
  FS_FILE_OPEN_S,           /* Deprecated                                  */
  FS_FILE_ALREADY_EXISTS_S, /* Deprecated                                  */
  FS_NONEXISTENT_FILE_S,    /* Deprecated                                  */
  FS_DIR_ALREADY_EXISTS_S,  /* Deprecated                                  */
  FS_NONEXISTENT_DIR_S,     /* Deprecated                                  */
  FS_DIR_NOT_EMPTY_S,       /* Deprecated                                  */
  FS_UNKNOWN_OPERATION_S,   /* Deprecated                                  */
  FS_ILLEGAL_OPERATION_S,   /* Deprecated                                  */
  FS_PARAMETER_ERROR_S,     /* Deprecated                                  */
  FS_BAD_FILE_NAME_S,       /* Deprecated                                  */
  FS_BAD_FILE_HANDLE_S,     /* Deprecated                                  */
  FS_BAD_SEEK_POS_S,        /* Deprecated                                  */
  FS_BAD_TRUNCATE_POS_S,    /* Deprecated                                  */
  FS_FILE_IS_REMOTE_S,      /* Deprecated                                  */
  FS_INVALID_CONF_CODE_S,   /* Deprecated                                  */
  FS_EOF_S,                 /* Deprecated                                  */
  FS_MORE_S,                /* Deprecated                                  */
  FS_GC_IN_PROGRESS_S,      /* Deprecated                                  */
  FS_SPACE_EXHAUSTED_S,     /* Deprecated                                  */
  FS_BLK_FULL_S,            /* Deprecated                                  */
  FS_DIR_SPACE_EXHAUSTED_S, /* Deprecated                                  */
  FS_FBL_POOL_EXHAUSTED_S,  /* Deprecated                                  */
  FS_OPEN_TABLE_FULL_S,     /* Deprecated                                  */
  FS_INVALID_ITERATOR_S,    /* Deprecated                                  */
  FS_ENUM_DONE_S,           /* Deprecated                                  */
  FS_RMLINK_EXHAUSTED_S,    /* Deprecated                                  */
  FS_NO_MEDIA_S,            /* Deprecated                                  */
  FS_MEDIA_CHANGED_S,       /* Deprecated                                  */
  FS_BAD_FORMAT_MEDIA_S,    /* Deprecated                                  */
  FS_INVALID_MEDIA_S        /* Deprecated                                  */
} fs_status_type;


/* Deprecated system configuration parameters */

/* Deprecated maximum length of a file name */
#define  FS_FILENAME_MAX_LENGTH  FS_FILENAME_MAX_LENGTH_P

/* Deprecated directory separator character */
#define  FS_DIRECTORY_CHAR       FS_DIRECTORY_CHAR_P

/* Deprecated maximum number of files in the system */
#define  FS_MAX_NUM_FILES        FS_MAX_NUM_FILES_P

/* Deprecated maximum number of simultaneously "open" files in the system */
#define  FS_MAX_OPEN_FILES       FS_MAX_OPEN_FILES_P

/* Deprecated maximum number of outstanding file space reservations */
#define  FS_MAX_RESERVATIONS     FS_MAX_RESERVATIONS_P

/* Deprecated maximum number of command packets to the file system task */
#define  FS_MAX_CMD_PACKETS      FS_MAX_CMD_PACKETS_P

/* Deprecated */
#define HAVE_SFAT_MMC_DIAG_WORKAROUND 

/* Deprecated file handle type definition */
typedef  word   fs_handle_type;

/* Deprecated Null file handle */
#define  FS_NULL_HANDLE  0


/* Deprecated file SEEK operation origin */
typedef enum {
  FS_SEEK_SET,            /* Deprecated                                    */
  FS_SEEK_CURRENT,        /* Deprecated                                    */
  FS_SEEK_EOF             /* Deprecated                                    */
} fs_seek_origin_type;


/* Deprecated file OPEN operation access options */
typedef enum {
  FS_OA_CREATE,           /* Deprecated                                    */
  FS_OA_APPEND,           /* Deprecated                                    */
  FS_OA_READONLY,         /* Deprecated                                    */
  FS_OA_READWRITE,        /* Deprecated                                    */
  FS_OA_TRUNCATE,         /* Deprecated                                    */
  FS_OA_TEST              /* Deprecated                                    */
} fs_access_option_type;


/* Deprecated File OPEN operation clean-up options */
typedef enum {
  FS_OC_CLOSE     = 0x00, /* Deprecated Close file as-is                   */
  FS_OC_DISCARD   = 0x01, /* Deprecated Del file and rm it from directory  */
  FS_OC_TRUNCATE  = 0x02, /* Deprecated Truncate file from specified pos   */
  FS_OC_REVERT    = 0x03  /* Deprecated Revert to last checkpointed ver    */
} fs_cleanup_option_type;


/* Deprecated File OPEN operation buffering options */
typedef enum {
  FS_OB_PROHIBIT,         /* Deprecated Prohibit file buffering            */
  FS_OB_ALLOW             /* Deprecated Allow file buffering               */
} fs_buffering_option_type;


/* Deprecated File SET ATTRIBUTE MASK operation file attribute mask values */
typedef enum {
  FS_FA_UNRESTRICTED =0x00FF, /* Deprecated No file access restrictions    */
  FS_FA_READONLY     =0x00BF, /* Deprecated File cannot be opened for update access   */
  FS_FA_PERMANENT    =0x007F, /* Deprecated File cannot be removed nor truncated      */
  FS_FA_SYS_PERMANENT=0x0037, /* Deprecated File persists across system reformats     */
  FS_FA_REMOTE       =0x001F  /* Deprecated Remote file (resides outside the space    */
} fs_attribute_mask_type;     /* Deprecated   managed by the file system)             */


/* Deprecated File Creation Date */
typedef  dword  fs_file_date_type;


/* Deprecated File position for TRUNCATE, SEEK, and TELL */
typedef  dword  fs_file_position_type;


/* Deprecated Extended file Open parameters */

/* Deprecated OPEN-Create parameters */
typedef struct {
  fs_cleanup_option_type      cleanup_option;   /* Deprecated bad powerdown recovery */
  fs_buffering_option_type    buffering_option; /* Deprecated buffering allowed? */
  fs_attribute_mask_type      attribute_mask;   /* Deprecated attribute mask */
  dword                       base_address;     /* Deprecated For remote files only */
} fs_open_create_type;

/* Deprecated OPEN-Truncate parameters */
typedef struct {
  fs_file_position_type       position;         /* Deprecated position to truncate at */
} fs_open_truncate_type;

/* Deprecated OPEN extended parameters for "create" and "truncate" */
typedef union {
  fs_open_create_type         create;            
  fs_open_truncate_type       truncate;
} fs_open_xparms_type;


/* Deprecated Type for requesting reservation of file system space for normal files */
typedef struct {
  dword                       amount;
} fs_rsvr_normal_type;

/* Deprecated Type for reserving file system space for revertable files */
typedef struct {
  dword                       amount;
  dword                       fsize;
} fs_rsvr_revert_type;

typedef union {
  fs_rsvr_normal_type         normal_file_rsvr;
  fs_rsvr_revert_type         revertable_file_rsvr;
} fs_rsvr_request_type;

/* Deprecated Type for reservation confirmation codes */
typedef struct {
  dword                       code;
  dword                       count;
} fs_conf_code_type;

/* Deprecated A Null confirmation code */
#define  FS_NULL_CONF_CODE  ((dword)0)

/* Deprecated Starting offset of user data area within system-permanent files       */
#define  FS_SYS_PERM_DATA_OFFSET       0x78

/* Deprecated Starting offset of owner ID field within system-permanent files       */
#define  FS_SYS_PERM_OWNER_ID_OFFSET   FSI_BLK_OFFSET_OWNER_ID

/* Deprecated Maximum size in bytes of user data area within system-permanent files */
#define  FS_SYS_PERM_DATA_SIZE         128

/* Deprecated Specifies whether to enumerate files or directories */
typedef enum {
  FS_ENUM_FILES,                           /* Deprecated Enumerate files only */
  FS_ENUM_DIRECTORIES                      /* Deprecated Enumerate directories only */
} fs_enum_type;

/* Deprecated Control block for file and directory enumeration services */
typedef struct {
  fs_enum_type                enum_kind;/* Deprecated File or subdir enumeration */
  dword                       fsequence;/* Deprecated Current file sequence number */
  word                        remote_file_processed_index;   
                             /* Deprecated Number of remote linked processed */ 
  byte                        length;     /* Deprecated Rootname length */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type              am_handle; /* Deprecated Internal handle used by
                                              access methods. */
#endif
  byte                        rootname[FS_FILENAME_MAX_LENGTH];
} fs_enum_iterator_type;

/* Deprecated Returned data from ENUM_NEXT and ENUM_PREV operation */
typedef struct {
  byte                        fullname_length;
  byte                        dirname_length;
  char                        fullname[FS_FILENAME_MAX_LENGTH];
  fs_attribute_mask_type      attributes;
  fs_file_date_type           creation_date;
  fs_buffering_option_type    buffering_option;
  fs_cleanup_option_type      cleanup_option;
  dword                       logical_size;
  dword                       physical_size;
} fs_enum_data_type;

/* Deprecated Specifies whether to nametest a file or a directory */
typedef enum {
  FS_TEST_FILE_OR_DIR,               /* Deprecated Nametest a file or a dir */
  FS_TEST_FILE,                      /* Deprecated Nametest a file only */
  FS_TEST_DIR                        /* Deprecated Nametest a directory only */
} fs_nametest_type;

/* Deprecated Response message from OPEN operation */
typedef struct {
  fs_status_type              status;/* Deprecated Returned operation status */
  fs_handle_type              handle;/* Deprecated Returned file handle */
} fs_open_rsp_type;

/* Deprecated Response message from CLOSE operation */
typedef struct {
  fs_status_type              status;/* Deprecated Returned operation status */
} fs_close_rsp_type;

/* Deprecated Response message from READ operation */
typedef struct {
  fs_status_type              status; /* Deprecated Returned operation status */
  dword                       count;  /* Deprecated Number of bytes read */
} fs_read_rsp_type;

/* Deprecated Response message from WRITE operation */
typedef struct {
  fs_status_type              status;/* Deprecated Returned operation status */
  dword                       count; /* Deprecated Number of bytes written */
} fs_write_rsp_type;

/* Deprecated Response message from SET_ATTRIBS operation */
typedef struct {
  fs_status_type              status;/* Deprecated Returned operation status */
} fs_set_attribs_rsp_type;

/* Deprecated Response message from GET_ATTRIBS operation */
typedef struct {
  fs_status_type           status;/* Deprecated Returned operation status */
  fs_attribute_mask_type   attributes;       /* Deprecated Attribute mask */
  fs_file_date_type        creation_date;/* Deprecated Date of file creation */
  fs_buffering_option_type buffering_option;/* Deprecated Whether bufferable */
  fs_cleanup_option_type   cleanup_option;/* Deprecated Bad powerdown cleanup*/
} fs_get_attribs_rsp_type;

/* Deprecated Response message from FILE_SIZE operation */
typedef struct {
  fs_status_type              status;/* Deprecated Returned operation status */
  dword                       size;  /* Deprecated Size of file in bytes */
} fs_file_size_rsp_type;

/* Deprecated Response message from SEEK operation */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
} fs_seek_rsp_type;

/* Deprecated Response message from SEEK_READ operation */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
  dword                       count;  /*Deprecated Number of bytes read */
} fs_seek_read_rsp_type;

/* Deprecated Response message from SEEK_WRITE operation */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
  dword                       count;  /*Deprecated Number of bytes written */
} fs_seek_write_rsp_type;

/* Deprecated Response message from TELL operation */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
  fs_file_position_type       position;    /* Deprecated Current file pointer pos */
} fs_tell_rsp_type;

/* Deprecated Response message from TRUNCATE operation */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
} fs_truncate_rsp_type;

/* Deprecated Response message from RMFILE operation */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
} fs_rmfile_rsp_type;

/* Deprecated Response message from RENAME operation */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
} fs_rename_rsp_type;

/* Deprecated Response message from MKDIR operation */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
} fs_mkdir_rsp_type;

/* Deprecated Response message from RMDIR operation */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
} fs_rmdir_rsp_type;

/* Deprecated Response message from FMEMSET operation */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
} fs_fmemset_rsp_type;

/* Deprecated Response message from NAMETEST operation */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
  boolean                     name_found;  /* Deprecated Whether file name was found */
} fs_nametest_rsp_type;

/* Deprecated Response message from NUM_FILES operation */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
  word                        num_files;   /* Deprecated Number of files in directory */
} fs_num_files_rsp_type;

/* Deprecated Response message from NUM_DIRS operation */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
  word                        num_dirs;    /* Deprecated Number of subdirectories */
} fs_num_dirs_rsp_type;

/* Deprecated Response message from LIST_FILES operation */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
  dword                       count;  /*Deprecated # bytes written to buffer */
} fs_list_files_rsp_type;

/* Deprecated Response message from LIST_DIRS operation */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
  dword                       count;  /*Deprecated # bytes written to buffer */
} fs_list_dirs_rsp_type;

/* Deprecated Response message from ENUM_INIT operation */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
} fs_enum_init_rsp_type;

/* Deprecated Response message from ENUM_NEXT operation */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
} fs_enum_next_rsp_type;

/* Deprecated Response message from ENUM_PREV operation */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
} fs_enum_prev_rsp_type;

/* Deprecated Response message from ENUM_FINISH operation */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
} fs_enum_finish_rsp_type;

/* Deprecated Response message from REMOTE_LINK operation */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
} fs_remote_link_rsp_type;

/* Deprecated Response message from REMOVE_REMOTE_LINKS operation */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
} fs_rmrlink_rsp_type;

/* Deprecated Response message from RESERVED operation */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
} fs_reserved_rsp_type;

/* Deprecated Response message from STATFS operation.
 * Rather than worthlessly take up space in a union, the fs_statfs call
 * takes a pointer argument where the result will reside. */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
} fs_statfs_rsp_type;

/* Deprecated Response message from FORMAT operation */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
} fs_format_rsp_type;

/* Deprecated The actual information returned by statfs.  Any field that is 
not meaningful for a given filesystem type will be set to FS_STATFS_UNKNOWN. */
typedef struct {
  uint32        f_type;    /*Deprecated Type of filesystem, see below. */
  int32         f_bsize;   /*Deprecated Filesystem blocksize (if known). */
  int32         f_blocks;  /*Deprecated Total data blocks in filesystem. */
  int32         f_bfree;   /*Deprecated Free blocks in filesystem. */
  int32         f_bavail;  /*Deprecated Free blocks available to user use. */
  int32         f_files;   /*Deprecated Total number of file nodes in FS. */
  int32         f_ffree;   /*Deprecated Free file nodes in FS. */
  int32         f_fsid;    /*Deprecated File system ID. */
  int32         f_namelen; /*Deprecated Maximum filename length. */
} statfs_info;

/* Deprecated Any field whose value is unknown or not meaningful for a given
 * filesystem type will be filled in with the following. */
#define FS_STATFS_UNKNOWN       (-1)

/* Deprecated The following fields have meaning for the various filesystem types:
 *
 * Flash - f_type, f_bsize, f_blocks, f_bfree, f_bavail, f_namelen.
 * SFAT  - f_type, f_bsize, f_blocks, f_bfree, f_bavail, f_namelen.
 *
 * The other fields will be set to FS_STATFS_UNKNOWN. */

/* Deprecated Filesystem types that can be returned in f_type. */
#define STATFS_TYPE_FLASH       0x20534645
#define STATFS_TYPE_SFAT        0x54414653

/* Deprecated Response message for ANY operation */
typedef struct {
  fs_status_type              status; /*Deprecated Returned operation status */
} fs_op_rsp_type;

/* Deprecated Generic response message passed back via callback function */
typedef union {
  fs_open_rsp_type            open;
  fs_close_rsp_type           close;
  fs_read_rsp_type            read;
  fs_write_rsp_type           write;
  fs_set_attribs_rsp_type     set_attribs;
  fs_get_attribs_rsp_type     get_attribs;
  fs_file_size_rsp_type       file_size;
  fs_seek_rsp_type            seek;
  fs_seek_read_rsp_type       seek_read;
  fs_seek_write_rsp_type      seek_write;
  fs_tell_rsp_type            tell;
  fs_truncate_rsp_type        truncate;
  fs_rmfile_rsp_type          rmfile;
  fs_rename_rsp_type          rename;
  fs_mkdir_rsp_type           mkdir;
  fs_rmdir_rsp_type           rmdir;
  fs_fmemset_rsp_type         fmemset;
  fs_nametest_rsp_type        nametest;
  fs_num_files_rsp_type       num_files;
  fs_num_dirs_rsp_type        num_dirs;
  fs_list_files_rsp_type      list_files;
  fs_list_dirs_rsp_type       list_dirs;
  fs_enum_init_rsp_type       enum_init;
  fs_enum_next_rsp_type       enum_next;
  fs_enum_prev_rsp_type       enum_prev;
  fs_enum_finish_rsp_type     enum_finish;
  fs_remote_link_rsp_type     remote_link;
  fs_rmrlink_rsp_type         rmrlink;
  fs_reserved_rsp_type        reserved;
  fs_statfs_rsp_type          statfs;
  fs_format_rsp_type          format;
  fs_op_rsp_type              any;
} fs_rsp_msg_type;

/* Deprecated Function pointer type for operation callback functions */
typedef  void (*fs_callback_p_type)(fs_rsp_msg_type *rsp_msg);

/* Deprecated Global variable for MMC workaround */
#ifdef HAVE_SFAT_MMC_DIAG_WORKAROUND
extern int mmc_present_flag;
#endif /* Deprecated MMC_DIAG_WORKAROUND */

/**************************************************************************/
/*             EMBEDDED FILE SYSTEM EXPORTED OPERATIONS                   */
/**************************************************************************/


/*===========================================================================

FUNCTION FS_DEV_GET_BASE_ADDR

DESCRIPTION
  This API is deprecated.

===========================================================================*/
dword fs_dev_get_base_addr(void);


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
);



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
);


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
);



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
);




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
);


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
);


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
);


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
);


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
);

/*===========================================================================

FUNCTION FS_OPENED_FILE_SIZE

DESCRIPTION
  This API is deprecated, please use efs_fstat() instead. For more information
  refer to fs_public.h

===========================================================================*/

void  fs_opened_file_size
(
  fs_handle_type      handle,
  fs_callback_p_type  callback_function,
  fs_rsp_msg_type     *file_size_rsp_msg
);



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
);


/*===========================================================================

FUNCTION FS_SEEK_READ

DESCRIPTION
  This API is deprecated, please use efs_lseek() and efs_read() instead. For
  more information refer to fs_public.h.
  
===========================================================================*/

void  fs_seek_read
(
  fs_handle_type        handle,            /* Handle of file to read from */
  fs_seek_origin_type   origin,            /* Seek origin */
  fs_file_position_type position,          /* Position to seek to */
  PACKED void           *buffer,           /* Buffer for read bytes */
  dword                 count,             /* Number of bytes to read */
  fs_callback_p_type    callback_function, /* Callback function pointer */
  fs_rsp_msg_type       *seek_read_rsp_msg /* Response message pointer */
);


/*===========================================================================

FUNCTION FS_SEEK_WRITE

DESCRIPTION
  This API is deprecated, please use efs_lseek() and efs_write() instead. For
  more information refer to fs_public.h.
  
===========================================================================*/

void  fs_seek_write
(
  fs_handle_type        handle,            /* Handle of file to write to */
  fs_seek_origin_type   origin,            /* Seek origin */
  fs_file_position_type position,          /* Position to seek to */
  PACKED const void     *buffer,           /* Buffer with bytes to write */
  dword                 count,             /* Number of bytes to write */
  fs_callback_p_type    callback_function, /* Callback function pointer */
  fs_rsp_msg_type       *write_rsp_msg     /* Response message pointer */
);



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
);


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
);



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
);


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
);



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
);


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
);



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
);


/*===========================================================================

MACRO FS_TEST

DESCRIPTION
  This API is deprecated. For more information refer to fs_public.h. To replace
  this macro call, the application can use efs_stat() check the return value.
  
=============================================================================*/
  
#define fs_test(filename, callback_function, test_rsp_msg)   \
        fs_nametest(filename, FS_TEST_FILE_OR_DIR, callback_function, test_rsp_msg)


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
  fs_nametest_type        type,                /* Test a file or a dir */
  fs_callback_p_type      callback_function,   /* Callback function ptr */
  fs_rsp_msg_type         *test_rsp_msg        /* Response message ptr */
);




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
);



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
);


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
);


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
);



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
);


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
);



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
);


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
  fs_rsp_msg_type             *enum_next_rsp_msg /* Response message ptr */
);



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
);



/*===========================================================================

FUNCTION FS_REMOVE_REMOTE_LINKS

DESCRIPTION
  This API is deprecated.

===========================================================================*/

void  fs_remove_remote_links
(
  fs_callback_p_type          callback_function, /* Callback function ptr */
  fs_rsp_msg_type             *rmrlink_rsp_msg   /* Response message ptr */
);


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
);



/*===========================================================================

FUNCTION FS_CANCEL_RESERVATION

DESCRIPTION
  This API is deprecated, please use efs_set_reservation() giving reservation 
  size of zero. For more information refer to fs_public.h.

===========================================================================*/

fs_status_type  fs_cancel_reservation
(
  fs_conf_code_type     conf_code
);



/*===========================================================================

FUNCTION FS_RESERVATION_IS_NULL

DESCRIPTION
  This API is deprecated.

===========================================================================*/

boolean  fs_reservation_is_null
(
  fs_conf_code_type     conf_code
);



/*===========================================================================

FUNCTION FS_SPACE_AVAILABLE

DESCRIPTION
  This API is deprecated, please use efs_statvfs() instead. For more
  information refer to fs_public.h
  
===========================================================================*/

dword  fs_space_available
(
  void
);



/*===========================================================================

FUNCTION FS_SPACE_ERASED

DESCRIPTION
  This API is deprecated, please use efs_statvfs() instead. Please refer to
  fs_public.h for more information.

===========================================================================*/

dword  fs_space_erased
(
  void
);



/*===========================================================================

FUNCTION FS_SPACE_USED

DESCRIPTION
  This API is deprecated, please use efs_statvfs() instead. For more
  information refer to fs_public.h
  
===========================================================================*/

dword  fs_space_used
(
  void
);


/*===========================================================================

FUNCTION FS_SPACE_RESERVED

DESCRIPTION
  This API is deprecated. To get the space reserved by a group ID use
  efs_get_group_info(). For more information refer to fs_public.h.

===========================================================================*/

dword  fs_space_reserved
(
  void
);


/*===========================================================================

 FUNCTION FS_STATFS

 DESCRIPTION
  This API is deprecated, please use efs_statvfs() instead. For more
  information refer to fs_public.h.

===========================================================================*/
void
fs_statfs
(
    const char         *filename,
    statfs_info        *info,
    fs_callback_p_type  callback_function,
    fs_rsp_msg_type    *statfs_rsp_msg
);


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
);



/*===========================================================================

FUNCTION FS_CMD_IS_WAITING

DESCRIPTION
  This API is deprecated.
  This function does not have any meaning in EFS2.

===========================================================================*/

boolean  fs_cmd_is_waiting
( 
  void
);


/*===========================================================================

FUNCTION FS_TASK

DESCRIPTION
  This API is deprecated.

===========================================================================*/

void  fs_task
( 
  dword              parm       /* from REX - ignored                     */
);


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
);
#endif /* FEATURE_FS_ALLOW_MASTER_RESET */

#endif /* FS_H */
