#ifndef __FLASH_NOR_H__
#define __FLASH_NOR_H__

/**********************************************************************
 * flash_nor.h
 *
 * File System Nor Device interface.
 *
 * This file implements all the function and data definitions
 * common to all the nor devices.
 *
 * Copyright (c) 2004-2008 Qualcomm Incorporated. 
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/flash/MSM_FLASH.01.04/flash_nor.h#6 $ $DateTime: 2008/04/11 10:13:28 $ $Author: c_surven $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2008-03-18   sv      Modified the probe table name to be generic
2007-02-28   rt      Add support for NOR partitioning.
2006-01-27   dp      Added Intel MLC support
2005-12-05   dp      NOR Flash Driver Unification
2005-08-24   rt      Merge L4 changes
2005-06-05   dng     Added spansion_probe function
2004-10-28   jkl     Increased FSD_MAX_SECTORS to support larger NOR.
2004-02-23   drh     Add support for partition tables.
                     Add support for MSM specific table based probe.
2004-01-21   drh     Came from services/efs/fs_nor_device.h

===========================================================================*/

#include "flash.h"
#include "flash_nor_device.h"
#include "flash_nor_msm.h"


/****************************************************************
 * Defines
 ***************************************************************/
/* Number of bit shifts to perform to convert page id to page address */
#define FS_NOR_PAGE_ADDR_BITS 9

/* The default page size for a NOR Device */
#define FLASH_NOR_DEFAULT_PAGE_SIZE 0x200

/****************************************************************
 * Typedef and Enum Definitions
 ***************************************************************/

/* This table is included only because the low level flash drivers
 * use it to report status using this table.  Was taken from the FS
 * layer of EFS1, but low level drivers use some of it.
 *
 * XXX:  Most of this table can be removed, as none of the higher
 * numbered status enums is used by the low level drivers.
 */

/* Operation status values */
/* Do not move these values around as they must be in sysn with diag */
typedef enum {
  FS_OKAY_S,                /* Good status                                 */
  FS_FAIL_S,                /* Low-level operation failed                  */
  FS_BUSY_S,                /* Operation is queued or in progress          */
  FS_FILE_NOT_OPEN_S,       /* File needs to be opened for this operation  */
  FS_FILE_OPEN_S,           /* File needs to be closed for this operation  */
  FS_FILE_ALREADY_EXISTS_S, /* File already exists                         */
  FS_NONEXISTENT_FILE_S,    /* File doesn't exist                          */
  FS_DIR_ALREADY_EXISTS_S,  /* User directory already exists               */
  FS_NONEXISTENT_DIR_S,     /* User directory doesn't exist                */
  FS_DIR_NOT_EMPTY_S,       /* User directory not empty for remove         */
  FS_UNKNOWN_OPERATION_S,   /* Client specified an unknown operation       */
  FS_ILLEGAL_OPERATION_S,   /* Client specified an invalid operation       */
  FS_PARAMETER_ERROR_S,     /* Client supplied a bad parameter value       */
  FS_BAD_FILE_NAME_S,       /* Client specified invalid file/directory name*/
  FS_BAD_FILE_HANDLE_S,     /* Client specified invalid file handle        */
  FS_BAD_SEEK_POS_S,        /* Client specified an invalid SEEK position   */
  FS_BAD_TRUNCATE_POS_S,    /* Client supplied an invalid truncate position*/
  FS_FILE_IS_REMOTE_S,      /* Operation invalid for remote files          */
  FS_INVALID_CONF_CODE_S,   /* Invalid confirmation code specified         */
  FS_EOF_S,                 /* End of file reached                         */
  FS_MORE_S,                /* More records exist to be processed          */
  FS_GC_IN_PROGRESS_S,      /* Garbage Collection in progress              */
  FS_SPACE_EXHAUSTED_S,     /* Out of file system space                    */
  FS_BLK_FULL_S,            /* File block is full                          */
  FS_DIR_SPACE_EXHAUSTED_S, /* Out of Master Directory space               */
  FS_FBL_POOL_EXHAUSTED_S,  /* Out of File Block List free pool space      */
  FS_OPEN_TABLE_FULL_S,     /* Out of open-file table slots                */
  FS_INVALID_ITERATOR_S,    /* Invalid enumeration iterator control struct */
  FS_ENUM_DONE_S,           /* Enumeration operation completed             */
  FS_RMLINK_EXHAUSTED_S,    /* Maximum number of remote links reached      */
  FS_NO_MEDIA_S,            /* Removable media not inserted                */
  FS_MEDIA_CHANGED_S        /* Media has been changed since opened         */
} fs_status_type;

/* Status of erasure operations. */
typedef enum {
  FSI_READ_MODE,
  FSI_ERASING,
  FSI_ERASE_SUSPENDED
} fsi_erase_state_type;

/****************************************************************
 * Global Data Prototypes
 ***************************************************************/
extern volatile word *fs_dev_base;
extern fsi_erase_state_type fsi_erase_state;
extern dword fsi_erase_location;

/* Table of addresses and functions to probe for flash */
extern ProbeTbl flash_nor_probe_table[];
/****************************************************************
 * Function Prototypes
 ***************************************************************/

/* Probe the devices to determine the device type */
int flash_nor_device_probe(struct fsi_device_data *self, int whichflash); 

#endif /* End of __FLASH_NOR_H__ */
