#ifndef FSDIAG_H
#define FSDIAG_H
/*==========================================================================

                      Diagnostic Packet Definitions

  Description: Packet definitions between the diagnostic subsystem
  and the external device.

  !!! NOTE: All member structures of DIAG packets must be PACKED.
  
  !!! WARNING: Each command code number is part of the externalized
  diagnostic command interface.  This number *MUST* be assigned
  by a member of QCT's tools development team.

Copyright (c) 2001-2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                            Edit History

  $Header: //depot/asic/msmshared/services/diag/fsdiag.h#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/12/04    as     Removed support for Remote file link.
11/05/03    pg     Added support for fs_format function.
07/10/03    gr     Moved access validation support into its own file.
04/07/03   jct     Added file access validation callbacks which invoke a
                   registered callback for any file system access via
                   DIAG - the callback returns a status indicating whether or
                   not the operation may proceed.  If no callback exists
                   the operation proceeds.
08/20/01   jal     Updated for core VU
01/26/01   lad     Created file from what used to be diagpkt.h
===========================================================================*/

#include "comdef.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "fs_diag_access.h"


/* -------------------------------------------------------------------------
** Packet definitions for logging services.
** ------------------------------------------------------------------------- */

#if defined(T_WINNT)
#error code not present
#endif
/*===========================================================================

PACKET   fsdiag_op_req_type

ID       DIAG_FS_OP_F

PURPOSE  This message requests a filesystem operation.  This message contains
         sub-commands to specify which file system operation to perform.
         It is a variable length request message - depending on the file
         system operation.

RESPONSE The fsdiag_op_rsp_type will be sent in response.

===========================================================================*/
typedef enum
{
  FSDIAG_MK_DIR      = 0,      /* Create directory                */
  FSDIAG_RM_DIR      = 1,      /* Remove Directory                */
  FSDIAG_DISP_DIRS   = 2,      /* Display directories list        */
  FSDIAG_DISP_FILES  = 3,      /* Display file list               */
                                /* NOTE: Display ops are temporary 
                                   implementations                 */ 
  FSDIAG_READ_FILE   = 4,      /* Read a file in FS               */
  FSDIAG_WRITE_FILE  = 5,      /* Write a file in FS              */  
  FSDIAG_REMOVE_FILE = 6,      /* Remove a file from FS           */  
  FSDIAG_GET_ATTRIB  = 7,      /* Get file attributes             */
  FSDIAG_SET_ATTRIB  = 8,      /* Set file attributes             */
  FSDIAG_ITER_DIRS   = 10,     /* Iterative display directories   */
  FSDIAG_ITER_FILES  = 11,     /* Iterative display files         */
  FSDIAG_SPACE_AVAIL = 12,     /* Amount of free space in EFS     */
  FSDIAG_FORMAT      = 13,     /* Format a card mounted on specific
                                  path                            */
  FSDIAG_LAST_OP               /* Last OP.  For range checking.   */
} fsdiag_op_enum_type;


/*--------------------------------------------------------------
   Generic structure definitions used in multiple operations.
--------------------------------------------------------------*/

#define FSDIAG_MAX_FILENAME_LEN      60 /* Specified by FS              */
#define FSDIAG_MAX_FILE_BLOCK_SIZE  256 /* Limited by req buffer of 256 */

typedef PACKED struct
{
  byte len;                            /* Length of filename string 
                                          including NULL terminating char   */
  char name[FSDIAG_MAX_FILENAME_LEN]; /* Filename string.  NULL terminated.
                                          Valid data len == filename_len+1  */
} fsdiag_filename_type;

typedef PACKED struct
{
  word len;                               /* Length of data block */
  byte data[FSDIAG_MAX_FILE_BLOCK_SIZE]; /* Data block           */
} fsdiag_data_block_type;

/* File attribute mask */
typedef enum {
  FSDIAG_FA_UNRESTRICTED =0x00FF,  /* No file access restrictions           */
  FSDIAG_FA_PERMANENT    =0x007F,  /* File can't be removed nor truncated   */
  FSDIAG_FA_READONLY     =0x00BF,  /* File can't be opened for update       */
  FSDIAG_FA_SYS_PERMANENT=0x0037,  /* File persists across system reformats */
  FSDIAG_FA_REMOTE       =0x001F   /* Remote file (resides outside file     */
                                    /* system address space)                 */
} fsdiag_attribute_mask_type;     

