#ifndef FSI_H
#define FSI_H
/*==========================================================================

                   E M B E D D E D   F I L E   S Y S T E M

DESCRIPTION

  This header file contains internal definitions needed by this module.

Copyright (c) 1999-2006 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/fsi.h_v   1.4   05 Dec 2001 16:21:04   davidb  $
$Header: //source/qcom/qct/core/storage/efs1_compat/main/latest/inc/fsi.h#1 $ $DateTime: 2008/06/03 18:36:07 $ $Author: richardp $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
15-09-06    yg     Featurized ABS macro.
08-08-05   sch     Added "filename" to fs_statfs_parms_type
10-24-03   pg      Added support for FS_FORMAT_OP to format MMC card.
12-05-01   dlb     Added fs_opened_file_size().
10-15-01   dlb     Explain values for FS_GC_CLEAN_RSVD and FSI_TLOG_RSVD.
09-10-01   dlb     Add fs_statfs() call.
04-02-01   dlb     Implement erase logs.  Add support for MSP1000.
11-09-00   pg      Initial version for common archive.
08-22-00   dlb     Cleanup FEATURE_EFS_ACCESS_METHODS code.
07-27-00   pg      Added comment for FSI_NUM_OF_BUCKETS.
07-25-00   pg      Added support to reserve some blocks for NV-writes along
                   with transation log operations. 
07-24-00   pg      Added support to reserve some blocks and directory
                   entries for transaction log files only.
                   Added support for hashing the master file directory by
                   the file-ids.
05-12-00   dlb     Added FEATURE_EFS_ACCESS_METHODS and code.
05-12-00   dlb     Added fs_enum_finish().
04-25-00   pg      Modified struct fs_nametest_parms_type to fixed the 
                   problem that fs_test() could not properly distinguish 
                   a file from a directory.
03-24-00   pg      Added support for fs_enum_prev().
03-16-00   pg      Added support for fs_seek_read() and fs_seek_write().
01-31-00   pg      Added support for fs_ops_enum_init() and fs_ops_enum_next().
01-21-00   pg      Added support for fs_ops_rename_file function.
12-16-99   rp      Added support for fs_ops_remove_remote_links function. 
11-01-99   ls      Moved tlog_index to master directory table union type.
10-28-99   ls      Backward-merged Version 3.0 fixes.
09-21-99   ls      Added constants for block data integrity check.
08-13-99   rp      Added support for block size larger than 256 bytes.
07-13-99   ls      Added constant for size of block number field.
07-02-99   ls      Added initial tlog count value constant.
06-24-99   ls      Added max number of remote files parameter.
06-22-99   ls      Changed transaction logging to support user revertable
                   files across multiple garbage collections.
06-08-99   ls      Added constants for system-permanent files.
05-26-99   ls      Allow "in-system" creation of factory data block file.
05-24-99   ls      Added support for system permanent files.
05-20-99   ls      Added fmemset and space limit support.
04-29-99   ls      Moved definition of device control option bits here.
04-27-99   ls      Added space available offset parameter.
04-25-99   ls      Added garbage collection processing states.
04-23-99   ls      Added new garbage collection directive.
04-22-99   ls      Removed enclosing ifdef.
04-21-99   ls      Version 2.2 release.
04-03-99   ls      Changed interface per Version 2.1 specification.
03-04-99   ls      Initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "queue.h"
#include "rex.h"
#include "fs.h"


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                             MACROS                                      */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to kick the watchdog timer and set a timer to wake it again.      */
#define KICK_WATCHDOG()         \
        dog_report(DOG_FS_RPT); \
        (void) rex_set_timer(&fs_rpt_timer, DOG_FS_RPT_TIME);

/* Macro to determine the number of file system blocks needed to store     */
/* the specified number of bytes                                           */
#define FS_NUM_BLKS(num_bytes)  \
        ((num_bytes>0) ? (((num_bytes-1) / FSI_BLK_DATA_SIZE) + 1) : 0)

/* Macro returns absolute value of argument */
#ifndef ABS
#define ABS(VAL) (((VAL)>0)?(VAL):(-(VAL)))
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                         DATA DEFINITIONS                                */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* REX Task Signals declarations.                                          */

#define  FS_RPT_TIMER_SIG     0x0001      /* Watchdog report signal */
#define  FS_WRITE_TIMER_SIG   0x0002      /* Write timer signal */
#define  FS_CMD_Q_SIG         0x0004      /* Command queue signal */

#ifdef T_80186
  #pragma pack(1)
#endif

/* Size of a physical block in bytes */
#define  FSD_PHYSICAL_BLKSIZE  FS_PHYSICAL_BLKSIZE_P
#define  FSI_PHYSICAL_BLKSIZE  FSD_PHYSICAL_BLKSIZE

/* Maximum total number of blocks in any sector */
#define  FSD_MAX_BLKS_ALL_SECTORS   FS_MAX_BLKS_ALL_SECTORS_P
#define  FSI_MAX_BLKS_ALL_SECTORS   FSD_MAX_BLKS_ALL_SECTORS

/* Maximum total number of sectors in the device */
#define  FSD_NUM_SECTORS  FS_NUM_SECTORS_P
#define  FSI_NUM_SECTORS  FSD_NUM_SECTORS

/* The highest sector address */
#define  FSI_MAX_SECTOR_ADRS   (FSI_NUM_SECTORS-1)

/* Special sector "address" reserved for remote files */
#define  FSI_REMOTE_SECTOR_ADRS   (FSI_MAX_SECTOR_ADRS+1)

/* Special block "address" reserved for remote files */
#define  FSI_REMOTE_BLK_ADRS      0

/* Type of file */
typedef  word   fsi_file_type;
#define  FSI_FILE_TYPE_USER             0
#define  FSI_FILE_TYPE_SYSTEM           1
#define  FSI_FILE_TYPE_SYS_PERM         2
#define  FSI_FILE_TYPE_TLOG             3

/* Allocation status (free or in-use) and type of block  */
typedef  byte   fsi_blk_status_type;

/* Value of an "erased" byte */
#define FSI_ERASED_BYTE_VALUE           0xFF

/* Value of an "erased" word */
#define FSI_ERASED_WORD_VALUE           0xFFFF

