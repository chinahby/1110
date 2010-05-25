/***********************************************************************
 * fs_babylon.h
 *
 * EFS2 definitions for Babylon (aka TIMETEST2) debugging
 * Copyright (C) 2006-2009 Qualcomm, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_babylon.h#7 $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2009-01-27    sh      Updating babylon function calls to match new VU
2008-12-12    rp      Introducing Managed Nand.
2008-12-04    rp      Fix FTL_BAB_EVENT_WATCH to use a distinct id.
2008-11-19    sh      Fix bab header file inclusion to be optional.
2008-11-06    sh      Support for new reduced BAB header files and MTP
2008-10-06    np      Added efs_shred API.
2008-03-19    sh      Improved return value messages
2007-11-16    sh      Renamed from TIMETEST2 to Babylon
2006-08-28    yg      Added some FTL layer related macros
2006-01-27    sh      Added EFS_TT_VALUE()
2006-01-09    sh      Created.

===========================================================================*/
#ifndef __FS_BABYLON_H__
#define __FS_BABYLON_H__

/* include these only to find out if the feature is enabled */
#include "customer.h"
#include "comdef.h"

/* Extended ID values.  EFS doesn't have many. */
#define EFS_BAB_API_CALL         0x22
#define EFS_BAB_TEST             0x99

/* Non-API ID values */
#define EFS_BAB_GLOBAL_LOCK      0x11 /* Marker to measure FS lock time */
#define EFS_BAB_PRINT_ID         0x33 /* Print message */

/* ID values for Public API calls */
#define EFS_BAB_SHRED            0x80
#define EFS_BAB_POWER_DOWN       0x81
#define EFS_BAB_OPEN             0x82
#define EFS_BAB_INITIALIZE       0x83
#define EFS_BAB_READ             0x84
#define EFS_BAB_DESC_READ        0x85
#define EFS_BAB_WRITE            0x86
#define EFS_BAB_DESC_WRITE       0x87
#define EFS_BAB_MKDIR            0x88
#define EFS_BAB_CREAT            0x89
#define EFS_BAB_OPENDIR          0x8A
#define EFS_BAB_READDIR          0x8B
#define EFS_BAB_RENAME           0x8C
#define EFS_BAB_RMDIR            0x8D
#define EFS_BAB_CHMOD            0x8E
#define EFS_BAB_MOUNT            0x8F
#define EFS_BAB_DESC_CLOSE       0x90
#define EFS_BAB_LSEEK            0x91
#define EFS_BAB_TRUNCATE         0x92
#define EFS_BAB_FTRUNCATE        0x93
#define EFS_BAB_STAT             0x94
#define EFS_BAB_FSTAT            0x95
#define EFS_BAB_LSTAT            0x96
#define EFS_BAB_STATVFS          0x97
#define EFS_BAB_FSTATVFS         0x98
#define EFS_BAB_UNLINK           0x99
#define EFS_BAB_CLOSEDIR         0x9A
#define EFS_BAB_UMOUNT           0x9B
#define EFS_BAB_REMOUNT          0x9C
#define EFS_BAB_SYMLINK          0x9D
#define EFS_BAB_READLINK         0x9E
#define EFS_BAB_ACCESS           0x9F
#define EFS_BAB_MKNOD            0xA0
#define EFS_BAB_RAW_PUT          0xA1
#define EFS_BAB_RAW_GET          0xA2
#define EFS_BAB_WAIT_FOR_XACT    0xA3
#define EFS_BAB_RESET            0xA4
#define EFS_BAB_RESET_NOSTOP     0xA5
#define EFS_BAB_GET_FS_DATA      0xA6
#define EFS_BAB_GET_DEVICE_INFO  0xA7
#define EFS_BAB_IMAGE_PREPARE    0xA8
#define EFS_BAB_CLOSE            0xA9
#define EFS_BAB_CHOWN            0xAA
#define EFS_BAB_SET_RESERVATION  0xAB
#define EFS_BAB_SET_QUOTA        0xAC
#define EFS_BAB_GET_GROUP_INFO   0xAD
#define EFS_BAB_FLASH_WRITE_PAGE 0xAE
#define EFS_BAB_FLASH_READ_PAGE  0xAF
#define EFS_BAB_FLASH_ERASE_BLOCK 0xB0
#define EFS_BAB_ZAP              0xB1
#define EFS_BAB_MEMCPY           0xB2
#define EFS_BAB_MEMSET           0xB3
#define EFS_BAB_SDCC_READ        0xB4
#define EFS_BAB_SDCC_WRITE       0xB5
#define EFS_BAB_BENCHMARK_TEST   0xB6
#define EFS_BAB_TESTING          0xB7
#define EFS_BAB_SDCC_WRITE_IOVEC 0xB8
#define EFS_BAB_SDCC_WRITE_REL   0xB9