/* File OPEN operation buffering options */
typedef enum {
  DIAGPKT__FS_OB_PROHIBIT,             /* Prohibit file buffering                */
  DIAGPKT__FS_OB_ALLOW                 /* Allow file buffering                   */
} fsdiag_buffering_option_type;

/* File OPEN operation clean-up options */
typedef enum {
  DIAGPKT__FS_OC_CLOSE     = 0x00, /* Close file as-is                           */
  DIAGPKT__FS_OC_DISCARD   = 0x01, /* Delete file and remove it from directory   */
  DIAGPKT__FS_OC_TRUNCATE  = 0x02, /* Truncate file from specified position      */
  DIAGPKT__FS_OC_REVERT    = 0x03  /* Revert to last checkpointed version        */
} fsdiag_cleanup_option_type;

typedef PACKED struct
{
  word      attribute_mask;  /* Use fsdiag_attribute_mask_type   */
  byte    buffering_option;  /* Use fsdiag_buffering_option_type */
  byte      cleanup_option;  /* Use fsdiag_cleanup_option_type   */
} fsdiag_attributes_type;

/*-------------------------------------------------------
  Definitions of data for specific operations.
-------------------------------------------------------*/
/* "Create Directory" operation */
typedef fsdiag_filename_type fsdiag_mkdir_req_type;

/* "Remove Directory" operation */
typedef fsdiag_mkdir_req_type fsdiag_rmdir_req_type;

/* "Display Directory List" operation */
typedef fsdiag_filename_type fsdiag_disp_dirs_req_type;

/* "Display File List" operation */
typedef fsdiag_disp_dirs_req_type fsdiag_disp_files_req_type;

/* "Read File" operation */
typedef PACKED struct
{
  byte seq_num;                        /* Sequence number for mult blocks */
  
  fsdiag_filename_type filename_info; /* File name info                  */
                                       /* Used only if seq_num == 0       */
} fsdiag_read_req_type;

/* "Write File" operation */
typedef enum
{
  FSDIAG_NO_OVERWRITE = 0,
  FSDIAG_OVERWRITE    = 1
} fsdiag_write_mode_enum_type;

typedef PACKED struct
{ 
  byte                          mode; /* Use fsdiag_write_mode_enum_type  */
  dword                 total_length; /* Total length of this file         */
  fsdiag_attributes_type     attrib; /* Attributes for this file          */
  
  PACKED union
  {
    fsdiag_filename_type  name_info;    /* File name info                 */
    byte raw_data[sizeof(fsdiag_filename_type) +
                  sizeof(fsdiag_data_block_type)];
                                         /* Full possible size of variable
                                            length buffer                  */
  } var_len_buf;
} fsdiag_write_begin_req_type;

typedef PACKED union
{
  fsdiag_write_begin_req_type  begin; /* First block of a write           */
  fsdiag_data_block_type      subseq; /* Subsequent blocks for write      */
} fsdiag_write_block_type;

typedef PACKED struct
{
  byte seq_num;                        /* Sequence number for mult blocks    */
  byte    more;                        /* Flag if more packets are needed to
                                          complete write                     */
  fsdiag_write_block_type     block;  /* Block for this portion of the file */
} fsdiag_write_req_type;

/* "Get File Attributes" operation */
typedef fsdiag_filename_type fsdiag_get_attrib_req_type;

/* "Set File Attributes" operation */
typedef PACKED struct
{
  fsdiag_attributes_type       attribs;
  dword                   creation_date;
  fsdiag_filename_type   filename_info;
} fsdiag_set_attrib_req_type;

/* "Remove File" operation */
typedef fsdiag_filename_type fsdiag_rmfile_req_type;

/* "Iterate Directories" operation */
typedef PACKED struct
{
  dword seq_num;
  fsdiag_filename_type dir_name;
} fsdiag_iter_dirs_req_type;

/* "Iterate Files" operation */
typedef fsdiag_iter_dirs_req_type fsdiag_iter_files_req_type;

/* "Format Card" operation */
typedef fsdiag_filename_type fsdiag_format_req_type;