/* Erased (free) block reads all binary ones */
#define  FSI_BLK_STATUS_FREE            FSI_ERASED_BYTE_VALUE
/* Identifies this as a generic used block */
#define  FSI_BLK_STATUS_USED            0xFE
/* Identifies this as a generic obsolete used block */
#define  FSI_BLK_STATUS_USED_OB         0x7E
/* Identifies this as a system file header block */
#define  FSI_BLK_STATUS_SFHEAD          0xED
/* Identifies this as an obsolete system file header block */
#define  FSI_BLK_STATUS_SFHEAD_OB       0x6D
/* Identifies this as a system transaction log file header block */
#define  FSI_BLK_STATUS_SFHEAD_TLOG     0xEC
/* Identifies this as an obsolete system transaction log file header block */
#define  FSI_BLK_STATUS_SFHEAD_TLOG_OB  0x6C
/* Identifies this as a client file header block */
#define  FSI_BLK_STATUS_CFHEAD          0xDB
/* Identifies this as an obsolete client file header block */
#define  FSI_BLK_STATUS_CFHEAD_OB       0x5B
/* Identifies this as a system-permanent file block */
#define  FSI_BLK_STATUS_SYS_PERMANENT   0xFB
#ifdef FEATURE_FS_HANDOVER_SUPPORT
/* Reserved block status value for Handover Block detection */
#define  FSI_BLK_STATUS_RSVD1_FOR_HO    0x68
/* Reserved block status value for Handover Block detection */
#define  FSI_BLK_STATUS_RSVD2_FOR_HO    0x38
#endif
/* Identifies this as a continuation of a file header block */
#define  FSI_BLK_STATUS_HEAD_C          0x9A
/* Identifies this as an obsolete continuation of a file header block */
#define  FSI_BLK_STATUS_HEAD_C_OB       0x1A
/* Identifies this as an explicit garbage block */
#define  FSI_BLK_STATUS_GARBAGE         0x7F
/* Identifies this as a corrupted block */
#define  FSI_BLK_STATUS_CORRUPTED       0x00

/* Identifies this as an erase log. */
#define  FSI_BLK_STATUS_ELOG            0xEA

/* All garbage block types share a first bit value of zero */
#define  FSI_BLK_GARBAGE_BIT_MASK  0x80

/* Identifies which file a block belongs to */
typedef  word   fsi_owner_id_type;
#define  FSI_OWNER_ID_SIZE        (sizeof(fsi_owner_id_type))
#define  FSI_MAX_OWNER_ID_VALUE   65535

/* The ordinal number of a block within a file */
typedef  word   fsi_blk_number_type;

/* The revision (version) number for a block */
typedef  word   fsi_blk_version_type;
/* Initial block version number constant */
#define  FSI_BLK_VERSION_INITIAL   1
/* Constant representing a block version number of zero */
#define  FSI_BLK_VERSION_ZERO      0

/* The relocation count for a block */
typedef  word   fsi_blk_reloc_type;
#define  FSI_BLK_RELOC_INITIAL     0
#define  FSI_BLK_RELOC_MIDPOINT    32767

/* Ops-level file name type definition */
typedef char  *fs_filename_type;

/* Internal type for a file handle */
typedef  fs_handle_type   fsi_handle_type;

/* The current file access mode */
typedef  byte   fsi_access_mode_type;

#define  FSI_MODE_CLOSED   0x00    /* File is not open */
#define  FSI_MODE_UPDATE   0x78    /* File is open for update access */
#define  FSI_MODE_APPEND   0xAB    /* File is open for append access */
#define  FSI_MODE_READONLY 0x31    /* File is open for readonly access */


/* Current file pointer position */
typedef  dword  fsi_current_pointer_type;

#define  FSI_START_OF_FILE_POSITION  ((fsi_current_pointer_type)0)


/* The common "header" part of every file block */
typedef PACKED struct {
  fsi_blk_status_type   blk_status;    /* block type & allocation status   */
  byte                  reserved1;     /* reserved byte                    */ 
  fsi_owner_id_type     owner_id;      /* which file this block belongs to */
  fsi_blk_number_type   blk_number;    /* block number within the file     */
  fsi_blk_version_type  blk_version;   /* block version number             */
  fsi_blk_reloc_type    blk_reloc;     /* block relocation count           */
  word                  data_count;    /* valid bytes in data part         */
} fsi_blk_header_type;

#define  FSI_BLK_HEADER_SIZE  (sizeof(fsi_blk_header_type))
#define  FSI_BLK_DATA_CHECK_SIZE  4
#define  FSI_BLK_DATA_SIZE    \
             (FSI_PHYSICAL_BLKSIZE - FSI_BLK_HEADER_SIZE - \
             FSI_BLK_DATA_CHECK_SIZE)
#define  FSI_BLK_BLK_STATUS_SIZE  (sizeof(fsi_blk_status_type))
#define  FSI_BLK_BLK_NUMBER_SIZE  (sizeof(fsi_blk_number_type))
#define  FSI_BLK_OFFSET_OWNER_ID  (FPOS(fsi_blk_header_type,owner_id))
#define  FSI_BLK_1ST_3_FIELDS_SIZE  (sizeof(fsi_blk_status_type) + \
                                  sizeof(byte) + sizeof(fsi_owner_id_type))

/* Physical generic file block definition  */
typedef PACKED struct {
  fsi_blk_header_type     hdr;
  byte                    data[FSI_BLK_DATA_SIZE];
  byte                    data_check[FSI_BLK_DATA_CHECK_SIZE];
} fsi_generic_blk_type;

/* Physical file block address */
typedef PACKED struct {
  byte                    sector;    /* which sector */
  byte                    blk;       /* which block within sector */
} fsi_file_adrs_type;


/* Filename definition (internal) */
typedef PACKED struct {
  byte                    length;
  byte                    name[FS_FILENAME_MAX_LENGTH];
} fsi_filename_type;

/* Absolute file creation sequence number */
typedef  dword  fsi_fsequence_num_type;
/* Reserved fsequence number for factory data block */
#define  FSI_BLK_FSEQUENCE_RSVD_1    1
/* Reserved fsequence number for phone's permanent non-volatile data */
#define  FSI_BLK_FSEQUENCE_RSVD_2    2
/* Starting fsequence number for files created by the file system */
#define  FSI_BLK_FSEQUENCE_INITIAL   ((fsi_fsequence_num_type)10)
/* Highest possible file sequence number */
#define  FSI_FSEQUENCE_NUM_MAX       ((dword)0xFFFFFFFF)

/* File Creation Date */
typedef  dword  fsi_file_date_type;

/* File Attribute Mask */
typedef  word   fsi_attribute_mask_type;

/* File Buffering Option */
typedef  byte   fsi_buffer_option_type;

/* File Abnormal Powerdown Clean-up Option */
typedef  byte   fsi_pwrdwn_option_type;

#define  FSI_PWRDWN_OPT_CLOSE     (0x00)
#define  FSI_PWRDWN_OPT_DISCARD   (0x01)
#define  FSI_PWRDWN_OPT_TRUNCATE  (0x02)
#define  FSI_PWRDWN_OPT_REVERT    (0x03)

