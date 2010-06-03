#ifndef _SFS_H_
#define _SFS_H_
/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
 
                           Secure File Sytem

                  I N T E R N A L    H E A D E R    F I L E

DESCRIPTION
  This header file is the internal header file for the secure file system.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2004-2009 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $PVCVPath: $
  $Header: //source/qcom/qct/core/securemsm/sfs/main/latest/src/sfs.h#14 $ 
  $DateTime: 2009/04/17 10:46:06 $ $Author: etarolli $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/17/09   ejt     added sfs_seg_flush routine 
03/22/09   sm      removed FEATURE_SEC_SFS_SECURE_PARTITION and kept functionality
12/29/08   ejt     removed 32 byte data alignment and moved it to crypto unit
12/17/08   ejt     removed SFS_SYSTEM_INTERFACE from SFS files
12/10/08   ejt     merged in sfs_dev_key_get & sfs_construct_unique_key
12/05/08   ejt     added 32 byte alignment fix
09/24/08   ejt     fixed spelling of FEATURE_SEC_SFS_SECURE_PARTITION
03/28/07   df      support for 7500 hw key
04/04/07   df      adding MINIMAL_DUAL_PD featurization
03/26/07   df      migrate malloc to UxMalloc
03/01/07   df      support for seek
02/01/07   df      added swid sector
01/09/06   df      local support for GetInfo
12/22/06   df      support for usfs
12/20/06   avm     use extern C malloc definition
12/05/06   df      convert to cpp
                   CR107506 Heap corruption due to path length
11/08/06   df      added support for 7500 pmem_malloc
04/04/06   df      removed AEE_ macros
03/22/06   df      numerous checks added
                   changes to support seed sector on secure partition
01/08/06   df      changes for critical sections
                   added more secure partition files
12/11/05   df      removed OEMHeap dependancy for thinUI builds
11/12/05   df      support for secure flash partition
09/29/05   df      updated name to OEMHeap.h header file
07/25/05   df      do not allow sfs access via diag (sfs remote access check)
07/22/05   df      added FEATURE_SEC_SFS
                   added nv item access for 6250 (FEATURE_SEC_SFS_NV_ITEM)
                   move char arrays to end of handle struct (for better align)
02/27/05   df      added missing proto sfs_def_init()

===========================================================================*/


#include "customer.h"
#ifdef FEATURE_SEC_SFS
#include "comdef.h"
#include "secerrno.h"
#include "UxCommon.h"         //UxMalloc

/* macros to remap sfs calls */
/* NOTE: some of these call are the same for client and server
 * so for a single pd they do not have to be duplicated...
 * but that would require making a utility file
 */

#ifndef IMAGE_APPS_PROC
  #define secapi_get_random(a,b,c) secrnd_get_random(b,c)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#include "secapi.h"
#include "fs_public.h"

#ifdef __cplusplus
}
#endif

#ifdef errno
#undef errno
#endif

/* this is for malloc defs */
#define SFS_MALLOC(m) UxMalloc(m, UX_ALLOC_LOCAL)
#define SFS_FREE UxFree

#ifdef T_MSM7500
#error code not present
#else
#define SFS_SEG_MALLOC SFS_MALLOC
#define SFS_SEG_FREE SFS_FREE
#endif

#define SFS_MAX_FILENAME_SIZE 128    //old from efs - 80
#define SFS_MAX_PATH_SIZE     256    //old from efs - 128
#define SFS_MAX_DRIVE_NAME_SIZE 8    //usr_sfs: sys_sfs:

#define SFS_DIR_CHAR '/'
#define SFS_DIR_STRING "/"

#define SFS_MAX_IX_ENTRY   0xfffffffe
#define SFS_MAX_SEGS       0xfffffffe
#define SFS_DEF_SEG_SIZE   1024 // 1K 
#define SFS_IX_SEG_SIZE    SFS_DEF_SEG_SIZE
#define SFS_LIST_SIZE      16
#define SFS_SUFFIX_SIZE    13  // ".sfs00000001" including null term
#define SFS_IX_SUFFIX_SIZE 19  // ".index.sfs00000001" including null term
#define SFS_MANGLED_SIZE   28
#define SFS_MANGLED_STR_SIZE (SFS_MANGLED_SIZE + 1) // add the null terminator
#define SFS_HMAC_SIZE      20
#define SFS_SHA_SIZE       20
#define SFS_COOKIE         0xaa7e7e55
#define SFS_AES_BLOCK_SIZE 16
#define SFS_ENDEC_ENCRYPT  0
#define SFS_ENDEC_DECRYPT  1
#define SFS_HMAC_CREATE    0
#define SFS_HMAC_VERIFY    1

#define SFS_SEG_FILE_SIZE(seg_size) ((seg_size) + \
                                     SFS_AES_BLOCK_SIZE + SFS_HMAC_SIZE)

