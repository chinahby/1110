#ifndef __FLASH_NAND_H__
#define __FLASH_NAND_H__

/**********************************************************************
 * flash_nand.h
 *
 * Nand Device interface.
 *
 * This header file includes all the function interfaces common to all the nand devices.
 * The contents of this file are MSM independent
 *
 * NOTE: The functions in this interface should be called only from
 *       task context unless FLASH_SINGLE_THREADED is defined.
 *
 * Copyright (c) 2004-2008 Qualcomm Incorporated. 
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/flash/MSM_FLASH.01.04/flash_nand.h#18 $ $DateTime: 2008/09/16 10:38:42 $ $Author: mhossain $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2008-08-15   mh      Add FlexOneNAND SUPORT MACROS: FLASH_ONENAND_DEFAULT_MREAD 
                     and FLASH_ONENAND_SUPERLOAD_MREAD
2008-07-14   op      Support Factory and Raw Images
2008-03-20   eo      Add return status to set_ecc_state API 
2007-12-26   dp      Added Cache read ONFI support
2007-06-30   mm      Moved ECC states to public file flash.h
2007-06-24   mm      Added new mode for Hardware ECC 
                     (FLASH_NAND_MAIN_AND_SPARE_ECC_ENABLED). This mode enables
                     Hardware ECC on Main and Spare Area.   
2007-05-10   rt      Add support for reading pages from OTP area.
2007-02-22   dp      Reserve OneNAND burst write and 2X flag
2006-06-27   dp      Adding Flash API updates for copy page & spare data bytes
2006-05-04   dp      Adding controller abstraction layer
2006-02-17   rt      Add functions to clear OP and WR_ERR INT bits individually
2005-08-02   drh     Changes to compile in L4 environment
2005-04-28   drh     Add function prototype
2004-09-30   drh     Move partition functions out to partition specific file
2004-09-29   drh     Remove extern no longer needed
2004-09-22   rg      added check for rex_is_initialized() for locks
2004-08-09   drh     Added support for ECC manipulation
2004-07-20    gr     Added multi-threading support.
2004-07-20   drh     Do not compile partition functions for boot_loader
2004-03-08   dlb     Rename FS_NAND... to FLASH_NAND...
2004-03-31   drh     Move NAND controller specific defines to flash_nand_msm.h 
                     in preparation for new controller type modifications.
2004-03-30   drh     Add external function prototype
2004-03-19   drh     Integrate functionality to ask if NAND controller is busy.
                     Change partition manipulation function prototypes.
2004-02-23   drh     Move MSM specific defines to fs_nand_msm.h.
                     Add new externs for private data returning functions.
2004-01-21   drh     Came from services/efs/flash_nand_device.h


===========================================================================*/

#include "flash.h"
#include "flashi.h"


/********************************
 * Different NAND device options 
 ********************************/

  /* Bits 0-3 of dev_specfic_flag1 
   * Different Copy-page/copyback features */
#define FLASH_NAND_COPY_PAGE_BITS(x)      ((x) & 0xF)
/* No optimization. Do complete read followed by write */
#define FLASH_NAND_CP_DEFAULT             0x00000000  
/* Optimized Option 1 for 512B page devices with no internal buffer */
/* No memcpy if no read error or only ECC correctable error 
 * encountered */
#define FLASH_NAND_CP_OPTION_1            0x00000001
/* Optimized Option 2 for 2KB page devices with internal buffer & cmds
 * Use NAND_SW commands to read/write via internal buffer commands 
 * (0x00-0x35 & 0x85-0x10) with no memcpy, if no read error or no ECC 
 * correctable error encountered */
#define FLASH_NAND_CP_OPTION_2            0x00000002

  /* Bits 7-4 of dev_specfic_flag1 
   * Different read-multiple page within block features */
#define FLASH_NAND_MREAD_BITS(x)      ((x) & 0xF0)
/* No optimization. Do page by page reading */
#define FLASH_NAND_MREAD_DEFAULT             0x00000000  
/* Optimized Option 1 for reading multiple 2K pages within same block
 * 0x00-0x30 for first page. All succedding pages (except last page)
 * using 0x31. Last page using 0x3F command */