/* Set of 32 user-definable flags */
typedef  dword  fsi_user_attribs_type;

/* File Header Format (stored in data part of first file block) */
typedef PACKED struct {
  fsi_fsequence_num_type  fsequence_number;
  fsi_file_date_type      creation_date;
  fsi_attribute_mask_type attribute_mask;
  fsi_buffer_option_type  buffer_option;
  fsi_pwrdwn_option_type  pwrdwn_option;
  fsi_user_attribs_type   user_attributes;
  fsi_filename_type       filename;
} fsi_file_header_type;

#define  FSI_FILE_HEADER_SIZE        (sizeof(fsi_file_header_type))
#define  FSI_FILE_HEADER_FIXED_SIZE  \
             (sizeof(fsi_fsequence_num_type)  + \
              sizeof(fsi_file_date_type)      + \
              sizeof(fsi_attribute_mask_type) + \
              sizeof(fsi_buffer_option_type)  + \
              sizeof(fsi_pwrdwn_option_type)  + \
              sizeof(fsi_user_attribs_type))
#define  FSI_FH_OFFSET_FSEQUENCE_NUM   \
             (FPOS(fsi_file_header_type,fsequence_number))
#define  FSI_FH_SIZEOF_FSEQUENCE_NUM   (sizeof(fsi_fsequence_num_type))
#define  FSI_FH_OFFSET_CREATION_DATE   \
             (FPOS(fsi_file_header_type,creation_date))
#define  FSI_FH_SIZEOF_CREATION_DATE   (sizeof(fsi_file_date_type))
#define  FSI_FH_OFFSET_ATTRIB_MASK     \
             (FPOS(fsi_file_header_type,attribute_mask))
#define  FSI_FH_SIZEOF_ATTRIB_MASK     (sizeof(fsi_attribute_mask_type))
#define  FSI_FH_OFFSET_BUFFER_OPTION   \
             (FPOS(fsi_file_header_type,buffer_option))
#define  FSI_FH_SIZEOF_BUFFER_OPTION   (sizeof(fsi_buffer_option_type))
#define  FSI_FH_OFFSET_PWRDWN_RECOVERY \
             (FPOS(fsi_file_header_type,pwrdwn_option))
#define  FSI_FH_SIZEOF_PWRDWN_RECOVERY (sizeof(fsi_pwrdwn_option_type))
#define  FSI_FH_OFFSET_USER_ATTRIBS    \
             (FPOS(fsi_file_header_type,user_attributes))
#define  FSI_FH_SIZEOF_USER_ATTRIBS    (sizeof(fsi_user_attribs_type))
#define  FSI_FH_OFFSET_FILENAME        \
             (FPOS(fsi_file_header_type,filename))

/* Pointer (index) type for lists of file blocks */
typedef  word   fsi_list_index_type;

/* For "varptr" union, contains saved file blocks (checkpoint) info */
typedef struct {
  fsi_list_index_type       sblkptr;    /* pointer to saved blocks list   */
  word                      tlog_index; /* transaction log file index     */
} fsi_chkpnt_info_type;

/* A master file directory table entry */
typedef struct {
  fsi_owner_id_type         fid;        /* unique file identifier         */
  byte                      reserved;   /* reserved byte                  */
  fsi_file_adrs_type        adrs;       /* address of file header block   */
  dword                     size;       /* current file size in bytes     */
  word                      numblks;    /* number of blocks used by file  */
  union {
    fsi_chkpnt_info_type    chkpnt;     /* saved file blocks information  */
    dword                   remote_ptr; /* for remote files only          */
  }                         varptr;     /* union field tag                */
  fsi_list_index_type       fblkptr;    /* pointer to file block list     */
  fsi_list_index_type       gblkptr;    /* pointer to garbage block list  */
  word                      next;       /* index to the next entry in the
                                           link list                      */
} fsi_directory_entry_type;

/* Size of a master file directory entry */
#define  FSI_SIZEOF_DIRECTORY_ENTRY  (sizeof(fsi_directory_entry_type))
/* Number of entries reserved for transaction log files */
#define  FSI_TLOG_RSVD_DIRECTORY_ENTRIES  10
/* Number of entries in the master file directory */
#define  FSI_MAX_DIRECTORY_ENTRIES  (FS_MAX_NUM_FILES_P+FSI_TLOG_RSVD_DIRECTORY_ENTRIES) 
/* An invalid master file directory index */
#define  FSI_END_OF_DIRECTORY_INDEX (FSI_MAX_DIRECTORY_ENTRIES+1)
/* Indicates varptr contains remote_ptr field */
#define  FSI_VARPTR_REMOTE     (0x00)
/* Indicates varptr contains sblkptr field */
#define  FSI_VARPTR_SBLKPTR    (0x01)
/* A Null remote file pointer */
#define  FSI_NULL_REMOTE_PTR   ((dword)0)

/* Type for the master file directory (stored in RAM) */
typedef struct {
  word                      number_of_entries;
  word                      number_of_free_blks;
  fsi_directory_entry_type  entry[FSI_MAX_DIRECTORY_ENTRIES];
} fsi_directory_type;

/* Array of ordered directory table indices */
typedef  word   fsi_directory_index_type[FSI_MAX_DIRECTORY_ENTRIES];


/* An entry of the Open Files table */
typedef struct {
  boolean                   in_use;     /* this entry currently in use?   */
  word                      mdir_index; /* corresponding master dir index */
  fsi_access_mode_type      fmode;      /* current file access mode       */
  fsi_current_pointer_type  fp;         /* current file pointer position  */
  dword                     orig_size;  /* file size when opened          */
} fsi_open_table_entry_type;

/* Size of an open files table entry */
#define  FSI_SIZEOF_OPEN_TABLE_ENTRY  (sizeof(fsi_open_table_entry_type))
/* Number of entries in the open files table */
#define  FSI_MAX_OPEN_TABLE_ENTRIES  (FS_MAX_OPEN_FILES_P+1)
/* An invalid open files table index */
#define  FSI_END_OF_OPEN_TABLE_INDEX (FSI_MAX_OPEN_TABLE_ENTRIES+1)

/* Type for the open files table */
typedef struct {
  fsi_open_table_entry_type entry[FSI_MAX_OPEN_TABLE_ENTRIES];
} fsi_open_table_type;


/* Bit array type */
#define  FSI_MAX_BIT_ARRAY_INDEX  ((FSI_MAX_BLKS_ALL_SECTORS+7)/8)

typedef PACKED struct {
  byte   value[FSI_MAX_BIT_ARRAY_INDEX];
} fsi_bit_array_type;


