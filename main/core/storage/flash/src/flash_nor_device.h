#ifndef __FLASH_NOR_DEVICE_H__
#define __FLASH_NOR_DEVICE_H__

/**********************************************************************
 * flash_nor_device.h
 *
 * Flash NOR Device interface.
 *
 * Copyright (c) 2005-2009 Qualcomm Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/flash/MSM_FLASH.01.04/flash_nor_device.h#15 $
  $DateTime: 2009/05/11 12:22:13 $
  $Author: c_surven $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2009-01-02   sv      Added 8MB Samsung NOR(K5N6433ABM) support
2008-08-01   sv      Added W18 NOR flash support
2008-07-01   jz      Added Samsung NOR flash support
2008-03-11   sv      Add support for M18 512Mb device
2007-09-17   op      Add support for Extended CFI NOR Intel.
2007-02-28   rt      Add support for NOR partitioning.
2007-02-23   dp      Increase FSD_MAX_SECTORS for ARMPRG download on WS512P
2006-08-09   ps      Added support for PL parts 
2005-11-26   dp      Unification of NOR flash drivers
===========================================================================*/

#include "flashi.h"
#include "msm.h"
#include "flash_nor_utils.h"
/****************************************************************
 * Defines
 ***************************************************************/

/* All uninitialized fields of fsi_nor_device structure and flash_mem_map 
 * structure are set to this default value and are initialized during runtime 
 */
#define INIT_USING_CFI_AT_RUNTIME   ((flash_geometry_info *)0x0)
#define INIT_NOT_NEEDED             ((flash_geometry_info *)0xFFFFFFFF)

/* For Probe table */
#define FLASH_PROBE_END 0xDEADBEEF

/* Max number of sectors based on Spansion WS512P. For a new device with larger
 * number of sectors this number needs to be revised.
 */
#define FSD_MAX_SECTORS       520
#define ERASE_VERIFY_BLK      128

/* This number is based on CFI ROM size for NOR devices. This will need to
 * adjusted when adding support for newer devices
 */
#define CFI_WORD_SIZE         (0x67 + 0x1)

/* Size of extended CFI Data */
#define CFI_EXT_WORD_SIZE     (0x38)

/* total size of CFI Data and CFI extended data */
#define CFI_TOTAL_WORD_SIZE   (CFI_EXT_WORD_SIZE + CFI_WORD_SIZE)

/* offset into CFI table to find extended query table */
#define EXT_CFI_OFFSET_HI          0x16
#define EXT_CFI_OFFSET_LO          0x15

/* CFI device compliant offsets */
#define EXT_CFI_DEV_COMPLIANT0_OFFSET          0x10L
#define EXT_CFI_DEV_COMPLIANT1_OFFSET          0x11L
#define EXT_CFI_DEV_COMPLIANT2_OFFSET          0x12L

/* CFI device info */
#define EXT_CFI_DEVICE_SIZE_OFFSET                  0x27L
#define EXT_CFI_DEVICE_X_IFACE_OFFSET               0x28L
#define EXT_CFI_DEVICE_WBUF_SIZE_OFFSET             0x2AL
#define EXT_CFI_DEVICE_ERASE_BLK_INFO_OFFSET        0x2CL

/* Spansion Extended CFI Offsets */
#define SPANSION_EXT_CFI_BOOT_TYPE_OFFSET           0x4FL
#define SPANSION_EXT_CFI_DEVICE_NUM_BANKS_OFFSET    0x57L
#define SPANSION_EXT_CFI_DEVICE_NUM_BLOCKS_OFFSET   0x58L

/* Samsung Extended CFI Offset */
#define SAMSUNG_EXT_CFI_BOOT_TYPE_OFFSET           0x4DL

/* Intel Extended CFI Offsets */
#define INTEL_EXT_CFI_MAJ_REV_OFFSET               0x03
#define INTEL_EXT_CFI_MIN_REV_OFFSET               0x04
#define INTEL_EXT_CFI_NUM_HWPART_REGION_OFFSET     0x22
#define INTEL_EXT_CFI_NUM_HWPART_HI                0x26
#define INTEL_EXT_CFI_NUM_HWPART_LO                0x25
#define INTEL_EXT_CFI_PART_NUM_BLOCKS_HI           0x2C
#define INTEL_EXT_CFI_PART_NUM_BLOCKS_LO           0x2B