#define SFS_FLASH_DEV_NAME             "_/sfs_flash_dev/"
#define SFS_FLASH_DEV_NAME_SIM         "_/sfs_flash_dev/sim"
#define SFS_FLASH_DEV_NAME_IMEI        "_/sfs_flash_dev/imei"
#define SFS_FLASH_DEV_NAME_PRIV_FILE_1 "_/sfs_flash_dev/pfile1"
#define SFS_FLASH_DEV_NAME_PRIV_FILE_2 "_/sfs_flash_dev/pfile2"
#define SFS_FLASH_DEV_NAME_PRIV_FILE_3 "_/sfs_flash_dev/pfile3"
#define SFS_FLASH_DEV_NAME_PRIV_FILE_4 "_/sfs_flash_dev/pfile4"
#define SFS_FLASH_DEV_NAME_PRIV_FILE_5 "_/sfs_flash_dev/pfile5"
#define SFS_FLASH_DEV_NAME_SEED        "_/sfs_flash_dev/seed"
#define SFS_FLASH_DEV_NAME_SWID        "_/sfs_flash_dev/swid"
#define SFS_FLASH_DEV_NAME_RESV        "_/sfs_flash_dev/resv"
#define SFS_FLASH_DEV_NLEN strlen(SFS_FLASH_DEV_NAME)

typedef enum
{
  SFS_FLASH_SIMLOC_SECTOR,
  SFS_FLASH_IMEI_SECTOR,
  SFS_FLASH_PRIV_FILE_1_SECTOR,
  SFS_FLASH_PRIV_FILE_2_SECTOR,
  SFS_FLASH_PRIV_FILE_3_SECTOR,
  SFS_FLASH_PRIV_FILE_4_SECTOR,
  SFS_FLASH_PRIV_FILE_5_SECTOR,
  SFS_FLASH_SWID_SECTOR,
  SFS_FLASH_RESERVED_SECTOR,
  SFS_FLASH_SEED_SECTOR,
  SFS_FLASH_KEY_SECTOR,
  SFS_FLASH_MAX_SECTOR
}sfs_sector_enum_type;