/* A sector status table entry */
typedef struct {
  dword                     volatility_sum; /* sum of block volatilities   */
  dword                     volatility_avg; /* average of blk volatilities */
  word                      total_count;    /* total number of blocks      */
  word                      alloc_count;    /* number of allocated blocks  */
  word                      garbage_count;  /* number of garbage blocks    */
  fsi_bit_array_type        garbage_blks;   /* which blocks are garbage    */
  byte                      rating;         /* garbage collection urgency  */
  byte                      reserved2;      /* reserved byte               */
} fsi_sector_status_entry_type;


/* The sector status table (stored in RAM) */
#define  FSI_MAX_SECTOR_STATUS_ENTRIES   FSI_NUM_SECTORS

typedef struct {
  fsi_sector_status_entry_type  entry[FSI_MAX_SECTOR_STATUS_ENTRIES];
} fsi_sector_status_type;

/* Lowest garbage collection eligibility rating */
#define  FSI_GC_RATING_ZERO  0
/* Garbage collection eligibility rating indicating GC in progress */
#define  FSI_GC_IN_PROGRESS  101

/* Set internal parameter specifying number of fbl entries per fbl node */
#define  FSI_FBL_ELEMS_PER_ENTRY   FS_FBL_ENTRIES_PER_NODE_P

/* File block list (fbl) node definition */
typedef PACKED struct {
  fsi_list_index_type    next;
  byte                   sector[FSI_FBL_ELEMS_PER_ENTRY];
  byte                   blk[FSI_FBL_ELEMS_PER_ENTRY];
  word                   version[FSI_FBL_ELEMS_PER_ENTRY];
} fsi_fbl_node_type;

/* Power-up scan temporary "obsolete file owner IDs" storage */
typedef PACKED struct {
  fsi_list_index_type    next;
  fsi_owner_id_type      owner_id;
} fsi_fbl_owner_id_type;

/* Type for elements of the fbl pool */
typedef union {
  fsi_fbl_node_type      node;
  fsi_fbl_owner_id_type  owid;
} fsi_fbl_pool_element_type;

/* One set of data from a file block list node */
typedef PACKED struct {
  byte                   sector;
  byte                   blk;
  word                   version;
} fsi_fbl_node_data_type;

/* Size of an fbl node in bytes */
#define  FSI_FBL_NODE_DATA_SIZE  (sizeof(fsi_fbl_node_data_type))

/* Abstract "index" of an fbl element */
typedef PACKED struct {
  fsi_list_index_type    node;     /* Which node */
  word                   element;  /* Which element within the node */
} fsi_fbl_element_index_type;

/* Element of a table of saved fbl elements */
typedef PACKED struct {
  fsi_fbl_node_data_type      node_data;  /* Data in the node */
  word                        dindex;     /* Which file index */
} fsi_fbl_saved_element_type;

#define  FSI_MAX_SAVED_LIST_ELEMENTS  FSI_MAX_BLKS_ALL_SECTORS

/* Table of saved fbl elements for later processing */  
typedef PACKED struct {
  fsi_fbl_saved_element_type  entry[FSI_MAX_SAVED_LIST_ELEMENTS];
} fsi_fbl_saved_list_type;

/* Identifies which fbl list (file, checkpoint, or garbage) to operate on */
typedef  byte   fsi_fbl_list_id_type;
/* Specifies file block list */
#define  FSI_FBL_FBLK_LIST    0
/* Specifies checkpointed file block list */
#define  FSI_FBL_SBLK_LIST    1
/* Specifies garbage block list */
#define  FSI_FBL_GBLK_LIST    2

/* Number of file block list (fbl) node spaces to have in the free pool */
#define  FSI_FBL_POOL_NODES     FS_MAX_FBL_NODES_P
/* Midpoint of the fbl pool for the temporary obsolete file headers list */
/* which shares space with the 2nd half of the fbl pool during power-up  */
#define  FSI_FBL_POOL_MIDPOINT  (FSI_FBL_POOL_NODES/2)
/* Number of bytes in file block list (fbl) free memory pool */
#define  FSI_FBL_POOL_SIZE    (FSI_FBL_POOL_NODES*sizeof(fsi_fbl_node_type))
/* Index of free list header block.  Note: zero'th table element is     */
/* used as the free blocks list head so cannot be allocated, hence a    */
/* pointer (index) value of NULL can denote the list end.               */
#define  FSI_FBL_HEAD_INDEX     0
/* File Block List (fbl) NULL index definition */
#define  FSI_FBL_NULL           0
/* Specifies full initialization of fbl pool to "init_fbl_pool" function */
#define  FSI_FBL_INIT_ALL       0
/* Specifies initialization of 1st half of fbl pool to "init_fbl_pool" */
#define  FSI_FBL_INIT_1ST_HALF  1
/* Specifies initialization of 2nd half of fbl pool to "init_fbl_pool" */
#define  FSI_FBL_INIT_2ND_HALF  2

/* Size of the file owner IDs list */
#define  FSI_OWNER_ID_LIST_SIZE (FSI_FBL_POOL_NODES/8)

/* Type for an entry in the user file directory */
typedef PACKED struct {
  byte                   entry_length;
  word                   num_files;
  fsi_filename_type      dirname;
}
#ifdef __GNUC__
  __attribute__((__packed__))
#endif
fsi_user_dir_entry_type;

#define  FSI_USER_DIR_ENTRY_SIZE   (sizeof(fsi_user_dir_entry_type))
#define  FSI_USER_DIR_BUFFER_SIZE  (FSI_USER_DIR_ENTRY_SIZE * 10)


/* "Remote files" system file header format */
typedef PACKED struct {
  dword                   sys_fsize;
  dword                   num_entries;
} fsi_sys_remote_header_type;

/* Type for a remote file entry in the "remote files" system file */
typedef PACKED struct {
  dword                   base_adrs;
  dword                   fsize;
  fsi_filename_type       fname;
} fsi_sys_remote_fentry_type;

/* Name of the system "remote-files" file */
#define  FS_REMOTE_FILENAME     ("$SYS_RMT")
#define  FS_REMOTE_FILENAME_LEN (8)

/* Size of the system "remote files" header */
#define  FS_SYS_REMOTE_HEADER_SIZE   (sizeof(fsi_sys_remote_header_type))

/* Size of the system "remote files" remote file entry */
#define  FS_SYS_REMOTE_ENTRY_SIZE    (sizeof(fsi_sys_remote_fentry_type))

/* Offset of remote file name within a remote file entry: 1st dword */
/* is for base_address, 2nd dword is file length                    */
#define  FS_REMOTE_FILE_NAME_OFFSET  (sizeof(dword) + sizeof(dword))