#define FLASH_NAND_MREAD_OPTION_1            0x00000010

/**********************************
 * Different OneNAND device options 
 **********************************/

  /* Bits 16-31 of device_id field */
#define FLASH_ONENAND_VER_ID_BITS(x)     ((x << 16) & 0xFFFF0000)
#define FLASH_ONENAND_VER_ID_SET(id)     ((id & 0xFFFF0000) != 0 ? 1 : 0)
#define FLASH_ONENAND_RESET_VER_ID(id)   (id & 0x0000FFFF)

/* Bits 0-3 of dev_specfic_flag1: 
 * Different block unlock schemes offered by OneNAND on different 
   devices */
#define FLASH_ONENAND_UNLOCK_BITS(x)      ((x) & 0xF)
#define FLASH_ONENAND_RANGE_UNLOCK        0x00000000
#define FLASH_ONENAND_BLOCK_UNLOCK        0x00000001

/* Bits 4-7 of dev_specfic_flag1:
 * Different read multiple page options of OneNAND */
#define FLASH_ONENAND_MREAD_BITS(x)       ((x) & 0xF0)
#define FLASH_ONENAND_DEFAULT_MREAD       0x00000000
#define FLASH_ONENAND_PINGPONG_MREAD      0x00000010
#define FLASH_ONENAND_CACHE_MREAD         0x00000020
#define FLASH_ONENAND_SYNCBURST_MREAD     0x00000030
#define FLASH_ONENAND_SUPERLOAD_MREAD     0x00000040

/* Bits 8-11 of dev_specfic_flag1:
 * Different is block erased mechanism offered by OneNAND */
#define FLASH_ONENAND_EVERIFY_BITS(x)    ((x) & 0xF00)
#define FLASH_ONENAND_EVERIFY_PAGES      0x00000000
#define FLASH_ONENAND_EVERIFY_BLOCK      0x00000100

/* Bit 12 of dev_specfic_flag1:
 * Enable support to distinguish burst read/write devices */
#define FLASH_ONENAND_BWR_SUPPORT_BIT(x) ((x) & 0x1000)
#define FLASH_ONENAND_BWR_NOT_SUPPORTED   0x00000000
#define FLASH_ONENAND_BWR_SUPPORTED       0x00001000

/* Bit 13 of dev_specfic_flag1:
 * Enable support to distinguish 2X mode devices */
#define FLASH_ONENAND_2X_SUPPORT_BIT(x) ((x) & 0x2000)
#define FLASH_ONENAND_2X_NOT_SUPPORTED   0x00000000
#define FLASH_ONENAND_2X_SUPPORTED       0x00002000

/* 
 * Common to both NAND and OneNAND device options 
 */
/* Bits 24-27 of dev_specific_flag1 */
#define FLASH_NANDS_MLC_PAIRING_BITS(x)   ((x) & 0xF000000)
/* No pairing of page consideration for sudden powerdown during writes */
#define FLASH_NANDS_MLC_PAIRING_NONE      0x00000000  
/* Pairing algorithm : Min pair distance_Max pair_distance_index 
   Sample page pairing: {(0,4), (1,5), (2,8), (3,9), (6,12), (7,13),
                         (10,16), (11,17), (14,20), (15, 21)}        */
#define FLASH_NANDS_MLC_PAIRING_4_6_1     0x01000000

/* Structure definition used to make an array of probe functions */
struct flash_probers {
  /* Query operation. */
  int (*probe) (fsi_device_t priv, int maker_id, int device_id);
};

/* Structure definition used to make an array of probe functions */
struct flash_ctlr_probers {
  /* Query operation. */
  int (*probe) (struct fsi_device_data *self, int whichflash);
  /* Is controller busy operation. */
  int (*is_busy) (void);
};

/****************************************************************
 * Global Data Prototypes
 ***************************************************************/

/****************************************************************
 * Global Function Prototypes
 ***************************************************************/

/****************************************************************
 * Function Prototypes
 ***************************************************************/

/* Probe the devices to determine the device type */