typedef enum
{
  SFS_FILE_TYPE_NORM      = 0,           // normal file
  SFS_FILE_TYPE_IX        = 1,           // index file
  SFS_FILE_TYPE_FLASH_DEV = 0x100,       // flash dev
  SFS_FILE_TYPE_FLASH_DEV_SIMLOC =   (0x100 | SFS_FLASH_SIMLOC_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_IMEI   =   (0x100 | SFS_FLASH_IMEI_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_1 = (0x100 | SFS_FLASH_PRIV_FILE_1_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_2 = (0x100 | SFS_FLASH_PRIV_FILE_2_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_3 = (0x100 | SFS_FLASH_PRIV_FILE_3_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_4 = (0x100 | SFS_FLASH_PRIV_FILE_4_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_PRIV_FILE_5 = (0x100 | SFS_FLASH_PRIV_FILE_5_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_SWID   =   (0x100 | SFS_FLASH_SWID_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_SEED   =   (0x100 | SFS_FLASH_SEED_SECTOR),
  SFS_FILE_TYPE_FLASH_DEV_RESV,
  SFS_FILE_TYPE_FLASH_DEV_MAX
}sfs_file_type_enum_type;

typedef enum
{
  SFS_NO_ERROR = 0,
  SFS_OPEN_ERROR,
  SFS_CLOSE_ERROR,
  SFS_READ_ERROR,
  SFS_WRITE_ERROR,
  SFS_VERIFICATION_ERROR,
  SFS_KEY_ACCESS_ERROR,
  SFS_EOF,
  SFS_RESV = 0x7fffffff
}sfs_errno_enum_type;


typedef struct
{
  uint32 cookie;
  uint32 num_segs;           // total number of segments used by index file
  uint32 num_entries;        // total number of entries in index file
  uint32 max_entry;          // need to figure out a better way of doing this
}sfs_ix_entry0_type;

typedef struct
{
  uint32 cookie;
  uint32 seg_size;           // segment size (does not include HMAC or IV)
  uint32 file_size;          // actual number of "data" bytes in file
  char   file_name[SFS_MAX_FILENAME_SIZE];
  uint8  hmac[SFS_HMAC_SIZE];
}sfs_ix_entry1_type;

typedef struct
{
  union
  {
    sfs_ix_entry0_type en0;  // the zeroth entry has general entry info
    sfs_ix_entry1_type en;   // the first entry has index file info
  }entry;                    // the second entry has reg file info
}sfs_ix_entry_type;

#define SFS_IX_ENTRY_SIZE sizeof(sfs_ix_entry)

typedef int sfs_segfh_type;

#define SFS_SYS_KEY_SIZE SFS_AES_BLOCK_SIZE

typedef struct
{
  /* file info */
  uint32  cookie;
  sfs_errno_enum_type errno;
  uint32  flags;             /* O_READ, O_WRITE, O_CREATE etc */
  uint32  mode;              /* read = 0 / write = 1*/
  sfs_file_type_enum_type type; /* normal, index, flashdev, etc */
  uint32  curr_pos;          /* 0..file_size - 1 */
  uint32  file_size;         /* number of bytes in file (can be zero) */
  uint32  dirty_file;        /* if file is dirty, update index file on close */
  uint8   master_hmac[SFS_HMAC_SIZE];       /* hmac over all segments */
  uint32  is_key_valid;      /* '1' if key field from below is valid. '0' otherwise.
                                 Key field is always valid when this is User SFS
                                 Key field is valid on System SFS if
                                  - Using MSM Device key on 7K targets on Modem ARM
                                  - Using MSM Device key on 7K targets on Apps ARM (here
                                    the key is a derivation of device key)
                                  - Using OEM Key as defined in sec_oem* layer.
                                  - Using a Key from Secure Partition, as controlled by
                                    sec_oem* function.
                             */
  uint8   key[SFS_SYS_KEY_SIZE];

  /* segment info */
  uint8* seg_buf;               // must be malloc'd for each instance, 32 byte aligned pointer
  uint32 seg_size;              // N * block size (multiple block sizes)
  uint32 curr_seg;              // 0..M-1
  uint32 opened_seg;            //   for now reserve 0xffffffff for initializing
  uint32 decrypted_seg;
  uint32 num_segs;              // M
  uint32 seg_pos;

  sfs_segfh_type  seg_fh;
  uint32 dirty_seg;
  uint32 ver_seg;

  /* seg names */
  char   seg_name[SFS_MAX_FILENAME_SIZE];
  char   m_seg_name[SFS_MANGLED_STR_SIZE];
  char   m_seg_npath[SFS_MAX_PATH_SIZE];

  /* path and dir info */
  char file_name[SFS_MAX_FILENAME_SIZE];
  char path[SFS_MAX_PATH_SIZE];        // path given to open command
  char root_dir[SFS_MAX_PATH_SIZE];    // the "root dir"
  char dir[SFS_MAX_FILENAME_SIZE];     // cwd to be mangled
  char mdir[SFS_MANGLED_STR_SIZE];     // mangled dir name includes null term
  char tmp_path[SFS_MAX_PATH_SIZE];    // temp or scratch path

  /* index file info */
  uint32 ix_num;           /* entry number in index file */
  char  ix_file_name[SFS_MAX_FILENAME_SIZE];
  char  mix_file_name[SFS_MANGLED_STR_SIZE];

}sfs_handle_type;


#include "rex.h"
typedef rex_crit_sect_type sfs_mutex_type;
#define SFS_MUTEX_INIT(m)  rex_init_crit_sect(&m)
#define SFS_MUTEX_ENTER(m) rex_enter_crit_sect(&m)
#define SFS_MUTEX_EXIT(m)  rex_leave_crit_sect(&m)


/* externalized functions */
#ifdef FEATURE_DIAG_FS_ACCESS_VALIDATION
void sfs_register_remote_access_check (void);
#endif

// client side interface
secerrno_enum_type sfs_open_seg(sfs_handle_type* fh);
secerrno_enum_type sfs_close_seg(sfs_handle_type* fh);
secerrno_enum_type sfs_seg_hmac (sfs_handle_type *fh, int mode);
secerrno_enum_type sfs_mangle_seg_npath (sfs_handle_type* fh);
secerrno_enum_type sfs_seg_endec (sfs_handle_type *fh, int mode);
secerrno_enum_type sfs_mangle (char *out, char *in, int in_len);
secerrno_enum_type sfs_close_mfile(sfs_handle_type* fh);
secerrno_enum_type sfs_trunc(sfs_handle_type* fh);
secerrno_enum_type sfs_seg_flush(sfs_handle_type* fh);
secerrno_enum_type sfs_delete(const char *path);
secerrno_enum_type sfs_def_init (uint32 seg_size);
secerrno_enum_type sfs_get_size (sfs_handle_type* fh, uint32* size);
secerrno_enum_type sfs_dir_create(const char *path);
secerrno_enum_type sfs_ixfile_dir_create
(
  char *new_path, 
  const char* path,
  int new_path_len
);

sfs_handle_type *sfs_open_mfile(const char* path, int flags);
int sfs_write_bytes(sfs_handle_type* fh, const char* buf, int num_bytes);
int sfs_read_bytes(sfs_handle_type* fh, char* buf, int num_bytes);
int sfs_seek_pos(sfs_handle_type* fh, int32 offset,int whence);

#ifdef __cplusplus
extern "C"
{
#endif
  secerrno_enum_type sfs_dev_key_get(uint8 *key, unsigned int *key_len);
  secerrno_enum_type sfs_construct_unique_key(uint8 *key, unsigned int key_len);
#ifdef __cplusplus
}
#endif

#endif // FEATURE_SEC_SFS
#endif // _SFS_H_