/* Maximum number of remote files in the system */
#define  FS_MAX_REMOTE_FILES         FS_MAX_REMOTE_FILES_P

/* Name of the system-permanent factory data block file */
#define  FS_FACTORY_FILENAME         ("$SYS.FACTORY")
#define  FS_FACTORY_FILENAME_LEN     12

#ifdef FEATURE_FS_HANDOVER_SUPPORT
/* Name of the system handover block intermediate file */
#define  FSI_HANDOVER1_FILENAME      ("$SYS.HAND.000")
#define  FSI_HANDOVER1_FILENAME_LEN  13
#define  FSI_HANDOVER1_OWNER_ID      FSI_BLK_FSEQUENCE_RSVD_1
#endif

/* Type for an entry in the reservations ledger */
typedef struct {
  fs_conf_code_type      conf_code;
} fsi_rsvr_entry_type;

/* Number of entries in the reservations ledger */
#define  FS_MAX_RESERVATIONS  FS_MAX_RESERVATIONS_P

/* Type for the reservations ledger */
typedef struct {
  fsi_rsvr_entry_type    entry[FS_MAX_RESERVATIONS];
} fsi_rsvr_ledger_type;



/***** INTERNAL OPERATION INTERFACE *****/

/* File system operations supported */
typedef enum { 
  FS_OPEN_OP,             /* Open a file                                   */
  FS_CLOSE_OP,            /* Close a file                                  */
  FS_READ_OP,             /* Read from an open file                        */
  FS_WRITE_OP,            /* Write to an open file                         */
  FS_RSVD_WRITE_OP,       /* Reserved write to an open file                */
  FS_SET_ATTRIBS_OP,      /* Set file attributes                           */
  FS_GET_ATTRIBS_OP,      /* Get file attributes                           */
  FS_FILE_SIZE_OP,        /* Return current file size                      */
  FS_SEEK_OP,             /* Seek to a specified file position             */
  FS_SEEK_READ_OP,        /* Seek to a specified file position then read   
                             from it                                       */
  FS_SEEK_WRITE_OP,       /* Seek to a specified file position then write
                             to it                                         */
  FS_TELL_OP,             /* Return the current file position              */
  FS_TRUNCATE_OP,         /* Truncate an open file                         */
  FS_RMFILE_OP,           /* Remove a file                                 */
  FS_RENAME_OP,           /* Rename a file                                 */
  FS_FMEMSET_OP,          /* Set a file's contents to a character value    */
  FS_MKDIR_OP,            /* Create a user file directory                  */
  FS_RMDIR_OP,            /* Remove a user file directory                  */
  FS_NAMETEST_OP,         /* Test for existence of specified file          */
  FS_NUM_FILES_OP,        /* Get number of files in a user file directory  */
  FS_NUM_DIRS_OP,         /* Get number of subdirectories in a directory   */
  FS_LIST_FILES_OP,       /* List the files in a user file directory       */
  FS_LIST_DIRS_OP,        /* List the subdirectories in a user directory   */
  FS_ENUM_INIT_OP,        /* Initialize file/directory enumeration object  */
  FS_ENUM_NEXT_OP,        /* Return information on next file or directory  */
  FS_ENUM_PREV_OP,        /* Return information on prev file or directory  */
  FS_ENUM_FINISH_OP,      /* Cleanup an enumeration object                 */
  FS_REMOTE_LINK_OP,      /* Create a link to a remote file                */
  FS_RM_REMOTE_LINK_OP,   /* Remove all links to remote files              */
  FS_RESERVED_OP,         /* Reserved file operation                       */
  FS_STATFS_OP,           /* Get filesystem information.                   */
  FS_OPENED_FILE_SIZE_OP, /* Get size of opened file                       */
  FS_FORMAT_OP            /* Format a media card mounted on specified path */
} fs_operation_type;


/* OPEN-Create */
typedef struct {
  fs_filename_type            filename;         /* name of file to create */
  fs_cleanup_option_type      cleanup_option;   /* bad powerdown recovery */
  fs_buffering_option_type    buffering_option; /* buffering allowed? */
  fs_attribute_mask_type      attribute_mask;   /* attribute mask */
  fsi_file_date_type          creation_date;    /* file creation date */
  fsi_file_type               file_type;        /* user or system file */
  fs_handle_type              handle;           /* returned file handle */
} fs_open_create_parms_type;

/* OPEN-Append */
typedef struct {
  fs_filename_type            filename;         /* name of file to append */
} fs_open_append_parms_type;

/* OPEN-Read */
typedef struct {                               
  fs_filename_type            filename;         /* name of file to read */
} fs_open_read_parms_type;

/* OPEN-Update */
typedef struct {
  fs_filename_type            filename;         /* name of file to update */
} fs_open_update_parms_type;

/* OPEN-Truncate */
typedef struct {
  fs_filename_type            filename;         /* name of file to truncate*/
  fs_file_position_type       position;         /* position to truncate at */
} fs_open_truncate_parms_type;

/* OPEN-Test */
typedef struct {
  fs_filename_type            filename;         /* name of file */
} fs_open_test_parms_type;   
  
/* OPEN options */
typedef union {
  fs_open_create_parms_type   create;            
  fs_open_append_parms_type   append;
  fs_open_read_parms_type     read;
  fs_open_update_parms_type   update;
  fs_open_truncate_parms_type truncate;
  fs_open_test_parms_type     test;
} fs_open_options_type;

/* OPEN parameters */
typedef struct {
  fs_access_option_type       access_kind;      /* Type of access */
  fs_open_options_type        option;           /* File open options */
} fs_open_parms_type;

/* CLOSE parameters */
typedef struct {
  fs_handle_type              fhandle;          /* the file handle */
} fs_close_parms_type;

/* READ parameters */
typedef struct {
  fs_handle_type              fhandle;          /* the file handle */
  byte                        *buf;             /* buffer for data */
  dword                       length;           /* num bytes to read */
#ifdef FEATURE_EFS_RPC
#error code not present
#endif /* FEATURE_EFS_RPC */
} fs_read_parms_type;

/* WRITE parameters */
typedef struct {
  fs_handle_type              fhandle;          /* the file handle */
  byte                        *buf;             /* ptr to data buffer */
  dword                       length;           /* num bytes to write */
  fs_conf_code_type           ccode;            /* confirmation code */
#ifdef FEATURE_EFS_RPC
#error code not present
#endif /* FEATURE_EFS_RPC */
} fs_write_parms_type;