/* Union of all possible operations.  Determined by cmd_code */
typedef PACKED union
{                               
  byte                             seq_num; /* Seq number in same place for
                                               all packets that use them */
  fsdiag_mkdir_req_type             mkdir;
  fsdiag_rmdir_req_type             rmdir;
  fsdiag_read_req_type               read;
  fsdiag_write_req_type             write;
  fsdiag_disp_dirs_req_type     disp_dirs;
  fsdiag_disp_files_req_type   disp_files;
  fsdiag_get_attrib_req_type   get_attrib;
  fsdiag_set_attrib_req_type   set_attrib;
  fsdiag_rmfile_req_type           rmfile;
  fsdiag_iter_dirs_req_type     iter_dirs;
  fsdiag_iter_files_req_type   iter_files;
  fsdiag_format_req_type           format;
} fsdiag_req_type;

typedef PACKED struct 
{
  byte cmd_code;
  byte file_op;                    /* From fsdiag_op_enum_type */
  fsdiag_req_type fs_req;      /* Filesystem request data union */

} fsdiag_req_pkt_type;

#define DIAG_FS_REQ_LEN_DIFF FPOS(fsdiag_req_pkt_type, fs_req)

/*===========================================================================

PACKET   fsdiag_op_rsp_type

ID       DIAG_FS_OP_F

PURPOSE  This response contains status and results data from an EFS 
         operation.  The contents of the response depends on the file
         system operation (sub-command) requested.

===========================================================================*/
/*------------------------------------------------
  Operation status values can be found in fs.h
------------------------------------------------*/

/*------------------------------------------------
  Definitions of data for specific operations.
------------------------------------------------*/
/* "Display Directory List" operation */
typedef PACKED struct
{
  word num_dirs;
  fsdiag_data_block_type  dir_list;
} fsdiag_disp_dirs_rsp_type;

/* "Display File List" operation */
typedef PACKED struct
{
  word num_files;
  fsdiag_data_block_type  file_list;
} fsdiag_disp_files_rsp_type;

/* "Read File" operation */
typedef PACKED union
{ 
  PACKED struct
  {
    dword                    total_length;   /* Total length of this file */
    fsdiag_data_block_type  block;
  } begin;

  fsdiag_data_block_type  block;
 
} fsdiag_read_block_type;

typedef PACKED struct
{
  byte                  seq_num;  /* Sequence number for mult blocks */
  byte                     more;  /* Flag if more packets are needed to
                                      complete transfer */
  fsdiag_read_block_type  data;  /* Current portion of the file */
} fsdiag_read_rsp_type;

/* "Write File" operation */
typedef PACKED struct
{
  byte seq_num;
} fsdiag_write_rsp_type;

/* "Get File Attributes" operation */
typedef PACKED struct
{
  fsdiag_attributes_type attrib; /* Attributes */
  dword            creation_date; /* File creation date */
  dword                     size; /* File size */
} fsdiag_get_attrib_rsp_type;

/* "Iterate Directories" and "Iterate Files" operation */
typedef PACKED struct
{
  dword seq_num;
  
  fsdiag_attributes_type   attrib; /* Attributes */
  dword              creation_date; /* File creation date */
  dword               logical_size; /* Size of data */
  dword              physical_size; /* Physical space on device */
  byte              dirname_length; /* Length of directory portion */
                                    /* of file name. */
  fsdiag_filename_type  item_name;
} fsdiag_iter_rsp_type;

/* "Space Available" operation */
typedef dword fsdiag_space_avail_rsp_type;

typedef PACKED union
{
  byte                             seq_num; /* Seq number in same place for 
                                               all packets that use them */
  fsdiag_disp_dirs_rsp_type     disp_dirs;
  fsdiag_disp_files_rsp_type   disp_files;
  fsdiag_read_rsp_type               read;
  fsdiag_write_rsp_type             write;
  fsdiag_get_attrib_rsp_type   get_attrib;
  fsdiag_iter_rsp_type               iter;
  fsdiag_space_avail_rsp_type space_avail;
} fsdiag_rsp_type;

typedef PACKED struct 
{
  byte cmd_code;
  byte file_op;                      /* Operation requested              */
  byte fs_status;                    /* Status of operation - values in  
                                        fs_status_enum_type              */
  fsdiag_rsp_type  fs_rsp;          /* Only used if returning more than 
                                        just status                      */

} fsdiag_rsp_pkt_type;

#if defined(T_WINNT)
#error code not present
#endif

#endif /* FSDIAG_H */

