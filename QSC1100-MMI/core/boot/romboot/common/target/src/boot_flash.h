#ifndef BOOT_FLASH_H
#define BOOT_FLASH_H

/*===========================================================================

                            B o o t  F l a s h
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains declarations and type definitions for the
  flash specific structures and definitions that are common between
  different images.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/romboot/shared/boot_flash.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/07   ty      Code review changes. Change min page size for detection to
                   2KB
04/23/07   ty      Initial LCU port.

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/

/* Define the FLASH types. */
typedef enum
{
  NO_FLASH        = 0,
  NOR_FLASH       = 1,
  NAND_FLASH      = 2,
  ONENAND_FLASH   = 3,
  MDOC_FLASH      = 4,
  NOR_FLASH_NAND_EFS= 5,
  RESERVED_1_FLASH= 6,
  RESERVED_2_FLASH= 7,
  SIZE_FLASH_TYPE = 0x7FFFFFFF    /* Force 4 byte alignment */
} boot_flash_type;

/*---------------------------------------------------------------------------
  Define the types of flash controllers that may be in use
---------------------------------------------------------------------------*/
typedef enum
{
  NO_CTRL,                          /* Invalid controller     */
  NOR_CTRL,                         /* NOR controller         */
  NAND_CTRL,                        /* NAND controller        */
  SFLASH_CTRL,                      /* SFLASH controller      */
  SIZE_FLASH_CTRL_TYPE = 0x7FFFFFFF /* Force 4 byte alignment */
} boot_flash_ctrl_type;

/*---------------------------------------------------------------------------
  NAND Flash shared data
---------------------------------------------------------------------------*/
typedef struct
{
  uint32   page_size_bytes;
  uint32   page_size_shift;
  uint32   block_size_pages;
  uint32   block_size_shift;
} boot_flash_shared_nand_dev_type;

/*---------------------------------------------------------------------------
  mDOC Flash shared data
---------------------------------------------------------------------------*/
typedef struct
{
  uint32   page_size_bytes;
  uint32   page_size_shift;

  uint32   dev_base_addr;
  uint32   dev_ata_base_addr;
  uint32   dev_h3_ata_cfg_base_addr;
  uint32   dev_data_port_base_addr;
} boot_flash_shared_mdoc_dev_type;

/*---------------------------------------------------------------------------
  Flash shared data
---------------------------------------------------------------------------*/
typedef struct
{
  boot_flash_type       type;        /* Type of flash                      */
  uint32                CS_base;     /* Base address of chip select device
                                        is connected to.                   */
  uint32                data_width;  /* Bit width of device ie: 8bit/16bit */
  boot_flash_ctrl_type  ctrl_type;   /* Controller used to interface with
                                        device                             */

  union {
    boot_flash_shared_nand_dev_type nand;  /* Shared info for NAND device  */
    boot_flash_shared_mdoc_dev_type mdoc;  /* Shared info for mDOC device  */
  }dev_info;
}boot_flash_shared_dev_info_type;

/*---------------------------------------------------------------------------
  Magic Number for AutoDetecting Page Size Algorithm
---------------------------------------------------------------------------*/
#define AUTODETECT_PAGE_SIZE_MAGIC_NUM  0x7D0B435A

/*---------------------------------------------------------------------------
  Page Size AutoDetect defines
---------------------------------------------------------------------------*/
#define BOOT_FLASH_1K_SIZE           1024                 /* 1K Byte Page   */
#define BOOT_FLASH_AUTO_DETECT_MAX_PAGE_SIZE_SUPPORTED  \
                              (4*BOOT_FLASH_1K_SIZE)      /* 4K Bytes Page  */
#define BOOT_FLASH_AUTO_DETECT_MIN_PAGE_SIZE_SUPPORTED  \
                              (2*BOOT_FLASH_1K_SIZE)      /* 512 Bytes Page */


#endif /* BOOT_FLASH_H */