/* SET ATTRIBUTES parameters */
typedef struct {
  fs_filename_type            filename;         /* name of file */
  fs_attribute_mask_type      attributes;       /* access attributes */
  fs_file_date_type           creation_date;    /* creation date */
  fs_buffering_option_type    buffering_option; /* RAM buffering option */
  fs_cleanup_option_type      cleanup_option;   /* powerdown recovery */
  fsi_file_type               file_type;        /* user or system file */
} fs_set_attribs_parms_type;

/* GET ATTRIBUTES parameters */
typedef struct {
  fs_filename_type            filename;         /* name of file */
  fs_handle_type              fhandle;          /* handle of file */
  fs_attribute_mask_type      attributes;       /* access attributes */
  fs_file_date_type           creation_date;    /* creation date */
  fs_buffering_option_type    buffering_option; /* RAM buffering option */
  fs_cleanup_option_type      cleanup_option;   /* powerdown recovery */
} fs_get_attribs_parms_type;

/* FILE SIZE parameters */
typedef struct {
  fs_filename_type            filename;         /* name of file */
  dword                       length;           /* num bytes in file */
} fs_file_size_parms_type;

/* OPENED FILE SIZE parameters. */
typedef struct {
  fs_handle_type              fhandle;
  dword                       length;
} fs_opened_file_size_parms_type;

/* SEEK parameters */
typedef struct {
  fs_handle_type              fhandle;          /* the file handle */
  fs_seek_origin_type         origin;           /* seek origin */
  fs_file_position_type       position;         /* seek position */
} fs_seek_parms_type;

/* SEEK_READ parameters */
typedef struct {
  fs_handle_type              fhandle;          /* the file handle */
  fs_seek_origin_type         origin;           /* seek origin */
  fs_file_position_type       position;         /* seek position */
  byte                        *buf;             /* Buffer for data */
  dword                       length;           /* num bytes to read */
#ifdef FEATURE_EFS_RPC
#error code not present
#endif /* FEATURE_EFS_RPC */
} fs_seek_read_parms_type;

/* SEEK_WRITE parameters */
typedef struct {
  fs_handle_type              fhandle;          /* the file handle */
  fs_seek_origin_type         origin;           /* seek origin */
  fs_file_position_type       position;         /* seek position */
  byte                        *buf;             /* ptr to data buffer */
  dword                       length;           /* num bytes to write */
} fs_seek_write_parms_type;

/* TELL parameters */
typedef struct
{
  fs_handle_type              fhandle;          /* the file handle */
  fs_file_position_type       position;         /* returned position */
} fs_tell_parms_type;

/* TRUNCATE parameters */
typedef struct
{
  fs_handle_type              fhandle;          /* the file handle */
  fs_file_position_type       position;         /* position to truncate at */
} fs_truncate_parms_type;

/* REMOVE (file) parameters */
typedef struct
{
  fs_filename_type            filename;         /* name of file */
  fs_handle_type              am_handle;        /* AM handle */
} fs_rmfile_parms_type;

/* RENAME (file) parameters */
typedef struct
{
  fs_filename_type            old_filename;    /* name of file to rename */
  fs_filename_type            new_filename;    /* new name of file */
} fs_rename_parms_type;

/* FMEMSET parameters */
typedef struct {
  fs_handle_type              fhandle;          /* the file handle */
  fs_file_position_type       position;         /* position to write to */
  byte                        fill_value;       /* for seeks past EOF */
} fs_fmemset_parms_type;

/* MKDIR parameters */
typedef struct
{
  fs_filename_type            dirname;          /* name of directory */
} fs_mkdir_parms_type;

/* RMDIR parameters */
typedef struct
{
  fs_filename_type            dirname;          /* name of directory */
} fs_rmdir_parms_type;

/* NAMETEST parameters */
typedef struct
{
  fs_filename_type            filename;         /* name of file */
  fs_nametest_type            type;             /* a file or a dir? */
} fs_nametest_parms_type;

/* NUM FILES parameters */
typedef struct
{
  fs_filename_type            dirname;          /* name of directory */
  word                        num_files;        /* number of files */
} fs_num_files_parms_type;

/* NUM DIRS parameters */
typedef struct
{
  fs_filename_type            dirname;          /* name of directory */
  word                        num_dirs;         /* number of sub-dirs */
} fs_num_dirs_parms_type;

/* LIST FILES parameters */
typedef struct
{
  fs_filename_type            dirname;          /* name of directory */
  byte                        *buf;             /* buffer for data */
  dword                       length;           /* size of buffer */
#ifdef FEATURE_EFS_ACCESS_METHODS
  dword                       full_length;      /* To keep the full length. */
#endif /* FEATURE_EFS_ACCESS_METHODS */
} fs_list_files_parms_type;

/* LIST DIRS parameters */
typedef struct
{
  fs_filename_type            dirname;          /* name of directory */
  byte                        *buf;             /* buffer for data */
  dword                       length;           /* size of buffer */
#ifdef FEATURE_EFS_ACCESS_METHODS
  dword                       full_length;      /* To keep the full length. */
#endif /* FEATURE_EFS_ACCESS_METHODS */
} fs_list_dirs_parms_type;

/* ENUM INIT parameters */
typedef struct
{
  fs_filename_type            dirname;          /* root directory name */
  fs_enum_type                enum_kind;        /* file or directory list */
  fs_enum_iterator_type       *iterator_ptr;    /* ptr to control struct */
  fs_enum_data_type           *data_ptr;        /* file or dir info */
} fs_enum_init_parms_type;

/* ENUM NEXT parameters */
typedef struct
{
  fs_enum_data_type           *data_ptr;        /* file or dir info */
  fs_enum_iterator_type       *iterator_ptr;    /* ptr to control struct */
} fs_enum_next_parms_type;

/* ENUM PREV parameters */
typedef struct
{
  fs_enum_data_type           *data_ptr;        /* file or dir info */
  fs_enum_iterator_type       *iterator_ptr;    /* ptr to control struct */
} fs_enum_prev_parms_type;

/* ENUM FINISH parameters */
typedef struct
{
  fs_enum_iterator_type       *iterator_ptr;    /* ptr to control struct */
} fs_enum_finish_parms_type;

/* REMOTE_LINK parameters */
typedef struct
{
  fs_filename_type            filename;         /* name of remote file */
  dword                       base_address;     /* file's absolute address */
  dword                       length;           /* size of remote file */
} fs_remote_link_parms_type;

/* STATFS parameters. */
typedef struct {
  fs_filename_type            filename;
  statfs_info                 *info;
} fs_statfs_parms_type;

/* Parameters for reserved operation */
typedef struct {
  fs_handle_type              fhandle;          /* the file handle */
  byte                        *buf;             /* buffer for data */
  dword                       length;           /* size of buffer */
} fs_reserved_parms_type;