/* When we must add a value given in byte offsets to a word pointer, this
 * macro is used to create a word offset.
 */
#define BYTE_TO_WORD_OFFSET(x) (x>>1)

/* how to handle suspended writes */
#define FSI_OPT_WRITE_WHILE_SUSPEND     0x0001


/* Structure for organizing a table of block boundaries within the
   Flash device. */
typedef struct
{
  dword start;                  /* First address of the block */
  dword len;                    /* Length of the block in bytes */
  word  is_erased;              /* Flag: TRUE if block is erased */
  word  bank_num;               /* Bank this sector is a constituent of */
} block_descriptor_type;

typedef void (*flash_copy_cfi_data_func)(volatile word *baseaddr, 
  word *dest, word count);

/* Definition of probe table structure and pointer type.  Defined in
 * MSM specific code and initialized to tell the flash layer where
 * to probe with what function.
 */

typedef struct {
  volatile word *base_addr;
  flashi_nor_device * (*probe_fcn) (volatile word *probe_addr);
} ProbeTbl;

/* pointer type to Probe Table Structure */
typedef ProbeTbl *ProbeTbl_t;

/* Table of addresses and functions to probe for flash */
extern ProbeTbl probe_info[];
extern int probe_info_size;

/* Spansion Parts */
extern flashi_nor_device S29WS512P;
extern flashi_nor_device S29WS256N0SB;
extern flashi_nor_device S71GL064A_tb;
extern flashi_nor_device S71GL064A_bb;
extern flashi_nor_device S71GL032A;
extern flashi_nor_device S71PL032J80_nsrw;
extern flashi_nor_device S71PL032J80_srw;
extern flashi_nor_device S71VS128R_TOP;
extern flashi_nor_device S71VS128R_BOT;
extern flashi_nor_device S71VS256R_TOP;
extern flashi_nor_device S71VS256R_BOT;

/* Samsung Parts */
extern flashi_nor_device K5N5629ABM;
extern flashi_nor_device K5N6433ABM;
extern flashi_nor_device K5N6433ATM;
extern flashi_nor_device K5N2833ATB;
extern flashi_nor_device K5N2833ABB;
extern flashi_nor_device K5N5629ATC;
extern flashi_nor_device K5N5629ABC;
extern flashi_nor_device K5N5629AUC;

/* Intel Parts */
extern flashi_nor_device Intel_PH28F256L18B85;
extern flashi_nor_device Intel_64W18_ADMux;
extern flashi_nor_device M36W0R5040U6ZS;
extern flashi_nor_device M58LR128KC;
extern flashi_nor_device Intel_128M18_ADMux;
extern flashi_nor_device Intel_256M18_ADMux;
extern flashi_nor_device Intel_512M18_ADMux;
extern flashi_nor_device Intel_1024M18_ADMux;

extern const flashi_nor_device *(spansion_parts[]);
extern const flashi_nor_device *(samsung_parts[]);
extern const flashi_nor_device *(intel_parts[]);
extern const flashi_nor_device **(global_parts[]);

/* Generic read of directly readable flash memory referred to
 * from the device specific dispatch tables to satisfy
 * read requests
 */
extern flash_status fsi_nor_device_worded_read (fs_device_t self,
                                                byte *buffer,
                                                dword offset,
                                                dword count);

extern flash_status flash_geometry_init (flashi_nor_device *nor_device, 
                                         flash_ptr_type baseaddr);

extern void flash_read_cfi(volatile word *baseaddr, 
                           flash_geometry_info *ginfo, 
                           word *cfi_data);

/* Functions for different drivers */
extern flashi_nor_device *spansion_probe (volatile word *baseaddr);
extern flashi_nor_device *intel_probe (volatile word *baseaddr);
extern flashi_nor_device *samsung_nor_probe (volatile word *baseaddr);

#endif /* End of __FLASH_NOR_DEVICE_H__ */