/* EFS-Stream, mNand Ids. */
#define EFS_BAB_MNAND_PM_INIT               0xBA
#define EFS_BAB_MNAND_PM_WRITE              0xBB
#define EFS_BAB_MNAND_PM_READ               0xBC
#define EFS_BAB_MNAND_PM_START_TRANS        0xBD
#define EFS_BAB_MNAND_PM_END_TRANS          0xBE
#define EFS_BAB_MNAND_PM_ONE_TRANSACTION    0xBF
#define EFS_BAB_MNAND_PM_MEMCPY             0xC0
#define EFS_BAB_MNAND_PM_CMB_IOVEC          0xC1
#define EFS_BAB_MNAND_PM_WRITE_SECTORS      0xC2
#define EFS_BAB_MNAND_PM_WRITE_IOVEC        0xC3
#define EFS_BAB_MNAND_PM_LIQUID_ALLOC       0xC4
#define EFS_BAB_MNAND_PM_SOLID_ALLOC        0xC5
#define EFS_BAB_MNAND_PM_FREE_ONE_LOG_PG    0xC6
#define EFS_BAB_MNAND_PM_REPLAY_LOGS        0xC7

#define EFS_BAB_PUT              0xC8
#define EFS_BAB_GET              0xC9

/* FTL ID values */
#define FTL_BAB_READ             0xAA
#define FTL_BAB_WRITE            0xAB
#define FTL_BAB_ERASE_UPDT       0xAC
#define FTL_BAB_READ_PAGE        0xAD
#define FTL_BAB_WRITE_PAGE       0xAE
#define FTL_BAB_WRITE_SPARE_DAT  0xAF
#define FTL_BAB_MEMCPY           0xB0
#define FTL_BAB_BLK_WRITE        0xB1
#define FTL_BAB_RS_ECC           0xB2
#define FTL_BAB_HC_ECC           0xB3
#define FTL_BAB_GET_FREE         0xB4
#define FTL_BAB_COPY_PAGE        0xB5
#define FTL_BAB_READ_SPARE       0xB6
#define FTL_BAB_EVENT_WATCH      0xB7

/*
 * Include the header files only if the feature is enabled by cust.
 * This is only necessary because not all builds include the babylon
 * service in the source code.
 */
#ifdef FEATURE_BABYLON
  #include "bab_public.h"
#endif

/* Activate the EFS macros only if requested */
#ifdef FEATURE_EFS_BABYLON

  #ifndef FEATURE_BABYLON
    #error "FEATURE_EFS_BABYLON requires the FEATURE_BABYLON service"
  #endif

  /* EFS logging macros */
  #define EFS_BAB_START(id) \
    bab_marker_start (BAB_STREAM_EFS, (id), 0)

  #define EFS_BAB_END(id) \
    bab_marker_end (BAB_STREAM_EFS, (id), 0)

  #define EFS_BAB_API(id) \
    bab_single_event (BAB_STREAM_EFS, (id), EFS_BAB_API_CALL)

  /* Assume all return values are signed 32-bit decimal values? */
  #define EFS_BAB_RETURN(id, val) \
    bab_return_value (BAB_STREAM_EFS, id,\
                      (uint32)(val), TRUE, TRUE, TRUE)

  #define EFS_BAB_PRINT(id, str) \
    bab_print_str (BAB_STREAM_EFS, (id), (str))

  #define EFS_BAB_VALUE(id, val, a, b, c) \
    bab_value (BAB_STREAM_EFS, (id), (val), (a), (b), (c))

  /* FTL logging macros */
  #define FTL_BAB_START(id) \
    bab_marker_start (BAB_STREAM_FTL, (id), 0)

  #define FTL_BAB_END(id) \
    bab_marker_end (BAB_STREAM_FTL, (id), 0)

  #define FTL_BAB_VALUE(id, val, is_dec, is_sign, is_32) \
    bab_value (BAB_STREAM_FTL, (id), (val), (is_dec), (is_sign), (is_32))

#else /* !FEATURE_EFS_BABYLON */

  /* No FEATURE_EFS_BABYLON is defined, so we need stubs for each.
   *
   * XXX: Once Babylon moves into the Storage SU and the header files
   * will be always available, it will kindly stub out the BAB_()
   * macros for us.  For now, we can't always find that file to
   * include, so we stub out our own EFS_BAB_ macros instead.
   */
  #define EFS_BAB_START(...)       (void)0
  #define EFS_BAB_END(...)         (void)0
  #define EFS_BAB_API(...)         (void)0
  #define EFS_BAB_RETURN(...)      (void)0
  #define EFS_BAB_PRINT(...)       (void)0
  #define EFS_BAB_VALUE(...)       (void)0
  #define FTL_BAB_START(...)       (void)0
  #define FTL_BAB_END(...)         (void)0
  #define FTL_BAB_VALUE(...)       (void)0
#endif /* FEATURE_EFS_BABYLON */

#endif /* __FS_BABYLON_H__ */