/* FORMAT parameters */
typedef struct
{
  fs_filename_type            dirname;          /* name of directory */
} fs_format_parms_type;

/* Client-specified parameters for each operation */
typedef union {
  fs_open_parms_type          open;
  fs_close_parms_type         close;
  fs_read_parms_type          read;
  fs_write_parms_type         write;
  fs_set_attribs_parms_type   set_attribs;
  fs_get_attribs_parms_type   get_attribs;
  fs_file_size_parms_type     file_size;
  fs_seek_parms_type          seek;
  fs_seek_read_parms_type     seek_read;
  fs_seek_write_parms_type    seek_write;
  fs_tell_parms_type          tell;
  fs_truncate_parms_type      truncate;
  fs_rmfile_parms_type        rmfile;
  fs_rename_parms_type        rename;
  fs_fmemset_parms_type       fmemset;
  fs_mkdir_parms_type         mkdir;
  fs_rmdir_parms_type         rmdir;
  fs_nametest_parms_type      nametest;
  fs_num_files_parms_type     num_files;
  fs_num_dirs_parms_type      num_dirs;
  fs_list_files_parms_type    list_files;
  fs_list_dirs_parms_type     list_dirs;
  fs_enum_init_parms_type     enum_init;
  fs_enum_next_parms_type     enum_next;
  fs_enum_prev_parms_type     enum_prev;
  fs_enum_finish_parms_type   enum_finish;
  fs_remote_link_parms_type   remote_link;
  fs_reserved_parms_type      reserved;
  fs_statfs_parms_type        statfs;
  fs_opened_file_size_parms_type opened_file_size;
  fs_format_parms_type        format;
} fs_operation_parms_type;

/* Task that requested the operation */
typedef enum {
  FSI_OP_REQ_BY_NV,
  FSI_OP_REQ_BY_OTHERS
} fsi_op_req_task_type;

/* Command interface to file operations */
typedef struct {
  fs_operation_type           op;          /* which operation */
  fsi_op_req_task_type        req_task;    /* which task requested the operation */
  fs_operation_parms_type     parms;       /* operation parameters */
  fs_status_type              status;      /* returned operation status */
} fs_op_request_type;


/* Maximum number of command packets to the fs task */
#define  FSI_MAX_CMD_PACKETS      FS_MAX_CMD_PACKETS_P

/* Embedded file system command packet definition */
typedef struct {
  q_link_type                 link;        /* Queue field */
  boolean                     in_use;      /* Cmd packet "in-use" flag */
  word                        index;       /* Cmd packet static index */
  fs_op_request_type          fs_op;       /* Requested operation & parms */
  rex_tcb_type                *tcb_ptr;    /* Task to signal */
  rex_sigs_type               sigs;        /* Rex signals for task */
  rex_priority_type           op_priority; /* Operation priority */
  q_type                      *done_q_ptr; /* Where to place done buffer */
  fs_callback_p_type          callback_p;  /* Callback function ptr */
  fs_rsp_msg_type             *rsp_msg;    /* Response message ptr */
  fs_status_type              status;      /* Status of request */
#ifdef FEATURE_EFS_ACCESS_METHODS
  fs_handle_type              am_handle;   /* Access method handle used. */
#endif /* FEATURE_EFS_ACCESS_METHODS */
} fs_cmd_type;


/* Maximum number of sub-commands on the internal command queue */
#define  FSI_MAX_SUBCMDS          FS_MAX_SUBCMDS_P

/* Type for an entry on the internal command queue (a "sub-command"). */
/* Note: for entry 0, "prev" points to the start of the free list and */
/* "next" points to the top of the active subcommand queue.           */
typedef struct {
  word                   cmd_queue_index; /* Ptr to task cmd queue entry */
  dword                  count;           /* Number of bytes to operate on */
  dword                  progress;        /* Number of bytes operated on */
  word                   prev;            /* Previous entry index */
  word                   next;            /* Next entry index */
} fsi_subcmd_type;

/* Type for the internal command queue */
typedef struct {
  fsi_subcmd_type       entry[FSI_MAX_SUBCMDS];
} fsi_subcmd_queue_type;

/* Definition of a Null "prev" or "next" index */
#define  FSI_NULL_SUBCMD_INDEX  0

/* The index reserved for the list header pointers */
#define  FSI_RSVD_SUBCMD_INDEX  0

/* Garbage collection eligibility rating threshold at which garbage */
/* collection begins on a sector                                    */
#define  FS_GC_INIT_THRESHOLD   FS_GC_INIT_THRESHOLD_P

/* Number of erased blocks to maintain beyond one sector's worth.  This
 * number must be at least at large as the number of sectors in the device.
 * Our worst case, is that each sector contains a single garbage block.  We
 * need to have 1 clean block available to write the elog. */
#define  FS_GC_CLEAN_RSVD       8

/* Number of blocks reserved only for transaction log files.  Needed is one
 * block for each transaction log opened, and one for each block of
 * transaction-protected data.  For small files, this can be 2x the number
 * of active revertible files. */
#define  FSI_TLOG_RSVD     10
 
/* Number of blocks to subtract from the "space_available" function */
/* to make the reported ceiling a bit less than the actual ceiling  */
#define  FS_SPACE_REPORT_OFFSET FS_SPACE_REPORT_OFFSET_P

/* User directory separator character */
#define  FS_DIRECTORY_CHAR      FS_DIRECTORY_CHAR_P

/* Maximum file system space */
#define  FS_FILE_SPACE_LIMIT    ((dword)FS_FILE_SPACE_LIMIT_P)

/* Maximum size in bytes of transaction log system files */
#define  FS_MAX_TLOG_SIZE       FS_MAX_TLOG_SIZE_P

/* Type for specifying which type of transaction log file exists */
typedef  word   fsi_tlog_type;
#define  FSI_TLOG_TYPE_SIZE   (sizeof(fsi_tlog_type))

/* Layout of transaction log file for types 0 through 3 */
typedef struct {
  fsi_tlog_type          type_of_tlog;      /* What kind of TLOG file */
  word                   tlog_file_length;  /* Size of this TLOG file */
  fsi_owner_id_type      owner_id;          /* Owner ID of target file */
  word                   num_blks;          /* Block count of target file */
  fsi_blk_version_type   blk_versions;      /* Start of blk versions list */
} fsi_tlog_generic_layout_type;

/* Layout of file rename recovery transaction log file */
typedef struct {
  fsi_tlog_type          type_of_tlog;      /* What kind of TLOG file */
  word                   tlog_file_length;  /* Size of this TLOG file */
  fsi_filename_type      old_filename;      /* Old (original) file name */
  fsi_filename_type      new_filename;      /* New file name */
} fsi_tlog_rename_layout_type;