/* Probe a particular flash device (i.e. 0, 1, 2...max devices).
 *     Caller allocates an empty fs_device_data structure and
 *     this function call fills it in if successful.  Initial values
 *     filled in allow access to entire flash and may be used
 *     immediately if the scope of the caller needs to access entire
 *     device.
 */
int flash_nand_device_probe (struct fsi_device_data *self, int whichflash);
int flash_nand_is_busy (void);
char *flash_nand_device_name (fs_device_t self);
int flash_nand_maker_id (fs_device_t self);
int flash_nand_device_id (fs_device_t self);
uint32 flash_nand_block_count (fs_device_t self);
uint32 flash_nand_block_size (fs_device_t self);
uint32 flash_nand_page_size (fs_device_t self);
uint32 flash_nand_total_page_size (fs_device_t self);
uint32 flash_nand_device_type (fs_device_t self);
uint32 flash_nand_device_width (fs_device_t self);
uint32 flash_nand_retrieve_ID (void);

/* Device Probe*/
int flash_nand_dev_probe_common(struct fs_device_data * self);  

/* Write operations. */
int flash_nand_write_page (fs_device_t self, page_id page, void *data);

/* Write page & user-spare-data operation. */
int flash_nand_write_page_and_udata (fs_device_t self, page_id page, 
  void *page_data, void *spare_udata, uint32 spare_udata_blen);

/* A full block erase */
int flash_nand_erase_block (fs_device_t self, block_id block);

/* Read the data into a buffer */
int flash_nand_read_page (fs_device_t self, page_id page, void *data);

/* Read just the user-spare-data from given page into a buffer */
int flash_nand_read_spare_udata (fs_device_t self, page_id page,
  void *spare_udata, uint32 spare_udata_blen);

/* Read the data and spare into a buffer */
int flash_nand_read_page_and_udata (fs_device_t self, page_id page,
  void *data, void *spare, uint32 spare_udata_blen);

/* Read page and spare area from OTP block */
int flash_nand_read_otp_page_and_spare (fs_device_t self, page_id page,
  void *data, void *spare, uint32 spare_blen, void *dev_otp_params);

/* Get corrected spare user data bytes available from controller */
int flash_nand_get_spare_corrected_bcount (fs_device_t self);

/* Determine if block is erased or not */
int flash_nand_is_block_erased (fs_device_t self, block_id block);

/* Determine if page is erased or not */
int flash_nand_is_page_erased (fs_device_t self, page_id page);

/* Mark the given block as bad by writing 0x0 to all pages */
int flash_nand_mark_block_bad (fs_device_t self, block_id block);

/* Set default state of ECC generation */
int flash_nand_set_ecc_state (fs_device_t self, int state);

fs_device_write_style_t flash_nand_get_write_style (fs_device_t self);

/* Obtain the manufacturer and device IDs */
void flash_nand_id_data (int *maker_id, int *device_id);

/* Read len spare bytes starting from spare_offset in page into data buffer */
int flash_nand_read_spare_bytes (fs_device_t self, page_id page, 
  uint32 spare_offset, uint32 len, void *data);

/* Lock/Unlock/Locktight from block_start to block_end if supported */
int flash_nand_lock_ops (fs_device_t self, block_id block_start, 
  block_id block_end, uint32 opcode);

/* Read num_pages from start_page into data */
int flash_nand_read_mpages (fs_device_t self, page_id start_page, 
  page_id num_pages, void *data, void (*dog_kick_fn_ptr)(void));

/* Copy data from src_page to dest_page */
int flash_nand_copy_page (fs_device_t self, page_id src_page, 
  page_id dest_page);

/* Determine the offset of bad block flag, in the  Code Word correctly based on
   flash width and page_size */
int flash_nand_check_bad_block_marker (fs_device_t self, uint32 spare_data);

/* Return physical boundaries of a current partition */
int flash_nand_get_partition_phy_limits (fs_device_t self, uint32 *start,
  uint32 *limit, void *rsrvd);

/* Return attributes of the current partition */
int flash_nand_get_partition_attribs (fs_device_t self, uint32 *attribute); 

#endif /* End of __FLASH_NAND_H__ */