/* Offset of the type of transaction log identifier field */
#define  FSI_TLOG_OFFSET_TLOG_TYPE   0
/* Offset of the length of the transaction log file */
#define  FSI_TLOG_OFFSET_LENGTH      \
             (FPOS(fsi_tlog_generic_layout_type,tlog_file_length))
/* Offset of the owner_id field of the target file */
#define  FSI_TLOG_OFFSET_OWNER_ID    \
             (FPOS(fsi_tlog_generic_layout_type,owner_id))
/* Offset of the number of blocks field in the target file */
#define  FSI_TLOG_OFFSET_NUMBLKS     \
             (FPOS(fsi_tlog_generic_layout_type,num_blks))
/* Offset of the start of the block versions list of the target file */
#define  FSI_TLOG_OFFSET_VERSIONS    \
             (FPOS(fsi_tlog_generic_layout_type,blk_versions))
/* Offset of the first of the two file names for rename-type TLOG files */
#define  FSI_TLOG_OFFSET_FNAME       \
             (FPOS(fsi_tlog_rename_layout_type,old_filename))

/* Indicates no transaction log associated with operation */
#define  FSI_TLOG_TYPE_NONE      0
/* Transaction log for a "truncate to zero size" recovery option */
#define  FSI_TLOG_TYPE_TRUNCATE  1
/* Transaction log for a "delete file" recovery option */
#define  FSI_TLOG_TYPE_DELETE    2
/* Transaction log for a "revert to checkpointed contents" recovery option */
#define  FSI_TLOG_TYPE_RESTORE   3
/* Transaction log for a file rename operation */
#define  FSI_TLOG_TYPE_RENAME        4

/* Common prefix name of all system transaction log files */
#define  FS_TLOG_FILENAME_PREFIX      ("$SYS_TLOG")
#define  FS_TLOG_FILENAME_PREFIX_LEN  (9)

/* Initial transaction log count constant */
#define  FSI_TLOG_COUNT_INITIAL  1
/* Maximum transaction log count (before rolling over) */
#define  FSI_TLOG_COUNT_ROLLOVER 32760

/* Constant indicates Null tlog_index field value */
#define  FSI_TLOG_INDEX_NULL     0


/* - - - - Procedure interface types - - - - */

/* Interface to lowest level WRITE BLOCK operation */
typedef struct {
  fsi_file_adrs_type     blk_adrs;      /* Physical address of this block */
  fsi_generic_blk_type   *outblk;       /* Formatted block to write */
} fsi_write_blk_type;

/* Interface to lowest level READ BLOCK operation */
typedef struct {
  fsi_file_adrs_type     blk_adrs;      /* Physical address of this block */
  fsi_generic_blk_type   *inblk;        /* Location to save read block */
} fsi_read_blk_type;


/* Interface to DEVICE INFO operation */
typedef struct {
  /* Total number of sectors in the device */
  word                   num_sectors;
  /* Size of a device block in bytes */
  word                   blk_size;
  /* Total number of blocks per each sector */
  word                   num_blks[FSI_NUM_SECTORS];
  /* Manufacturer's codes */
  byte                   codes[4];
  /* Device control options */
  word                   device_control_options;
} fsi_info_type;

/* Definiton of bits in device_control_options */
/* This part allows writes during suspend */
#define FSI_OPT_WRITE_WHILE_SUSPEND     0x0001
/* This part perfers writes in page sized units */
#define FSI_OPT_PREFER_PAGE_WRITES      0x0002

/* Types of allocations */
typedef enum {
  FSI_ALLOC_MODE_NORMAL,                /* Standard allocation */
  FSI_ALLOC_MODE_TLOG,                  /* Allocation for tlog files */
  FSI_ALLOC_MODE_GC,                    /* Allocation during GC */
  FSI_ALLOC_MODE_HO                     /* Allocation during HO processing */
} fsi_alloc_mode_type;

/* Interface to file block allocation operation */
typedef struct {
  fsi_alloc_mode_type    alloc_mode;    /* Allocation mode */
  dword                  blk_volatility;   /* Volatility of the new block */
  boolean                blk_invalidated;  /* Will a block be invalidated? */
  fsi_file_adrs_type     old_blk;       /* Invalidated block */
  fsi_file_adrs_type     new_blk;       /* Returned newly allocated block */
  fsi_op_req_task_type   req_task;      /* task requesting the operation */
} fsi_alloc_type;

/* Interface to set/read file name operation */
typedef struct {
  fsi_filename_type      filename;      /* The file name string */
  fsi_file_adrs_type     hdr_adrs;      /* Location of file header block */
  dword                  remote_ptr;    /* Remote file metadata ptr */
} fsi_access_filename_type;

/* Interface to compare file names operation */
typedef  byte   fsi_comparison_type;
#define  FSI_CMP_LESS_THAN     0x01
#define  FSI_CMP_EQUAL         0x02
#define  FSI_CMP_GREATER_THAN  0x03

typedef struct {
  fsi_file_adrs_type     fhdrblk1;      /* Block containing 1st file name */
  fsi_file_adrs_type     fhdrblk2;      /* Block containing 2nd file name */
  fsi_comparison_type    result;        /* Comparison result (<,=,>) */
} fsi_compare_filenames_type;

/* Return parameters from block data integrity check operation */
#define  FSI_INTEGRITY_BAD     0
#define  FSI_INTEGRITY_OK      1
#define  FSI_INTEGRITY_ERASED  2

/* Garbage collection directives */
typedef enum {
  FSI_GC_DYNAMIC,      /* Dynamically evaluate conditions */
  FSI_GC_FAVORABLE,    /* Bias towards doing garbage collection */
  FSI_GC_IF_REQ,       /* Do GC only if required to maintain clean blocks */
  FSI_GC_ONE,          /* Do GC on at least one sector */
  FSI_GC_ALL           /* Do GC on all sectors */
} fsi_gc_directives_type;           

/* Garbage Collection processing states */
typedef enum {
  FSI_GC_STATE_UNITIALIZED, /* Not ready to do garbage collection yet */
  FSI_GC_STATE_IDLE,        /* Not doing garbage collection */
  FSI_GC_STATE_COPYING,     /* Checking for & copying allocated blocks */
  FSI_GC_STATE_ERASING,     /* Erasing sector */
  FSI_GC_STATE_SUSPENDED    /* Sector erase suspended */
} fsi_gc_state_type;

/* Number of buckets in the directory hash table */
/* This number should always be a power of two. */                                                   
#define  FSI_NUM_OF_BUCKETS   128

extern void  fsi_free_cmd_buffer (int i);

#endif /* FSI_H */

